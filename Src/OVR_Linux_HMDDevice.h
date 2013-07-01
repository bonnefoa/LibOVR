#ifndef OVR_LINUXHMDDEVICE_H
#define OVR_LINUXHMDDEVICE_H

#include "OVR_Linux_DeviceManager.h"
#include <X11/extensions/Xrandr.h>

namespace OVR { namespace Linux {

class HMDDevice;

class HMDDeviceFactory : public DeviceFactory
{
        public:
                static HMDDeviceFactory Instance;

                virtual void EnumerateDevices(EnumerateVisitor &visitor);

        protected:
                DeviceManager *getManager() const { return (DeviceManager*) pManager; }
        private:
                void checkHMDDevice(Display *dpy, EnumerateVisitor &visitor);
                void processDisplayAtom(RROutput outputXID,
                                XRROutputInfo *outputInfo,
                                Atom atom, Display *dpy,
                                EnumerateVisitor &visitor);
};

class HMDDeviceCreateDesc : public DeviceCreateDesc
{
        friend class HMDDevice;

        protected:
        enum
        {
                Contents_Screen = 1,
                Contents_Distortion = 2,
                Contents_7Inch = 4,
        };
        String DeviceId;
        String DisplayDeviceName;
        int DesktopX, DesktopY;
        unsigned Contents;
        unsigned HResolution, VResolution;
        float HScreenSize, VScreenSize;
        float DistortionK[4];

        public:
        HMDDeviceCreateDesc(DeviceFactory *factory,
                        const String& deviceId, const String &displayDeviceName)
            : DeviceCreateDesc(factory, Device_HMD),
              DeviceId(deviceId), DisplayDeviceName(displayDeviceName),
              DesktopX(0), DesktopY(0), Contents(0),
              HResolution(0), VResolution(0), HScreenSize(0), VScreenSize(0) {};
        HMDDeviceCreateDesc(const HMDDeviceCreateDesc &other)
            : DeviceCreateDesc(other.pFactory, Device_HMD),
              DeviceId(other.DeviceId), DisplayDeviceName(other.DisplayDeviceName),
              DesktopX(other.DesktopX), DesktopY(other.DesktopY), Contents(other.Contents),
              HResolution(other.HResolution), VResolution(other.VResolution),
              HScreenSize(other.HScreenSize), VScreenSize(other.VScreenSize) {};

        virtual DeviceCreateDesc *Clone() const
        {
                return new HMDDeviceCreateDesc(*this);
        }

        virtual DeviceBase* NewDeviceInstance();

        virtual MatchResult MatchDevice(const DeviceCreateDesc &other,
                        DeviceCreateDesc**) const;

        virtual bool        MatchDevice(const String& path);

        virtual bool UpdateMatchedCandidate(const DeviceCreateDesc&, bool *newDeviceFlag = NULL);

        virtual bool GetDeviceInfo(DeviceInfo *info) const;

        void SetScreenParameters(int x, int y, unsigned hres, unsigned vres, float hsize, float vsize)
        {
                DesktopX = x;
                DesktopY = y;
                HResolution = hres;
                VResolution = vres;
                HScreenSize = hsize;
                VScreenSize = vsize;
                Contents |= Contents_Screen;
        }

        void SetDistortion(const float *dks)
        {
                for (int i = 0; i < 4; i++)
                        DistortionK[i] = dks[i];
                Contents |= Contents_Distortion;
        }

        void Set7Inch() { Contents |= Contents_7Inch; }

        bool Is7Inch() const;
};

class HMDDevice : public DeviceImpl<OVR::HMDDevice>
{
        public:
                HMDDevice(HMDDeviceCreateDesc *createDesc)
                    : OVR::DeviceImpl<OVR::HMDDevice>(createDesc, NULL) {};
                ~HMDDevice() {};

                virtual bool Initialize(DeviceBase *parent);
                virtual void Shutdown();

                virtual OVR::SensorDevice *GetSensor();
};

} // namespace Linux
} // namespace OVR


#endif /* end of include guard: OVR_LINUXHMDDEVICE_H */
