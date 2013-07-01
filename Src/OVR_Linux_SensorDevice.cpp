#include "OVR_Linux_SensorDevice.h"

#include "OVR_Linux_HMDDevice.h"
#include "OVR_SensorImpl.h"
#include "OVR_DeviceImpl.h"

namespace OVR { namespace Linux {

} // namespace Linux

void SensorDeviceImpl::EnumerateHMDFromSensorDisplayInfo
        (const SensorDisplayInfoImpl &displayInfo,
         DeviceFactory::EnumerateVisitor &visitor)
{
    OVR_UNUSED1(displayInfo);
    OVR_UNUSED1(visitor);
}

} // namespace OVR
