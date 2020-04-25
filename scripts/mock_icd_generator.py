#!/usr/bin/python3 -i
#
# Copyright (c) 2015-2017 The Khronos Group Inc.
# Copyright (c) 2015-2017 Valve Corporation
# Copyright (c) 2015-2017 LunarG, Inc.
# Copyright (c) 2015-2017 Google Inc.
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

import os,re,sys
from generator import *
from common_codegen import *

MANUAL_COMMANDS = [
    # Include functions here to be interecpted w/ manually implemented function bodies
    'vkGetInstanceProcAddr',
    'vkEnumerateInstanceVersion',
    'vkEnumerateInstanceExtensionProperties',
    'vkEnumerateInstanceLayerProperties',
    'vkCreateInstance',
    'vkDestroyInstance',
    'vkEnumeratePhysicalDevices',
    'vkEnumerateDeviceExtensionProperties',
    'vkEnumerateDeviceLayerProperties',
    'vkGetPhysicalDeviceQueueFamilyProperties',
    'vkGetPhysicalDeviceQueueFamilyProperties2',
    'vkCreateDevice',
    'vkDestroyDevice',
    'vkGetDeviceProcAddr',
    'vkGetDeviceQueue',
    'vkGetDeviceQueue2',
]

ALIAS_COMMANDS = {
    'vkGetPhysicalDeviceQueueFamilyProperties2KHR': 'vkGetPhysicalDeviceQueueFamilyProperties2'
}

CUSTOM_C_INTERCEPTS = {
'vkGetPhysicalDeviceSurfacePresentModesKHR': '''
    // Currently always say that all present modes are supported
    if (!pPresentModes) {
        *pPresentModeCount = 6;
    } else {
        // Intentionally falling through and just filling however many modes are requested
        switch(*pPresentModeCount) {
        case 6:
            pPresentModes[5] = VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR;
            // fall through
        case 5:
            pPresentModes[4] = VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR;
            // fall through
        case 4:
            pPresentModes[3] = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
            // fall through
        case 3:
            pPresentModes[2] = VK_PRESENT_MODE_FIFO_KHR;
            // fall through
        case 2:
            pPresentModes[1] = VK_PRESENT_MODE_MAILBOX_KHR;
            // fall through
        default:
            pPresentModes[0] = VK_PRESENT_MODE_IMMEDIATE_KHR;
            break;
        }
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceSurfaceFormatsKHR': '''
    // Currently always say that RGBA8 & BGRA8 are supported
    if (!pSurfaceFormats) {
        *pSurfaceFormatCount = 2;
    } else {
        // Intentionally falling through and just filling however many types are requested
        switch(*pSurfaceFormatCount) {
        case 2:
            pSurfaceFormats[1].format = VK_FORMAT_R8G8B8A8_UNORM;
            pSurfaceFormats[1].colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
            // fall through
        default:
            pSurfaceFormats[0].format = VK_FORMAT_B8G8R8A8_UNORM;
            pSurfaceFormats[0].colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
            break;
        }
    }
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceSurfaceFormats2KHR': '''
    // Currently always say that RGBA8 & BGRA8 are supported
    if (!pSurfaceFormats) {
        *pSurfaceFormatCount = 2;
    } else {
        // Intentionally falling through and just filling however many types are requested
        switch(*pSurfaceFormatCount) {
        case 2:
            pSurfaceFormats[1].pNext = nullptr;
            pSurfaceFormats[1].surfaceFormat.format = VK_FORMAT_R8G8B8A8_UNORM;
            pSurfaceFormats[1].surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
            // fall through
        default:
            pSurfaceFormats[1].pNext = nullptr;
            pSurfaceFormats[0].surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
            pSurfaceFormats[0].surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
            break;
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
    pSurfaceCapabilities->maxImageExtent.width = 3840;
    pSurfaceCapabilities->maxImageExtent.height = 2160;
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
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceMemoryProperties': '''
    pMemoryProperties->memoryTypeCount = 2;
    pMemoryProperties->memoryTypes[0].propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    pMemoryProperties->memoryTypes[0].heapIndex = 0;
    pMemoryProperties->memoryTypes[1].propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    pMemoryProperties->memoryTypes[1].heapIndex = 1;
    pMemoryProperties->memoryHeapCount = 2;
    pMemoryProperties->memoryHeaps[0].flags = 0;
    pMemoryProperties->memoryHeaps[0].size = 8000000000;
    pMemoryProperties->memoryHeaps[1].flags = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;
    pMemoryProperties->memoryHeaps[1].size = 8000000000;
''',
'vkGetPhysicalDeviceMemoryProperties2KHR': '''
    GetPhysicalDeviceMemoryProperties(physicalDevice, &pMemoryProperties->memoryProperties);
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
''',
'vkGetPhysicalDeviceFormatProperties': '''
    if (VK_FORMAT_UNDEFINED == format) {
        *pFormatProperties = { 0x0, 0x0, 0x0 };
    } else {
        // TODO: Just returning full support for everything initially
        *pFormatProperties = { 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF };
    }
''',
'vkGetPhysicalDeviceFormatProperties2KHR': '''
    GetPhysicalDeviceFormatProperties(physicalDevice, format, &pFormatProperties->formatProperties);
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
    GetPhysicalDeviceImageFormatProperties(physicalDevice, pImageFormatInfo->format, pImageFormatInfo->type, pImageFormatInfo->tiling, pImageFormatInfo->usage, pImageFormatInfo->flags, &pImageFormatProperties->imageFormatProperties);
    return VK_SUCCESS;
''',
'vkGetPhysicalDeviceProperties': '''
    // TODO: Just hard-coding some values for now
    pProperties->apiVersion = VK_API_VERSION_1_0;
    pProperties->driverVersion = 1;
    pProperties->vendorID = 0xba5eba11;
    pProperties->deviceID = 0xf005ba11;
    pProperties->deviceType = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
    //std::string devName = "Vulkan Mock Device";
    std::strcpy(pProperties->deviceName, "Vulkan Mock Device");
    pProperties->pipelineCacheUUID[0] = 18;
    pProperties->limits = SetLimits(&pProperties->limits);
    pProperties->sparseProperties = { VK_TRUE, VK_TRUE, VK_TRUE, VK_TRUE, VK_TRUE };
''',
'vkGetPhysicalDeviceProperties2KHR': '''
    GetPhysicalDeviceProperties(physicalDevice, &pProperties->properties);
    const auto *desc_idx_props = lvl_find_in_chain<VkPhysicalDeviceDescriptorIndexingPropertiesEXT>(pProperties->pNext);
    if (desc_idx_props) {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT* write_props = (VkPhysicalDeviceDescriptorIndexingPropertiesEXT*)desc_idx_props;
        write_props->maxUpdateAfterBindDescriptorsInAllPools = 500000;
        write_props->shaderUniformBufferArrayNonUniformIndexingNative = false;
        write_props->shaderSampledImageArrayNonUniformIndexingNative = false;
        write_props->shaderStorageBufferArrayNonUniformIndexingNative = false;
        write_props->shaderStorageImageArrayNonUniformIndexingNative = false;
        write_props->shaderInputAttachmentArrayNonUniformIndexingNative = false;
        write_props->robustBufferAccessUpdateAfterBind = true;
        write_props->quadDivergentImplicitLod = true;
        write_props->maxPerStageDescriptorUpdateAfterBindSamplers = 500000;
        write_props->maxPerStageDescriptorUpdateAfterBindUniformBuffers = 500000;
        write_props->maxPerStageDescriptorUpdateAfterBindStorageBuffers = 500000;
        write_props->maxPerStageDescriptorUpdateAfterBindSampledImages = 500000;
        write_props->maxPerStageDescriptorUpdateAfterBindStorageImages = 500000;
        write_props->maxPerStageDescriptorUpdateAfterBindInputAttachments = 500000;
        write_props->maxPerStageUpdateAfterBindResources = 500000;
        write_props->maxDescriptorSetUpdateAfterBindSamplers = 500000;
        write_props->maxDescriptorSetUpdateAfterBindUniformBuffers = 96;
        write_props->maxDescriptorSetUpdateAfterBindUniformBuffersDynamic = 8;
        write_props->maxDescriptorSetUpdateAfterBindStorageBuffers = 500000;
        write_props->maxDescriptorSetUpdateAfterBindStorageBuffersDynamic = 4;
        write_props->maxDescriptorSetUpdateAfterBindSampledImages = 500000;
        write_props->maxDescriptorSetUpdateAfterBindStorageImages = 500000;
        write_props->maxDescriptorSetUpdateAfterBindInputAttachments = 500000;
    }

    const auto *push_descriptor_props = lvl_find_in_chain<VkPhysicalDevicePushDescriptorPropertiesKHR>(pProperties->pNext);
    if (push_descriptor_props) {
        VkPhysicalDevicePushDescriptorPropertiesKHR* write_props = (VkPhysicalDevicePushDescriptorPropertiesKHR*)push_descriptor_props;
        write_props->maxPushDescriptors = 256;
    }

    const auto *depth_stencil_resolve_props = lvl_find_in_chain<VkPhysicalDeviceDepthStencilResolvePropertiesKHR>(pProperties->pNext);
    if (depth_stencil_resolve_props) {
        VkPhysicalDeviceDepthStencilResolvePropertiesKHR* write_props = (VkPhysicalDeviceDepthStencilResolvePropertiesKHR*)depth_stencil_resolve_props;
        write_props->supportedDepthResolveModes = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR;
        write_props->supportedStencilResolveModes = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR;
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
    // Hard-code support for all handle types and features
    pExternalBufferProperties->externalMemoryProperties.externalMemoryFeatures = 0x7;
    pExternalBufferProperties->externalMemoryProperties.exportFromImportedHandleTypes = 0x1FF;
    pExternalBufferProperties->externalMemoryProperties.compatibleHandleTypes = 0x1FF;
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
'vkGetImageMemoryRequirements': '''
    pMemoryRequirements->size = 0;
    pMemoryRequirements->alignment = 1;

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
'vkUnmapMemory': '''
    unique_lock_t lock(global_lock);
    for (auto map_addr : mapped_memory_map[memory]) {
        free(map_addr);
    }
    mapped_memory_map.erase(memory);
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
    buffer_map[device][*pBuffer] = *pCreateInfo;
    return VK_SUCCESS;
''',
'vkDestroyBuffer': '''
    unique_lock_t lock(global_lock);
    buffer_map[device].erase(buffer);
''',
'vkCreateImage': '''
    unique_lock_t lock(global_lock);
    *pImage = (VkImage)global_unique_handle++;
    // TODO: A pixel size is 32 bytes. This accounts for the largest possible pixel size of any format. It could be changed to more accurate size if need be.
    image_memory_size_map[device][*pImage] = pCreateInfo->extent.width * pCreateInfo->extent.height * pCreateInfo->extent.depth *
                                             32 * pCreateInfo->arrayLayers * (pCreateInfo->mipLevels > 1 ? 2 : 1);
    // plane count
    switch (pCreateInfo->format) {
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
        case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
        case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
        case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
        case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
        case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
            image_memory_size_map[device][*pImage] *= 3;
            break;
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
        case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
        case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
            image_memory_size_map[device][*pImage] *= 2;
            break;
        default:
            break;
    }
    return VK_SUCCESS;
''',
'vkDestroyImage': '''
    unique_lock_t lock(global_lock);
    image_memory_size_map[device].erase(image);
''',
}

# MockICDGeneratorOptions - subclass of GeneratorOptions.
#
# Adds options used by MockICDOutputGenerator objects during Mock
# ICD generation.
#
# Additional members
#   prefixText - list of strings to prefix generated header with
#     (usually a copyright statement + calling convention macros).
#   protectFile - True if multiple inclusion protection should be
#     generated (based on the filename) around the entire header.
#   protectFeature - True if #ifndef..#endif protection should be
#     generated around a feature interface in the header file.
#   genFuncPointers - True if function pointer typedefs should be
#     generated
#   protectProto - If conditional protection should be generated
#     around prototype declarations, set to either '#ifdef'
#     to require opt-in (#ifdef protectProtoStr) or '#ifndef'
#     to require opt-out (#ifndef protectProtoStr). Otherwise
#     set to None.
#   protectProtoStr - #ifdef/#ifndef symbol to use around prototype
#     declarations, if protectProto is set
#   apicall - string to use for the function declaration prefix,
#     such as APICALL on Windows.
#   apientry - string to use for the calling convention macro,
#     in typedefs, such as APIENTRY.
#   apientryp - string to use for the calling convention macro
#     in function pointer typedefs, such as APIENTRYP.
#   indentFuncProto - True if prototype declarations should put each
#     parameter on a separate line
#   indentFuncPointer - True if typedefed function pointers should put each
#     parameter on a separate line
#   alignFuncParam - if nonzero and parameters are being put on a
#     separate line, align parameter names at the specified column
class MockICDGeneratorOptions(GeneratorOptions):
    def __init__(self,
                 conventions = None,
                 filename = None,
                 directory = '.',
                 apiname = None,
                 profile = None,
                 versions = '.*',
                 emitversions = '.*',
                 defaultExtensions = None,
                 addExtensions = None,
                 removeExtensions = None,
                 emitExtensions = None,
                 sortProcedure = regSortFeatures,
                 prefixText = "",
                 genFuncPointers = True,
                 protectFile = True,
                 protectFeature = True,
                 protectProto = None,
                 protectProtoStr = None,
                 apicall = '',
                 apientry = '',
                 apientryp = '',
                 indentFuncProto = True,
                 indentFuncPointer = False,
                 alignFuncParam = 0,
                 expandEnumerants = True,
                 helper_file_type = ''):
        GeneratorOptions.__init__(self, conventions, filename, directory, apiname, profile,
                                  versions, emitversions, defaultExtensions,
                                  addExtensions, removeExtensions, emitExtensions, sortProcedure)
        self.prefixText      = prefixText
        self.genFuncPointers = genFuncPointers
        self.protectFile     = protectFile
        self.protectFeature  = protectFeature
        self.protectProto    = protectProto
        self.protectProtoStr = protectProtoStr
        self.apicall         = apicall
        self.apientry        = apientry
        self.apientryp       = apientryp
        self.indentFuncProto = indentFuncProto
        self.indentFuncPointer = indentFuncPointer
        self.alignFuncParam  = alignFuncParam
        self.helper_file_type = helper_file_type

# MockICDOutputGenerator - subclass of OutputGenerator.
# Generates a mock vulkan ICD.
#  This is intended to be a minimal replacement for a vulkan device in order
#  to enable Vulkan Validation testing.
#
# ---- methods ----
# MockOutputGenerator(errFile, warnFile, diagFile) - args as for
#   OutputGenerator. Defines additional internal state.
# ---- methods overriding base class ----
# beginFile(genOpts)
# endFile()
# beginFeature(interface, emit)
# endFeature()
# genType(typeinfo,name)
# genStruct(typeinfo,name)
# genGroup(groupinfo,name)
# genEnum(enuminfo, name)
# genCmd(cmdinfo)
class MockICDOutputGenerator(OutputGenerator):
    def __init__(self,
                 errFile = sys.stderr,
                 warnFile = sys.stderr,
                 diagFile = sys.stdout):
        OutputGenerator.__init__(self, errFile, warnFile, diagFile)
        self.namespace = 'mock_icd'
        self.extension_blacklist = ['VK_EXT_validation_cache']
        # Internal state - accumulators for different inner block text
        self.genNoVkPrefix = False
        self.current_feature = ''
        self.sections = {'command': []}
        self.intercepts = []
        self.device_extensions = []
        self.instance_extensions = []

    # Check if the parameter passed in is a pointer to an array
    def paramIsArray(self, param):
        return param.attrib.get('len') is not None

    # Check if the parameter passed in is a pointer
    def paramIsPointer(self, param):
        ispointer = False
        for elem in param:
            if ((elem.tag != 'type') and (elem.tail is not None)) and '*' in elem.tail:
                ispointer = True
        return ispointer

    # Check if an object is a non-dispatchable handle
    def isHandleTypeNonDispatchable(self, handletype):
        handle = self.registry.tree.find("types/type/[name='" + handletype + "'][@category='handle']")
        if handle is not None and handle.find('type').text == 'VK_DEFINE_NON_DISPATCHABLE_HANDLE':
            return True
        else:
            return False

    # Check if an object is a dispatchable handle
    def isHandleTypeDispatchable(self, handletype):
        handle = self.registry.tree.find("types/type/[name='" + handletype + "'][@category='handle']")
        if handle is not None and handle.find('type').text == 'VK_DEFINE_HANDLE':
            return True
        else:
            return False

    def genHeaderIncludeGuard(self):
        if self.genOpts.protectFile:
            self.include_guard = 'VULKAN_TOOLS_' + re.sub(r'\.h', '_H_', os.path.basename(self.genOpts.filename)).upper()
            write('#ifndef', self.include_guard, file=self.outFile)
            write('#define', self.include_guard, '1', file=self.outFile)
            self.newline()

    def beginExtensionListFile(self):
        self.genHeaderIncludeGuard()
        write('#include <array>', file=self.outFile)
        self.newline()
        write('#include "vulkan/vk_icd.h"', file=self.outFile)
        self.newline()
        write('namespace', self.namespace, '{', file=self.outFile)
        self.newline()

    def beginCommandsHeaderFile(self):
        self.genHeaderIncludeGuard()
        write('#include <string>', file=self.outFile)
        write('#include <unordered_map>', file=self.outFile)
        self.newline()
        write('#include "vulkan/vk_icd.h"', file=self.outFile)
        self.newline()
        write('namespace', self.namespace, '{', file=self.outFile)
        self.newline()

    def beginCommandsSourceFile(self):
        write('namespace', self.namespace, '{', file=self.outFile)
        self.newline()

    def beginFile(self, genOpts):
        OutputGenerator.beginFile(self, genOpts)

        if genOpts.prefixText:
            for s in genOpts.prefixText:
                write(s, file=self.outFile)

        if self.genOpts.helper_file_type == 'ext_list':
            self.beginExtensionListFile()
        elif self.genOpts.helper_file_type == 'commands_header':
            self.beginCommandsHeaderFile()
        elif self.genOpts.helper_file_type == 'mock_icd_source':
            self.beginCommandsSourceFile()

    def genHeaderFooter(self):
        self.newline()
        write('} // namespace', self.namespace, file=self.outFile)
        if self.include_guard is not None:
            write('#endif', '//', self.include_guard, file=self.outFile)

    def endExtensionListFile(self):
        write('constexpr std::array<VkExtensionProperties, %d> kInstanceExtensions = {{' % len(self.instance_extensions), file=self.outFile)
        write(',\n'.join('    {"' + e['name'] + '", ' + e['revision'] + '}' for e in self.instance_extensions), file=self.outFile)
        write('}};', file=self.outFile)
        self.newline()
        write('constexpr std::array<VkExtensionProperties, %d>  kDeviceExtensions = {{'  % len(self.device_extensions), file=self.outFile)
        write(',\n'.join('    {"' + e['name'] + '", ' + e['revision'] + '}' for e in self.device_extensions), file=self.outFile)
        write('}};', file=self.outFile)
        self.genHeaderFooter()

    def endCommandsHeaderFile(self):
        self.newline()
        write('// Map of all APIs to be intercepted by this layer', file=self.outFile)
        write('static const std::unordered_map<std::string, void*> name_to_funcptr_map = { // TODO(krOoze): technically violates styleguide', file=self.outFile)
        write('\n'.join(self.intercepts), file=self.outFile)
        write('};\n', file=self.outFile)
        self.genHeaderFooter()

    def endCommandsSourceFile(self):
        self.newline()
        write('} // namespace', self.namespace, file=self.outFile)

    def endFile(self):
        if self.genOpts.helper_file_type == 'ext_list':
            self.endExtensionListFile()
        elif self.genOpts.helper_file_type == 'commands_header':
            self.endCommandsHeaderFile()
        elif self.genOpts.helper_file_type == 'mock_icd_source': # Loader-layer-interface, need to implement global interface functions
            self.endCommandsSourceFile()
        OutputGenerator.endFile(self)

    def beginFeature(self, interface, emit):
        OutputGenerator.beginFeature(self, interface, emit)

        self.featureExtraProtect = GetFeatureProtect(interface)

        self.sections = {'command': []}

        name = self.featureName
        self.current_feature = name

        if interface.tag == 'extension':
            ext_type = interface.attrib['type']
            revision = interface[0][0].attrib['value']

            if name not in self.extension_blacklist:
                if ext_type == 'instance':
                    self.instance_extensions.append({'name': name, 'revision': revision})
                elif ext_type == 'device':
                    self.device_extensions.append({'name': name, 'revision': revision})

    def endFeature(self):
        if self.genOpts.helper_file_type == 'ext_list':
            pass
        elif self.genOpts.helper_file_type == 'commands_header' \
          or self.genOpts.helper_file_type == 'mock_icd_source' \
          or self.genOpts.helper_file_type == 'wsi_exports':
            if self.emit and self.featureName not in self.extension_blacklist \
            and len(self.sections['command']) != 0:
                if self.genOpts.protectFeature:
                    write('#ifndef', self.featureName, file=self.outFile)
                if self.featureExtraProtect:
                    write('#ifdef', self.featureExtraProtect, file=self.outFile)
                if (self.sections['command']):
                    write('\n\n'.join(self.sections['command']), file=self.outFile)
                if self.featureExtraProtect:
                    write('#endif //', self.featureExtraProtect, file=self.outFile)
                if self.genOpts.protectFeature:
                    write('#endif //', self.featureName, file=self.outFile)
                self.newline()
        # Finish processing in superclass
        OutputGenerator.endFeature(self)
    #
    # Append a definition to the specified section
    def appendSection(self, section, text):
        self.sections[section].append(text)

    # Command generation
    def genCmd(self, cmdinfo, name, alias):
        OutputGenerator.genCmd(self, cmdinfo, name, alias)

        if self.current_feature in self.extension_blacklist:
            return

        self.genNoVkPrefix = False
        vkdecls = self.makeCDecls(cmdinfo.elem)
        self.genNoVkPrefix = True
        decls = self.makeCDecls(cmdinfo.elem)

        if self.genOpts.helper_file_type == 'ext_list':
            return
        elif self.genOpts.helper_file_type == 'commands_header':
            self.appendSection('command', 'static %s' % (decls[0]))
            if (self.featureExtraProtect != None):
                self.intercepts += [ '#ifdef %s' % self.featureExtraProtect ]
            self.intercepts += [ '    {"%s", (void*)%s},' % (name,name[2:]) ]
            if (self.featureExtraProtect != None):
                self.intercepts += [ '#endif' ]
            return
        elif self.genOpts.helper_file_type == 'wsi_exports':
            # if command takes VkSurfaceKHR, then export it
            if any( param_t.text == 'VkSurfaceKHR' for param_t in cmdinfo.elem.findall('param/type')):
                result_type = cmdinfo.elem.find('proto/type')
                if result_type != None: return_string = 'return '
                else: return_string = ''
                param_list = ', '.join( param.text for param in cmdinfo.elem.findall('param/name'))

                self.appendSection('command', 'EXPORT %s{\n    %s%s(%s);\n}' % (vkdecls[0][:-1], return_string, name[2:], param_list))
            return

        # TODO(krOoze): lot of mess in below I do not want to detangle rn
        if name in MANUAL_COMMANDS:
            self.appendSection('command', '')
            if name not in CUSTOM_C_INTERCEPTS:
                pass
            else:
                self.appendSection('command', 'static %s' % (decls[0][:-1]))
                self.appendSection('command', '{\n%s}' % (CUSTOM_C_INTERCEPTS[name]))
            self.intercepts += [ '    {"%s", (void*)%s},' % (name,name[2:]) ]
            return
        # record that the function will be intercepted
        if (self.featureExtraProtect != None):
            self.intercepts += [ '#ifdef %s' % self.featureExtraProtect ]
        self.intercepts += [ '    {"%s", (void*)%s},' % (name,name[2:]) ]
        if (self.featureExtraProtect != None):
            self.intercepts += [ '#endif' ]
        #
        self.appendSection('command', '')
        self.appendSection('command', 'static %s' % (decls[0][:-1]))
        if name in CUSTOM_C_INTERCEPTS:
            self.appendSection('command', '{%s}' % (CUSTOM_C_INTERCEPTS[name]))
            return

        # Declare result variable, if any.
        resulttype = cmdinfo.elem.find('proto/type')
        if (resulttype != None and resulttype.text == 'void'):
            resulttype = None
        # if the name w/ KHR postfix is in the CUSTOM_C_INTERCEPTS
        # Call the KHR custom version instead of generating separate code
        khr_name = name + "KHR"
        if khr_name in CUSTOM_C_INTERCEPTS:
            return_string = ''
            if resulttype != None:
                return_string = 'return '
            params = cmdinfo.elem.findall('param/name')
            param_names = []
            for param in params:
                param_names.append(param.text)
            self.appendSection('command', '{\n    %s%s(%s);\n}' % (return_string, khr_name[2:], ", ".join(param_names)))
            return
        elif name in ALIAS_COMMANDS:
            return_string = ''
            if resulttype != None:
                return_string = 'return '
            params = cmdinfo.elem.findall('param/name')
            param_names = []
            for param in params:
                param_names.append(param.text)
            self.appendSection('command', '{\n    %s%s(%s);\n}' % (return_string, ALIAS_COMMANDS[name][2:], ", ".join(param_names)))
            return

        self.appendSection('command', '{')

        api_function_name = cmdinfo.elem.attrib.get('name')
        # GET THE TYPE OF FUNCTION
        if True in [ftxt in api_function_name for ftxt in ['Create', 'Allocate']]:
            # Get last param
            last_param = cmdinfo.elem.findall('param')[-1]
            lp_txt = last_param.find('name').text
            lp_len = None
            if ('len' in last_param.attrib):
                lp_len = last_param.attrib['len']
                lp_len = lp_len.replace('::', '->')
            lp_type = last_param.find('type').text
            handle_type = 'dispatchable'
            allocator_txt = 'CreateDispObjHandle()';
            if (self.isHandleTypeNonDispatchable(lp_type)):
                handle_type = 'non-' + handle_type
                allocator_txt = 'global_unique_handle++';
            # Need to lock in both cases
            self.appendSection('command', '    unique_lock_t lock(global_lock);')
            if (lp_len != None):
                #print("%s last params (%s) has len %s" % (handle_type, lp_txt, lp_len))
                self.appendSection('command', '    for (uint32_t i = 0; i < %s; ++i) {' % (lp_len))
                self.appendSection('command', '        %s[i] = (%s)%s;' % (lp_txt, lp_type, allocator_txt))
                self.appendSection('command', '    }')
            else:
                #print("Single %s last param is '%s' w/ type '%s'" % (handle_type, lp_txt, lp_type))
                if 'AllocateMemory' in api_function_name:
                    # Store allocation size in case it's mapped
                    self.appendSection('command', '    allocated_memory_size_map[(VkDeviceMemory)global_unique_handle] = pAllocateInfo->allocationSize;')
                self.appendSection('command', '    *%s = (%s)%s;' % (lp_txt, lp_type, allocator_txt))
        elif True in [ftxt in api_function_name for ftxt in ['Destroy', 'Free']]:
            self.appendSection('command', '//Destroy object')
            if 'FreeMemory' in api_function_name:
                # Remove from allocation map
                self.appendSection('command', '    allocated_memory_size_map.erase(memory);')
        else:
            self.appendSection('command', '//Not a CREATE or DESTROY function')

        # Return result variable, if any.
        if (resulttype != None):
            if api_function_name == 'vkGetEventStatus':
                self.appendSection('command', '    return VK_EVENT_SET;')
            else:
                self.appendSection('command', '    return VK_SUCCESS;')
        self.appendSection('command', '}')
    #
    # override makeProtoName to drop the "vk" prefix
    def makeProtoName(self, name, tail):
        if self.genNoVkPrefix:
            return self.genOpts.apientry + name[2:] + tail
        else:
            return self.genOpts.apientry + name + tail
