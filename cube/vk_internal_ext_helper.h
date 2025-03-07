/*
************************************************************************************************************************
*
*  Copyright (C) 2014-2021 Advanced Micro Devices, Inc. All rights reserved.
*
***********************************************************************************************************************/
/**
 **********************************************************************************************************************
 * @file  vk_internal_ext_helper.h
 * @brief Helper header for unpublished extensions.
 **********************************************************************************************************************
 */

#ifndef VK_INTERNAL_EXT_HELPER_H_
#define VK_INTERNAL_EXT_HELPER_H_

#define VK_EXTENSION_ENUM_BASE_VALUE        1000000000ull
#define VK_EXTENSION_ENUM_RANGE_SIZE        1000ull

#define VK_EXTENSION_ENUM(extnr, type, offset) \
    ((type)(VK_EXTENSION_ENUM_BASE_VALUE + (((extnr)-1) * VK_EXTENSION_ENUM_RANGE_SIZE) + (offset)))

#define VK_EXTENSION_BIT(type, bit) \
    ((type)(1ull << (bit)))

#endif /* VK_INTERNAL_EXT_HELPER_H_ */
