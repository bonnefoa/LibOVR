#include "OVR_Linux_HIDDevice.h"

#include <linux/hidraw.h>
#include <locale.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <poll.h>

namespace OVR { namespace Linux {

bool HIDDevice::HIDInitialize(const String &path)
{
    DevDesc.Path = path;
    HIDManager->Initialize();
    bool res = HIDManager->ReadSysfsPath(HIDManager->udev, path, &DeviceHandle, &DevDesc);
    if(!res) {
        LogText("OVR::Linux::HIDDevice - Failed to open HIDDevice: %s\n", path.ToCStr());
        return false;
    }

    HIDManager->DevManager->pThread->AddTicksNotifier(this);

    struct pollfd poll;
    poll.fd = DeviceHandle;
    poll.events = POLLIN;
    poll.revents = 0;

    HIDManager->DevManager->pThread->AddOverlappedEvent(this, poll);

    LogText("OVR::Linux::HIDDevice - Opened '%s' (handle %i)\n"
            "                    Manufacturer:'%s'  Product:'%s'  Serial#:'%s'\n",
            path.ToCStr(), DeviceHandle,
            DevDesc.Manufacturer.ToCStr(), DevDesc.Product.ToCStr(),
            DevDesc.SerialNumber.ToCStr());
    return true;
}

void HIDDevice::HIDShutdown()
{
    if(DeviceHandle) {
        close(DeviceHandle);
    }
}

bool HIDDevice::SetFeatureReport(UByte *data, UInt32 length)
{
    int res = ioctl(DeviceHandle, HIDIOCSFEATURE(length), data);
    return res != 0;
}

bool HIDDevice::GetFeatureReport(UByte *data, UInt32 length)
{
    int res = ioctl(DeviceHandle, HIDIOCGFEATURE(length), data);
    return res != 0;
}

bool HIDDevice::Write(UByte *data, UInt32 length)
{
    int bytesWritten = write(DeviceHandle, data, length);
    return bytesWritten > 0;
}

void HIDDevice::OnOverlappedEvent()
{
    int bytesRead = read(DeviceHandle, &ReadBuffer, ReadBufferSize);
    if (bytesRead < 0 && (errno == EAGAIN || errno == EINPROGRESS)) {
        LogText("Got error on read\n");
    }
    Handler->OnInputReport(ReadBuffer, bytesRead);
}

UInt64 HIDDevice::OnTicks(UInt64 ticksMks)
{
    if (Handler)
    {
        return Handler->OnTicks(ticksMks);
    }
    return DeviceManagerThread::Notifier::OnTicks(ticksMks);
}

bool HIDDeviceManager::Initialize()
{
    const char *locale;
    locale = setlocale(LC_CTYPE, NULL);
    if (!locale)
        setlocale(LC_CTYPE, "");
    LogText("OVR::Linux::HIDDeviceManager - Initialized.\n");
    udev = udev_new();
    return true;
}

bool HIDDeviceManager::Shutdown()
{
    OVR_ASSERT_LOG(HIDManager, ("Should have called 'Initialize' before 'Shutdown'."));
    LogText("OVR::Linux::HIDDeviceManager - shutting down.\n");
    udev_unref(udev);
    return true;
}

bool HIDDeviceManager::ReadUevent(struct udev_device *hidDev, char **hidName, int *busType,
        unsigned short *vendorId, unsigned short *productId)
{
    const char *uevent = udev_device_get_sysattr_value(hidDev, "uevent");
    char *tmp = strdup(uevent);
    char *saveptr;
    char *line = strtok_r(tmp, "\n", &saveptr);

    while (line != NULL) {
        char *value = strchr(line, '=');
        if(value) {
            value++;
        }
        if(strncmp(line, "HID_ID", 6) == 0) {
            sscanf(value, "%x:%hx:%hx", busType, vendorId, productId);
        } else if (strncmp(line, "HID_NAME", 8) == 0) {
            *hidName = strdup(value);
        }
        line = strtok_r(NULL, "\n", &saveptr);
        if(*hidName && busType && vendorId && productId) {
            return true;
        }
    }
    return false;
}

bool HIDDeviceManager::ReadHID(struct udev_device *hidDev,
        struct udev_device *rawDev,
        int *deviceHandle,
        HIDDeviceDesc *devDesc)
{
    const char *devPath = udev_device_get_devnode(rawDev);

    char *hidName = NULL;
    int busType;
    unsigned short vendorId;
    unsigned short productId;
    ReadUevent(hidDev, &hidName, &busType, &vendorId, &productId);

    if(hidName == NULL || strcmp(hidName, "Oculus VR, Inc. Tracker DK") != 0) {
        return false;
    }

    *deviceHandle = open(devPath, O_RDONLY);
    if (*deviceHandle < 0) {
        LogText("Could not open system path %s\n", devPath);
        return false;
    }

    struct udev_device *usbDev = udev_device_get_parent_with_subsystem_devtype(
            rawDev, "usb", "usb_device");
    const char *manufacturerString = udev_device_get_sysattr_value(usbDev, "manufacturer");
    const char *productString = udev_device_get_sysattr_value(usbDev, "product");
    const char *serialString = udev_device_get_sysattr_value(usbDev, "serial");

    devDesc->VendorId = vendorId;
    devDesc->ProductId = productId;
    devDesc->Path = udev_device_get_syspath(rawDev);
    devDesc->Manufacturer = strdup(manufacturerString);
    devDesc->Product = productString;
    devDesc->SerialNumber = serialString;
    return true;
}

bool HIDDeviceManager::ReadSysfsPath(struct udev *udev, const char *sysfsPath, int *deviceHandle, HIDDeviceDesc *devDesc)
{
    bool res = false;
    struct udev_device *rawDev;
    rawDev = udev_device_new_from_syspath(udev, sysfsPath);
    struct udev_device *hidDev;
    hidDev = udev_device_get_parent_with_subsystem_devtype(rawDev, "hid", NULL);
    if (hidDev) {
        res = ReadHID(hidDev, rawDev, deviceHandle, devDesc);
    } else {
        LogText("HIDDeviceManager::ReadSysfsPath %s is not an hid device\n", sysfsPath);
    }
    udev_device_unref(rawDev);
    return res;
}

bool HIDDeviceManager::Enumerate(HIDEnumerateVisitor *enumVisitor)
{
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "hidraw");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *sysfsPath;
        sysfsPath = udev_list_entry_get_name(dev_list_entry);

        int deviceHandle;
        HIDDeviceDesc devDesc;

        bool res = ReadSysfsPath(udev, sysfsPath, &deviceHandle, &devDesc);
        if(res) {
            HIDDevice device(this, deviceHandle);
            enumVisitor->Visit(device, devDesc);
        }
    }
    udev_enumerate_unref(enumerate);
    return true;
}

OVR::HIDDevice *HIDDeviceManager::Open(const String &path)
{
    Ptr<Linux::HIDDevice> device = *new Linux::HIDDevice(this);
    if (!device->HIDInitialize(path))
    {
        return NULL;
    }

    device->AddRef();

    return device;
}

HIDDeviceManager *HIDDeviceManager::CreateInternal(Linux::DeviceManager *devManager)
{
    if(!System::IsInitialized())
    {
        OVR_DEBUG_STATEMENT(Log::GetDefaultLog()->
                LogMessage(Log_Debug,
                    "HIDDeviceManager::Create failed - OVR::System not initialized"); );
        return 0;
    }

    Ptr<Linux::HIDDeviceManager> manager = *new Linux::HIDDeviceManager(devManager);

    if(manager)
    {
        if(manager->Initialize())
        {
            manager->AddRef();
        }
        else
        {
            manager.Clear();
        }
    }
    return manager.GetPtr();
}

} // namespace Linux
} // namespace OVR
