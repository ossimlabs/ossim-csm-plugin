cmake_minimum_required(VERSION 2.8)

set(LIB_NAME ossim_csm3_plugin)
MESSAGE( "************** LIBRARY SETUP FOR ossim_csm3_plugin******************")

# Include ossim cmake stuff:
include(OssimVersion)
include(OssimCommonVariables)
include(OssimUtilities)

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
   message( "CSM3 plugin is using MSP." )
else(MSP_FOUND)
   message( "Could not find MSP package. Building CSM3 plugin without MSP support. Searching for CSM API")
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

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/ossimCsm3Config.h.in
               ${CMAKE_CURRENT_SOURCE_DIR}/src/ossimCsm3Config.h @ONLY)

add_subdirectory( src )

#IF(BUILD_OSSIM_TESTS)
#   add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test ${CMAKE_CURRENT_BINARY_DIR}/test)
#ENDIF()


