ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = autogen.sh
AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libovr-@LIBOVR_API_VERSION@.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libovr-$(LIBOVR_API_VERSION).pc

maindir = $(includedir)/ovr-$(LIBOVR_API_VERSION)
nobase_main_HEADERS = \
	Include/OVR.h \
	Include/OVRVersion.h \
	Src/OVR_Linux_HIDDevice.h \
	Src/OVR_SensorImpl.h \
	Src/OVR_LatencyTestImpl.h \
	Src/OVR_Linux_DeviceManager.h \
	Src/OVR_DeviceConstants.h \
	Src/OVR_ThreadCommandQueue.h \
	Src/OVR_HIDDevice.h \
	Src/OVR_HIDDeviceBase.h \
	Src/OVR_SensorFilter.h \
	Src/OVR_DeviceMessages.h \
	Src/OVR_Device.h \
	Src/OVR_Linux_SensorDevice.h \
	Src/OVR_HIDDeviceImpl.h \
	Src/OVR_SensorFusion.h \
	Src/OVR_Linux_HMDDevice.h \
	Src/OVR_DeviceHandle.h \
	Src/OVR_DeviceImpl.h \
	Src/Util/Util_MagCalibration.h \
	Src/Util/Util_LatencyTest.h \
	Src/Util/Util_Render_Stereo.h \
	Src/Kernel/OVR_Allocator.h \
	Src/Kernel/OVR_Types.h \
	Src/Kernel/OVR_Threads.h \
	Src/Kernel/OVR_RefCount.h \
	Src/Kernel/OVR_List.h \
	Src/Kernel/OVR_Atomic.h \
	Src/Kernel/OVR_Hash.h \
	Src/Kernel/OVR_Array.h \
	Src/Kernel/OVR_Std.h \
	Src/Kernel/OVR_Alg.h \
	Src/Kernel/OVR_String.h \
	Src/Kernel/OVR_Color.h \
	Src/Kernel/OVR_KeyCodes.h \
	Src/Kernel/OVR_Math.h \
	Src/Kernel/OVR_ContainerAllocator.h \
	Src/Kernel/OVR_SysFile.h \
	Src/Kernel/OVR_File.h \
	Src/Kernel/OVR_StringHash.h \
	Src/Kernel/OVR_UTF8Util.h \
	Src/Kernel/OVR_Timer.h \
	Src/Kernel/OVR_System.h \
	Src/Kernel/OVR_Log.h

libovr_@LIBOVR_API_VERSION@_la_SOURCES = \
	Src/OVR_DeviceHandle.cpp \
	Src/OVR_Linux_HIDDevice.cpp \
	Src/OVR_SensorFusion.cpp \
	Src/OVR_Linux_SensorDevice.cpp \
	Src/OVR_SensorFilter.cpp \
	Src/OVR_SensorImpl.cpp \
	Src/OVR_Linux_DeviceManager.cpp \
	Src/OVR_Linux_HMDDevice.cpp \
	Src/Kernel/OVR_Alg.cpp \
	Src/Kernel/OVR_Timer.cpp \
	Src/Kernel/OVR_File.cpp \
	Src/Kernel/OVR_String_PathUtil.cpp \
	Src/Kernel/OVR_ThreadsPthread.cpp \
	Src/Kernel/OVR_String_FormatUtil.cpp \
	Src/Kernel/OVR_System.cpp \
	Src/Kernel/OVR_Allocator.cpp \
	Src/Kernel/OVR_Std.cpp \
	Src/Kernel/OVR_SysFile.cpp \
	Src/Kernel/OVR_FileFILE.cpp \
	Src/Kernel/OVR_String.cpp \
	Src/Kernel/OVR_Math.cpp \
	Src/Kernel/OVR_Atomic.cpp \
	Src/Kernel/OVR_UTF8Util.cpp \
	Src/Kernel/OVR_Log.cpp \
	Src/Kernel/OVR_RefCount.cpp \
	Src/OVR_DeviceImpl.cpp \
	Src/OVR_ThreadCommandQueue.cpp \
	Src/OVR_LatencyTestImpl.cpp \
	Src/Util/Util_MagCalibration.cpp \
	Src/Util/Util_Render_Stereo.cpp \
	Src/Util/Util_LatencyTest.cpp

libovr_@LIBOVR_API_VERSION@_la_LDFLAGS = -version-info $(LIBOVR_SO_VERSION)
