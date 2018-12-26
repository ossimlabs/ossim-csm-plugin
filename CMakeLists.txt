cmake_minimum_required(VERSION 2.8)

set(LIB_NAME ossim_csm_plugin)
MESSAGE( "************** LIBRARY SETUP FOR ossim_csm_plugin******************")

# Include ossim cmake stuff:
include(OssimVersion)
include(OssimCommonVariables)
include(OssimUtilities)

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} $ENV{OSSIM_CXX_FLAGS}" )

# Inject HP Fortify into C++ compilation if requested
IF ( "$ENV{BUILD_FORTIFY}" STREQUAL "true" )
   MESSAGE("Building with HP Fortify...")
   
   SET( ORIGINAL_COMPILER "${CMAKE_CXX_COMPILER}" )
   SET( CMAKE_CXX_COMPILER "/opt/HPE_Security/Fortify_SCA_and_Apps_17.20/bin/sourceanalyzer" )
   SET( CMAKE_CXX_FLAGS "-b ossimlabs ${ORIGINAL_COMPILER} ${CMAKE_CXX_FLAGS} " )
ENDIF()

MESSAGE( STATUS "\n========================== CMAKE FINAL SETTINGS ==========================" )
MESSAGE( STATUS "CMAKE_MODULE_PATH           = ${CMAKE_MODULE_PATH}")
MESSAGE( STATUS "CMAKE_IGNORE_PATH           = ${CMAKE_IGNORE_PATH}")
MESSAGE( STATUS "CMAKE_INCLUDE_PATH          = ${CMAKE_INCLUDE_PATH}")
MESSAGE( STATUS "CMAKE_LIBRARY_PATH          = ${CMAKE_LIBRARY_PATH}")
MESSAGE( STATUS "CMAKE_PREFIX_PATH           = ${CMAKE_PREFIX_PATH}")
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX        = ${CMAKE_INSTALL_PREFIX}")
MESSAGE( STATUS "CMAKE_FRAMEWORK_PATH        = ${CMAKE_FRAMEWORK_PATH}")
MESSAGE( STATUS "CMAKE_FIND_ROOT_PATH        = ${CMAKE_FIND_ROOT_PATH}")
MESSAGE( STATUS "CMAKE_FIND_LIBRARY_PREFIXES = ${CMAKE_FIND_LIBRARY_PREFIXES}")
MESSAGE( STATUS "CMAKE_FIND_LIBRARY_SUFFIXES = ${CMAKE_FIND_LIBRARY_SUFFIXES}")
MESSAGE( STATUS "CMAKE_SYSTEM_IGNORE_PATH    = ${CMAKE_SYSTEM_IGNORE_PATH}")
MESSAGE( STATUS "CMAKE_SYSTEM_INCLUDE_PATH   = ${CMAKE_SYSTEM_INCLUDE_PATH}")
MESSAGE( STATUS "CMAKE_SYSTEM_LIBRARY_PATH   = ${CMAKE_SYSTEM_LIBRARY_PATH}")
MESSAGE( STATUS "CMAKE_SYSTEM_PREFIX_PATH    = ${CMAKE_SYSTEM_PREFIX_PATH}")
MESSAGE( STATUS "CMAKE_SYSTEM_IGNORE_PATH    = ${CMAKE_SYSTEM_IGNORE_PATH}\n")

set(requiredLibs)

find_package(ossim)
if (NOT OSSIM_FOUND)
  message( FATAL_ERROR "OSSIM is required to build" )
endif(NOT OSSIM_FOUND)

include_directories(${OSSIM_INCLUDE_DIR})
set(requiredLibs ${OSSIM_LIBRARIES})

find_package(MSP)
if (MSP_FOUND)
   include_directories(${MSP_INCLUDE_DIRS})
   set( requiredLibs ${requiredLibs} ${MSP_LIBRARIES} )
   set( OSSIM_HAS_MSP 1 )
   message( "CSM plugin is using MSP." )
else(MSP_FOUND)
   message( "Could not find MSP package. Building CSM plugin without MSP support. Searching for CSM API")
   set( OSSIM_HAS_MSP 0 )
   find_package(CsmApi)
   if (CSMAPI_FOUND)
      message( "Found CSM API.")
      include_directories(${CSMAPI_INCLUDE_DIR})
      set( requiredLibs ${requiredLibs} ${CSMAPI_LIBRARY} )      
   else ()
      message (FATAL_ERROR "Could not find CSM API. Cannot build plugin.")
   endif()
endif(MSP_FOUND)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/ossimCsmConfig.h.in
               ${CMAKE_CURRENT_SOURCE_DIR}/src/ossimCsmConfig.h @ONLY)

add_subdirectory( src )

#IF(BUILD_OSSIM_TESTS)
#   add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test ${CMAKE_CURRENT_BINARY_DIR}/test)
#ENDIF()


