/*
 * Copyright (c) 2023 The Khronos Group Inc.
 * Copyright (c) 2023 Valve Corporation
 * Copyright (c) 2023 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdlib.h>

#include <iostream>
#include <vector>

#include "gtest/gtest.h"
#include "vulkan/vulkan.h"

// Location of the built binaries in this repo
#include "binary_locations.h"

#if defined(WIN32)
int set_environment_var(const char* name, const char* value) { return SetEnvironmentVariableA(name, cur_value); }
#else
int set_environment_var(const char* name, const char* value) { return setenv(name, value, 1); }
#endif

TEST(MockICD, Basic) {
    // Necessary to point the loader at the mock driver
    set_environment_var("VK_DRIVER_FILES", MOCK_ICD_JSON_MANIFEST_PATH);
    // Prevents layers from being loaded at all
    set_environment_var("VK_LOADER_LAYERS_DISABLE", "~all~");

    uint32_t count = 0;
    VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
    ASSERT_EQ(res, VK_SUCCESS);
    ASSERT_GT(count, 0);
    std::vector<VkExtensionProperties> inst_ext_props{count, VkExtensionProperties{}};
    res = vkEnumerateInstanceExtensionProperties(nullptr, &count, inst_ext_props.data());
    ASSERT_EQ(res, VK_SUCCESS);

    VkInstanceCreateInfo inst_create_info{};
    VkInstance inst{};
    res = vkCreateInstance(&inst_create_info, nullptr, &inst);
    ASSERT_EQ(res, VK_SUCCESS);

    res = vkEnumeratePhysicalDevices(inst, &count, nullptr);
    ASSERT_EQ(res, VK_SUCCESS);
    ASSERT_GT(count, 0);
    std::vector<VkPhysicalDevice> phys_devs{count};
    res = vkEnumeratePhysicalDevices(inst, &count, phys_devs.data());
    ASSERT_EQ(res, VK_SUCCESS);

    VkDeviceCreateInfo dev_create_info{};
    VkDevice device{};
    res = vkCreateDevice(phys_devs.at(0), &dev_create_info, nullptr, &device);
    ASSERT_EQ(res, VK_SUCCESS);
}
