# ~~~
# Copyright (c) 2018-2019 Valve Corporation
# Copyright (c) 2018-2019 LunarG, Inc.
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
    add_executable(vulkaninfo vulkaninfo.cpp vulkaninfo.rc)
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
    include(FindPkgConfig)
    option(BUILD_WSI_XCB_SUPPORT "Build XCB WSI support" ON)
    option(BUILD_WSI_XLIB_SUPPORT "Build Xlib WSI support" ON)
    option(BUILD_WSI_WAYLAND_SUPPORT "Build Wayland WSI support" ON)
    option(BUILD_WSI_DIRECTFB_SUPPORT "Build DirectFB WSI support" OFF)

    if(BUILD_WSI_XCB_SUPPORT)
        find_package(XCB REQUIRED)
        target_include_directories(vulkaninfo PRIVATE ${XCB_INCLUDE_DIRS})
        target_link_libraries(vulkaninfo ${XCB_LIBRARIES})
        target_compile_definitions(vulkaninfo PRIVATE -DVK_USE_PLATFORM_XCB_KHR -DVK_NO_PROTOTYPES)
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        find_package(X11 REQUIRED)
        target_include_directories(vulkaninfo PRIVATE ${X11_INCLUDE_DIR})
        target_link_libraries(vulkaninfo ${X11_LIBRARIES})
        target_compile_definitions(vulkaninfo PRIVATE -DVK_USE_PLATFORM_XLIB_KHR -DVK_NO_PROTOTYPES)
    endif()

    if(BUILD_WSI_WAYLAND_SUPPORT)
        find_package(Wayland REQUIRED)
        target_include_directories(vulkaninfo PRIVATE ${WAYLAND_CLIENT_INCLUDE_DIR})
        target_link_libraries(vulkaninfo ${WAYLAND_CLIENT_LIBRARIES})
        target_compile_definitions(vulkaninfo PRIVATE -DVK_USE_PLATFORM_WAYLAND_KHR -DVK_NO_PROTOTYPES)
    endif()

    if(BUILD_WSI_DIRECTFB_SUPPORT)
        find_package(DirectFB REQUIRED)
        target_include_directories(vulkaninfo PRIVATE ${DIRECTFB_INCLUDE_DIR})
        target_link_libraries(vulkaninfo ${DIRECTFB_LIBRARIES})
        target_compile_definitions(vulkaninfo PRIVATE -DVK_USE_PLATFORM_DIRECTFB_EXT -DVK_NO_PROTOTYPES)
    endif()
endif()

if(APPLE)
    # We do this so vulkaninfo is linked to an individual library and NOT a framework.
    target_link_libraries(vulkaninfo ${Vulkan_LIBRARY} "-framework AppKit -framework QuartzCore")
    target_include_directories(vulkaninfo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo ${VulkanHeaders_INCLUDE_DIR})
else()
    target_include_directories(vulkaninfo PRIVATE ${VulkanHeaders_INCLUDE_DIR})
endif()

# Create vulkaninfo application bundle for MacOS
if(APPLE)
    include(${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo.cmake)
endif()

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

