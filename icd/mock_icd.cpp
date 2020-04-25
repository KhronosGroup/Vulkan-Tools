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

#include "mock_icd.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

#define VK_NO_PROTOTYPES
#include "vulkan/vk_icd.h"

#include "generated/vk_typemap_helper.h"

namespace mock_icd {

using std::unordered_map;

static constexpr uint32_t kInstanceVersion = VK_API_VERSION_1_2;  // TODO(krOoze): autogen?

static constexpr uint32_t kPhysicalDeviceCount = 1;
static unordered_map<VkInstance, std::array<VkPhysicalDevice, kPhysicalDeviceCount>> physical_device_map;

// TODO(krOoze): more queues? protected?
static constexpr VkQueueFamilyProperties kQueueFamilyProps = {
    VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT, 1, 0, {1, 1, 1}};
static unordered_map<VkDevice, VkQueue> queue_map;

// Map device memory handle to any mapped allocations that we'll need to free on unmap
static unordered_map<VkDeviceMemory, std::vector<void*>> mapped_memory_map;

// Map device memory allocation handle to the size
static unordered_map<VkDeviceMemory, VkDeviceSize> allocated_memory_size_map;

static unordered_map<VkDevice, unordered_map<VkBuffer, VkBufferCreateInfo>> buffer_map;
static unordered_map<VkDevice, unordered_map<VkImage, VkDeviceSize>> image_memory_size_map;

static constexpr uint32_t icd_swapchain_image_count = 1;
static unordered_map<VkSwapchainKHR, VkImage[icd_swapchain_image_count]> swapchain_image_map;

// TODO: Would like to codegen this but limits aren't in XML
static VkPhysicalDeviceLimits SetLimits(VkPhysicalDeviceLimits* limits) {
    limits->maxImageDimension1D = 4096;
    limits->maxImageDimension2D = 4096;
    limits->maxImageDimension3D = 256;
    limits->maxImageDimensionCube = 4096;
    limits->maxImageArrayLayers = 256;
    limits->maxTexelBufferElements = 65536;
    limits->maxUniformBufferRange = 16384;
    limits->maxStorageBufferRange = 134217728;
    limits->maxPushConstantsSize = 128;
    limits->maxMemoryAllocationCount = 4096;
    limits->maxSamplerAllocationCount = 4000;
    limits->bufferImageGranularity = 1;
    limits->sparseAddressSpaceSize = 2147483648;
    limits->maxBoundDescriptorSets = 4;
    limits->maxPerStageDescriptorSamplers = 16;
    limits->maxPerStageDescriptorUniformBuffers = 12;
    limits->maxPerStageDescriptorStorageBuffers = 4;
    limits->maxPerStageDescriptorSampledImages = 16;
    limits->maxPerStageDescriptorStorageImages = 4;
    limits->maxPerStageDescriptorInputAttachments = 4;
    limits->maxPerStageResources = 128;
    limits->maxDescriptorSetSamplers = 96;
    limits->maxDescriptorSetUniformBuffers = 72;
    limits->maxDescriptorSetUniformBuffersDynamic = 8;
    limits->maxDescriptorSetStorageBuffers = 24;
    limits->maxDescriptorSetStorageBuffersDynamic = 4;
    limits->maxDescriptorSetSampledImages = 96;
    limits->maxDescriptorSetStorageImages = 24;
    limits->maxDescriptorSetInputAttachments = 4;
    limits->maxVertexInputAttributes = 16;
    limits->maxVertexInputBindings = 16;
    limits->maxVertexInputAttributeOffset = 2047;
    limits->maxVertexInputBindingStride = 2048;
    limits->maxVertexOutputComponents = 64;
    limits->maxTessellationGenerationLevel = 64;
    limits->maxTessellationPatchSize = 32;
    limits->maxTessellationControlPerVertexInputComponents = 64;
    limits->maxTessellationControlPerVertexOutputComponents = 64;
    limits->maxTessellationControlPerPatchOutputComponents = 120;
    limits->maxTessellationControlTotalOutputComponents = 2048;
    limits->maxTessellationEvaluationInputComponents = 64;
    limits->maxTessellationEvaluationOutputComponents = 64;
    limits->maxGeometryShaderInvocations = 32;
    limits->maxGeometryInputComponents = 64;
    limits->maxGeometryOutputComponents = 64;
    limits->maxGeometryOutputVertices = 256;
    limits->maxGeometryTotalOutputComponents = 1024;
    limits->maxFragmentInputComponents = 64;
    limits->maxFragmentOutputAttachments = 4;
    limits->maxFragmentDualSrcAttachments = 1;
    limits->maxFragmentCombinedOutputResources = 4;
    limits->maxComputeSharedMemorySize = 16384;
    limits->maxComputeWorkGroupCount[0] = 65535;
    limits->maxComputeWorkGroupCount[1] = 65535;
    limits->maxComputeWorkGroupCount[2] = 65535;
    limits->maxComputeWorkGroupInvocations = 128;
    limits->maxComputeWorkGroupSize[0] = 128;
    limits->maxComputeWorkGroupSize[1] = 128;
    limits->maxComputeWorkGroupSize[2] = 64;
    limits->subPixelPrecisionBits = 4;
    limits->subTexelPrecisionBits = 4;
    limits->mipmapPrecisionBits = 4;
    limits->maxDrawIndexedIndexValue = UINT32_MAX;
    limits->maxDrawIndirectCount = UINT16_MAX;
    limits->maxSamplerLodBias = 2.0f;
    limits->maxSamplerAnisotropy = 16;
    limits->maxViewports = 16;
    limits->maxViewportDimensions[0] = 4096;
    limits->maxViewportDimensions[1] = 4096;
    limits->viewportBoundsRange[0] = -8192;
    limits->viewportBoundsRange[1] = 8191;
    limits->viewportSubPixelBits = 0;
    limits->minMemoryMapAlignment = 64;
    limits->minTexelBufferOffsetAlignment = 16;
    limits->minUniformBufferOffsetAlignment = 16;
    limits->minStorageBufferOffsetAlignment = 16;
    limits->minTexelOffset = -8;
    limits->maxTexelOffset = 7;
    limits->minTexelGatherOffset = -8;
    limits->maxTexelGatherOffset = 7;
    limits->minInterpolationOffset = 0.0f;
    limits->maxInterpolationOffset = 0.5f;
    limits->subPixelInterpolationOffsetBits = 4;
    limits->maxFramebufferWidth = 4096;
    limits->maxFramebufferHeight = 4096;
    limits->maxFramebufferLayers = 256;
    limits->framebufferColorSampleCounts = 0x7F;
    limits->framebufferDepthSampleCounts = 0x7F;
    limits->framebufferStencilSampleCounts = 0x7F;
    limits->framebufferNoAttachmentsSampleCounts = 0x7F;
    limits->maxColorAttachments = 4;
    limits->sampledImageColorSampleCounts = 0x7F;
    limits->sampledImageIntegerSampleCounts = 0x7F;
    limits->sampledImageDepthSampleCounts = 0x7F;
    limits->sampledImageStencilSampleCounts = 0x7F;
    limits->storageImageSampleCounts = 0x7F;
    limits->maxSampleMaskWords = 1;
    limits->timestampComputeAndGraphics = VK_TRUE;
    limits->timestampPeriod = 1;
    limits->maxClipDistances = 8;
    limits->maxCullDistances = 8;
    limits->maxCombinedClipAndCullDistances = 8;
    limits->discreteQueuePriorities = 2;
    limits->pointSizeRange[0] = 1.0f;
    limits->pointSizeRange[1] = 64.0f;
    limits->lineWidthRange[0] = 1.0f;
    limits->lineWidthRange[1] = 8.0f;
    limits->pointSizeGranularity = 1.0f;
    limits->lineWidthGranularity = 1.0f;
    limits->strictLines = VK_TRUE;
    limits->standardSampleLocations = VK_TRUE;
    limits->optimalBufferCopyOffsetAlignment = 1;
    limits->optimalBufferCopyRowPitchAlignment = 1;
    limits->nonCoherentAtomSize = 256;

    return *limits;
}

void SetBoolArrayTrue(VkBool32* bool_array, uint32_t num_bools) { std::fill_n(bool_array, num_bools, VK_TRUE); }

static PFN_vkVoidFunction GetFptr(const unordered_map<std::string, void*>& fptr_map, const std::string& command_name) {
    // TODO(krOoze): Should filter based on enabled extensions.
    const auto& command_iter = fptr_map.find(command_name);
    if (command_iter != fptr_map.end())
        return reinterpret_cast<PFN_vkVoidFunction>(command_iter->second);
    else
        return nullptr;
}

template <typename T, std::size_t SIZE>
static VkResult Enumerate(const std::array<T, SIZE>& enumerable, uint32_t* count, T* out) {
    VkResult result_code = VK_SUCCESS;

    if (out) {
        const uint32_t return_count = (std::min)(*count, static_cast<uint32_t>(enumerable.size()));
        std::copy_n(enumerable.begin(), return_count, out);

        if (return_count < enumerable.size()) result_code = VK_INCOMPLETE;
        *count = return_count;
    } else {  // !out
        *count = static_cast<uint32_t>(enumerable.size());
    }

    return result_code;
}

template <std::size_t SIZE>
static VkResult EnumerateExtensions(const std::array<VkExtensionProperties, SIZE>& supported_extensions, const char* pLayerName,
                                    uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
    VkResult result_code = VK_SUCCESS;

    if (pLayerName) {
        assert(false);  // Loader should never call ICD with pLayerName
        *pPropertyCount = 0;
    } else {  // !pLayerName
        result_code = Enumerate(supported_extensions, pPropertyCount, pProperties);
    }

    return result_code;
}

}  // namespace mock_icd

#include "generated/mock_icd_commands.cpp.inc"
#include "mock_icd_custom_commands.cpp.inc"
#include "mock_icd_exports.cpp.inc"