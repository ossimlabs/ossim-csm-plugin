# Welcome to the ossim-csm-plugin

You must already have access to the MSP API developmenet kit that has the headers and libraries to build to.

## Build

```
export OSSIM_INSTALL_PREFIX=<path to ossim installation>
export MSP_HOME=<path to Msp root ToolKit>

cmake  -DCMAKE_INSTALL_PREFIX=$OSSIM_INSTALL_PREFIX -DCMAKE_MODULE_PATH=$OSSIM_INSTALL_PREFIX/share/ossim/CMakeModules -DMSP_HOME=$MSP_HOME .. 
```

