# ~~~
# Copyright (c) 2023 Valve Corporation
# Copyright (c) 2023 LunarG, Inc.
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

# On Windows, We only need the .lib to build the repo, but to run tests we need the .dll.
# Thus, we have to fixup the Vulkan::Vulkan target to contain the correct details.
if (WIN32)
    list(APPEND CMAKE_PROGRAM_PATH ${VULKAN_LOADER_INSTALL_DIR})

    find_program(Vulkan_LIBRARY_DLL NAMES vulkan-1.dll)
    find_library(Vulkan_LIBRARY_IMPLIB NAMES vulkan-1)

    # Change Vulkan::Vulkan's location to vulkan-1.dll and set the IMPLIB to vulkan-1.lib
    set_target_properties(Vulkan::Vulkan PROPERTIES
        IMPORTED_LOCATION ${Vulkan_LIBRARY_DLL})
    set_target_properties(Vulkan::Vulkan PROPERTIES
        IMPORTED_IMPLIB ${Vulkan_LIBRARY_IMPLIB})
endif()

# setup binary_locations_$<CONFIG>.h.in using binary_locations.h.in as a source
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/binary_locations_$<CONFIG>.h" INPUT "${CMAKE_CURRENT_SOURCE_DIR}/binary_locations.h.in")

# copy binary_locations_$<CONFIG> to the build directory
add_custom_command(
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} "-E" "copy_if_different" "${CMAKE_CURRENT_BINARY_DIR}/binary_locations_$<CONFIG>.h" "${CMAKE_CURRENT_BINARY_DIR}/binary_locations.h"
    VERBATIM
    DEPENDS  "${CMAKE_CURRENT_BINARY_DIR}/binary_locations_$<CONFIG>.h"
    OUTPUT   "${CMAKE_CURRENT_BINARY_DIR}/binary_locations.h"
    COMMENT  "creating binary_locations.h file ({event: PRE_BUILD}, {filename: binary_locations.h })"
    )
add_custom_target (generate_binary_locations DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/binary_locations.h")
add_dependencies (generate_binary_locations vkcube vkcubepp VkICD_mock_icd vulkaninfo)

find_package(GTest REQUIRED CONFIG QUIET)

add_executable(vulkan_tools_tests)
target_sources(vulkan_tools_tests PRIVATE
    main.cpp
    test_common.h
    icd/mock_icd_tests.cpp
)
get_target_property(TEST_SOURCES vulkan_tools_tests SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${TEST_SOURCES})

target_include_directories(vulkan_tools_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(vulkan_tools_tests GTest::gtest Vulkan::Vulkan)
add_dependencies(vulkan_tools_tests generate_binary_locations)
if (WIN32)
    target_compile_definitions(vulkan_tools_tests PUBLIC -DVK_USE_PLATFORM_WIN32_KHR -DWIN32_LEAN_AND_MEAN -DNOMINMAX)
endif()
set_target_properties(vulkan_tools_tests PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

if (ENABLE_ADDRESS_SANITIZER)
    target_compile_options(vulkan_tools_tests PUBLIC -fsanitize=address)
    target_link_options(vulkan_tools_tests PUBLIC -fsanitize=address)
endif ()

if (WIN32)
    # Copy the loader shared lib (if built) to the test application directory so the test app finds it.
    add_custom_command(TARGET vulkan_tools_tests POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Vulkan::Vulkan> $<TARGET_FILE_DIR:vulkan_tools_tests>)
endif()

include(GoogleTest)
gtest_discover_tests(vulkan_tools_tests DISCOVERY_TIMEOUT 100)
