LibOVR
=================
This project add linux specific code for oculus rift's LibOVR.

Compilation
-----------
This project use the usual autoconf workflow for compilation and installation. Just run

    ./autogen.sh && ./configure && make

Installation
-----------

To install shared lib and headers to /usr/local/{lib,include}, just run

    sudo make install

The headers are installed to match the expected hierarchy in oculus sdk demos.

To use the shared library and pkgconfig file, you may need to update your environment variable to include /usr/local path :

    export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig/
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

Usage
-----

You need to add an udev rule in order to access the oculus hid as user.

    echo 'KERNEL=="hidraw*", ATTR{idVendor}=="2833", ATTRS{busnum}=="1", MODE="0666"' > /etc/udev/rules.d/99-hid.rules

To compile your project with libovr, use pkg-config on compilation and link

    `pkg-config --cflags libovr-0.2.2`
    `pkg-config --libs libovr-0.2.2`

You can use libovr as described in the oculus sdk documentation.

License
-------
This library falls under the Oculus Rift SDK License.

(C) Oculus VR, Inc. 2013. All Rights Reserved.
The latest version of the Oculus SDK is available at http://developer.oculusvr.com.
