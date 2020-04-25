/*
** Copyright (c) 2015-2020 The Khronos Group Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef VULKAN_TOOLS_MOCK_ICD_H_
#define VULKAN_TOOLS_MOCK_ICD_H_

#include <mutex>

#include "vulkan/vk_icd.h"

#include "generated/mock_icd_extension_list.h"
#include "generated/mock_icd_commands.h"

namespace mock_icd {

// TODO(krOoze): It is not immediately obvious what stuff should be in header and what in source
//               , and whether there even should be a header
static constexpr uint32_t kIcdInterfaceVersion = 5;
static constexpr uint32_t kMinRequiredInterfaceVersion = 4;  // we need 4 because we expose physical-device-level extensions
static uint32_t loader_interface_version = 0;                // value to be discovered later

// TODO(krOoze): remove all this lock stuff
using mutex_t = std::mutex;
using lock_guard_t = std::lock_guard<mutex_t>;
using unique_lock_t = std::unique_lock<mutex_t>;
static mutex_t global_lock;
static uint64_t global_unique_handle = 1;

static void* CreateDispObjHandle() {
    auto handle = new VK_LOADER_DATA;
    set_loader_magic_value(handle);
    return handle;
}
static void DestroyDispObjHandle(void* handle) { delete reinterpret_cast<VK_LOADER_DATA*>(handle); }

}  // namespace mock_icd
#endif  // VULKAN_TOOLS_MOCK_ICD_H_