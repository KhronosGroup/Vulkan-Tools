/*
************************************************************************************************************************
*
*  Copyright (C) 2022 Advanced Micro Devices, Inc. All rights reserved.
*
***********************************************************************************************************************/
/**
**********************************************************************************************************************
* @file  vk_khr_device_address_commands.h
* @brief Temporary internal header for KHR pipeline binaries extension. This file should be removed once the
*        extension is published and the API gets included in the official Vulkan header.
**********************************************************************************************************************
*/

#ifndef VK_KHR_DEVICE_ADDRESS_COMMANDS_H_
#define VK_KHR_DEVICE_ADDRESS_COMMANDS_H_

#define VK_KHR_device_address_commands                      1
#define VK_KHR_DEVICE_ADDRESS_COMMANDS_SPEC_VERSION         1
#define VK_KHR_DEVICE_ADDRESS_COMMANDS_EXTENSION_NAME       "VK_KHR_device_address_commands"

// ====================================================================================================================
typedef struct VkDeviceAddressRangeKHR {
    VkDeviceAddress     address;
    VkDeviceSize        size;
} VkDeviceAddressRangeKHR;

typedef VkBufferCopy2 VkMemoryCopyKHR;

typedef struct VkCopyMemoryInfoKHR {
    VkStructureType                             sType;
    const void*                                 pNext;
    VkDeviceAddressRangeKHR                     srcRange;
    VkDeviceAddressRangeKHR                     dstRange;
    uint32_t                                    copyCount;
    const VkMemoryCopyKHR*                      pCopies;
} VkCopyMemoryInfoKHR;

typedef VkBufferImageCopy2 VkMemoryImageCopyKHR;

typedef struct VkMemoryRangeBarrierKHR {
    VkStructureType                             sType;
    const void*                                 pNext;
    VkPipelineStageFlags2                       srcStageMask;
    VkAccessFlags2                              srcAccessMask;
    VkPipelineStageFlags2                       dstStageMask;
    VkAccessFlags2                              dstAccessMask;
    uint32_t                                    srcQueueFamilyIndex;
    uint32_t                                    dstQueueFamilyIndex;
    VkDeviceAddressRangeKHR                     addressRange;
} VkMemoryRangeBarrierKHR;

typedef struct VkCopyMemoryImageInfoKHR {
    VkStructureType                             sType;
    const void*                                 pNext;
    VkDeviceAddressRangeKHR                     addressRange;
    VkImage                                     image;
    uint32_t                                    copyCount;
    const VkMemoryImageCopyKHR*                 pCopies;
} VkCopyMemoryImageInfoKHR;

typedef struct VkMemoryRangeBarriersInfoKHR {
    VkStructureType                             sType;
    const void*                                 pNext;
    uint32_t                                    memoryRangeBarrierCount;
    const VkMemoryRangeBarrierKHR*              pMemoryRangeBarriers;
} VkMemoryRangeBarriersInfoKHR;

typedef struct VkConditionalRenderingBeginInfo2EXT {
    VkStructureType                             sType;
    const void*                                 pNext;
    VkDeviceAddressRangeKHR                     addressRange;
    VkConditionalRenderingFlagsEXT              flags;
} VkConditionalRenderingBeginInfo2EXT;

// ====================================================================================================================
typedef void(VKAPI_PTR* PFN_vkCmdBindIndexAddressKHR)(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    VkIndexType                                 indexType);

typedef void(VKAPI_PTR* PFN_vkCmdBindVertexAddressesKHR)(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkDeviceAddressRangeKHR*              pRanges,
    const VkDeviceSize*                         pStrides);

typedef void(VKAPI_PTR* PFN_vkCmdDrawIndirect2KHR)(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    uint32_t                                    drawCount,
    uint32_t                                    stride);

typedef void(VKAPI_PTR* PFN_vkCmdDrawIndexedIndirect2KHR)(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    uint32_t                                    drawCount,
    uint32_t                                    stride);

typedef void(VKAPI_PTR* PFN_vkCmdDispatchIndirect2KHR)(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range);

typedef void(VKAPI_PTR* PFN_vkCmdDrawIndirectCount2KHR)(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    VkDeviceAddressRangeKHR                     countRange,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride);

typedef void(VKAPI_PTR* PFN_vkCmdDrawIndexedIndirectCount2KHR)(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    VkDeviceAddressRangeKHR                     countRange,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride);

typedef void(VKAPI_PTR* PFN_vkCmdDrawMeshTasksIndirect2EXT)(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    uint32_t                                    drawCount,
    uint32_t                                    stride);

typedef void(VKAPI_PTR* PFN_vkCmdDrawMeshTasksIndirectCount2EXT)(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    VkDeviceAddressRangeKHR                     countRange,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride);

typedef void(VKAPI_PTR* PFN_vkCmdCopyMemoryKHR)(
    VkCommandBuffer                             commandBuffer,
    const VkCopyMemoryInfoKHR                   pCopyMemoryInfo);

typedef void(VKAPI_PTR* PFN_vkCmdCopyMemoryToImageKHR)(
    VkCommandBuffer                             commandBuffer,
    const VkCopyMemoryImageInfoKHR              pCopyMemoryInfo);

typedef void(VKAPI_PTR* PFN_vkCmdCopyImageToMemoryKHR)(
    VkCommandBuffer                             commandBuffer,
    const VkCopyMemoryImageInfoKHR              pCopyMemoryInfo);

typedef void(VKAPI_PTR* PFN_vkCmdUpdateMemoryKHR)(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     dstRange,
    VkDeviceSize                                dataSize,
    const void*                                 pData);

typedef void(VKAPI_PTR* PFN_vkCmdFillMemoryKHR)(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    uint32_t                                    data);

typedef void(VKAPI_PTR* PFN_vkCmdCopyQueryPoolResults2KHR)(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    VkDeviceAddressRangeKHR                     range,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags);

typedef void(VKAPI_PTR* PFN_vkCmdBeginConditionalRendering2EXT)(
    VkCommandBuffer                             commandBuffer,
    const VkConditionalRenderingBeginInfo2EXT* pConditionalRenderingBegin);

typedef void(VKAPI_PTR* PFN_vkCmdBindTransformFeedbackBuffers2EXT)(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkDeviceAddressRangeKHR*              pRanges);

typedef void(VKAPI_PTR* PFN_vkCmdBeginTransformFeedback2EXT)(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterRange,
    uint32_t                                    counterRangeCount,
    const VkDeviceAddressRangeKHR*              pCounterRanges);

typedef void(VKAPI_PTR* PFN_vkCmdEndTransformFeedback2EXT)(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterRange,
    uint32_t                                    counterRangeCount,
    const VkDeviceAddressRangeKHR*              pCounterRanges);

typedef void(VKAPI_PTR* PFN_vkCmdDrawIndirectByteCount2EXT)(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    instanceCount,
    uint32_t                                    firstInstance,
    VkDeviceAddressRangeKHR                     counterRange,
    uint32_t                                    counterOffset,
    uint32_t                                    vertexStride);

typedef void(VKAPI_PTR* PFN_vkCmdWriteBufferMarker3AMD)(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlags2KHR                    stage,
    VkDeviceAddress                             dstAddress,
    uint32_t                                    marker);

#ifndef VK_NO_PROTOTYPES
// ====================================================================================================================
// Index and Vertex Binding
void vkCmdBindIndexAddressKHR(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    VkIndexType                                 indexType);

void vkCmdBindVertexAddressesKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkDeviceAddressRangeKHR*              pRanges,
    const VkDeviceSize*                         pStrides);

// ====================================================================================================================
// Indirect Commands
void vkCmdDrawIndirect2KHR(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    uint32_t                                    drawCount,
    uint32_t                                    stride);

void vkCmdDrawIndexedIndirect2KHR(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    uint32_t                                    drawCount,
    uint32_t                                    stride);

void vkCmdDispatchIndirect2KHR(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range);

void vkCmdDrawIndirectCount2KHR(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    VkDeviceAddressRangeKHR                     countRange,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride);

void vkCmdDrawIndexedIndirectCount2KHR(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    VkDeviceAddressRangeKHR                     countRange,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride);

void vkCmdDrawMeshTasksIndirect2EXT(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    uint32_t                                    drawCount,
    uint32_t                                    stride);

void vkCmdDrawMeshTasksIndirectCount2EXT(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    VkDeviceAddressRangeKHR                     countRange,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride);

// ====================================================================================================================
// Transfer Commands
void vkCmdCopyMemoryKHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyMemoryInfoKHR                   pCopyMemoryInfo);

void vkCmdCopyMemoryToImageKHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyMemoryImageInfoKHR              pCopyMemoryInfo);

void vkCmdCopyImageToMemoryKHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyMemoryImageInfoKHR              pCopyMemoryInfo);

void vkCmdUpdateMemoryKHR(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     dstRange,
    VkDeviceSize                                dataSize,
    const void*                                 pData);

void vkCmdFillMemoryKHR(
    VkCommandBuffer                             commandBuffer,
    VkDeviceAddressRangeKHR                     range,
    uint32_t                                    data);

void vkCmdCopyQueryPoolResults2KHR(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    VkDeviceAddressRangeKHR                     range,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags);

// ====================================================================================================================
// Conditional Rendering
void vkCmdBeginConditionalRendering2EXT(
    VkCommandBuffer                             commandBuffer,
    const VkConditionalRenderingBeginInfo2EXT*  pConditionalRenderingBegin);

// ====================================================================================================================
// Transform Feedback
void vkCmdBindTransformFeedbackBuffers2EXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkDeviceAddressRangeKHR*              pRanges);

void vkCmdBeginTransformFeedback2EXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterRange,
    uint32_t                                    counterRangeCount,
    const VkDeviceAddressRangeKHR*              pCounterRanges);

void vkCmdEndTransformFeedback2EXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterRange,
    uint32_t                                    counterRangeCount,
    const VkDeviceAddressRangeKHR*              pCounterRanges);

void vkCmdDrawIndirectByteCount2EXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    instanceCount,
    uint32_t                                    firstInstance,
    VkDeviceAddressRangeKHR                     counterRange,
    uint32_t                                    counterOffset,
    uint32_t                                    vertexStride);

// ====================================================================================================================
// Buffer Markers
void vkCmdWriteBufferMarker3AMD(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlags2KHR                    stage,
    VkDeviceAddress                             dstAddress,
    uint32_t                                    marker);
#endif

#endif /* VK_KHR_KHR_DEVICE_ADDRESS_COMMANDS_H_ */
