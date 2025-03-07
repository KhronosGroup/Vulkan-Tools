/*
***********************************************************************************************************************
*
*  Copyright (c) 2025 Advanced Micro Devices, Inc. All rights reserved.
*
**********************************************************************************************************************/
/**
**********************************************************************************************************************
* @file  vk_khr_descriptor_heap.h
* @brief Temporary internal header for KHR descriptor heap extension. This file should be removed once the
*        extension is published and the API gets included in the official Vulkan header.
**********************************************************************************************************************
*/

//# NOTE: generated via script using scripts/devext.py

#ifndef VK_KHR_DESCRIPTOR_HEAP_H_
#define VK_KHR_DESCRIPTOR_HEAP_H_

#pragma once
#include "vk_internal_ext_helper.h"
#include "vk_khr_device_address_commands.h"

#define VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER 136

// extended core or other extension enums:
#define VK_BUFFER_USAGE_DESCRIPTOR_HEAP_BIT_KHR                                       VK_EXTENSION_BIT(VkBufferUsageFlagBits, 28)
#define VK_BUFFER_USAGE_2_DESCRIPTOR_HEAP_BIT_KHR                                     VK_EXTENSION_BIT(VkBufferUsageFlagBits2KHR, 28)
#define VK_PIPELINE_CREATE_2_DESCRIPTOR_HEAP_BIT_KHR                                  VK_EXTENSION_BIT(VkPipelineCreateFlagBits2KHR, 36)
#define VK_PIPELINE_CREATE_2_BUFFER_AND_IMAGE_DESCRIPTOR_HEAP_ADDRESSES_MATCH_BIT_KHR VK_EXTENSION_BIT(VkPipelineCreateFlagBits2KHR, 39)
#define VK_IMAGE_CREATE_DESCRIPTOR_HEAP_CAPTURE_REPLAY_BIT_KHR                        VK_EXTENSION_BIT(VkImageCreateFlagBits, 16)
#define VK_STRUCTURE_TYPE_BUFFER_DESCRIPTOR_INFO_KHR                                  VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 0)
#define VK_STRUCTURE_TYPE_IMAGE_DESCRIPTOR_TEXEL_BUFFER_INFO_KHR                      VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 1)
#define VK_STRUCTURE_TYPE_IMAGE_DESCRIPTOR_IMAGE_INFO_KHR                             VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 2)
#define VK_STRUCTURE_TYPE_IMAGE_DESCRIPTOR_INFO_KHR                                   VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 3)
#define VK_STRUCTURE_TYPE_BIND_HEAP_INFO_KHR                                          VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 4)
#define VK_STRUCTURE_TYPE_DESCRIPTOR_HEAP_STRIDES_KHR                                 VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 5)
#define VK_STRUCTURE_TYPE_PUSH_DATA_INFO_KHR                                          VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 6)
#define VK_STRUCTURE_TYPE_DESCRIPTOR_SET_AND_BINDING_MAPPING_KHR                      VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 7)
#define VK_STRUCTURE_TYPE_SHADER_DESCRIPTOR_SET_AND_BINDING_MAPPING_INFO_KHR          VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 8)
#define VK_ACCESS_2_SAMPLER_HEAP_READ_BIT_KHR                                         VK_EXTENSION_BIT(VkAccessFlagBits2, 57)
#define VK_ACCESS_2_IMAGE_HEAP_READ_BIT_KHR                                           VK_EXTENSION_BIT(VkAccessFlagBits2, 58)
#define VK_ACCESS_2_BUFFER_HEAP_READ_BIT_KHR                                          VK_EXTENSION_BIT(VkAccessFlagBits2, 59)
#define VK_STRUCTURE_TYPE_OPAQUE_CAPTURE_DATA_CREATE_INFO_KHR                         VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 9)
#define VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_PROPERTIES_KHR              VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 10)
#define VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_KHR                VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 11)
#define VK_STRUCTURE_TYPE_SAMPLER_CUSTOM_BORDER_COLOR_INDEX_CREATE_INFO_EXT           VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 12)
#define VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_DESCRIPTOR_HEAP_INFO_KHR         VK_EXTENSION_ENUM(VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NUMBER, VkStructureType, 14)

// copy-paste of new extension types/functions from merged header:
#define VK_KHR_descriptor_heap 1
#define VK_KHR_DESCRIPTOR_HEAP_SPEC_VERSION 1
#define VK_KHR_DESCRIPTOR_HEAP_EXTENSION_NAME "VK_KHR_descriptor_heap"


typedef enum VkDescriptorMappingSourceKHR {
    VK_DESCRIPTOR_MAPPING_SOURCE_HEAP_WITH_CONSTANT_INDEX_KHR      = 0,
    VK_DESCRIPTOR_MAPPING_SOURCE_HEAP_WITH_PUSH_INDEX_KHR          = 1,
    VK_DESCRIPTOR_MAPPING_SOURCE_HEAP_WITH_INDIRECT_INDEX_KHR      = 2,
    VK_DESCRIPTOR_MAPPING_SOURCE_HEAP_DATA_KHR                     = 3,
    VK_DESCRIPTOR_MAPPING_SOURCE_PUSH_DATA_KHR                     = 4,
    VK_DESCRIPTOR_MAPPING_SOURCE_PUSH_ADDRESS_KHR                  = 5,
    VK_DESCRIPTOR_MAPPING_SOURCE_HEAP_WITH_SHADER_RECORD_INDEX_KHR = 6,
    VK_DESCRIPTOR_MAPPING_SOURCE_SHADER_RECORD_DATA_KHR            = 7,
    VK_DESCRIPTOR_MAPPING_SOURCE_SHADER_RECORD_ADDRESS_KHR         = 8,
    VK_DESCRIPTOR_MAPPING_SOURCE_MAX_ENUM_KHR                      = 0x7FFFFFFF
} VkDescriptorMappingSourceKHR;

typedef struct VkHostAddressStridedRangeKHR {
    void*     pData;
    size_t    stride;
    size_t    size;
} VkHostAddressStridedRangeKHR;

typedef struct VkBufferDescriptorInfoKHR {
    VkStructureType            sType;
    const void*                pNext;
    VkDescriptorType           type;
    VkDeviceAddressRangeKHR    addressRange;
} VkBufferDescriptorInfoKHR;

typedef struct VkImageDescriptorTexelBufferInfoKHR {
    VkStructureType            sType;
    const void*                pNext;
    VkFormat                   format;
    VkDeviceAddressRangeKHR    addressRange;
} VkImageDescriptorTexelBufferInfoKHR;

typedef struct VkImageDescriptorImageInfoKHR {
    VkStructureType                 sType;
    const void*                     pNext;
    const VkImageViewCreateInfo*    pView;
    VkImageLayout                   layout;
} VkImageDescriptorImageInfoKHR;

typedef union VkImageDescriptorDataKHR {
    const VkImageDescriptorImageInfoKHR*          pImage;
    const VkImageDescriptorTexelBufferInfoKHR*    pTexelBuffer;
} VkImageDescriptorDataKHR;

typedef struct VkImageDescriptorInfoKHR {
    VkStructureType             sType;
    const void*                 pNext;
    VkDescriptorType            type;
    VkImageDescriptorDataKHR    data;
} VkImageDescriptorInfoKHR;

typedef struct VkBindHeapInfoKHR {
    VkStructureType            sType;
    const void*                pNext;
    VkDeviceAddressRangeKHR    heapRange;
    VkDeviceSize               reservedRangeOffset;
} VkBindHeapInfoKHR;

typedef struct VkDescriptorHeapStridesKHR {
    VkStructureType    sType;
    const void*        pNext;
    VkDeviceSize       samplerHeapStride;
    VkDeviceSize       imageHeapStride;
    VkDeviceSize       bufferHeapStride;
} VkDescriptorHeapStridesKHR;

typedef struct VkPushDataInfoKHR {
    VkStructureType    sType;
    const void*        pNext;
    uint32_t           offset;
    uint32_t           size;
    const void*        pData;
} VkPushDataInfoKHR;

typedef struct VkDescriptorMappingSourceConstantIndexKHR {
    int32_t                       heapIndex;
    const VkSamplerCreateInfo*    pEmbeddedSampler;
    VkBool32                      useCombinedImageSamplerIndex;
    int32_t                       samplerHeapIndex;
} VkDescriptorMappingSourceConstantIndexKHR;

typedef struct VkDescriptorMappingSourcePushIndexKHR {
    int32_t                       heapIndex;
    uint32_t                      pushOffset;
    const VkSamplerCreateInfo*    pEmbeddedSampler;
    VkBool32                      useCombinedImageSamplerIndex;
    int32_t                       samplerHeapIndex;
    uint32_t                      samplerPushOffset;
} VkDescriptorMappingSourcePushIndexKHR;

typedef struct VkDescriptorMappingSourceIndirectIndexKHR {
    int32_t                       heapIndex;
    uint32_t                      pushOffset;
    uint32_t                      addressOffset;
    const VkSamplerCreateInfo*    pEmbeddedSampler;
    VkBool32                      useCombinedImageSamplerIndex;
    int32_t                       samplerHeapIndex;
    uint32_t                      samplerPushOffset;
    uint32_t                      samplerAddressOffset;
} VkDescriptorMappingSourceIndirectIndexKHR;

typedef struct VkDescriptorMappingSourceHeapDataKHR {
    int32_t     heapOffset;
    uint32_t    pushOffset;
} VkDescriptorMappingSourceHeapDataKHR;

typedef struct VkDescriptorMappingSourceShaderRecordIndexKHR {
    int32_t                       heapIndex;
    uint32_t                      shaderRecordOffset;
    const VkSamplerCreateInfo*    pEmbeddedSampler;
    VkBool32                      useCombinedImageSamplerIndex;
    int32_t                       samplerHeapIndex;
    uint32_t                      samplerShaderRecordOffset;
} VkDescriptorMappingSourceShaderRecordIndexKHR;

typedef union VkDescriptorMappingSourceDataKHR {
    VkDescriptorMappingSourceConstantIndexKHR        constantIndex;
    VkDescriptorMappingSourcePushIndexKHR            pushIndex;
    VkDescriptorMappingSourceIndirectIndexKHR        indirectIndex;
    VkDescriptorMappingSourceHeapDataKHR             heapData;
    uint32_t                                         pushDataOffset;
    uint32_t                                         pushAddressOffset;
    VkDescriptorMappingSourceShaderRecordIndexKHR    shaderRecordIndex;
    uint32_t                                         shaderRecordDataOffset;
    uint32_t                                         shaderRecordAddressOffset;
} VkDescriptorMappingSourceDataKHR;

typedef struct VkDescriptorSetAndBindingMappingKHR {
    VkStructureType                     sType;
    const void*                         pNext;
    uint32_t                            descriptorSet;
    uint32_t                            firstBinding;
    uint32_t                            bindingCount;
    VkDescriptorMappingSourceKHR        source;
    VkDescriptorMappingSourceDataKHR    sourceData;
} VkDescriptorSetAndBindingMappingKHR;

typedef struct VkShaderDescriptorSetAndBindingMappingInfoKHR {
    VkStructureType                               sType;
    const void*                                   pNext;
    uint32_t                                      mappingCount;
    const VkDescriptorSetAndBindingMappingKHR*    pMappings;
} VkShaderDescriptorSetAndBindingMappingInfoKHR;

typedef struct VkOpaqueCaptureDataCreateInfoKHR {
    VkStructureType    sType;
    const void*        pNext;
    size_t             size;
    const void*        pData;
} VkOpaqueCaptureDataCreateInfoKHR;

typedef struct VkPhysicalDeviceDescriptorHeapFeaturesKHR {
    VkStructureType    sType;
    void*              pNext;
    VkBool32           descriptorHeap;
    VkBool32           descriptorHeapCaptureReplay;
} VkPhysicalDeviceDescriptorHeapFeaturesKHR;

typedef struct VkPhysicalDeviceDescriptorHeapPropertiesKHR {
    VkStructureType    sType;
    void*              pNext;
    VkDeviceSize       samplerHeapAlignment;
    VkDeviceSize       imageHeapAlignment;
    VkDeviceSize       bufferHeapAlignment;
    VkDeviceSize       samplerHeapDescriptorAlignment;
    VkDeviceSize       imageHeapDescriptorAlignment;
    VkDeviceSize       bufferHeapDescriptorAlignment;
    VkDeviceSize       maxSamplerHeapSize;
    VkDeviceSize       maxImageHeapSize;
    VkDeviceSize       maxBufferHeapSize;
    VkDeviceSize       samplerHeapReservedRange;
    VkDeviceSize       samplerHeapReservedRangeWithEmbedded;
    VkDeviceSize       imageHeapReservedRange;
    VkDeviceSize       bufferHeapReservedRange;
    VkDeviceSize       samplerDescriptorSize;
    VkDeviceSize       imageDescriptorSize;
    VkDeviceSize       bufferDescriptorSize;
    VkDeviceSize       maxPushDataSize;
    size_t             imageCaptureReplayOpaqueDataSize;
    uint32_t           maxDescriptorHeapEmbeddedSamplers;
    uint32_t           samplerYcbcrConversionCount;
    VkBool32           separateImageAndBufferHeaps;
    VkBool32           sparseDescriptorHeaps;
    VkBool32           protectedDescriptorHeaps;
} VkPhysicalDeviceDescriptorHeapPropertiesKHR;

typedef struct VkCommandBufferInheritanceDescriptorHeapInfoKHR {
    VkStructureType    sType;
    const void*        pNext;
    VkDeviceAddress    samplerHeap;
    VkDeviceAddress    imageHeap;
    VkDeviceAddress    bufferHeap;
} VkCommandBufferInheritanceDescriptorHeapInfoKHR;

typedef struct VkSamplerCustomBorderColorIndexCreateInfoEXT {
    VkStructureType    sType;
    const void*        pNext;
    uint32_t           index;
} VkSamplerCustomBorderColorIndexCreateInfoEXT;

typedef struct VkIndirectCommandsLayoutPushDataTokenNV {
    VkStructureType    sType;
    const void*        pNext;
    uint32_t           pushDataOffset;
    uint32_t           pushDataSize;
} VkIndirectCommandsLayoutPushDataTokenNV;

typedef VkResult (VKAPI_PTR *PFN_vkWriteSamplerDescriptorsKHR)(
    VkDevice                                            device,
    uint32_t                                            samplerCount,
    const VkSamplerCreateInfo*                          pSamplers,
    const VkHostAddressStridedRangeKHR*                 pDescriptors);

typedef VkResult (VKAPI_PTR *PFN_vkWriteImageDescriptorsKHR)(
    VkDevice                                            device,
    uint32_t                                            imageCount,
    const VkImageDescriptorInfoKHR*                     pImages,
    const VkHostAddressStridedRangeKHR*                 pDescriptors);

typedef VkResult (VKAPI_PTR *PFN_vkWriteBufferDescriptorsKHR)(
    VkDevice                                            device,
    uint32_t                                            bufferCount,
    const VkBufferDescriptorInfoKHR*                    pBuffers,
    const VkHostAddressStridedRangeKHR*                 pDescriptors);

typedef void (VKAPI_PTR *PFN_vkCmdBindSamplerHeapKHR)(
    VkCommandBuffer                                     commandBuffer,
    const VkBindHeapInfoKHR*                            pBindInfo);

typedef void (VKAPI_PTR *PFN_vkCmdBindImageHeapKHR)(
    VkCommandBuffer                                     commandBuffer,
    const VkBindHeapInfoKHR*                            pBindInfo);

typedef void (VKAPI_PTR *PFN_vkCmdBindBufferHeapKHR)(
    VkCommandBuffer                                     commandBuffer,
    const VkBindHeapInfoKHR*                            pBindInfo);

typedef void (VKAPI_PTR *PFN_vkCmdPushDataKHR)(
    VkCommandBuffer                                     commandBuffer,
    const VkPushDataInfoKHR*                            pPushDataInfo);

typedef VkResult (VKAPI_PTR *PFN_vkGetImageOpaqueCaptureDataKHR)(
    VkDevice                                            device,
    VkImage                                             image,
    size_t                                              size,
    void*                                               pData);

typedef VkDeviceSize (VKAPI_PTR *PFN_vkGetPhysicalDeviceDescriptorSizeKHR)(
    VkPhysicalDevice                                    physicalDevice,
    VkDescriptorType                                    descriptorType);

typedef VkResult (VKAPI_PTR *PFN_vkRegisterCustomBorderColorEXT)(
    VkDevice                                            device,
    const VkSamplerCustomBorderColorCreateInfoEXT*      pBorderColor,
    VkBool32                                            requestIndex,
    uint32_t*                                           pIndex);

typedef void (VKAPI_PTR *PFN_vkUnregisterCustomBorderColorEXT)(
    VkDevice                                            device,
    uint32_t                                            index);

#ifndef VK_NO_PROTOTYPES
VKAPI_ATTR VkResult VKAPI_CALL vkWriteSamplerDescriptorsKHR(
    VkDevice                                    device,
    uint32_t                                    samplerCount,
    const VkSamplerCreateInfo*                  pSamplers,
    const VkHostAddressStridedRangeKHR*         pDescriptors);

VKAPI_ATTR VkResult VKAPI_CALL vkWriteImageDescriptorsKHR(
    VkDevice                                    device,
    uint32_t                                    imageCount,
    const VkImageDescriptorInfoKHR*             pImages,
    const VkHostAddressStridedRangeKHR*         pDescriptors);

VKAPI_ATTR VkResult VKAPI_CALL vkWriteBufferDescriptorsKHR(
    VkDevice                                    device,
    uint32_t                                    bufferCount,
    const VkBufferDescriptorInfoKHR*            pBuffers,
    const VkHostAddressStridedRangeKHR*         pDescriptors);

VKAPI_ATTR void VKAPI_CALL vkCmdBindSamplerHeapKHR(
    VkCommandBuffer                             commandBuffer,
    const VkBindHeapInfoKHR*                    pBindInfo);

VKAPI_ATTR void VKAPI_CALL vkCmdBindImageHeapKHR(
    VkCommandBuffer                             commandBuffer,
    const VkBindHeapInfoKHR*                    pBindInfo);

VKAPI_ATTR void VKAPI_CALL vkCmdBindBufferHeapKHR(
    VkCommandBuffer                             commandBuffer,
    const VkBindHeapInfoKHR*                    pBindInfo);

VKAPI_ATTR void VKAPI_CALL vkCmdPushDataKHR(
    VkCommandBuffer                             commandBuffer,
    const VkPushDataInfoKHR*                    pPushDataInfo);

VKAPI_ATTR VkResult VKAPI_CALL vkGetImageOpaqueCaptureDataKHR(
    VkDevice                                    device,
    VkImage                                     image,
    size_t                                      size,
    void*                                       pData);

VKAPI_ATTR VkDeviceSize VKAPI_CALL vkGetPhysicalDeviceDescriptorSizeKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDescriptorType                            descriptorType);

VKAPI_ATTR VkResult VKAPI_CALL vkRegisterCustomBorderColorEXT(
    VkDevice                                    device,
    const VkSamplerCustomBorderColorCreateInfoEXT* pBorderColor,
    VkBool32                                    requestIndex,
    uint32_t*                                   pIndex);

VKAPI_ATTR void VKAPI_CALL vkUnregisterCustomBorderColorEXT(
    VkDevice                                    device,
    uint32_t                                    index);
#endif

#endif // VK_KHR_DESCRIPTOR_HEAP_H_
