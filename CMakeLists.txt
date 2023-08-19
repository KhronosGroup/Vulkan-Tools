# ~~~
# Copyright (c) 2014-2023 Valve Corporation
# Copyright (c) 2014-2023 LunarG, Inc.
# Copyright (c) 2023-2023 RasterGrid Kft.
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

# This variable enables downstream users to customize the target API
# variant (e.g. Vulkan SC)
set(API_TYPE "vulkan")

add_subdirectory(scripts)

# User-interface declarations ----------------------------------------------------------------------------------------------------
# This section contains variables that affect development GUIs (e.g. CMake GUI and IDEs), such as option(), folders, and variables
# with the CACHE property.

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(BUILD_CUBE "Build cube" ON)
option(BUILD_VULKANINFO "Build vulkaninfo" ON)
option(BUILD_ICD "Build icd" ON)
# Installing the Mock ICD to system directories is probably not desired since this ICD is not a very complete implementation.
# Require the user to ask that it be installed if they really want it.
option(INSTALL_ICD "Install icd" OFF)

if (UNIX AND NOT APPLE) # i.e. Linux
    option(ENABLE_ADDRESS_SANITIZER "Use addres sanitization" OFF)
endif()

if(WIN32)
    # Optional: Allow specify the exact version used in the vulkaninfo executable
    # Format is major.minor.patch.build
    set(VULKANINFO_BUILD_DLL_VERSIONINFO "" CACHE STRING "Set the version to be used in the vulkaninfo.rc file. Default value is 1.0.1111.2222")
endif()

# Enable IDE GUI folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# "Helper" targets that don't have interesting source code should set their FOLDER property to this
set(TOOLS_HELPER_FOLDER "Helper Targets")

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

find_package(VulkanHeaders REQUIRED CONFIG)

# Find and create the Vulkan::Vulkan library
find_library(Vulkan_LIBRARY NAMES vulkan vulkan-1)
add_library(Vulkan::Vulkan UNKNOWN IMPORTED)
set_target_properties(Vulkan::Vulkan PROPERTIES
    IMPORTED_LOCATION ${Vulkan_LIBRARY})
target_link_libraries(Vulkan::Vulkan INTERFACE Vulkan::Headers)

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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS} -std=c++14 -fno-rtti")
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

# NOTE: Our custom code generation target isn't desirable for system package managers or add_subdirectory users.
# So this target needs to be off by default to avoid obtuse build errors or patches.
option(TOOLS_CODEGEN "Enable helper codegen target")
if (TOOLS_CODEGEN)
    find_package(Python3 REQUIRED QUIET)
    add_custom_target(tools_codegen
        COMMAND Python3::Interpreter ${PROJECT_SOURCE_DIR}/scripts/generate_source.py
            "${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry"
            --incremental --api ${API_TYPE}
    )
endif()

# Default to using the static CRT
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

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

add_subdirectory(windows-runtime-installer)

option(BUILD_TESTS "Build the tests")
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
