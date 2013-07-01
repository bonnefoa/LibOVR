#ifndef OVR_LINUX_HIDDEVICE_H
#define OVR_LINUX_HIDDEVICE_H

#include "OVR_Linux_DeviceManager.h"
#include <libudev.h>

namespace OVR { namespace Linux {

class HIDDeviceManager;

class HIDDevice : public OVR::HIDDevice, public DeviceManagerThread::Notifier
{
    private:
        friend class HIDDeviceManager;

    public:
        HIDDevice(HIDDeviceManager *manager)
            : HIDManager(manager) {};
        HIDDevice(HIDDeviceManager *manager, int deviceHandle)
            : HIDManager(manager), DeviceHandle(deviceHandle) {};

        ~HIDDevice() {};

        bool HIDInitialize(const String &path);
        void HIDShutdown();

        virtual bool SetFeatureReport(UByte *data, UInt32 length);
        virtual bool GetFeatureReport(UByte *data, UInt32 length);

        void OnOverlappedEvent();

        bool Write(UByte *data, UInt32 length);

        bool Read(UByte *pData, UInt32 length, UInt32 timeoutMilliS);
        bool ReadBlocking(UByte *pData, UInt32 length);

        UInt64 OnTicks(UInt64 ticksMks);

    private:
        HIDDeviceManager *HIDManager;
        int DeviceHandle;
        HIDDeviceDesc DevDesc;

        enum { ReadBufferSize = 96 };
        UByte               ReadBuffer[ReadBufferSize];

};

class HIDDeviceManager : public OVR::HIDDeviceManager
{
    friend class HIDDevice;

public:
    HIDDeviceManager(Linux::DeviceManager *manager)
        : DevManager(manager) {};
    ~HIDDeviceManager() {};

    virtual bool Initialize();
    virtual bool Shutdown();

    virtual bool Enumerate(HIDEnumerateVisitor *enumVisitor);
    virtual OVR::HIDDevice *Open(const String &path);

    static HIDDeviceManager *CreateInternal(DeviceManager *manager);

private:
    bool ReadUevent(struct udev_device *hidDev,
            char **hidName, int *busType,
            unsigned short *vendorId, unsigned short *productId);
    bool ReadHID(struct udev_device *hidDev,
            struct udev_device *rawDev,
            int *deviceHandle,
            HIDDeviceDesc *devDesc);
    bool ReadSysfsPath(struct udev *udev, const char *sysfsPath, int *deviceHandle, HIDDeviceDesc *devDesc);

private:
    DeviceManager* DevManager;
    struct udev *udev;

};

} // namespace Linux
} // namespace OVR

#endif /* end of include guard: OVR_LINUX_HIDDEVICE_H */
