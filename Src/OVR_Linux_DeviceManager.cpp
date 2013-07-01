#include "OVR_Linux_DeviceManager.h"

#include "OVR_SensorImpl.h"
#include "OVR_Linux_HMDDevice.h"
#include "OVR_Linux_HIDDevice.h"

#include "Kernel/OVR_Timer.h"

namespace OVR { namespace Linux {

DeviceManager::DeviceManager()
{
}

DeviceManager::~DeviceManager()
{
    OVR_ASSERT(!pThread);
}

bool DeviceManager::Initialize(DeviceBase*)
{
    if(!DeviceManagerImpl::Initialize(0))
        return false;

    pThread = *new DeviceManagerThread(this);
    if (!pThread || !pThread->Start())
        return false;

    HidDeviceManager = *HIDDeviceManager::CreateInternal(this);
    pCreateDesc->pDevice = this;
    LogText("OVR::DeviceManager - initialized.\n");
    return true;
}

void DeviceManager::Shutdown()
{
    LogText("OVR::DeviceManager - shutting down.\n");

    pCreateDesc->pLock->pManager = 0;

    pThread->PushExitCommand(false);
    pThread.Clear();

    DeviceManagerImpl::Shutdown();
}

ThreadCommandQueue *DeviceManager::GetThreadQueue()
{
    return pThread;
}

bool DeviceManager::GetDeviceInfo(DeviceInfo *info) const
{
    if ((info->InfoClassType != Device_Manager) &&
            (info->InfoClassType != Device_None))
        return false;

    info->Type = Device_Manager;
    info->Version = 0;
    OVR_strcpy(info->ProductName, DeviceInfo::MaxNameLength, "DeviceManager");
    OVR_strcpy(info->Manufacturer, DeviceInfo::MaxNameLength, "Oculus VR, Inc.");
    return true;
}

DeviceEnumerator<> DeviceManager::EnumerateDevicesEx(const DeviceEnumerationArgs &args)
{
    if (GetThreadId() != OVR::GetCurrentThreadId())
    {
        pThread->PushCall((DeviceManagerImpl*)this,
                &DeviceManager::EnumerateAllFactoryDevices, true);
    }
    else
        DeviceManager::EnumerateAllFactoryDevices();

    return DeviceManagerImpl::EnumerateDevicesEx(args);
}

ThreadId DeviceManager::GetThreadId() const
{
    return pThread->GetThreadId();
}

bool DeviceManagerThread::AddTicksNotifier(Notifier* notify)
{
    TicksNotifiers.PushBack(notify);
    return true;
}

bool DeviceManagerThread::RemoveTicksNotifier(Notifier* notify)
{
    for (UPInt i = 0; i < TicksNotifiers.GetSize(); i++)
    {
        if (TicksNotifiers[i] == notify)
        {
            TicksNotifiers.RemoveAt(i);
            return true;
        }
    }
    return false;
}

bool DeviceManagerThread::AddOverlappedEvent(Notifier* notify, struct pollfd poll)
{
        WaitNotifiers.PushBack(notify);
        PollFds.PushBack(poll);
        OVR_ASSERT(WaitNotifiers.GetSize() <= MAXIMUM_WAIT_OBJECTS);
        return true;
}

bool DeviceManagerThread::RemoveOverlappedEvent(Notifier* notify)
{
        for (UPInt i = 0; i < WaitNotifiers.GetSize(); i++)
        {
                if (WaitNotifiers[i] == notify)
                {
                        WaitNotifiers.RemoveAt(i);
                        return true;
                }
        }
        return false;
}

DeviceManagerThread::DeviceManagerThread(DeviceManager *pdevMgr) :Thread(ThreadStackSize)
{
    (void) pdevMgr;
}

DeviceManagerThread::~DeviceManagerThread()
{
}

int DeviceManagerThread::Run()
{
    ThreadCommand::PopBuffer command;

    unsigned int threadId = *((unsigned int*)GetThreadId());
    SetThreadName("OVR::DeviceManagerThread");
    LogText("OVR::DeviceManagerThread - running (ThreadId=0x%X).\n", threadId);
    while(!IsExiting())
    {
        if (PopCommand(&command))
        {
            command.Execute();
        }
        else
        {
            UInt32 waitMs = INT_MAX;
            if (!TicksNotifiers.IsEmpty())
            {
                UInt64 ticksMks = Timer::GetTicks();
                UInt32  waitAllowed;
                for (UPInt j = 0; j < TicksNotifiers.GetSize(); j++)
                {
                    waitAllowed = (UInt32)(TicksNotifiers[j]->OnTicks(ticksMks) / Timer::MksPerMs);
                    if (waitAllowed < waitMs)
                        waitMs = waitAllowed;
                }
            }
            if (!PollFds.IsEmpty()) {
                int ret = poll(&PollFds[0], PollFds.GetSize(), waitMs);
                if (ret == -1) {
                    LogText("OVR::DeviceManagerThread - Got invalid return on polling %i\n", ret);
                }
                if(ret > 0) {
                    for (UPInt i = 0; i < PollFds.GetSize(); i++) {
                        struct pollfd fd = PollFds[i];
                        if(fd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
                            LogText("OVR::DeviceManagerThread - Invalid revents %i\n", fd.revents);
                        } else {
                            if(WaitNotifiers[i]) {
                                WaitNotifiers[i]->OnOverlappedEvent();
                            }
                        }
                    }
                }
            }
        }
    }
    LogText("OVR::DeviceManagerThread - exiting (ThreadId=0x%X).\n", threadId);
    return 0;
}

} // namespace Linux

DeviceManager *DeviceManager::Create()
{
    if(!System::IsInitialized())
    {
        // Use custom message, since Log is not yet installed.
        OVR_DEBUG_STATEMENT(Log::GetDefaultLog()->
                LogMessage(Log_Debug,
                    "DeviceManager::Create failed - OVR::System not initialized"); );
        return 0;
    }
    Ptr<Linux::DeviceManager> manager = *new Linux::DeviceManager;
    if(manager)
    {
        if(manager->Initialize(0))
        {
            manager->AddFactory(&SensorDeviceFactory::Instance);
            manager->AddFactory(&Linux::HMDDeviceFactory::Instance);
            manager->AddRef();
        }
        else
        {
            manager.Clear();
        }
    }
    return manager.GetPtr();
}

} // namespace OVR
