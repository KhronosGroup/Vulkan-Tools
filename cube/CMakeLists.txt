# ~~~
# Copyright (c) 2018 Valve Corporation
# Copyright (c) 2018 LunarG, Inc.
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

if (CMAKE_SYSTEM_NAME MATCHES "Linux|BSD")
    option(BUILD_WSI_XCB_SUPPORT "Build XCB WSI support" ON)
    option(BUILD_WSI_XLIB_SUPPORT "Build Xlib WSI support" ON)
    option(BUILD_WSI_WAYLAND_SUPPORT "Build Wayland WSI support" ON)
    option(BUILD_WSI_DIRECTFB_SUPPORT "Build DirectFB WSI support" OFF)
    set(CUBE_WSI_SELECTION "XCB" CACHE STRING "Select WSI target for vkcube (XCB, XLIB, WAYLAND, DIRECTFB, DISPLAY)")

    find_package(PkgConfig REQUIRED QUIET) # Use PkgConfig to find Linux system libraries

    if(BUILD_WSI_XCB_SUPPORT)
        pkg_check_modules(XCB REQUIRED QUIET IMPORTED_TARGET xcb)
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        pkg_check_modules(X11 REQUIRED QUIET IMPORTED_TARGET x11)
    endif()

    if(BUILD_WSI_WAYLAND_SUPPORT)
        pkg_check_modules(WAYLAND_CLIENT REQUIRED IMPORTED_TARGET wayland-client)

        pkg_get_variable(WAYLAND_SCANNER_EXECUTABLE wayland-scanner wayland_scanner)
        message(STATUS "WAYLAND_SCANNER_EXECUTABLE = ${WAYLAND_SCANNER_EXECUTABLE}")

        pkg_get_variable(WAYLAND_PROTOCOLS_PATH wayland-protocols pkgdatadir)
        message(STATUS "WAYLAND_PROTOCOLS_PATH = ${WAYLAND_PROTOCOLS_PATH}")
        set(XDG_SHELL_PROTOCOL ${WAYLAND_PROTOCOLS_PATH}/stable/xdg-shell/xdg-shell.xml)
        add_custom_command(COMMENT "Generating xdg-shell protocol dispatch data"
                           OUTPUT xdg-shell-code.c
                           COMMAND ${WAYLAND_SCANNER_EXECUTABLE}
                                   private-code
                                   ${XDG_SHELL_PROTOCOL}
                                   ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-code.c
                           MAIN_DEPENDENCY ${XDG_SHELL_PROTOCOL}
                           DEPENDS ${XDG_SHELL_PROTOCOL} ${WAYLAND_SCANNER_EXECUTABLE})
        add_custom_command(COMMENT "Generating xdg-shell protocol header"
                           OUTPUT xdg-shell-client-header.h
                           COMMAND ${WAYLAND_SCANNER_EXECUTABLE}
                                   client-header
                                   ${XDG_SHELL_PROTOCOL}
                                   ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-client-header.h
                           MAIN_DEPENDENCY ${XDG_SHELL_PROTOCOL}
                           DEPENDS ${XDG_SHELL_PROTOCOL} ${WAYLAND_SCANNER_EXECUTABLE})

        set(XDG_DECORATION_PROTOCOL ${WAYLAND_PROTOCOLS_PATH}/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml)
        add_custom_command(COMMENT "Generating xdg-decoration protocol dispatch data"
                           OUTPUT xdg-decoration-code.c
                           COMMAND ${WAYLAND_SCANNER_EXECUTABLE}
                                   private-code
                                   ${XDG_DECORATION_PROTOCOL}
                                   ${CMAKE_CURRENT_BINARY_DIR}/xdg-decoration-code.c
                           MAIN_DEPENDENCY ${XDG_DECORATION_PROTOCOL}
                           DEPENDS ${XDG_DECORATION_PROTOCOL} ${WAYLAND_SCANNER_EXECUTABLE})
        add_custom_command(COMMENT "Generating xdg-decoration protocol header"
                           OUTPUT xdg-decoration-client-header.h
                           COMMAND ${WAYLAND_SCANNER_EXECUTABLE}
                                   client-header
                                   ${XDG_DECORATION_PROTOCOL}
                                   ${CMAKE_CURRENT_BINARY_DIR}/xdg-decoration-client-header.h
                           MAIN_DEPENDENCY ${XDG_DECORATION_PROTOCOL}
                           DEPENDS ${XDG_DECORATION_PROTOCOL} ${WAYLAND_SCANNER_EXECUTABLE})
    endif()

    if(BUILD_WSI_DIRECTFB_SUPPORT)
        pkg_check_modules(DirectFB REQUIRED QUIET IMPORTED_TARGET directfb)
    endif()
endif()

if(WIN32)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR -DWIN32_LEAN_AND_MEAN)
    if(MSVC AND NOT MSVC_VERSION LESS 1900)
        # If MSVC, Enable control flow guard
        message(STATUS "Building vkcube with control flow guard")
        add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/guard:cf>")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /guard:cf")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /guard:cf")
    endif()
elseif(ANDROID)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
elseif(APPLE)
    add_definitions(-DVK_USE_PLATFORM_METAL_EXT)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux|BSD")
    if(NOT CUBE_WSI_SELECTION)
        set(CUBE_WSI_SELECTION "XCB")
    endif()

    if(CUBE_WSI_SELECTION STREQUAL "XCB")
        if(NOT BUILD_WSI_XCB_SUPPORT)
            message(FATAL_ERROR "Selected XCB for vkcube build but not building Xcb support")
        endif()
        link_libraries(PkgConfig::XCB)
        set(CUBE_PLATFORM VK_USE_PLATFORM_XCB_KHR)
    elseif(CUBE_WSI_SELECTION STREQUAL "XLIB")
        if(NOT BUILD_WSI_XLIB_SUPPORT)
            message(FATAL_ERROR "Selected XLIB for vkcube build but not building Xlib support")
        endif()
        link_libraries(PkgConfig::X11)
        set(CUBE_PLATFORM VK_USE_PLATFORM_XLIB_KHR)
    elseif(CUBE_WSI_SELECTION STREQUAL "WAYLAND")
        if(NOT BUILD_WSI_WAYLAND_SUPPORT)
            message(FATAL_ERROR "Selected Wayland for vkcube build but not building Wayland support")
        endif()
        link_libraries(PkgConfig::WAYLAND_CLIENT)
        set(CUBE_PLATFORM VK_USE_PLATFORM_WAYLAND_KHR)
        set(XDG_SHELL_PROTOCOL ${WAYLAND_PROTOCOLS_PATH}/stable/xdg-shell/xdg-shell.xml)
        set(OPTIONAL_WAYLAND_DATA_FILES
            ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-code.c
            ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-client-header.h
            ${CMAKE_CURRENT_BINARY_DIR}/xdg-decoration-code.c
            ${CMAKE_CURRENT_BINARY_DIR}/xdg-decoration-client-header.h)
        include_directories(${CMAKE_CURRENT_BINARY_DIR})
    elseif(CUBE_WSI_SELECTION STREQUAL "DIRECTFB")
        if(NOT BUILD_WSI_DIRECTFB_SUPPORT)
            message(FATAL_ERROR "Selected DIRECTFB for vkcube build but not building DirectFB support")
        endif()
        link_libraries(PkgConfig::DirectFB)
        set(CUBE_PLATFORM VK_USE_PLATFORM_DIRECTFB_EXT)
    elseif(CUBE_WSI_SELECTION STREQUAL "DISPLAY")
        set(CUBE_PLATFORM VK_USE_PLATFORM_DISPLAY_KHR)
    else()
        message(FATAL_ERROR "Unrecognized value for CUBE_WSI_SELECTION: ${CUBE_WSI_SELECTION}")
    endif()

    link_libraries(${API_LOWERCASE} m)
else()
    message(FATAL_ERROR "Unsupported Platform!")
endif()

if (COMPILE_CUBE_SHADERS)
    # Try to find glslang in system paths or in an SDK if the VULKAN_SDK env-var is set
    find_program(GLSLANG_VALIDATOR names glslang glslangValidator HINTS $ENV{GLSLANG_INSTALL_DIR} $ENV{VULKAN_SDK}/bin $ENV{VULKAN_SDK}/Bin)

    add_custom_command(COMMENT "Compiling cube vertex shader"
                    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/cube.vert.inc
                    COMMAND ${GLSLANG_VALIDATOR} -V -x -o ${CMAKE_CURRENT_SOURCE_DIR}/cube.vert.inc
                            ${PROJECT_SOURCE_DIR}/cube/cube.vert
                    MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/cube/cube.vert
                    DEPENDS ${PROJECT_SOURCE_DIR}/cube/cube.vert ${GLSLANG_VALIDATOR})
    add_custom_command(COMMENT "Compiling cube fragment shader"
                    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/cube.frag.inc
                    COMMAND ${GLSLANG_VALIDATOR} -V -x -o ${CMAKE_CURRENT_SOURCE_DIR}/cube.frag.inc
                            ${PROJECT_SOURCE_DIR}/cube/cube.frag
                    MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/cube/cube.frag
                    DEPENDS ${PROJECT_SOURCE_DIR}/cube/cube.frag ${GLSLANG_VALIDATOR})
endif()

if(WIN32)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    # vkcube / vkcube make use of M_PI and various other math defines.
    add_compile_definitions(_USE_MATH_DEFINES)
endif()

# ----------------------------------------------------------------------------
# vkcube

if(APPLE)
    include(macOS/cube/cube.cmake)
elseif (ANDROID)
    add_library(vkcube MODULE)

    target_sources(vkcube PRIVATE cube.c)

    add_subdirectory(android)

    target_link_libraries(vkcube PRIVATE Vulkan::Headers)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux|BSD")
    add_executable(vkcube)
    target_sources(vkcube PRIVATE
        cube.c
        ${PROJECT_SOURCE_DIR}/cube/cube.vert
        ${PROJECT_SOURCE_DIR}/cube/cube.frag
        cube.vert.inc
        cube.frag.inc
        ${OPTIONAL_WAYLAND_DATA_FILES}
    )
    target_compile_definitions(vkcube PUBLIC ${CUBE_PLATFORM})
    include(CheckLibraryExists)
    CHECK_LIBRARY_EXISTS("rt" clock_gettime "" NEED_RT)
    if (NEED_RT)
        target_link_libraries(vkcube PRIVATE rt)
    endif()
    target_link_libraries(vkcube PRIVATE Vulkan::Headers Vulkan::Loader)
elseif(WIN32)
    add_executable(vkcube WIN32)
    target_sources(vkcube PRIVATE
        cube.c
        ${PROJECT_SOURCE_DIR}/cube/cube.vert
        ${PROJECT_SOURCE_DIR}/cube/cube.frag
        cube.vert.inc
        cube.frag.inc
    )
    target_link_libraries(vkcube PRIVATE Vulkan::Headers Vulkan::Loader)
else()
    message(FATAL_ERROR "Unsupported Platform!")
endif()

if (ENABLE_ADDRESS_SANITIZER)
    target_compile_options(vkcube PUBLIC -fsanitize=address)
    target_link_options(vkcube PUBLIC -fsanitize=address)
endif()

target_include_directories(vkcube PRIVATE .)

if (ANDROID)
    install(TARGETS vkcube DESTINATION ${CMAKE_INSTALL_LIBDIR})    
elseif(APPLE)
    # Keep RPATH so fixup_bundle can use it to find libraries
    set_target_properties(vkcube PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
    install(TARGETS vkcube BUNDLE DESTINATION "cube")
    # Fix up the library references to be self-contained within the bundle.
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\${CMAKE_INSTALL_PREFIX}/cube/vkcube.app \"\" \"${Vulkan_LIBRARY_DIR}\")
        ")
else()
    install(TARGETS vkcube)
endif()

if (ANDROID)
    return()
endif()

# ----------------------------------------------------------------------------
# vkcubepp

if(APPLE)
    include(macOS/cubepp/cubepp.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux|BSD")
    add_executable(vkcubepp
                   cube.cpp
                   ${PROJECT_SOURCE_DIR}/cube/cube.vert
                   ${PROJECT_SOURCE_DIR}/cube/cube.frag
                   cube.vert.inc
                   cube.frag.inc
                   ${OPTIONAL_WAYLAND_DATA_FILES})
    target_link_libraries(vkcubepp Vulkan::Headers Vulkan::Loader)
    target_compile_definitions(vkcubepp PUBLIC ${CUBE_PLATFORM})

    if (ENABLE_ADDRESS_SANITIZER)
        target_compile_options(vkcubepp PUBLIC -fsanitize=address)
        target_link_options(vkcubepp PUBLIC -fsanitize=address)
    endif ()
else()
    add_executable(vkcubepp
                   WIN32
                   cube.cpp
                   ${PROJECT_SOURCE_DIR}/cube/cube.vert
                   ${PROJECT_SOURCE_DIR}/cube/cube.frag
                   cube.vert.inc
                   cube.frag.inc)
    target_link_libraries(vkcubepp Vulkan::Headers Vulkan::Loader)
endif()
target_include_directories(vkcubepp PRIVATE .)

if(APPLE)
    # Keep RPATH so fixup_bundle can use it to find libraries
    set_target_properties(vkcubepp PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
    install(TARGETS vkcubepp BUNDLE DESTINATION "cube")
    # Fix up the library references to be self-contained within the bundle.
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\${CMAKE_INSTALL_PREFIX}/cube/vkcubepp.app \"\" \"${Vulkan_LIBRARY_DIR}\")
        ")
else()
    install(TARGETS vkcubepp)
endif()

# ----------------------------------------------------------------------------
# vkcube-wayland

if (CMAKE_SYSTEM_NAME MATCHES "Linux|BSD")
    if(BUILD_WSI_WAYLAND_SUPPORT AND EXISTS ${WAYLAND_PROTOCOLS_PATH}/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml)
        add_executable(vkcube-wayland)

        target_sources(vkcube-wayland PRIVATE
            cube.c
            ${PROJECT_SOURCE_DIR}/cube/cube.vert
            ${PROJECT_SOURCE_DIR}/cube/cube.frag
            cube.vert.inc
            cube.frag.inc
            ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-code.c
            ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-client-header.h
            ${CMAKE_CURRENT_BINARY_DIR}/xdg-decoration-code.c
            ${CMAKE_CURRENT_BINARY_DIR}/xdg-decoration-client-header.h
        )
        target_include_directories(vkcube-wayland PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}
            .
        )
        target_link_libraries(vkcube-wayland PRIVATE
            Vulkan::Headers
            Vulkan::Loader
            PkgConfig::WAYLAND_CLIENT
        )
        target_compile_definitions(vkcube-wayland PRIVATE VK_USE_PLATFORM_WAYLAND_KHR)
        include(CheckLibraryExists)
        CHECK_LIBRARY_EXISTS("rt" clock_gettime "" NEED_RT)
        if (NEED_RT)
            target_link_libraries(vkcube-wayland PRIVATE rt)
        endif()
        install(TARGETS vkcube-wayland)

        if (ENABLE_ADDRESS_SANITIZER)
            target_compile_options(vkcube-wayland PUBLIC -fsanitize=address)
            target_link_options(vkcube-wayland PUBLIC -fsanitize=address)
        endif ()
    endif()
endif()
