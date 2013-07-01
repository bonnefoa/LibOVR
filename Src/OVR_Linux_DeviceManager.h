#ifndef OVR_LINUX_DEVICEMANAGER_H
#define OVR_LINUX_DEVICEMANAGER_H

#include "Kernel/OVR_Threads.h"
#include "OVR_DeviceImpl.h"
#include <pthread.h>
#include <poll.h>

namespace OVR { namespace Linux {

class DeviceManagerThread;

class DeviceManager : public DeviceManagerImpl
{
        public:
                DeviceManager();
                ~DeviceManager();

                virtual bool Initialize(DeviceBase *parent);
                virtual void Shutdown();

                virtual ThreadCommandQueue *GetThreadQueue();
                virtual ThreadId GetThreadId() const;

                virtual DeviceEnumerator<> EnumerateDevicesEx(const DeviceEnumerationArgs &args);
                virtual bool GetDeviceInfo(DeviceInfo *info) const;

        private:

        public:
                Ptr<DeviceManagerThread> pThread;

};

class DeviceManagerThread : public Thread, public ThreadCommandQueue
{
        friend class DeviceManager;
        enum { ThreadStackSize = 32 * 1024 };

        public:
        DeviceManagerThread(DeviceManager *pdevMgr);
        ~DeviceManagerThread();

        virtual int Run();

        virtual void OnPushNonEmpty_Locked()
        {
            TriggerCondition.Notify();
        }

        virtual void OnPopEmpty_Locked()     {}

        class Notifier
        {
                public:
                        // Called when timing ticks are updated. // Returns the largest number of microseconds
                        // this function can wait till next call.
                        virtual UInt64  OnTicks(UInt64 ticksMks)
                        { OVR_UNUSED1(ticksMks);  return Timer::MksPerSecond * 1000; }

                        virtual void    OnOverlappedEvent() { }
        };

        // Add notifier that will be called at regular intervals.
        bool AddTicksNotifier(Notifier* notify);
        bool RemoveTicksNotifier(Notifier* notify);

        bool AddOverlappedEvent(Notifier* notify, struct pollfd poll);
        bool RemoveOverlappedEvent(Notifier* notify);

        private:
        Array<Notifier*>    WaitNotifiers;
        Array<Notifier*>    TicksNotifiers;
        Array<pollfd>       PollFds;
        WaitCondition       TriggerCondition;
};

} }

#endif /* end of include guard: OVR_LINUX_DEVICEMANAGER_H */
