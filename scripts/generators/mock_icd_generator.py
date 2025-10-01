#!/usr/bin/python3 -i
#
# Copyright (c) 2015-2025 The Khronos Group Inc.
# Copyright (c) 2015-2025 Valve Corporation
# Copyright (c) 2015-2025 LunarG, Inc.
# Copyright (c) 2015-2025 Google Inc.
# Copyright (c) 2023-2025 RasterGrid Kft.
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
#
# Author: Tobin Ehlis <tobine@google.com>
#
# This script generates a Mock ICD that intercepts almost all Vulkan
#  functions. That layer is not intended to be useful or even compilable
#  in its initial state. Rather it's intended to be a starting point that
#  can be copied and customized to assist in creation of a new layer.

from base_generator import BaseGenerator

CUSTOM_C_INTERCEPTS = {
'vkCreateInstance': '''
    // TODO: If loader ver <=4 ICD must fail with VK_ERROR_INCOMPATIBLE_DRIVER for all vkCreateInstance calls with
    //  apiVersion set to > Vulkan 1.0 because the loader is still at interface version <= 4. Otherwise, the
    //  ICD should behave as normal.
    if (loader_interface_version <= 4) {
        return VK_ERROR_INCOMPATIBLE_DRIVER;
    }
    *pInstance = (VkInstance)CreateDispObjHandle();
    for (auto& physical_device : physical_device_map[*pInstance])
        physical_device = (VkPhysicalDevice)CreateDispObjHandle();
    // TODO: If emulating specific device caps, will need to add intelligence here
    return VK_SUCCESS;
''',
'vkDestroyInstance': '''
    if (instance) {
        for (const auto physical_device : physical_device_map.at(instance)) {
            display_map.erase(physical_device);
            DestroyDispObjHandle((void*)physical_device);
        }
        physical_device_map.erase(instance);
        DestroyDispObjHandle((void*)instance);
    }
''',
'vkAllocateCommandBuffers': '''
    unique_lock_t lock(global_lock);
    for (uint32_t i = 0; i < pAllocateInfo->commandBufferCount; ++i) {
        pCommandBuffers[i] = (VkCommandBuffer)CreateDispObjHandle();
        command_pool_buffer_map[pAllocateInfo->commandPool].push_back(pCommandBuffers[i]);
    }
    return VK_SUCCESS;
''',
'vkFreeCommandBuffers': '''
    unique_lock_t lock(global_lock);
    for (auto i = 0u; i < commandBufferCount; ++i) {
        if (!pCommandBuffers[i]) {
            continue;
        }

        for (auto& pair : command_pool_buffer_map) {
            auto& cbs = pair.second;
            auto it = std::find(cbs.begin(), cbs.end(), pCommandBuffers[i]);
            if (it != cbs.end()) {
                cbs.erase(it);
            }
        }

        DestroyDispObjHandle((void*) pCommandBuffers[i]);
    }
''',
'vkCreateCommandPool': '''
    unique_lock_t lock(global_lock);
    *pCommandPool = (VkCommandPool)global_unique_handle++;
    command_pool_map[device].insert(*pCommandPool);
    return VK_SUCCESS;
''',
'vkDestroyCommandPool': '''
    // destroy command buffers for this pool
    unique_lock_t lock(global_lock);
    auto it = command_pool_buffer_map.find(commandPool);
    if (it != command_pool_buffer_map.end()) {
        for (auto& cb : it->second) {
            DestroyDispObjHandle((void*) cb);
        }
        command_pool_buffer_map.erase(it);
    }
    command_pool_map[device].erase(commandPool);
''',
'vkEnumeratePhysicalDevices': '''
    VkResult result_code = VK_SUCCESS;
    if (pPhysicalDevices) {
        const auto return_count = (std::min)(*pPhysicalDeviceCount, icd_physical_device_count);
        for (uint32_t i = 0; i < return_count; ++i) pPhysicalDevices[i] = physical_device_map.at(instance)[i];
        if (return_count < icd_physical_device_count) result_code = VK_INCOMPLETE;
        *pPhysicalDeviceCount = return_count;
    } else {
        *pPhysicalDeviceCount = icd_physical_device_count;
    }
    return result_code;
''',
'vkCreateDevice': '''
    *pDevice = (VkDevice)CreateDispObjHandle();
    // TODO: If emulating specific device caps, will need to add intelligence here
    return VK_SUCCESS;
''',
'vkDestroyDevice': '''
    unique_lock_t lock(global_lock);
    // First destroy sub-device objects
    // Destroy Queues
    for (auto queue_family_map_pair : queue_map[device]) {
        for (auto index_queue_pair : queue_map[device][queue_family_map_pair.first]) {
            DestroyDispObjHandle((void*)index_queue_pair.second);
        }
    }

    for (auto& cp : command_pool_map[device]) {
        for (auto& cb : command_pool_buffer_map[cp]) {
            DestroyDispObjHandle((void*) cb);
        }
        command_pool_buffer_map.erase(cp);
    }
    command_pool_map[device].clear();

    queue_map.erase(device);
    buffer_map.erase(device);
    image_memory_size_map.erase(device);
    // Now destroy device
    DestroyDispObjHandle((void*)device);
    // TODO: If emulating specific device caps, will need to add intelligence here
''',
'vkGetDeviceQueue': '''
    unique_lock_t lock(global_lock);
    auto queue = queue_map[device][queueFamilyIndex][queueIndex];
    if (queue) {
        *pQueue = queue;
    } else {
        *pQueue = queue_map[device][queueFamilyIndex][queueIndex] = (VkQueue)CreateDispObjHandle();
    }
    // TODO: If emulating specific device caps, will need to add intelligence here
    return;
''',
'vkGetDeviceQueue2': '''
    GetDeviceQueue(device, pQueueInfo->queueFamilyIndex, pQueueInfo->queueIndex, pQueue);
    // TODO: Add further support for GetDeviceQueue2 features
''',
'vkEnumerateInstanceLayerProperties': '''
    return VK_SUCCESS;
''',
'vkEnumerateInstanceVersion': '''
    *pApiVersion = VK_HEADER_VERSION_COMPLETE;
    return VK_SUCCESS;
''',
'vkEnumerateDeviceLayerProperties': '''
    return VK_SUCCESS;
''',
'vkEnumerateInstanceExtensionProperties': '''
    // If requesting number of extensions, return that
    if (!pLayerName) {
        if (!pProperties) {
            *pPropertyCount = (uint32_t)instance_extension_map.size();
        } else {
            uint32_t i = 0;
            for (const auto &name_ver_pair : instance_extension_map) {
                if (i == *pPropertyCount) {
                    break;
                }
                std::strncpy(pProperties[i].extensionName, name_ver_pair.first.c_str(), sizeof(pProperties[i].extensionName));
                pProperties[i].extensionName[sizeof(pProperties[i].extensionName) - 1] = 0;
                pProperties[i].specVersion = name_ver_pair.second;
                ++i;
            }
            if (i != instance_extension_map.size()) {
                return VK_INCOMPLETE;
            }
        }
    }
    // If requesting extension properties, fill in data struct for number of extensions
    return VK_SUCCESS;
''',
'vkEnumerateDeviceExtensionProperties': '''
    // If requesting number of extensions, return that
    if (!pLayerName) {
        if (!pProperties) {
            *pPropertyCount = (uint32_t)device_extension_map.size();
        } else {
            uint32_t i = 0;
            for (const auto &name_ver_pair : device_extension_map) {
                if (i == *pPropertyCount) {
                    break;
                }
                std::strncpy(pProperties[i].extensionName, name_ver_pair.first.c_str(), sizeof(pProperties[i].extensionName));
                pProperties[i].extensionName[sizeof(pProperties[i].extensionName) - 1] = 0;
                pProperties[i].specVersion = name_ver_pair.second;
                ++i;
            }
            *pPropertyCount = i;
            if (i != device_extension_map.size()) {
                return VK_INCOMPLETE;
            }
        }
    }
    // If requesting extension properties, fill in data struct for number of extensions
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceSurfacePresentModesKHR': '''
    // Currently always say that all present modes are supported
    if (!pPresentModes) {
        *pPresentModeCount = 6;
    } else {
        if (*pPresentModeCount >= 6) pPresentModes[5] = VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR;
        if (*pPresentModeCount >= 5) pPresentModes[4] = VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR;
        if (*pPresentModeCount >= 4) pPresentModes[3] = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
        if (*pPresentModeCount >= 3) pPresentModes[2] = VK_PRESENT_MODE_FIFO_KHR;
        if (*pPresentModeCount >= 2) pPresentModes[1] = VK_PRESENT_MODE_MAILBOX_KHR;
        if (*pPresentModeCount >= 1) pPresentModes[0] = VK_PRESENT_MODE_IMMEDIATE_KHR;
        *pPresentModeCount = *pPresentModeCount < 6 ? *pPresentModeCount : 6;
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceSurfaceFormatsKHR': '''
    // Currently always say that RGBA8 & BGRA8 are supported
    if (!pSurfaceFormats) {
        *pSurfaceFormatCount = 2;
    } else {
        if (*pSurfaceFormatCount >= 2) {
            pSurfaceFormats[1].format = VK_FORMAT_R8G8B8A8_UNORM;
            pSurfaceFormats[1].colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        }
        if (*pSurfaceFormatCount >= 1) {
            pSurfaceFormats[0].format = VK_FORMAT_B8G8R8A8_UNORM;
            pSurfaceFormats[0].colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        }
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceSurfaceFormats2KHR': '''
    // Currently always say that RGBA8 & BGRA8 are supported
    if (!pSurfaceFormats) {
        *pSurfaceFormatCount = 2;
    } else {
        if (*pSurfaceFormatCount >= 2) {
            pSurfaceFormats[1].pNext = nullptr;
            pSurfaceFormats[1].surfaceFormat.format = VK_FORMAT_R8G8B8A8_UNORM;
            pSurfaceFormats[1].surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        }
        if (*pSurfaceFormatCount >= 1) {
            pSurfaceFormats[1].pNext = nullptr;
            pSurfaceFormats[0].surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
            pSurfaceFormats[0].surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        }
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceSurfaceSupportKHR': '''
    // Currently say that all surface/queue combos are supported
    *pSupported = VK_TRUE;
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceSurfaceCapabilitiesKHR': '''
    // In general just say max supported is available for requested surface
    pSurfaceCapabilities->minImageCount = 1;
    pSurfaceCapabilities->maxImageCount = 0;
    pSurfaceCapabilities->currentExtent.width = 0xFFFFFFFF;
    pSurfaceCapabilities->currentExtent.height = 0xFFFFFFFF;
    pSurfaceCapabilities->minImageExtent.width = 1;
    pSurfaceCapabilities->minImageExtent.height = 1;
    pSurfaceCapabilities->maxImageExtent.width = 0xFFFF;
    pSurfaceCapabilities->maxImageExtent.height = 0xFFFF;
    pSurfaceCapabilities->maxImageArrayLayers = 128;
    pSurfaceCapabilities->supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR |
                                                VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR |
                                                VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR |
                                                VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR |
                                                VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR |
                                                VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR |
                                                VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR |
                                                VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR |
                                                VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR;
    pSurfaceCapabilities->currentTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    pSurfaceCapabilities->supportedCompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR |
                                                    VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR |
                                                    VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR |
                                                    VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    pSurfaceCapabilities->supportedUsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                                VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                VK_IMAGE_USAGE_SAMPLED_BIT |
                                                VK_IMAGE_USAGE_STORAGE_BIT |
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                                                VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceSurfaceCapabilities2KHR': '''
    GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, pSurfaceInfo->surface, &pSurfaceCapabilities->surfaceCapabilities);

    auto *present_mode_compatibility = lvl_find_mod_in_chain<VkSurfacePresentModeCompatibilityEXT>(pSurfaceCapabilities->pNext);
    if (present_mode_compatibility) {
        if (!present_mode_compatibility->pPresentModes) {
            present_mode_compatibility->presentModeCount = 3;
        } else {
            // arbitrary
            present_mode_compatibility->pPresentModes[0] = VK_PRESENT_MODE_IMMEDIATE_KHR;
            present_mode_compatibility->pPresentModes[1] = VK_PRESENT_MODE_FIFO_KHR;
            present_mode_compatibility->pPresentModes[2] = VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR;
        }
    }
    return VK_SUCCESS;
''',
'vkGetInstanceProcAddr': '''
    if (!negotiate_loader_icd_interface_called) {
        loader_interface_version = 0;
    }
    const auto &item = name_to_funcptr_map.find(pName);
    if (item != name_to_funcptr_map.end()) {
        return reinterpret_cast<PFN_vkVoidFunction>(item->second);
    }
    // Mock should intercept all functions so if we get here just return null
    return nullptr;
''',
'vkGetDeviceProcAddr': '''
    return GetInstanceProcAddr(nullptr, pName);
''',
'vkGetPhysicalDeviceMemoryProperties': '''
    pMemoryProperties->memoryTypeCount = 6;
    // Host visible Coherent
    pMemoryProperties->memoryTypes[0].propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    pMemoryProperties->memoryTypes[0].heapIndex = 0;
    // Host visible Cached
    pMemoryProperties->memoryTypes[1].propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    pMemoryProperties->memoryTypes[1].heapIndex = 0;
    // Device local and Host visible
    pMemoryProperties->memoryTypes[2].propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    pMemoryProperties->memoryTypes[2].heapIndex = 1;
    // Device local lazily
    pMemoryProperties->memoryTypes[3].propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
    pMemoryProperties->memoryTypes[3].heapIndex = 1;
    // Device local protected
    pMemoryProperties->memoryTypes[4].propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_PROTECTED_BIT;
    pMemoryProperties->memoryTypes[4].heapIndex = 1;
    // Device local only
    pMemoryProperties->memoryTypes[5].propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    pMemoryProperties->memoryTypes[5].heapIndex = 1;
    pMemoryProperties->memoryHeapCount = 2;
    pMemoryProperties->memoryHeaps[0].flags = VK_MEMORY_HEAP_MULTI_INSTANCE_BIT;
    pMemoryProperties->memoryHeaps[0].size = 8000000000;
    pMemoryProperties->memoryHeaps[1].flags = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;
    pMemoryProperties->memoryHeaps[1].size = 8000000000;
''',
'vkGetPhysicalDeviceMemoryProperties2KHR': '''
    GetPhysicalDeviceMemoryProperties(physicalDevice, &pMemoryProperties->memoryProperties);
''',
'vkGetPhysicalDeviceQueueFamilyProperties': '''
    if (pQueueFamilyProperties) {
        std::vector<VkQueueFamilyProperties2KHR> props2(*pQueueFamilyPropertyCount, {
            VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR});
        GetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, props2.data());
        for (uint32_t i = 0; i < *pQueueFamilyPropertyCount; ++i) {
            pQueueFamilyProperties[i] = props2[i].queueFamilyProperties;
        }
    } else {
        GetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, nullptr);
    }
''',
'vkGetPhysicalDeviceQueueFamilyProperties2KHR': '''
    if (pQueueFamilyProperties) {
        if (*pQueueFamilyPropertyCount >= 1) {
            auto props = &pQueueFamilyProperties[0].queueFamilyProperties;
            props->queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT
                                | VK_QUEUE_SPARSE_BINDING_BIT | VK_QUEUE_PROTECTED_BIT;
            props->queueCount = 1;
            props->timestampValidBits = 16;
            props->minImageTransferGranularity = {1,1,1};
        }
        if (*pQueueFamilyPropertyCount >= 2) {
            auto props = &pQueueFamilyProperties[1].queueFamilyProperties;
            props->queueFlags = VK_QUEUE_TRANSFER_BIT | VK_QUEUE_PROTECTED_BIT | VK_QUEUE_VIDEO_DECODE_BIT_KHR;
            props->queueCount = 1;
            props->timestampValidBits = 16;
            props->minImageTransferGranularity = {1,1,1};

            auto status_query_props = lvl_find_mod_in_chain<VkQueueFamilyQueryResultStatusPropertiesKHR>(pQueueFamilyProperties[1].pNext);
            if (status_query_props) {
                status_query_props->queryResultStatusSupport = VK_TRUE;
            }
            auto video_props = lvl_find_mod_in_chain<VkQueueFamilyVideoPropertiesKHR>(pQueueFamilyProperties[1].pNext);
            if (video_props) {
                video_props->videoCodecOperations = VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR
                                                  | VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR
                                                  | VK_VIDEO_CODEC_OPERATION_DECODE_AV1_BIT_KHR;
            }
        }
        if (*pQueueFamilyPropertyCount >= 3) {
            auto props = &pQueueFamilyProperties[2].queueFamilyProperties;
            props->queueFlags = VK_QUEUE_TRANSFER_BIT | VK_QUEUE_PROTECTED_BIT | VK_QUEUE_VIDEO_ENCODE_BIT_KHR;
            props->queueCount = 1;
            props->timestampValidBits = 16;
            props->minImageTransferGranularity = {1,1,1};

            auto status_query_props = lvl_find_mod_in_chain<VkQueueFamilyQueryResultStatusPropertiesKHR>(pQueueFamilyProperties[2].pNext);
            if (status_query_props) {
                status_query_props->queryResultStatusSupport = VK_TRUE;
            }
            auto video_props = lvl_find_mod_in_chain<VkQueueFamilyVideoPropertiesKHR>(pQueueFamilyProperties[2].pNext);
            if (video_props) {
                video_props->videoCodecOperations = VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR
                                                  | VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR
                                                  | VK_VIDEO_CODEC_OPERATION_ENCODE_AV1_BIT_KHR;
            }
        }
        if (*pQueueFamilyPropertyCount > 3) {
            *pQueueFamilyPropertyCount = 3;
        }
    } else {
        *pQueueFamilyPropertyCount = 3;
    }
''',
'vkGetPhysicalDeviceFeatures': '''
    uint32_t num_bools = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);
    VkBool32 *bool_array = &pFeatures->robustBufferAccess;
    SetBoolArrayTrue(bool_array, num_bools);
''',
'vkGetPhysicalDeviceFeatures2KHR': '''
    GetPhysicalDeviceFeatures(physicalDevice, &pFeatures->features);
    uint32_t num_bools = 0; // Count number of VkBool32s in extension structs
    VkBool32* feat_bools = nullptr;
    auto vk_1_1_features = lvl_find_mod_in_chain<VkPhysicalDeviceVulkan11Features>(pFeatures->pNext);
    if (vk_1_1_features) {
        vk_1_1_features->protectedMemory = VK_TRUE;
    }
    auto vk_1_3_features = lvl_find_mod_in_chain<VkPhysicalDeviceVulkan13Features>(pFeatures->pNext);
    if (vk_1_3_features) {
        vk_1_3_features->synchronization2 = VK_TRUE;
    }
    auto prot_features = lvl_find_mod_in_chain<VkPhysicalDeviceProtectedMemoryFeatures>(pFeatures->pNext);
    if (prot_features) {
        prot_features->protectedMemory = VK_TRUE;
    }
    auto sync2_features = lvl_find_mod_in_chain<VkPhysicalDeviceSynchronization2FeaturesKHR>(pFeatures->pNext);
    if (sync2_features) {
        sync2_features->synchronization2 = VK_TRUE;
    }
    auto video_maintenance1_features = lvl_find_mod_in_chain<VkPhysicalDeviceVideoMaintenance1FeaturesKHR>(pFeatures->pNext);
    if (video_maintenance1_features) {
        video_maintenance1_features->videoMaintenance1 = VK_TRUE;
    }
    const auto *desc_idx_features = lvl_find_in_chain<VkPhysicalDeviceDescriptorIndexingFeaturesEXT>(pFeatures->pNext);
    if (desc_idx_features) {
        const auto bool_size = sizeof(VkPhysicalDeviceDescriptorIndexingFeaturesEXT) - offsetof(VkPhysicalDeviceDescriptorIndexingFeaturesEXT, shaderInputAttachmentArrayDynamicIndexing);
        num_bools = bool_size/sizeof(VkBool32);
        feat_bools = (VkBool32*)&desc_idx_features->shaderInputAttachmentArrayDynamicIndexing;
        SetBoolArrayTrue(feat_bools, num_bools);
    }
    const auto *blendop_features = lvl_find_in_chain<VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT>(pFeatures->pNext);
    if (blendop_features) {
        const auto bool_size = sizeof(VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT) - offsetof(VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT, advancedBlendCoherentOperations);
        num_bools = bool_size/sizeof(VkBool32);
        feat_bools = (VkBool32*)&blendop_features->advancedBlendCoherentOperations;
        SetBoolArrayTrue(feat_bools, num_bools);
    }
    const auto *host_image_copy_features = lvl_find_in_chain<VkPhysicalDeviceHostImageCopyFeaturesEXT>(pFeatures->pNext);
    if (host_image_copy_features) {
       feat_bools = (VkBool32*)&host_image_copy_features->hostImageCopy;
       SetBoolArrayTrue(feat_bools, 1);
    }
''',
'vkGetPhysicalDeviceFormatProperties': '''
    if (VK_FORMAT_UNDEFINED == format) {
        *pFormatProperties = { 0x0, 0x0, 0x0 };
    } else {
        // Default to a color format, skip DS bit
        *pFormatProperties = { 0x00FFFDFF, 0x00FFFDFF, 0x00FFFDFF };
        switch (format) {
            case VK_FORMAT_D16_UNORM:
            case VK_FORMAT_X8_D24_UNORM_PACK32:
            case VK_FORMAT_D32_SFLOAT:
            case VK_FORMAT_S8_UINT:
            case VK_FORMAT_D16_UNORM_S8_UINT:
            case VK_FORMAT_D24_UNORM_S8_UINT:
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                // Don't set color bits for DS formats
                *pFormatProperties = { 0x00FFFE7F, 0x00FFFE7F, 0x00FFFE7F };
                break;
            case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
            case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
            case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
            case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
            case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM:
                // Set decode/encode bits for these formats
                *pFormatProperties = { 0x1EFFFDFF, 0x1EFFFDFF, 0x00FFFDFF };
                break;
            default:
                break;
        }
    }
''',
'vkGetPhysicalDeviceFormatProperties2KHR': '''
    GetPhysicalDeviceFormatProperties(physicalDevice, format, &pFormatProperties->formatProperties);
    VkFormatProperties3KHR *props_3 = lvl_find_mod_in_chain<VkFormatProperties3KHR>(pFormatProperties->pNext);
    if (props_3) {
        props_3->linearTilingFeatures = pFormatProperties->formatProperties.linearTilingFeatures;
        props_3->optimalTilingFeatures = pFormatProperties->formatProperties.optimalTilingFeatures;
        props_3->bufferFeatures = pFormatProperties->formatProperties.bufferFeatures;
        props_3->optimalTilingFeatures |= VK_FORMAT_FEATURE_2_HOST_IMAGE_TRANSFER_BIT_EXT;
    }
''',
'vkGetPhysicalDeviceImageFormatProperties': '''
    // A hardcoded unsupported format
    if (format == VK_FORMAT_E5B9G9R9_UFLOAT_PACK32) {
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    // TODO: Just hard-coding some values for now
    // TODO: If tiling is linear, limit the mips, levels, & sample count
    if (VK_IMAGE_TILING_LINEAR == tiling) {
        *pImageFormatProperties = { { 4096, 4096, 256 }, 1, 1, VK_SAMPLE_COUNT_1_BIT, 4294967296 };
    } else {
        // We hard-code support for all sample counts except 64 bits.
        *pImageFormatProperties = { { 4096, 4096, 256 }, 12, 256, 0x7F & ~VK_SAMPLE_COUNT_64_BIT, 4294967296 };
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceImageFormatProperties2KHR': '''
    auto *external_image_prop = lvl_find_mod_in_chain<VkExternalImageFormatProperties>(pImageFormatProperties->pNext);
    auto *external_image_format = lvl_find_in_chain<VkPhysicalDeviceExternalImageFormatInfo>(pImageFormatInfo->pNext);
    if (external_image_prop && external_image_format) {
        external_image_prop->externalMemoryProperties.externalMemoryFeatures = VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT | VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT;
        external_image_prop->externalMemoryProperties.compatibleHandleTypes = external_image_format->handleType;
    }

    GetPhysicalDeviceImageFormatProperties(physicalDevice, pImageFormatInfo->format, pImageFormatInfo->type, pImageFormatInfo->tiling, pImageFormatInfo->usage, pImageFormatInfo->flags, &pImageFormatProperties->imageFormatProperties);
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceSparseImageFormatProperties': '''
    if (!pProperties) {
        *pPropertyCount = 1;
    } else {
        // arbitrary
        pProperties->imageGranularity = {4, 4, 4};
        pProperties->flags = VK_SPARSE_IMAGE_FORMAT_SINGLE_MIPTAIL_BIT;
        switch (format) {
            case VK_FORMAT_D16_UNORM:
            case VK_FORMAT_D32_SFLOAT:
                pProperties->aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                break;
            case VK_FORMAT_S8_UINT:
                pProperties->aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
                break;
            case VK_FORMAT_X8_D24_UNORM_PACK32:
            case VK_FORMAT_D16_UNORM_S8_UINT:
            case VK_FORMAT_D24_UNORM_S8_UINT:
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                pProperties->aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                break;
            default:
                pProperties->aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                break;
        }
    }
''',
'vkGetPhysicalDeviceSparseImageFormatProperties2KHR': '''
    if (pPropertyCount && pProperties) {
        GetPhysicalDeviceSparseImageFormatProperties(physicalDevice, pFormatInfo->format, pFormatInfo->type, pFormatInfo->samples, pFormatInfo->usage, pFormatInfo->tiling, pPropertyCount, &pProperties->properties);
    } else {
        GetPhysicalDeviceSparseImageFormatProperties(physicalDevice, pFormatInfo->format, pFormatInfo->type, pFormatInfo->samples, pFormatInfo->usage, pFormatInfo->tiling, pPropertyCount, nullptr);
    }
''',
'vkGetPhysicalDeviceProperties': '''
    pProperties->apiVersion = VK_HEADER_VERSION_COMPLETE;
    pProperties->driverVersion = 1;
    pProperties->vendorID = 0xba5eba11;
    pProperties->deviceID = 0xf005ba11;
    pProperties->deviceType = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
    //std::string devName = "Vulkan Mock Device";
    strcpy(pProperties->deviceName, "Vulkan Mock Device");
    pProperties->pipelineCacheUUID[0] = 18;
    pProperties->limits = SetLimits(&pProperties->limits);
    pProperties->sparseProperties = { VK_TRUE, VK_TRUE, VK_TRUE, VK_TRUE, VK_TRUE };
''',
'vkGetPhysicalDeviceProperties2KHR': '''
    // The only value that need to be set are those the Profile layer can't set
    // see https://github.com/KhronosGroup/Vulkan-Profiles/issues/352
    // All values set are arbitrary
    GetPhysicalDeviceProperties(physicalDevice, &pProperties->properties);

    auto *props_11 = lvl_find_mod_in_chain<VkPhysicalDeviceVulkan11Properties>(pProperties->pNext);
    if (props_11) {
        props_11->protectedNoFault = VK_FALSE;
    }

    auto *props_12 = lvl_find_mod_in_chain<VkPhysicalDeviceVulkan12Properties>(pProperties->pNext);
    if (props_12) {
        props_12->denormBehaviorIndependence = VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL;
        props_12->roundingModeIndependence = VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL;
    }

    auto *props_13 = lvl_find_mod_in_chain<VkPhysicalDeviceVulkan13Properties>(pProperties->pNext);
    if (props_13) {
        props_13->storageTexelBufferOffsetSingleTexelAlignment = VK_TRUE;
        props_13->uniformTexelBufferOffsetSingleTexelAlignment = VK_TRUE;
        props_13->storageTexelBufferOffsetAlignmentBytes = 16;
        props_13->uniformTexelBufferOffsetAlignmentBytes = 16;
    }

    auto *protected_memory_props = lvl_find_mod_in_chain<VkPhysicalDeviceProtectedMemoryProperties>(pProperties->pNext);
    if (protected_memory_props) {
        protected_memory_props->protectedNoFault = VK_FALSE;
    }

    auto *float_controls_props = lvl_find_mod_in_chain<VkPhysicalDeviceFloatControlsProperties>(pProperties->pNext);
    if (float_controls_props) {
        float_controls_props->denormBehaviorIndependence = VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL;
        float_controls_props->roundingModeIndependence = VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL;
    }

    auto *conservative_raster_props = lvl_find_mod_in_chain<VkPhysicalDeviceConservativeRasterizationPropertiesEXT>(pProperties->pNext);
    if (conservative_raster_props) {
        conservative_raster_props->primitiveOverestimationSize = 0.00195313f;
        conservative_raster_props->conservativePointAndLineRasterization = VK_TRUE;
        conservative_raster_props->degenerateTrianglesRasterized = VK_TRUE;
        conservative_raster_props->degenerateLinesRasterized = VK_TRUE;
    }

    auto *rt_pipeline_props = lvl_find_mod_in_chain<VkPhysicalDeviceRayTracingPipelinePropertiesKHR>(pProperties->pNext);
    if (rt_pipeline_props) {
        rt_pipeline_props->shaderGroupHandleSize = 32;
        rt_pipeline_props->shaderGroupBaseAlignment = 64;
        rt_pipeline_props->shaderGroupHandleCaptureReplaySize = 32;
    }

    auto *rt_pipeline_nv_props = lvl_find_mod_in_chain<VkPhysicalDeviceRayTracingPropertiesNV>(pProperties->pNext);
    if (rt_pipeline_nv_props) {
        rt_pipeline_nv_props->shaderGroupHandleSize = 32;
        rt_pipeline_nv_props->shaderGroupBaseAlignment = 64;
    }

    auto *texel_buffer_props = lvl_find_mod_in_chain<VkPhysicalDeviceTexelBufferAlignmentProperties>(pProperties->pNext);
    if (texel_buffer_props) {
        texel_buffer_props->storageTexelBufferOffsetSingleTexelAlignment = VK_TRUE;
        texel_buffer_props->uniformTexelBufferOffsetSingleTexelAlignment = VK_TRUE;
        texel_buffer_props->storageTexelBufferOffsetAlignmentBytes = 16;
        texel_buffer_props->uniformTexelBufferOffsetAlignmentBytes = 16;
    }

    auto *descriptor_buffer_props = lvl_find_mod_in_chain<VkPhysicalDeviceDescriptorBufferPropertiesEXT>(pProperties->pNext);
    if (descriptor_buffer_props) {
        descriptor_buffer_props->combinedImageSamplerDescriptorSingleArray = VK_TRUE;
        descriptor_buffer_props->bufferlessPushDescriptors = VK_TRUE;
        descriptor_buffer_props->allowSamplerImageViewPostSubmitCreation = VK_TRUE;
        descriptor_buffer_props->descriptorBufferOffsetAlignment = 4;
    }

    auto *mesh_shader_props = lvl_find_mod_in_chain<VkPhysicalDeviceMeshShaderPropertiesEXT>(pProperties->pNext);
    if (mesh_shader_props) {
        mesh_shader_props->meshOutputPerVertexGranularity = 32;
        mesh_shader_props->meshOutputPerPrimitiveGranularity = 32;
        mesh_shader_props->prefersLocalInvocationVertexOutput = VK_TRUE;
        mesh_shader_props->prefersLocalInvocationPrimitiveOutput = VK_TRUE;
        mesh_shader_props->prefersCompactVertexOutput = VK_TRUE;
        mesh_shader_props->prefersCompactPrimitiveOutput = VK_TRUE;
    }

    auto *fragment_density_map2_props = lvl_find_mod_in_chain<VkPhysicalDeviceFragmentDensityMap2PropertiesEXT>(pProperties->pNext);
    if (fragment_density_map2_props) {
        fragment_density_map2_props->subsampledLoads = VK_FALSE;
        fragment_density_map2_props->subsampledCoarseReconstructionEarlyAccess = VK_FALSE;
        fragment_density_map2_props->maxSubsampledArrayLayers = 2;
        fragment_density_map2_props->maxDescriptorSetSubsampledSamplers = 1;
    }

    auto *maintenance3_props = lvl_find_mod_in_chain<VkPhysicalDeviceMaintenance3Properties>(pProperties->pNext);
    if (maintenance3_props) {
        maintenance3_props->maxMemoryAllocationSize = 1073741824;
        maintenance3_props->maxPerSetDescriptors = 1024;
    }

    const uint32_t num_copy_layouts = 5;
    const VkImageLayout HostCopyLayouts[]{
       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
       VK_IMAGE_LAYOUT_GENERAL,
       VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
       VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
       VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
    };

    auto *host_image_copy_props = lvl_find_mod_in_chain<VkPhysicalDeviceHostImageCopyPropertiesEXT>(pProperties->pNext);
    if (host_image_copy_props){
        if (host_image_copy_props->pCopyDstLayouts == nullptr) host_image_copy_props->copyDstLayoutCount = num_copy_layouts;
        else {
            uint32_t num_layouts = (std::min)(host_image_copy_props->copyDstLayoutCount, num_copy_layouts);
            for (uint32_t i = 0; i < num_layouts; i++) {
                host_image_copy_props->pCopyDstLayouts[i] = HostCopyLayouts[i];
            }
        }
        if (host_image_copy_props->pCopySrcLayouts == nullptr) host_image_copy_props->copySrcLayoutCount = num_copy_layouts;
        else {
            uint32_t num_layouts = (std::min)(host_image_copy_props->copySrcLayoutCount, num_copy_layouts);
             for (uint32_t i = 0; i < num_layouts; i++) {
                host_image_copy_props->pCopySrcLayouts[i] = HostCopyLayouts[i];
            }
        }
    }

    auto *driver_properties = lvl_find_mod_in_chain<VkPhysicalDeviceDriverProperties>(pProperties->pNext);
    if (driver_properties) {
        std::strncpy(driver_properties->driverName, "Vulkan Mock Device", VK_MAX_DRIVER_NAME_SIZE);
#if defined(GIT_BRANCH_NAME) && defined(GIT_TAG_INFO)
        std::strncpy(driver_properties->driverInfo, "Branch: " GIT_BRANCH_NAME " Tag Info: " GIT_TAG_INFO, VK_MAX_DRIVER_INFO_SIZE);
#else
        std::strncpy(driver_properties->driverInfo, "Branch: --unknown-- Tag Info: --unknown--", VK_MAX_DRIVER_INFO_SIZE);
#endif
    }

    auto *layered_properties = lvl_find_mod_in_chain<VkPhysicalDeviceLayeredApiPropertiesListKHR>(pProperties->pNext);
    if (layered_properties) {
        layered_properties->layeredApiCount = 1;
        if (layered_properties->pLayeredApis) {
            layered_properties->pLayeredApis[0] = VkPhysicalDeviceLayeredApiPropertiesKHR{
                VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LAYERED_API_PROPERTIES_KHR,
                nullptr,
                0xba5eba11,
                0xf005ba11,
                VK_PHYSICAL_DEVICE_LAYERED_API_VULKAN_KHR
            };
            std::strncpy(layered_properties->pLayeredApis[0].deviceName, "Fake Driver", VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
        }
    }
''',
'vkGetPhysicalDeviceExternalSemaphoreProperties':'''
    // Hard code support for all handle types and features
    pExternalSemaphoreProperties->exportFromImportedHandleTypes = 0x1F;
    pExternalSemaphoreProperties->compatibleHandleTypes = 0x1F;
    pExternalSemaphoreProperties->externalSemaphoreFeatures = 0x3;
''',
'vkGetPhysicalDeviceExternalSemaphorePropertiesKHR':'''
    GetPhysicalDeviceExternalSemaphoreProperties(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
''',
'vkGetPhysicalDeviceExternalFenceProperties':'''
    // Hard-code support for all handle types and features
    pExternalFenceProperties->exportFromImportedHandleTypes = 0xF;
    pExternalFenceProperties->compatibleHandleTypes = 0xF;
    pExternalFenceProperties->externalFenceFeatures = 0x3;
''',
'vkGetPhysicalDeviceExternalFencePropertiesKHR':'''
    GetPhysicalDeviceExternalFenceProperties(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
''',
'vkGetPhysicalDeviceExternalBufferProperties':'''
    constexpr VkExternalMemoryHandleTypeFlags supported_flags = 0x1FF;
    if (pExternalBufferInfo->handleType & VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID) {
        // Can't have dedicated memory with AHB
        pExternalBufferProperties->externalMemoryProperties.externalMemoryFeatures = VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT | VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT;
        pExternalBufferProperties->externalMemoryProperties.exportFromImportedHandleTypes = pExternalBufferInfo->handleType;
        pExternalBufferProperties->externalMemoryProperties.compatibleHandleTypes = pExternalBufferInfo->handleType;
    } else if (pExternalBufferInfo->handleType & supported_flags) {
        pExternalBufferProperties->externalMemoryProperties.externalMemoryFeatures = 0x7;
        pExternalBufferProperties->externalMemoryProperties.exportFromImportedHandleTypes = supported_flags;
        pExternalBufferProperties->externalMemoryProperties.compatibleHandleTypes = supported_flags;
    } else {
        pExternalBufferProperties->externalMemoryProperties.externalMemoryFeatures = 0;
        pExternalBufferProperties->externalMemoryProperties.exportFromImportedHandleTypes = 0;
        // According to spec, handle type is always compatible with itself. Even if export/import
        // not supported, it's important to properly implement self-compatibility property since
        // application's control flow can rely on this.
        pExternalBufferProperties->externalMemoryProperties.compatibleHandleTypes = pExternalBufferInfo->handleType;
    }
''',
'vkGetPhysicalDeviceExternalBufferPropertiesKHR':'''
    GetPhysicalDeviceExternalBufferProperties(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
''',
'vkGetBufferMemoryRequirements': '''
    // TODO: Just hard-coding reqs for now
    pMemoryRequirements->size = 4096;
    pMemoryRequirements->alignment = 1;
    pMemoryRequirements->memoryTypeBits = 0xFFFF;
    // Return a better size based on the buffer size from the create info.
    unique_lock_t lock(global_lock);
    auto d_iter = buffer_map.find(device);
    if (d_iter != buffer_map.end()) {
        auto iter = d_iter->second.find(buffer);
        if (iter != d_iter->second.end()) {
            pMemoryRequirements->size = ((iter->second.size + 4095) / 4096) * 4096;
        }
    }
''',
'vkGetBufferMemoryRequirements2KHR': '''
    GetBufferMemoryRequirements(device, pInfo->buffer, &pMemoryRequirements->memoryRequirements);
''',
'vkGetDeviceBufferMemoryRequirements': '''
    // TODO: Just hard-coding reqs for now
    pMemoryRequirements->memoryRequirements.alignment = 1;
    pMemoryRequirements->memoryRequirements.memoryTypeBits = 0xFFFF;

    // Return a size based on the buffer size from the create info.
    pMemoryRequirements->memoryRequirements.size = ((pInfo->pCreateInfo->size + 4095) / 4096) * 4096;
''',
'vkGetDeviceBufferMemoryRequirementsKHR': '''
    GetDeviceBufferMemoryRequirements(device, pInfo, pMemoryRequirements);
''',
'vkGetImageMemoryRequirements': '''
    pMemoryRequirements->size = 0;
    pMemoryRequirements->alignment = 1;

    unique_lock_t lock(global_lock);
    auto d_iter = image_memory_size_map.find(device);
    if(d_iter != image_memory_size_map.end()){
        auto iter = d_iter->second.find(image);
        if (iter != d_iter->second.end()) {
            pMemoryRequirements->size = iter->second;
        }
    }
    // Here we hard-code that the memory type at index 3 doesn't support this image.
    pMemoryRequirements->memoryTypeBits = 0xFFFF & ~(0x1 << 3);
''',
'vkGetImageMemoryRequirements2KHR': '''
    GetImageMemoryRequirements(device, pInfo->image, &pMemoryRequirements->memoryRequirements);
''',
'vkGetDeviceImageMemoryRequirements': '''
    pMemoryRequirements->memoryRequirements.size = GetImageSizeFromCreateInfo(pInfo->pCreateInfo);
    pMemoryRequirements->memoryRequirements.alignment = 1;
    // Here we hard-code that the memory type at index 3 doesn't support this image.
    pMemoryRequirements->memoryRequirements.memoryTypeBits = 0xFFFF & ~(0x1 << 3);
''',
'vkGetDeviceImageMemoryRequirementsKHR': '''
    GetDeviceImageMemoryRequirements(device, pInfo, pMemoryRequirements);
''',
'vkMapMemory': '''
    unique_lock_t lock(global_lock);
    if (VK_WHOLE_SIZE == size) {
        if (allocated_memory_size_map.count(memory) != 0)
            size = allocated_memory_size_map[memory] - offset;
        else
            size = 0x10000;
    }
    void* map_addr = malloc((size_t)size);
    mapped_memory_map[memory].push_back(map_addr);
    *ppData = map_addr;
    return VK_SUCCESS;
''',
'vkMapMemory2KHR': '''
    return MapMemory(device, pMemoryMapInfo->memory, pMemoryMapInfo->offset, pMemoryMapInfo->size, pMemoryMapInfo->flags, ppData);
''',
'vkUnmapMemory': '''
    unique_lock_t lock(global_lock);
    for (auto map_addr : mapped_memory_map[memory]) {
        free(map_addr);
    }
    mapped_memory_map.erase(memory);
''',
'vkUnmapMemory2KHR': '''
    UnmapMemory(device, pMemoryUnmapInfo->memory);
    return VK_SUCCESS;
''',
'vkGetImageSubresourceLayout': '''
    // Need safe values. Callers are computing memory offsets from pLayout, with no return code to flag failure.
    *pLayout = VkSubresourceLayout(); // Default constructor zero values.
''',
'vkCreateSwapchainKHR': '''
    unique_lock_t lock(global_lock);
    *pSwapchain = (VkSwapchainKHR)global_unique_handle++;
    for(uint32_t i = 0; i < icd_swapchain_image_count; ++i){
        swapchain_image_map[*pSwapchain][i] = (VkImage)global_unique_handle++;
    }
    return VK_SUCCESS;
''',
'vkDestroySwapchainKHR': '''
    unique_lock_t lock(global_lock);
    swapchain_image_map.clear();
''',
'vkGetSwapchainImagesKHR': '''
    if (!pSwapchainImages) {
        *pSwapchainImageCount = icd_swapchain_image_count;
    } else {
        unique_lock_t lock(global_lock);
        for (uint32_t img_i = 0; img_i < (std::min)(*pSwapchainImageCount, icd_swapchain_image_count); ++img_i){
            pSwapchainImages[img_i] = swapchain_image_map.at(swapchain)[img_i];
        }

        if (*pSwapchainImageCount < icd_swapchain_image_count) return VK_INCOMPLETE;
        else if (*pSwapchainImageCount > icd_swapchain_image_count) *pSwapchainImageCount = icd_swapchain_image_count;
    }
    return VK_SUCCESS;
''',
'vkAcquireNextImageKHR': '''
    *pImageIndex = 0;
    return VK_SUCCESS;
''',
'vkAcquireNextImage2KHR': '''
    *pImageIndex = 0;
    return VK_SUCCESS;
''',
'vkCreateBuffer': '''
    unique_lock_t lock(global_lock);
    *pBuffer = (VkBuffer)global_unique_handle++;
     buffer_map[device][*pBuffer] = {
         pCreateInfo->size,
         current_available_address
     };
     current_available_address += pCreateInfo->size;
     // Always align to next 64-bit pointer
     const uint64_t alignment = current_available_address % 64;
     if (alignment != 0) {
         current_available_address += (64 - alignment);
     }
    return VK_SUCCESS;
''',
'vkDestroyBuffer': '''
    unique_lock_t lock(global_lock);
    buffer_map[device].erase(buffer);
''',
'vkCreateImage': '''
    unique_lock_t lock(global_lock);
    *pImage = (VkImage)global_unique_handle++;
    image_memory_size_map[device][*pImage] = GetImageSizeFromCreateInfo(pCreateInfo);
    return VK_SUCCESS;
''',
'vkDestroyImage': '''
    unique_lock_t lock(global_lock);
    image_memory_size_map[device].erase(image);
''',
'vkEnumeratePhysicalDeviceGroupsKHR': '''
    if (!pPhysicalDeviceGroupProperties) {
        *pPhysicalDeviceGroupCount = 1;
    } else {
        // arbitrary
        pPhysicalDeviceGroupProperties->physicalDeviceCount = 1;
        pPhysicalDeviceGroupProperties->physicalDevices[0] = physical_device_map.at(instance)[0];
        pPhysicalDeviceGroupProperties->subsetAllocation = VK_FALSE;
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceMultisamplePropertiesEXT': '''
    if (pMultisampleProperties) {
        // arbitrary
        pMultisampleProperties->maxSampleLocationGridSize = {32, 32};
    }
''',
'vkGetPhysicalDeviceFragmentShadingRatesKHR': '''
    if (!pFragmentShadingRates) {
        *pFragmentShadingRateCount = 1;
    } else {
        // arbitrary
        pFragmentShadingRates->sampleCounts = VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT;
        pFragmentShadingRates->fragmentSize = {8, 8};
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceCalibrateableTimeDomainsEXT': '''
    if (!pTimeDomains) {
        *pTimeDomainCount = 1;
    } else {
        // arbitrary
        *pTimeDomains = VK_TIME_DOMAIN_DEVICE_EXT;
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceCalibrateableTimeDomainsKHR': '''
    if (!pTimeDomains) {
        *pTimeDomainCount = 1;
    } else {
        // arbitrary
        *pTimeDomains = VK_TIME_DOMAIN_DEVICE_KHR;
    }
    return VK_SUCCESS;
''',
'vkGetFenceWin32HandleKHR': '''
    *pHandle = (HANDLE)0x12345678;
    return VK_SUCCESS;
''',
'vkGetFenceFdKHR': '''
    *pFd = 0x42;
    return VK_SUCCESS;
''',
'vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR': '''
    if (!pCounters) {
        *pCounterCount = 3;
    } else {
        if (*pCounterCount == 0){
            return VK_INCOMPLETE;
        }
        // arbitrary
        pCounters[0].unit = VK_PERFORMANCE_COUNTER_UNIT_GENERIC_KHR;
        pCounters[0].scope = VK_QUERY_SCOPE_COMMAND_BUFFER_KHR;
        pCounters[0].storage = VK_PERFORMANCE_COUNTER_STORAGE_INT32_KHR;
        pCounters[0].uuid[0] = 0x01;
        if (*pCounterCount == 1){
            return VK_INCOMPLETE;
        }
        pCounters[1].unit = VK_PERFORMANCE_COUNTER_UNIT_GENERIC_KHR;
        pCounters[1].scope = VK_QUERY_SCOPE_RENDER_PASS_KHR;
        pCounters[1].storage = VK_PERFORMANCE_COUNTER_STORAGE_INT32_KHR;
        pCounters[1].uuid[0] = 0x02;
        if (*pCounterCount == 2){
            return VK_INCOMPLETE;
        }
        pCounters[2].unit = VK_PERFORMANCE_COUNTER_UNIT_GENERIC_KHR;
        pCounters[2].scope = VK_QUERY_SCOPE_COMMAND_KHR;
        pCounters[2].storage = VK_PERFORMANCE_COUNTER_STORAGE_INT32_KHR;
        pCounters[2].uuid[0] = 0x03;
        *pCounterCount = 3;
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR': '''
    if (pNumPasses) {
        // arbitrary
        *pNumPasses = 1;
    }
''',
'vkGetShaderModuleIdentifierEXT': '''
    if (pIdentifier) {
        // arbitrary
        pIdentifier->identifierSize = 1;
        pIdentifier->identifier[0] = 0x01;
    }
''',
'vkGetImageSparseMemoryRequirements': '''
    if (!pSparseMemoryRequirements) {
        *pSparseMemoryRequirementCount = 1;
    } else {
        // arbitrary
        pSparseMemoryRequirements->imageMipTailFirstLod = 0;
        pSparseMemoryRequirements->imageMipTailSize = 8;
        pSparseMemoryRequirements->imageMipTailOffset = 0;
        pSparseMemoryRequirements->imageMipTailStride = 4;
        pSparseMemoryRequirements->formatProperties.imageGranularity = {4, 4, 4};
        pSparseMemoryRequirements->formatProperties.flags = VK_SPARSE_IMAGE_FORMAT_SINGLE_MIPTAIL_BIT;
        // Would need to track the VkImage to know format for better value here
        pSparseMemoryRequirements->formatProperties.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_METADATA_BIT;
    }

''',
'vkGetImageSparseMemoryRequirements2KHR': '''
    if (pSparseMemoryRequirementCount && pSparseMemoryRequirements) {
        GetImageSparseMemoryRequirements(device, pInfo->image, pSparseMemoryRequirementCount, &pSparseMemoryRequirements->memoryRequirements);
    } else {
        GetImageSparseMemoryRequirements(device, pInfo->image, pSparseMemoryRequirementCount, nullptr);
    }
''',
'vkGetBufferDeviceAddress': '''
    VkDeviceAddress address = 0;
    auto d_iter = buffer_map.find(device);
    if (d_iter != buffer_map.end()) {
        auto iter = d_iter->second.find(pInfo->buffer);
        if (iter != d_iter->second.end()) {
            address = iter->second.address;
        }
    }
    return address;
''',
'vkGetBufferDeviceAddressKHR': '''
    return GetBufferDeviceAddress(device, pInfo);
''',
'vkGetBufferDeviceAddressEXT': '''
    return GetBufferDeviceAddress(device, pInfo);
''',
'vkGetDescriptorSetLayoutSizeEXT': '''
    // Need to give something non-zero
    *pLayoutSizeInBytes = 4;
''',
'vkGetAccelerationStructureBuildSizesKHR': '''
    // arbitrary
    pSizeInfo->accelerationStructureSize = 4;
    pSizeInfo->updateScratchSize = 4;
    pSizeInfo->buildScratchSize = 4;
''',
'vkGetAccelerationStructureMemoryRequirementsNV': '''
    // arbitrary
    pMemoryRequirements->memoryRequirements.size = 4096;
    pMemoryRequirements->memoryRequirements.alignment = 1;
    pMemoryRequirements->memoryRequirements.memoryTypeBits = 0xFFFF;
''',
'vkGetAccelerationStructureDeviceAddressKHR': '''
    // arbitrary - need to be aligned to 256 bytes
    return 0x262144;
''',
'vkGetVideoSessionMemoryRequirementsKHR': '''
    if (!pMemoryRequirements) {
        *pMemoryRequirementsCount = 1;
    } else {
        // arbitrary
        pMemoryRequirements[0].memoryBindIndex = 0;
        pMemoryRequirements[0].memoryRequirements.size = 4096;
        pMemoryRequirements[0].memoryRequirements.alignment = 1;
        pMemoryRequirements[0].memoryRequirements.memoryTypeBits = 0xFFFF;
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR': '''
    if (!pProperties) {
        *pPropertyCount = 2;
    } else {
        // arbitrary
        pProperties[0].MSize = 16;
        pProperties[0].NSize = 16;
        pProperties[0].KSize = 16;
        pProperties[0].AType = VK_COMPONENT_TYPE_UINT32_KHR;
        pProperties[0].BType = VK_COMPONENT_TYPE_UINT32_KHR;
        pProperties[0].CType = VK_COMPONENT_TYPE_UINT32_KHR;
        pProperties[0].ResultType = VK_COMPONENT_TYPE_UINT32_KHR;
        pProperties[0].saturatingAccumulation = VK_FALSE;
        pProperties[0].scope = VK_SCOPE_SUBGROUP_KHR;

        pProperties[1] = pProperties[0];
        pProperties[1].scope = VK_SCOPE_DEVICE_KHR;
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceVideoCapabilitiesKHR': '''
    return VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR;
''',
'vkGetPhysicalDeviceVideoFormatPropertiesKHR': '''
    return VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR;
''',
'vkGetDescriptorSetLayoutSupport':'''
    if (pSupport) {
        pSupport->supported = VK_TRUE;
    }
''',
'vkGetDescriptorSetLayoutSupportKHR':'''
    GetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
''',
'vkGetRenderAreaGranularity': '''
    pGranularity->width = 1;
    pGranularity->height = 1;
''',
'vkGetMemoryFdKHR': '''
    *pFd = 1;
    return VK_SUCCESS;
''',
'vkGetMemoryHostPointerPropertiesEXT': '''
    pMemoryHostPointerProperties->memoryTypeBits = 1 << 5; // DEVICE_LOCAL only type
    return VK_SUCCESS;
''',
'vkGetAndroidHardwareBufferPropertiesANDROID': '''
    pProperties->allocationSize = 65536;
    pProperties->memoryTypeBits = 1 << 5; // DEVICE_LOCAL only type

    auto *format_prop = lvl_find_mod_in_chain<VkAndroidHardwareBufferFormatPropertiesANDROID>(pProperties->pNext);
    if (format_prop) {
        // Likley using this format
        format_prop->format = VK_FORMAT_R8G8B8A8_UNORM;
        format_prop->externalFormat = 37;
    }

    auto *format_resolve_prop = lvl_find_mod_in_chain<VkAndroidHardwareBufferFormatResolvePropertiesANDROID>(pProperties->pNext);
    if (format_resolve_prop) {
        format_resolve_prop->colorAttachmentFormat = VK_FORMAT_R8G8B8A8_UNORM;
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceDisplayPropertiesKHR': '''
    if (!pProperties) {
        *pPropertyCount = 1;
    } else {
        unique_lock_t lock(global_lock);
        pProperties[0].display = (VkDisplayKHR)global_unique_handle++;
        display_map[physicalDevice].insert(pProperties[0].display);
    }
    return VK_SUCCESS;
''',
'vkRegisterDisplayEventEXT': '''
    unique_lock_t lock(global_lock);
    *pFence = (VkFence)global_unique_handle++;
    return VK_SUCCESS;
''',
'vkQueueSubmit': '''
    // Special way to cause DEVICE_LOST
    // Picked VkExportFenceCreateInfo because needed some struct that wouldn't get cleared by validation Safe Struct
    // ... TODO - It would be MUCH nicer to have a layer or other setting control when this occured
    // For now this is used to allow Validation Layers test reacting to device losts
    if (submitCount > 0 && pSubmits) {
        auto pNext = reinterpret_cast<const VkBaseInStructure *>(pSubmits[0].pNext);
        if (pNext && pNext->sType == VK_STRUCTURE_TYPE_EXPORT_FENCE_CREATE_INFO && pNext->pNext == nullptr) {
            return VK_ERROR_DEVICE_LOST;
        }
    }
    return VK_SUCCESS;
''',
'vkGetMemoryWin32HandlePropertiesKHR': '''
    pMemoryWin32HandleProperties->memoryTypeBits = 0xFFFF;
    return VK_SUCCESS;
''',
'vkCreatePipelineBinariesKHR': '''
    unique_lock_t lock(global_lock);
    for (uint32_t i = 0; i < pBinaries->pipelineBinaryCount; ++i) {
        pBinaries->pPipelineBinaries[i] = (VkPipelineBinaryKHR)global_unique_handle++;
    }
    return VK_SUCCESS;
'''
}

# MockICDOutputGenerator
# Generates a mock vulkan ICD.
#  This is intended to be a minimal replacement for a vulkan device in order
#  to enable testing of Vulkan applications and layers
#
class MockICDOutputGenerator(BaseGenerator):
    def __init__(self):
        BaseGenerator.__init__(self)

        # Ignore extensions that ICDs should not implement or are not safe to report
        self.ignore_exts = ['VK_EXT_validation_cache', 'VK_KHR_portability_subset']

        # Dispatchable handles
        self.dispatchable_handles = ['VkInstance','VkPhysicalDevice', 'VkDevice', 'VkCommandBuffer', 'VkQueue']

    def generate_function_declarations(self, out):

        out.append('#include <stdint.h>\n')
        out.append('#include <cstring>\n')
        out.append('#include <string>\n')
        out.append('#include <unordered_map>\n')
        out.append('#include <vulkan/vulkan.h>\n')
        out.append('\n')
        out.append('namespace vkmock {\n')
        out.append('// Map of instance extension name to version\n')
        out.append('static const std::unordered_map<std::string, uint32_t> instance_extension_map = {\n')
        for ext in [x for x in self.vk.extensions.values() if x.instance and x.name not in self.ignore_exts]:
            if ext.protect:
                out.append(f'#ifdef {ext.protect}\n')
            out.append(f'    {{"{ext.name}", {ext.specVersion}}},\n')
            if ext.protect:
                out.append('#endif\n')
        out.append('};\n')
        out.append('// Map of device extension name to version\n')
        out.append('static const std::unordered_map<std::string, uint32_t> device_extension_map = {\n')
        for ext in [x for x in self.vk.extensions.values() if x.device and x.name not in self.ignore_exts]:
            if ext.protect:
                out.append(f'#ifdef {ext.protect}\n')
            out.append(f'    {{"{ext.name}", {ext.specVersion}}},\n')
            if ext.protect:
                out.append('#endif\n')
        out.append('};\n')

        current_protect = None
        for name, cmd in self.vk.commands.items():
            prepend_newline = '\n'
            if cmd.protect != current_protect:
                if current_protect is not None:
                    out.append(f'#endif /* {current_protect} */\n')
                    prepend_newline = ''
                if current_protect is not None and cmd.protect is not None:
                    out.append('\n')
                if cmd.protect is not None:
                    out.append(f'#ifdef {cmd.protect}\n')
                current_protect = cmd.protect
            out.append(f'{prepend_newline}static {cmd.cPrototype.replace(name, name[2:])}\n')
        if current_protect is not None:
            out.append('#endif\n')

            # record intercepted procedures
        out.append('// Map of all APIs to be intercepted by this layer\n')
        out.append('static const std::unordered_map<std::string, void*> name_to_funcptr_map = {\n')
        for name, cmd in self.vk.commands.items():
            if cmd.protect:
                out.append(f'#ifdef {cmd.protect}\n')
            out.append(f'    {{"{name}", (void*){name[2:]}}},\n')
            if cmd.protect:
                out.append('#endif\n')
        out.append('};\n')

    def generate_function_definitions(self, out):
        out.append('#include "mock_icd.h"\n')
        out.append('#include "function_declarations.h"\n')
        out.append('namespace vkmock {\n')

        manual_functions = [
            # Include functions here to be intercepted w/ manually implemented function bodies
            'vkGetDeviceProcAddr',
            'vkGetInstanceProcAddr',
            'vkCreateDevice',
            'vkDestroyDevice',
            'vkCreateInstance',
            'vkDestroyInstance',
            'vkFreeCommandBuffers',
            'vkAllocateCommandBuffers',
            'vkDestroyCommandPool',
            #'vkCreateDebugReportCallbackEXT',
            #'vkDestroyDebugReportCallbackEXT',
            'vkEnumerateInstanceLayerProperties',
            'vkEnumerateInstanceVersion',
            'vkEnumerateInstanceExtensionProperties',
            'vkEnumerateDeviceLayerProperties',
            'vkEnumerateDeviceExtensionProperties',
        ]
        current_protect = None
        for name, cmd in self.vk.commands.items():
            if cmd.protect != current_protect:
                if current_protect is not None:
                    out.append(f'#endif /* {current_protect} */\n')
                if current_protect is not None and cmd.protect is not None:
                    out.append('\n')
                if cmd.protect is not None:
                    out.append(f'#ifdef {cmd.protect}\n')
                current_protect = cmd.protect

            if name in manual_functions:
                if name not in CUSTOM_C_INTERCEPTS:
                    out.append(f'static {cmd.cPrototype.replace(name, name[2:])}\n')
                    out.append('// TODO: Implement custom intercept body\n')
                else:
                    out.append(f'static {cmd.cPrototype[:-1].replace(name, name[2:])}\n')
                    out.append(f'{{{CUSTOM_C_INTERCEPTS[name]}}}\n')
                continue

            out.append(f'static {cmd.cPrototype[:-1].replace(name, name[2:])}\n')
            if name in CUSTOM_C_INTERCEPTS:
                out.append(f'{{{CUSTOM_C_INTERCEPTS[name]}}}\n')
                continue

            # if the name w/ KHR postfix is in the CUSTOM_C_INTERCEPTS
            # Call the KHR custom version instead of generating separate code
            khr_name = name + "KHR"
            if khr_name in CUSTOM_C_INTERCEPTS:
                return_string = ''
                if cmd.returnType != 'void':
                    return_string = 'return '

                param_names = []
                for param in cmd.params:
                    param_names.append(param.name)
                out.append(f'{{\n    {return_string}{khr_name[2:]}({", ".join(param_names)});\n}}\n')
                continue
            out.append('{\n')

            # GET THE TYPE OF FUNCTION
            if any(name.startswith(ftxt) for ftxt in ('vkCreate', 'vkAllocate')):
                # Get last param
                last_param = cmd.params[-1]
                lp_txt = last_param.name
                lp_len = None
                if last_param.length is not None:
                    lp_len = last_param.length
                    lp_len = lp_len.replace('::', '->')
                lp_type = last_param.type
                handle_type = 'dispatchable'
                allocator_txt = 'CreateDispObjHandle()'
                if lp_type not in self.dispatchable_handles:
                    handle_type = 'non-' + handle_type
                    allocator_txt = 'global_unique_handle++'
                # Need to lock in both cases
                out.append('    unique_lock_t lock(global_lock);\n')
                if lp_len is not None:
                    #print("%s last params (%s) has len %s" % (handle_type, lp_txt, lp_len))
                    out.append(f'    for (uint32_t i = 0; i < {lp_len}; ++i) {{\n')
                    out.append(f'        {lp_txt}[i] = ({lp_type}){allocator_txt};\n')
                    out.append('    }\n')
                else:
                    #print("Single %s last param is '%s' w/ type '%s'" % (handle_type, lp_txt, lp_type))
                    if 'AllocateMemory' in name:
                        # Store allocation size in case it's mapped
                        out.append('    allocated_memory_size_map[(VkDeviceMemory)global_unique_handle] = pAllocateInfo->allocationSize;\n')
                    out.append(f'    *{lp_txt} = ({lp_type}){allocator_txt};\n')
            elif True in [ftxt in name for ftxt in ['Destroy', 'Free']]:
                out.append('//Destroy object\n')
                if 'FreeMemory' in name:
                    # If the memory is mapped, unmap it
                    out.append('    UnmapMemory(device, memory);\n')
                    # Remove from allocation map
                    out.append('    unique_lock_t lock(global_lock);\n')
                    out.append('    allocated_memory_size_map.erase(memory);\n')
            else:
                out.append('//Not a CREATE or DESTROY function\n')

            # Return result variable, if any.
            if cmd.returnType != 'void':
                if name == 'vkGetEventStatus':
                    out.append('    return VK_EVENT_SET;\n')
                else:
                    out.append('    return VK_SUCCESS;\n')
            out.append('}\n')
        if current_protect is not None:
            out.append('#endif\n')

    def generate(self):
        out = []
        out.append('''/*
** Copyright (c) 2015-2025 The Khronos Group Inc.
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

/*
** This header is generated from the Khronos Vulkan XML API Registry.
**
*/

#pragma once
''')

        if self.filename == "function_declarations.h":
            self.generate_function_declarations(out)
        else:
            self.generate_function_definitions(out)

        out.append('\n')
        out.append('} // namespace vkmock\n')
        out.append('\n')
        self.write(''.join(out))
