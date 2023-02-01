# ~~~
# Copyright (c) 2014-2023 Valve Corporation
# Copyright (c) 2014-2023 LunarG, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ~~~
cmake_minimum_required(VERSION 3.17.2)

project(Vulkan-Tools)

# find_package(), include() and global project settings --------------------------------------------------------------------------

find_package(PythonInterp 3 QUIET)

# User-interface declarations ----------------------------------------------------------------------------------------------------
# This section contains variables that affect development GUIs (e.g. CMake GUI and IDEs), such as option(), folders, and variables
# with the CACHE property.

# Enable beta Vulkan extensions
add_definitions(-DVK_ENABLE_BETA_EXTENSIONS)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(BUILD_CUBE "Build cube" ON)
option(BUILD_VULKANINFO "Build vulkaninfo" ON)
option(BUILD_ICD "Build icd" ON)
# Installing the Mock ICD to system directories is probably not desired since this ICD is not a very complete implementation.
# Require the user to ask that it be installed if they really want it.
option(INSTALL_ICD "Install icd" OFF)

if(WIN32)
    # Optional: Allow specify the exact version used in the vulkaninfo executable
    # Format is major.minor.patch.build
    set(VULKANINFO_BUILD_DLL_VERSIONINFO "" CACHE STRING "Set the version to be used in the vulkaninfo.rc file. Default value is 1.0.1111.2222")
endif()

# Enable IDE GUI folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# "Helper" targets that don't have interesting source code should set their FOLDER property to this
set(TOOLS_HELPER_FOLDER "Helper Targets")

# ~~~
# Find Vulkan Headers and Loader
# Search order:
#  User-supplied CMAKE_PREFIX_PATH containing paths to the header and/or loader install dirs
#  CMake options VULKAN_HEADERS_INSTALL_DIR and/or VULKAN_LOADER_INSTALL_DIR
#  Env vars VULKAN_HEADERS_INSTALL_DIR and/or VULKAN_LOADER_INSTALL_DIR
#  If on MacOS
#   CMake option MOTLENVK_REPO_ROOT
#   Env vars MOLTENVK_REPO_ROOT
#  Fallback to FindVulkan operation using SDK install or system installed components.
# ~~~
set(VULKAN_HEADERS_INSTALL_DIR "HEADERS-NOTFOUND" CACHE PATH "Absolute path to a Vulkan-Headers install directory")
set(VULKAN_LOADER_INSTALL_DIR "LOADER-NOTFOUND" CACHE PATH "Absolute path to a Vulkan-Loader install directory")
if(WIN32 AND "${VULKAN_LOADER_INSTALL_DIR}" STREQUAL "LOADER-NOTFOUND")
    if(CMAKE_CL_64)
        set(VULKAN_LOADER_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/x64")
    else()
        set(VULKAN_LOADER_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/x86")
    endif()
endif()
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${VULKAN_HEADERS_INSTALL_DIR};${VULKAN_LOADER_INSTALL_DIR};
    $ENV{VULKAN_HEADERS_INSTALL_DIR};$ENV{VULKAN_LOADER_INSTALL_DIR})

if(APPLE)
    set(MOLTENVK_REPO_ROOT "MOLTENVK-NOTFOUND" CACHE PATH "Absolute path to a MoltenVK repo directory")
    if(NOT MOLTENVK_REPO_ROOT AND NOT DEFINED ENV{MOLTENVK_REPO_ROOT})
        message(FATAL_ERROR "Must define location of MoltenVK repo -- see BUILD.md")
    endif()

    if(NOT MOLTENVK_REPO_ROOT)
        set(MOLTENVK_REPO_ROOT $ENV{MOLTENVK_REPO_ROOT})
    endif()
    message(STATUS "Using MoltenVK repo location at ${MOLTENVK_REPO_ROOT}")
endif()
message(STATUS "Using find_package to locate Vulkan")
find_package(Vulkan)
find_package(VulkanHeaders REQUIRED CONFIG)
get_filename_component(Vulkan_LIBRARY_DIR ${Vulkan_LIBRARY} DIRECTORY)
message(STATUS "Vulkan FOUND = ${Vulkan_FOUND}")
message(STATUS "Vulkan Lib Dir = ${Vulkan_LIBRARY_DIR}")
message(STATUS "Vulkan Lib = ${Vulkan_LIBRARY}")

include(GNUInstallDirs)

option(BUILD_WERROR "Treat compiler warnings as errors" ON)
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(COMMON_COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
    set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -fno-strict-aliasing -fno-builtin-memcmp")

    if(MAKE_C_COMPILER_ID MATCHES "Clang")
        set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -Wno-sign-conversion -Wno-shorten-64-to-32 -Wno-string-conversion -Wno-implicit-in-conversion -Wno-enum-enum-conversion")
    endif()

    if(BUILD_WERROR)
        if((CMAKE_COMPILER_IS_GNUCXX AND NOT (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.3.0)) OR
          (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") AND (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6.0.0)))
          set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -Werror")
        endif()
    endif()

    # For GCC version 7.1 or greater, we need to disable the implicit fallthrough warning since there's no consistent way to satisfy
    # all compilers until they all accept the C++17 standard
    if(CMAKE_COMPILER_IS_GNUCC AND NOT (CMAKE_CXX_COMPILER_VERSION LESS 7.1))
        set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -Wimplicit-fallthrough=0")
    endif()

    if(APPLE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILE_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 ${COMMON_COMPILE_FLAGS}")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS} -std=c++11 -fno-rtti")
    if(UNIX)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    endif()
elseif(MSVC)
    if(BUILD_WERROR)
        add_compile_options("/WX")
    endif()
    add_compile_options("/W3")
    # Warn about nested declarations
    add_compile_options("/w34456")
    # Warn about potentially uninitialized variables
    add_compile_options("/w34701")
    add_compile_options("/w34703")
    # Warn about different indirection types.
    add_compile_options("/w34057")
    # Warn about signed/unsigned mismatch.
    add_compile_options("/w34245")
endif()

# Optional codegen target
if(PYTHONINTERP_FOUND)
    add_custom_target(VulkanTools_generated_source
                      COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/generate_source.py
                              ${VULKAN_HEADERS_REGISTRY_DIRECTORY} --incremental
                      )
else()
    message("WARNING: VulkanTools_generated_source target requires python 3")
endif()

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
