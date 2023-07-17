# ~~~
# Copyright (c) 2014-2018 Valve Corporation
# Copyright (c) 2014-2018 LunarG, Inc.
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

# These variables enable downstream users to customize the CMake targets
# based on the target API variant (e.g. Vulkan SC)
set(MOCK_ICD_NAME VkICD_mock_icd)
set(GENERATED generated)

if(WIN32)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR -DVK_USE_PLATFORM_WIN32_KHX -DWIN32_LEAN_AND_MEAN)
elseif(ANDROID)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR -DVK_USE_PLATFORM_ANDROID_KHX)
elseif(APPLE)
    add_definitions(-DVK_USE_PLATFORM_MACOS_MVK)
elseif(UNIX AND NOT APPLE) # i.e. Linux
    if(BUILD_WSI_XCB_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_XCB_KHR -DVK_USE_PLATFORM_XCB_KHX)
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_XLIB_KHR -DVK_USE_PLATFORM_XLIB_KHX -DVK_USE_PLATFORM_XLIB_XRANDR_EXT)
    endif()

    if(BUILD_WSI_WAYLAND_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_WAYLAND_KHR -DVK_USE_PLATFORM_WAYLAND_KHX)
    endif()
else()
    message(FATAL_ERROR "Unsupported Platform!")
endif()

# Copy or link the JSON files to the binary directory for ease of use in the build tree.
if(WIN32)
    # extra setup for out-of-tree builds
    if(NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR))
        if(CMAKE_GENERATOR MATCHES "^Visual Studio.*")
            file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/windows/${MOCK_ICD_NAME}.json src_json)
            file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${MOCK_ICD_NAME}.json dst_json)
            add_custom_target(VkICD_mock_icd-json ALL COMMAND copy ${src_json} ${dst_json} VERBATIM)
            set_target_properties(VkICD_mock_icd-json PROPERTIES FOLDER ${TOOLS_HELPER_FOLDER})
        else()
            file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/windows/${MOCK_ICD_NAME}.json src_json)
            file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/${MOCK_ICD_NAME}.json dst_json)
            add_custom_target(VkICD_mock_icd-json ALL COMMAND copy ${src_json} ${dst_json} VERBATIM)
        endif()
    endif()
elseif(APPLE)
    # extra setup for out-of-tree builds
    if(NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR))
        if(CMAKE_GENERATOR MATCHES "^Xcode.*")
            add_custom_target(mk_icd_config_dir ALL
                              COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
            add_custom_target(VkICD_mock_icd-json ALL
                                DEPENDS mk_icd_config_dir
                                COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/macos/${MOCK_ICD_NAME}.json
                                        $<CONFIG> ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${MOCK_ICD_NAME}.json
                                VERBATIM)
        else()
            add_custom_target(VkICD_mock_icd-json ALL
                                COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/macos/${MOCK_ICD_NAME}.json
                                        ${MOCK_ICD_NAME}.json
                                VERBATIM)
        endif()
    endif()
else()
    # extra setup for out-of-tree builds
    if(NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR))
        add_custom_target(VkICD_mock_icd-json ALL
                            COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/linux/${MOCK_ICD_NAME}.json
                                    ${MOCK_ICD_NAME}.json
                            VERBATIM)
    endif()
endif()

# For ICD with a direct dependency on a project with the same name, use it.
if(NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR))
    add_dependencies(VkICD_mock_icd-json VkICD_mock_icd)
endif()
add_custom_target(generate_icd_files DEPENDS mock_icd.cpp ${GENERATED}/function_definitions.h ${GENERATED}/function_declarations.h)
set_target_properties(generate_icd_files PROPERTIES FOLDER ${TOOLS_HELPER_FOLDER})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${PROJECT_BINARY_DIR}
                    ${CMAKE_BINARY_DIR})
if(BUILD_WSI_WAYLAND_SUPPORT)
    include_directories(${WAYLAND_CLIENT_INCLUDE_DIR})
endif()

if(WIN32)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_CRT_SECURE_NO_WARNINGS /bigobj")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_CRT_SECURE_NO_WARNINGS /bigobj")
    # Turn off transitional "changed behavior" warning message for Visual Studio versions prior to 2015. The changed behavior is
    # that constructor initializers are now fixed to clear the struct members.
    add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,19>>:/wd4351>")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith -Wno-unused-function -Wno-sign-compare")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith -Wno-unused-function -Wno-sign-compare")
endif()


if(WIN32)
    file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${MOCK_ICD_NAME}.def DEF_FILE)
    add_custom_target(copy-mock_icd-def-file ALL
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DEF_FILE} ${MOCK_ICD_NAME}.def
                        VERBATIM)
    set_target_properties(copy-mock_icd-def-file PROPERTIES FOLDER ${TOOLS_HELPER_FOLDER})
    add_library(VkICD_mock_icd SHARED mock_icd.cpp ${MOCK_ICD_NAME}.def)
    target_link_libraries(VkICD_mock_icd PRIVATE Vulkan::Headers)
    if(INSTALL_ICD)
        install(TARGETS VkICD_mock_icd DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
elseif(APPLE)
    add_library(VkICD_mock_icd SHARED mock_icd.cpp )
    target_link_libraries(VkICD_mock_icd PRIVATE Vulkan::Headers)
    set_target_properties(VkICD_mock_icd PROPERTIES LINK_FLAGS "-Wl")
    if(INSTALL_ICD)
        install(TARGETS VkICD_mock_icd DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif()
else()
    add_library(VkICD_mock_icd SHARED mock_icd.cpp )
    target_link_libraries(VkICD_mock_icd PRIVATE Vulkan::Headers)
    set_target_properties(VkICD_mock_icd PROPERTIES LINK_FLAGS "-Wl,-export-dynamic,-Bsymbolic,--exclude-libs,ALL")
    if((UNIX AND NOT APPLE) AND INSTALL_ICD) # i.e. Linux
        install(TARGETS VkICD_mock_icd DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif()
    if (ENABLE_ADDRESS_SANITIZER)
        target_compile_options(VkICD_mock_icd PUBLIC -fsanitize=address)
        target_link_options(VkICD_mock_icd PUBLIC -fsanitize=address)
    endif ()
endif()

set_target_properties(VkICD_mock_icd PROPERTIES OUTPUT_NAME ${MOCK_ICD_NAME})

# JSON file(s) install targets. For Linux, need to remove the "./" from the library path before installing to system directories.
if((UNIX AND NOT APPLE) AND INSTALL_ICD) # i.e. Linux
    add_custom_target(VkICD_mock_icd-staging-json ALL
                        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/staging-json
                        COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/linux/${MOCK_ICD_NAME}.json ${CMAKE_CURRENT_BINARY_DIR}/staging-json
                        COMMAND sed -i -e "/\"library_path\":/s$./libVkICD$libVkICD$"
                                ${CMAKE_CURRENT_BINARY_DIR}/staging-json/${MOCK_ICD_NAME}.json
                        VERBATIM
                        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/linux/${MOCK_ICD_NAME}.json)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/staging-json/${MOCK_ICD_NAME}.json
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${API_TYPE}/icd.d)
endif()

# Windows uses the JSON file as-is.
if(WIN32 AND INSTALL_ICD)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/windows/${MOCK_ICD_NAME}.json DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
