cmake_minimum_required(VERSION 2.8.11)

# This must come before the project command.
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")

project (Vulkan-Tools)
# set (CMAKE_VERBOSE_MAKEFILE 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find Vulkan Headers and Loader
# Search order:
#  User-supplied CMAKE_PREFIX_PATH containing paths to the header and/or loader install dirs
#  CMake options VULKAN_HEADERS_INSTALL_DIR and/or VULKAN_LOADER_INSTALL_DIR
#  Env vars VULKAN_HEADERS_INSTALL_DIR and/or VULKAN_LOADER_INSTALL_DIR
#  Fallback to FindVulkan operation using SDK install or system installed components.
set(VULKAN_HEADERS_INSTALL_DIR "HEADERS-NOTFOUND" CACHE PATH "Absolute path to a Vulkan-Headers install directory")
set(VULKAN_LOADER_INSTALL_DIR "LOADER-NOTFOUND" CACHE PATH "Absolute path to a Vulkan-Loader install directory")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${VULKAN_HEADERS_INSTALL_DIR};${VULKAN_LOADER_INSTALL_DIR};$ENV{VULKAN_HEADERS_INSTALL_DIR};$ENV{VULKAN_LOADER_INSTALL_DIR})
message(STATUS "Using find_package to locate Vulkan")
find_package(Vulkan)
# "Vulkan::Vulkan" on macOS causes the framework to be linked to the app instead of an individual library
set(LIBVK "Vulkan::Vulkan")
message(STATUS "Vulkan FOUND = ${Vulkan_FOUND}")
message(STATUS "Vulkan Include = ${Vulkan_INCLUDE_DIR}")
message(STATUS "Vulkan Lib = ${Vulkan_LIBRARY}")

include(GNUInstallDirs)
# Set a better default install location for Windows only if the user did not provide one.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND WIN32)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

if(APPLE)
    # CMake versions 3 or later need CMAKE_MACOSX_RPATH defined.
    # This avoids the CMP0042 policy message.
    set(CMAKE_MACOSX_RPATH 1)
    # The "install" target for MacOS fixes up bundles in place.
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
endif()

# Enable cmake folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(TOOLS_TARGET_FOLDER "Helper Targets")

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(COMMON_COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
    set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -fno-strict-aliasing -fno-builtin-memcmp")

    # For GCC version 7.1 or greater, we need to disable the implicit fallthrough warning since
    # there's no consistent way to satisfy all compilers until they all accept the C++17 standard
    if (CMAKE_COMPILER_IS_GNUCC AND NOT (CMAKE_CXX_COMPILER_VERSION LESS 7.1))
        set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -Wimplicit-fallthrough=0")
    endif()

    if (APPLE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILE_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 ${COMMON_COMPILE_FLAGS}")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS} -std=c++11 -fno-rtti")
    if (UNIX)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    endif()
endif()

if(NOT WIN32)
    set (BUILDTGT_DIR build)
    set (BINDATA_DIR Bin)
    set (LIBSOURCE_DIR Lib)
else()
    # For Windows, since 32-bit and 64-bit items can co-exist, we build each in its own build directory.
    # 32-bit target data goes in build32, and 64-bit target data goes into build.  So, include/link the
    # appropriate data at build time.
    if (DISABLE_BUILDTGT_DIR_DECORATION)
        set (BUILDTGT_DIR "")
        set (BINDATA_DIR "")
        set (LIBSOURCE_DIR "")
    elseif (CMAKE_CL_64)
        set (BUILDTGT_DIR build)
        set (BINDATA_DIR Bin)
        set (LIBSOURCE_DIR Lib)
    else()
        set (BUILDTGT_DIR build32)
        set (BINDATA_DIR Bin32)
        set (LIBSOURCE_DIR Lib32)
    endif()
    if(DISABLE_BUILD_PATH_DECORATION)
        set (DEBUG_DECORATION "")
        set (RELEASE_DECORATION "")
    else()
        set (DEBUG_DECORATION "Debug")
        set (RELEASE_DECORATION "Release")
    endif()

endif()

option(BUILD_CUBE "Build cube" ON)
option(BUILD_VULKANINFO "Build vulkaninfo" ON)
option(BUILD_ICD "Build icd" ON)
# Installing the Mock ICD to system directories is probably not desired since
# this ICD is not a very complete implementation.
# Require the user to ask that it be installed if they really want it.
option(INSTALL_ICD "Install icd" OFF)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall-Vulkan-Tools
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
set_target_properties(uninstall-Vulkan-Tools PROPERTIES FOLDER ${TOOLS_TARGET_FOLDER})

if(APPLE)
    include(mac_common.cmake)
endif()
if(BUILD_CUBE)
    add_subdirectory(cube)
endif()
if(BUILD_VULKANINFO)
    add_subdirectory(vulkaninfo)
endif()
if(BUILD_ICD)
    add_subdirectory(icd)
endif()
