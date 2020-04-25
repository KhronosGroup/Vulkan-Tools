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

using namespace mock_icd;

extern "C" {

#if defined(__GNUC__) && __GNUC__ >= 4
#define EXPORT __attribute__((visibility("default")))
#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590)
#define EXPORT __attribute__((visibility("default")))
#elif defined(_WIN32)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT VKAPI_ATTR VkResult VKAPI_CALL vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t* pSupportedVersion) {
    assert(pSupportedVersion && *pSupportedVersion >= loader_interface_version && *pSupportedVersion >= 2);
    loader_interface_version = *pSupportedVersion;

    if (loader_interface_version < kMinRequiredInterfaceVersion)
        return VK_ERROR_INCOMPATIBLE_DRIVER;
    else {
        const uint32_t negotiated_interface_version = (std::min)(loader_interface_version, kIcdInterfaceVersion);
        *pSupportedVersion = negotiated_interface_version;
        return VK_SUCCESS;
    }
}

EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vk_icdGetInstanceProcAddr(VkInstance instance, const char* pName) {
    loader_interface_version = (std::max)(loader_interface_version, (uint32_t)1);  // supports gipa, so infer it is at least 1
    return GetInstanceProcAddr(instance, pName);
}

// function must be provided, as we do expose physical-device-level extensions in the mock icd
EXPORT PFN_vkVoidFunction vk_icdGetPhysicalDeviceProcAddr(VkInstance instance, const char* pName) {
    assert(loader_interface_version >= 4);  // we should not get here if loader is not ver. 4, which calls negotiate
    // TODO: This function should only care about physical device functions and return nullptr for other functions
    return GetInstanceProcAddr(instance, pName);
}

// opt out of Loader version 0 by not exporting
// vkGetInstanceProcAddr, vkEnumerateInstanceExtensionProperties, and vkCreateInstance

// opt in to WSI handling by exporting all VkSurfaceKHR commands (loader version 3)
// TODO(krOoze): Needs some VK_USE_PLATFORM_* in cmake to work really everywhere
#include "generated/mock_icd_wsi_exports.cpp.inc"

}  // extern "C"