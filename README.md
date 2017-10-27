# Welcome to the ossim-csm-plugin

This plugin provides support for Community Sensor Model (CSM) utilization in OSSIM image processing and geolocation. You must have a version 3 distribution of the CSM API, including the shared library (libcsmapi.so) and a directory containing the CSM sensor model plugin libraries.

The CSM plugin code can make use of the Mensuration Services Program (MSP) API, version 1.5+ instead of the CSM API. MSP provides additional support for parsing different file formats, and includes the aforementioned CSM API. In general though, only the CSM package is available.

As with all OSSIM plugins, you will need to specify the plugin library in your ossim preferences configuration file. The shared library will be loaded at runtime by most ossim applications, and the plugin will register itself with OSSIM projection factory system.

## Build

You can reference the buildNative.sh script for the commands necessary to build. The build process invilves creating makefiles via the `cmake` command, then building the library with `make`. You'll need to download and configure your ossim repository first, as it contains all of the cmake modules needed, as well as the OSSIM library and include files.

The steps to build are:
1. Clone the ossim and ossim-csm-plugin repos. Make sure they sit in the same top-level "ossimlabs" directory.
2. Make sure you enable the build of the plugin by passing the variable `BUILD_CSM_PLUGIN="ON"` to the cmake script `ossim/cmake/scripts/ossim-cmake-config.sh`. 
3. Assuming cmake ran successfully, simply do a `make` in the build directory. Also `make install` may be desired. Refer to the build instructions in ossim/README.md.
4. The build should have produced libossim_csm_plugin.so (or dll) in the build's `lib` directory, or, if install was done, in your installation directory.
```

