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

string(TIMESTAMP CURRENT_YEAR "%Y")

# Keep windows-runtime-installer/VulkanRT-License.txt up to date.
# https://github.com/KhronosGroup/Vulkan-Tools/issues/754
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/VulkanRT-License.txt.in
    ${CMAKE_CURRENT_LIST_DIR}/VulkanRT-License.txt
    @ONLY
)
