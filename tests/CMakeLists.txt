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
    icd/mock_icd_tests.cpp
)
get_target_property(TEST_SOURCES vulkan_tools_tests SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${TEST_SOURCES})

target_include_directories(vulkan_tools_tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(vulkan_tools_tests GTest::gtest Vulkan::Vulkan)
add_dependencies(vulkan_tools_tests generate_binary_locations)

if (ENABLE_ADDRESS_SANITIZER)
    target_compile_options(vulkan_tools_tests PUBLIC -fsanitize=address)
    target_link_options(vulkan_tools_tests PUBLIC -fsanitize=address)
endif ()

include(GoogleTest)
gtest_discover_tests(vulkan_tools_tests DISCOVERY_TIMEOUT 100)
