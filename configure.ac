AC_INIT(libovr, 0.2.2, me@mail.com)
AC_CONFIG_SRCDIR(Src/OVR_Linux_DeviceManager.cpp)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 -Wall])
AM_PROG_AR

LT_INIT
AC_PROG_CXX

PKG_CHECK_MODULES([DEPS], [xrandr udev])

AC_SUBST([LIBOVR_SO_VERSION], [1:0:0])
AC_SUBST([LIBOVR_API_VERSION], [0.2.2])

AC_CONFIG_FILES([Makefile
                 libovr-${LIBOVR_API_VERSION}.pc:libovr.pc.in])
AC_OUTPUT
