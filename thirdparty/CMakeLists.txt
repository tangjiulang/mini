
# Disable shared library building for all thirdparty libraries
set( BUILD_SHARED_LIBS OFF )

# Note: The glew folder isn't added here because it is added inside the main CMakeLists.txt

# If this is not set, then we cannot set things like ARGPARSE_INSTALL to OFF before the
# argparse subdirectory is added.  Under the new policy, add_option will allow itself to
# be overridden by a previously-set variable like we want to do here.  Doing it this way
# instead of with cmake_policy will apply the change to the third-party CMakeLists files.
set( CMAKE_POLICY_DEFAULT_CMP0077 NEW )

# Avoid spurious warnings on g++
if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    set_source_files_properties(
        ${CMAKE_SOURCE_DIR}/thirdparty/3dxware_sdk/src/navlib_stub.c
        PROPERTIES COMPILE_FLAGS "-Wno-attributes"
    )
endif()

set( ARGPARSE_INSTALL OFF )
add_subdirectory( clipper2 )