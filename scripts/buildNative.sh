#!/bin/bash
#################################################################
#
# Script to build the ossim-isa library and ossim-isa-service 
# native application.
#
# The build environment must have the following variables defined 
# (in addition to other OSSIM requirements):
#
#     OSSIM_INSTALL_PREFIX or OSSIM_DEV_HOME
#     MSP_HOME or CSM_HOME
#
# OSSIM_DEV_HOME is used for cases where the build is being 
# done in a development environment.

#################################################################

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
pushd $SCRIPT_DIR/.. >/dev/null
export CMAKE_DIR=$PWD
popd >/dev/null

mkdir -p $CMAKE_DIR/build
pushd $CMAKE_DIR/build

echo "buildNative.sh: "
echo "  OSSIM_DEV_HOME       = $OSSIM_DEV_HOME"
echo "  OSSIM_INSTALL_PREFIX = $OSSIM_INSTALL_PREFIX"
echo "  MSP_HOME             = $MSP_HOME"
echo "  CSM_HOME             = $CSM_HOME"
echo "  CMAKE_DIR            = $CMAKE_DIR"

if [ "${BUILD_FORTIFY}" == "true" ] ; then
  INSERT_FORTIFY_TOOLCHAIN=-DCMAKE_TOOLCHAIN_FILE=${CMAKE_DIR}/fortify/fortify_linux_toolchain.cmake
  echo;echo "Building with fortify toolchain"; echo
fi

echo; echo "Generate makefiles."
cmake "$INSERT_FORTIFY_TOOLCHAIN" \
-DCMAKE_BUILD_TYPE="RelWithDebug" \
-DOSSIM_INSTALL_PREFIX=$OSSIM_INSTALL_PREFIX \
-DOSSIM_DEV_HOME=$OSSIM_DEV_HOME \
-DCSM_HOME=$CSM_HOME \
-DMSP_HOME=$MSP_HOME \
-DCMAKE_INSTALL_PREFIX=$CMAKE_DIR/install \
-DCMAKE_MODULE_PATH=$OSSIM_INSTALL_PREFIX/share/ossim/CMakeModules \
$CMAKE_DIR

if [ $? != 0 ]; then
  echo "Error encountered in cmake command."
  popd; exit 1
fi
  
echo; echo "Build/install native service locally."
make -j 8 install
if [ $? != 0 ]; then
  echo "Error encountered in build."
  popd; exit 1
fi
echo "Build successful!"; 
popd

echo

