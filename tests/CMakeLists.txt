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

find_package(GTest REQUIRED CONFIG QUIET)

add_executable(vulkan_tools_tests)
target_sources(vulkan_tools_tests PRIVATE
    main.cpp
    icd/mock_icd_tests.cpp
)
get_target_property(TEST_SOURCES vulkan_tools_tests SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${TEST_SOURCES})

# make sure tests are built after all of the following are built
add_dependencies(vulkan_tools_tests vkcube vkcubepp VkICD_mock_icd vulkaninfo)

target_link_libraries(vulkan_tools_tests GTest::gtest)

include(GoogleTest)
gtest_discover_tests(vulkan_tools_tests DISCOVERY_TIMEOUT 100)
