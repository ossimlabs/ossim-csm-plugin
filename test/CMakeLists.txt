cmake_minimum_required (VERSION 2.8)

# Get the library suffix for lib or lib64.
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)       
if(LIB64)
   set(LIBSUFFIX 64)
else()
   set(LIBSUFFIX "")
endif()


set(requiredLibs ${requiredLibs} ossim ossim_csm3_plugin )
message( STATUS "Required libs       = ${requiredLibs}" )

# ---
# pdal-plugin-test app:
# ---

# Add the executable:
add_executable(csm3-plugin-test csm3-plugin-test.cpp )

# Set the output dir:
set_target_properties(csm3-plugin-test 
                      PROPERTIES 
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# pdal-plugin-test application links with required libs.
target_link_libraries( csm3-plugin-test ${requiredLibs} )

