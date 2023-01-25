# ~~~
# Copyright (c) 2018-2023 Valve Corporation
# Copyright (c) 2018-2023 LunarG, Inc.
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

# CMakeLists.txt file for building Vulkaninfo

if(WIN32)
    # ~~~
    # Setup the vulkaninfo.rc file to contain the correct info
    # Optionally uses the VULKANINFO_BUILD_DLL_VERSIONINFO build option to allow setting the exact build version
    # When VULKANINFO_BUILD_DLL_VERSIONINFO is not provided, "Dev Build" is added to the version strings
    # ~~~
    string(TIMESTAMP CURRENT_YEAR "%Y")
    set(VULKANINFO_CUR_COPYRIGHT_STR "${CURRENT_YEAR}")
    if ("$CACHE{VULKANINFO_BUILD_DLL_VERSIONINFO}" STREQUAL "")
        set(VULKANINFO_RC_VERSION "${VulkanHeaders_VERSION}")
        set(VULKANINFO_VER_FILE_VERSION_STR "\"${VULKANINFO_RC_VERSION}.Dev Build\"")
        set(VULKANINFO_VER_FILE_DESCRIPTION_STR "\"Vulkaninfo - Dev Build\"")
    else()
        set(VULKANINFO_RC_VERSION "$CACHE{VULKANINFO_BUILD_DLL_VERSIONINFO}")
        set(VULKANINFO_VER_FILE_VERSION_STR "\"${VULKANINFO_RC_VERSION}\"")
        set(VULKANINFO_VER_FILE_DESCRIPTION_STR "\"vulkaninfo\"")
    endif()

    # RC file wants the value of FILEVERSION to separated by commas
    string(REPLACE "." ", " VULKANINFO_VER_FILE_VERSION "${VULKANINFO_RC_VERSION}")

    # Configure the file to include the versioning info
    configure_file(vulkaninfo.rc.in ${CMAKE_CURRENT_BINARY_DIR}/vulkaninfo.rc)

    add_executable(vulkaninfo vulkaninfo.cpp ${CMAKE_CURRENT_BINARY_DIR}/vulkaninfo.rc)
    if (MINGW)
      target_link_libraries(vulkaninfo ucrt)
    endif ()
elseif(APPLE)
    add_executable(vulkaninfo
                   vulkaninfo.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo/metal_view.mm
                   ${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo/metal_view.h)
else()
    add_executable(vulkaninfo vulkaninfo.cpp)
endif()

target_include_directories(vulkaninfo PRIVATE ${CMAKE_SOURCE_DIR}/vulkaninfo)
target_include_directories(vulkaninfo PRIVATE ${CMAKE_SOURCE_DIR}/vulkaninfo/generated)

if (NOT WIN32)
    target_link_libraries(vulkaninfo ${CMAKE_DL_LIBS})
endif()

if(UNIX AND NOT APPLE) # i.e. Linux
    option(BUILD_WSI_XCB_SUPPORT "Build XCB WSI support" ON)
    option(BUILD_WSI_XLIB_SUPPORT "Build Xlib WSI support" ON)
    option(BUILD_WSI_WAYLAND_SUPPORT "Build Wayland WSI support" ON)
    option(BUILD_WSI_DIRECTFB_SUPPORT "Build DirectFB WSI support" OFF)

    find_package(PkgConfig REQUIRED QUIET) # Use PkgConfig to find Linux system libraries

    if(BUILD_WSI_XCB_SUPPORT)
        pkg_check_modules(XCB REQUIRED QUIET IMPORTED_TARGET xcb)
        target_link_libraries(vulkaninfo PkgConfig::XCB)
        target_compile_definitions(vulkaninfo PRIVATE -DVK_USE_PLATFORM_XCB_KHR -DVK_NO_PROTOTYPES)
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        pkg_check_modules(X11 REQUIRED QUIET IMPORTED_TARGET x11)
        target_link_libraries(vulkaninfo PkgConfig::X11)
        target_compile_definitions(vulkaninfo PRIVATE -DVK_USE_PLATFORM_XLIB_KHR -DVK_NO_PROTOTYPES)
    endif()

    if(BUILD_WSI_WAYLAND_SUPPORT)
        find_package(Wayland REQUIRED)
        target_include_directories(vulkaninfo PRIVATE ${WAYLAND_CLIENT_INCLUDE_DIR})
        target_link_libraries(vulkaninfo ${WAYLAND_CLIENT_LIBRARIES})
        target_compile_definitions(vulkaninfo PRIVATE -DVK_USE_PLATFORM_WAYLAND_KHR -DVK_NO_PROTOTYPES)
    endif()

    if(BUILD_WSI_DIRECTFB_SUPPORT)
        pkg_check_modules(DirectFB REQUIRED QUIET IMPORTED_TARGET directfb)
        target_link_libraries(vulkaninfo PkgConfig::DirectFB)
        target_compile_definitions(vulkaninfo PRIVATE -DVK_USE_PLATFORM_DIRECTFB_EXT -DVK_NO_PROTOTYPES)
    endif()
endif()

if(APPLE)
    # We do this so vulkaninfo is linked to an individual library and NOT a framework.
    target_link_libraries(vulkaninfo ${Vulkan_LIBRARY} "-framework AppKit -framework QuartzCore")
    target_include_directories(vulkaninfo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo)
endif()

target_link_libraries(vulkaninfo Vulkan::Headers)

if(WIN32)
    target_compile_definitions(vulkaninfo PUBLIC -DVK_USE_PLATFORM_WIN32_KHR -DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS -DVK_NO_PROTOTYPES)
    if(MSVC AND NOT MSVC_VERSION LESS 1900)
        # If MSVC, Enable control flow guard
        message(STATUS "Building vulkaninfo with control flow guard")
        add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/guard:cf>")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /guard:cf")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /guard:cf")
    endif()

    # Use static MSVCRT libraries
    foreach(configuration
            in
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${configuration} MATCHES "/MD")
            string(REGEX
                   REPLACE "/MD"
                           "/MT"
                           ${configuration}
                           "${${configuration}}")
        endif()
    endforeach()
elseif(APPLE)
    add_definitions(-DVK_USE_PLATFORM_MACOS_MVK -DVK_USE_PLATFORM_METAL_EXT)
endif()

if(APPLE)
    install(TARGETS vulkaninfo RUNTIME DESTINATION "vulkaninfo")
else()
    install(TARGETS vulkaninfo RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

