
/*
 * Copyright (c) 2019 The Khronos Group Inc.
 * Copyright (c) 2019 Valve Corporation
 * Copyright (c) 2019 LunarG, Inc.
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
 * Author: Charles Giessen <charles@lunarg.com>
 *
 */

/*
 * This file is generated from the Khronos Vulkan XML API Registry.
 */

#include "vulkaninfo.h"
#include "outputprinter.h"

std::ostream &operator<<(std::ostream &o, VkConformanceVersionKHR &c) {
    return o << std::to_string(c.major) << "." << std::to_string(c.minor) << "." << std::to_string(c.subminor) << "."
             << std::to_string(c.patch);
}

std::string VkExtent3DString(VkExtent3D e) {
    return std::string("(") + std::to_string(e.width) + ", " + std::to_string(e.height) + ", " + std::to_string(e.depth) + ")";
}

template <typename T>
std::string to_hex_str(T i) {
    std::stringstream stream;
    stream << "0x" << std::setfill('0') << std::setw(sizeof(T)) << std::hex << i;
    return stream.str();
}

template <typename T>
std::string to_hex_str(Printer &p, T i) {
    if (p.Type() == OutputType::json)
        return std::to_string(i);
    else
        return to_hex_str(i);
}
static const char *VkColorSpaceKHRString(VkColorSpaceKHR value) {
    switch (value) {
        case (0): return "COLOR_SPACE_SRGB_NONLINEAR_KHR";
        case (1000104001): return "COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT";
        case (1000104002): return "COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT";
        case (1000104003): return "COLOR_SPACE_DISPLAY_P3_LINEAR_EXT";
        case (1000104004): return "COLOR_SPACE_DCI_P3_NONLINEAR_EXT";
        case (1000104005): return "COLOR_SPACE_BT709_LINEAR_EXT";
        case (1000104006): return "COLOR_SPACE_BT709_NONLINEAR_EXT";
        case (1000104007): return "COLOR_SPACE_BT2020_LINEAR_EXT";
        case (1000104008): return "COLOR_SPACE_HDR10_ST2084_EXT";
        case (1000104009): return "COLOR_SPACE_DOLBYVISION_EXT";
        case (1000104010): return "COLOR_SPACE_HDR10_HLG_EXT";
        case (1000104011): return "COLOR_SPACE_ADOBERGB_LINEAR_EXT";
        case (1000104012): return "COLOR_SPACE_ADOBERGB_NONLINEAR_EXT";
        case (1000104013): return "COLOR_SPACE_PASS_THROUGH_EXT";
        case (1000104014): return "COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT";
        case (1000213000): return "COLOR_SPACE_DISPLAY_NATIVE_AMD";
        default: return "UNKNOWN_VkColorSpaceKHR";
    }
}
void DumpVkColorSpaceKHR(Printer &p, std::string name, VkColorSpaceKHR value, int width = 0) {
    if (p.Type() == OutputType::json) {
        p.PrintKeyValue(name, value, width);
        return;
    } else {
        p.PrintKeyValue(name, VkColorSpaceKHRString(value), width);
    }
}
static const char *VkDriverIdKHRString(VkDriverIdKHR value) {
    switch (value) {
        case (1): return "DRIVER_ID_AMD_PROPRIETARY_KHR";
        case (2): return "DRIVER_ID_AMD_OPEN_SOURCE_KHR";
        case (3): return "DRIVER_ID_MESA_RADV_KHR";
        case (4): return "DRIVER_ID_NVIDIA_PROPRIETARY_KHR";
        case (5): return "DRIVER_ID_INTEL_PROPRIETARY_WINDOWS_KHR";
        case (6): return "DRIVER_ID_INTEL_OPEN_SOURCE_MESA_KHR";
        case (7): return "DRIVER_ID_IMAGINATION_PROPRIETARY_KHR";
        case (8): return "DRIVER_ID_QUALCOMM_PROPRIETARY_KHR";
        case (9): return "DRIVER_ID_ARM_PROPRIETARY_KHR";
        case (10): return "DRIVER_ID_GOOGLE_SWIFTSHADER_KHR";
        case (11): return "DRIVER_ID_GGP_PROPRIETARY_KHR";
        case (12): return "DRIVER_ID_BROADCOM_PROPRIETARY_KHR";
        default: return "UNKNOWN_VkDriverIdKHR";
    }
}
void DumpVkDriverIdKHR(Printer &p, std::string name, VkDriverIdKHR value, int width = 0) {
    if (p.Type() == OutputType::json) {
        p.PrintKeyValue(name, value, width);
        return;
    } else {
        p.PrintKeyValue(name, VkDriverIdKHRString(value), width);
    }
}
static const char *VkFormatString(VkFormat value) {
    switch (value) {
        case (0): return "FORMAT_UNDEFINED";
        case (1): return "FORMAT_R4G4_UNORM_PACK8";
        case (2): return "FORMAT_R4G4B4A4_UNORM_PACK16";
        case (3): return "FORMAT_B4G4R4A4_UNORM_PACK16";
        case (4): return "FORMAT_R5G6B5_UNORM_PACK16";
        case (5): return "FORMAT_B5G6R5_UNORM_PACK16";
        case (6): return "FORMAT_R5G5B5A1_UNORM_PACK16";
        case (7): return "FORMAT_B5G5R5A1_UNORM_PACK16";
        case (8): return "FORMAT_A1R5G5B5_UNORM_PACK16";
        case (9): return "FORMAT_R8_UNORM";
        case (10): return "FORMAT_R8_SNORM";
        case (11): return "FORMAT_R8_USCALED";
        case (12): return "FORMAT_R8_SSCALED";
        case (13): return "FORMAT_R8_UINT";
        case (14): return "FORMAT_R8_SINT";
        case (15): return "FORMAT_R8_SRGB";
        case (16): return "FORMAT_R8G8_UNORM";
        case (17): return "FORMAT_R8G8_SNORM";
        case (18): return "FORMAT_R8G8_USCALED";
        case (19): return "FORMAT_R8G8_SSCALED";
        case (20): return "FORMAT_R8G8_UINT";
        case (21): return "FORMAT_R8G8_SINT";
        case (22): return "FORMAT_R8G8_SRGB";
        case (23): return "FORMAT_R8G8B8_UNORM";
        case (24): return "FORMAT_R8G8B8_SNORM";
        case (25): return "FORMAT_R8G8B8_USCALED";
        case (26): return "FORMAT_R8G8B8_SSCALED";
        case (27): return "FORMAT_R8G8B8_UINT";
        case (28): return "FORMAT_R8G8B8_SINT";
        case (29): return "FORMAT_R8G8B8_SRGB";
        case (30): return "FORMAT_B8G8R8_UNORM";
        case (31): return "FORMAT_B8G8R8_SNORM";
        case (32): return "FORMAT_B8G8R8_USCALED";
        case (33): return "FORMAT_B8G8R8_SSCALED";
        case (34): return "FORMAT_B8G8R8_UINT";
        case (35): return "FORMAT_B8G8R8_SINT";
        case (36): return "FORMAT_B8G8R8_SRGB";
        case (37): return "FORMAT_R8G8B8A8_UNORM";
        case (38): return "FORMAT_R8G8B8A8_SNORM";
        case (39): return "FORMAT_R8G8B8A8_USCALED";
        case (40): return "FORMAT_R8G8B8A8_SSCALED";
        case (41): return "FORMAT_R8G8B8A8_UINT";
        case (42): return "FORMAT_R8G8B8A8_SINT";
        case (43): return "FORMAT_R8G8B8A8_SRGB";
        case (44): return "FORMAT_B8G8R8A8_UNORM";
        case (45): return "FORMAT_B8G8R8A8_SNORM";
        case (46): return "FORMAT_B8G8R8A8_USCALED";
        case (47): return "FORMAT_B8G8R8A8_SSCALED";
        case (48): return "FORMAT_B8G8R8A8_UINT";
        case (49): return "FORMAT_B8G8R8A8_SINT";
        case (50): return "FORMAT_B8G8R8A8_SRGB";
        case (51): return "FORMAT_A8B8G8R8_UNORM_PACK32";
        case (52): return "FORMAT_A8B8G8R8_SNORM_PACK32";
        case (53): return "FORMAT_A8B8G8R8_USCALED_PACK32";
        case (54): return "FORMAT_A8B8G8R8_SSCALED_PACK32";
        case (55): return "FORMAT_A8B8G8R8_UINT_PACK32";
        case (56): return "FORMAT_A8B8G8R8_SINT_PACK32";
        case (57): return "FORMAT_A8B8G8R8_SRGB_PACK32";
        case (58): return "FORMAT_A2R10G10B10_UNORM_PACK32";
        case (59): return "FORMAT_A2R10G10B10_SNORM_PACK32";
        case (60): return "FORMAT_A2R10G10B10_USCALED_PACK32";
        case (61): return "FORMAT_A2R10G10B10_SSCALED_PACK32";
        case (62): return "FORMAT_A2R10G10B10_UINT_PACK32";
        case (63): return "FORMAT_A2R10G10B10_SINT_PACK32";
        case (64): return "FORMAT_A2B10G10R10_UNORM_PACK32";
        case (65): return "FORMAT_A2B10G10R10_SNORM_PACK32";
        case (66): return "FORMAT_A2B10G10R10_USCALED_PACK32";
        case (67): return "FORMAT_A2B10G10R10_SSCALED_PACK32";
        case (68): return "FORMAT_A2B10G10R10_UINT_PACK32";
        case (69): return "FORMAT_A2B10G10R10_SINT_PACK32";
        case (70): return "FORMAT_R16_UNORM";
        case (71): return "FORMAT_R16_SNORM";
        case (72): return "FORMAT_R16_USCALED";
        case (73): return "FORMAT_R16_SSCALED";
        case (74): return "FORMAT_R16_UINT";
        case (75): return "FORMAT_R16_SINT";
        case (76): return "FORMAT_R16_SFLOAT";
        case (77): return "FORMAT_R16G16_UNORM";
        case (78): return "FORMAT_R16G16_SNORM";
        case (79): return "FORMAT_R16G16_USCALED";
        case (80): return "FORMAT_R16G16_SSCALED";
        case (81): return "FORMAT_R16G16_UINT";
        case (82): return "FORMAT_R16G16_SINT";
        case (83): return "FORMAT_R16G16_SFLOAT";
        case (84): return "FORMAT_R16G16B16_UNORM";
        case (85): return "FORMAT_R16G16B16_SNORM";
        case (86): return "FORMAT_R16G16B16_USCALED";
        case (87): return "FORMAT_R16G16B16_SSCALED";
        case (88): return "FORMAT_R16G16B16_UINT";
        case (89): return "FORMAT_R16G16B16_SINT";
        case (90): return "FORMAT_R16G16B16_SFLOAT";
        case (91): return "FORMAT_R16G16B16A16_UNORM";
        case (92): return "FORMAT_R16G16B16A16_SNORM";
        case (93): return "FORMAT_R16G16B16A16_USCALED";
        case (94): return "FORMAT_R16G16B16A16_SSCALED";
        case (95): return "FORMAT_R16G16B16A16_UINT";
        case (96): return "FORMAT_R16G16B16A16_SINT";
        case (97): return "FORMAT_R16G16B16A16_SFLOAT";
        case (98): return "FORMAT_R32_UINT";
        case (99): return "FORMAT_R32_SINT";
        case (100): return "FORMAT_R32_SFLOAT";
        case (101): return "FORMAT_R32G32_UINT";
        case (102): return "FORMAT_R32G32_SINT";
        case (103): return "FORMAT_R32G32_SFLOAT";
        case (104): return "FORMAT_R32G32B32_UINT";
        case (105): return "FORMAT_R32G32B32_SINT";
        case (106): return "FORMAT_R32G32B32_SFLOAT";
        case (107): return "FORMAT_R32G32B32A32_UINT";
        case (108): return "FORMAT_R32G32B32A32_SINT";
        case (109): return "FORMAT_R32G32B32A32_SFLOAT";
        case (110): return "FORMAT_R64_UINT";
        case (111): return "FORMAT_R64_SINT";
        case (112): return "FORMAT_R64_SFLOAT";
        case (113): return "FORMAT_R64G64_UINT";
        case (114): return "FORMAT_R64G64_SINT";
        case (115): return "FORMAT_R64G64_SFLOAT";
        case (116): return "FORMAT_R64G64B64_UINT";
        case (117): return "FORMAT_R64G64B64_SINT";
        case (118): return "FORMAT_R64G64B64_SFLOAT";
        case (119): return "FORMAT_R64G64B64A64_UINT";
        case (120): return "FORMAT_R64G64B64A64_SINT";
        case (121): return "FORMAT_R64G64B64A64_SFLOAT";
        case (122): return "FORMAT_B10G11R11_UFLOAT_PACK32";
        case (123): return "FORMAT_E5B9G9R9_UFLOAT_PACK32";
        case (124): return "FORMAT_D16_UNORM";
        case (125): return "FORMAT_X8_D24_UNORM_PACK32";
        case (126): return "FORMAT_D32_SFLOAT";
        case (127): return "FORMAT_S8_UINT";
        case (128): return "FORMAT_D16_UNORM_S8_UINT";
        case (129): return "FORMAT_D24_UNORM_S8_UINT";
        case (130): return "FORMAT_D32_SFLOAT_S8_UINT";
        case (131): return "FORMAT_BC1_RGB_UNORM_BLOCK";
        case (132): return "FORMAT_BC1_RGB_SRGB_BLOCK";
        case (133): return "FORMAT_BC1_RGBA_UNORM_BLOCK";
        case (134): return "FORMAT_BC1_RGBA_SRGB_BLOCK";
        case (135): return "FORMAT_BC2_UNORM_BLOCK";
        case (136): return "FORMAT_BC2_SRGB_BLOCK";
        case (137): return "FORMAT_BC3_UNORM_BLOCK";
        case (138): return "FORMAT_BC3_SRGB_BLOCK";
        case (139): return "FORMAT_BC4_UNORM_BLOCK";
        case (140): return "FORMAT_BC4_SNORM_BLOCK";
        case (141): return "FORMAT_BC5_UNORM_BLOCK";
        case (142): return "FORMAT_BC5_SNORM_BLOCK";
        case (143): return "FORMAT_BC6H_UFLOAT_BLOCK";
        case (144): return "FORMAT_BC6H_SFLOAT_BLOCK";
        case (145): return "FORMAT_BC7_UNORM_BLOCK";
        case (146): return "FORMAT_BC7_SRGB_BLOCK";
        case (147): return "FORMAT_ETC2_R8G8B8_UNORM_BLOCK";
        case (148): return "FORMAT_ETC2_R8G8B8_SRGB_BLOCK";
        case (149): return "FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK";
        case (150): return "FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK";
        case (151): return "FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK";
        case (152): return "FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK";
        case (153): return "FORMAT_EAC_R11_UNORM_BLOCK";
        case (154): return "FORMAT_EAC_R11_SNORM_BLOCK";
        case (155): return "FORMAT_EAC_R11G11_UNORM_BLOCK";
        case (156): return "FORMAT_EAC_R11G11_SNORM_BLOCK";
        case (157): return "FORMAT_ASTC_4x4_UNORM_BLOCK";
        case (158): return "FORMAT_ASTC_4x4_SRGB_BLOCK";
        case (159): return "FORMAT_ASTC_5x4_UNORM_BLOCK";
        case (160): return "FORMAT_ASTC_5x4_SRGB_BLOCK";
        case (161): return "FORMAT_ASTC_5x5_UNORM_BLOCK";
        case (162): return "FORMAT_ASTC_5x5_SRGB_BLOCK";
        case (163): return "FORMAT_ASTC_6x5_UNORM_BLOCK";
        case (164): return "FORMAT_ASTC_6x5_SRGB_BLOCK";
        case (165): return "FORMAT_ASTC_6x6_UNORM_BLOCK";
        case (166): return "FORMAT_ASTC_6x6_SRGB_BLOCK";
        case (167): return "FORMAT_ASTC_8x5_UNORM_BLOCK";
        case (168): return "FORMAT_ASTC_8x5_SRGB_BLOCK";
        case (169): return "FORMAT_ASTC_8x6_UNORM_BLOCK";
        case (170): return "FORMAT_ASTC_8x6_SRGB_BLOCK";
        case (171): return "FORMAT_ASTC_8x8_UNORM_BLOCK";
        case (172): return "FORMAT_ASTC_8x8_SRGB_BLOCK";
        case (173): return "FORMAT_ASTC_10x5_UNORM_BLOCK";
        case (174): return "FORMAT_ASTC_10x5_SRGB_BLOCK";
        case (175): return "FORMAT_ASTC_10x6_UNORM_BLOCK";
        case (176): return "FORMAT_ASTC_10x6_SRGB_BLOCK";
        case (177): return "FORMAT_ASTC_10x8_UNORM_BLOCK";
        case (178): return "FORMAT_ASTC_10x8_SRGB_BLOCK";
        case (179): return "FORMAT_ASTC_10x10_UNORM_BLOCK";
        case (180): return "FORMAT_ASTC_10x10_SRGB_BLOCK";
        case (181): return "FORMAT_ASTC_12x10_UNORM_BLOCK";
        case (182): return "FORMAT_ASTC_12x10_SRGB_BLOCK";
        case (183): return "FORMAT_ASTC_12x12_UNORM_BLOCK";
        case (184): return "FORMAT_ASTC_12x12_SRGB_BLOCK";
        case (1000156000): return "FORMAT_G8B8G8R8_422_UNORM";
        case (1000156001): return "FORMAT_B8G8R8G8_422_UNORM";
        case (1000156002): return "FORMAT_G8_B8_R8_3PLANE_420_UNORM";
        case (1000156003): return "FORMAT_G8_B8R8_2PLANE_420_UNORM";
        case (1000156004): return "FORMAT_G8_B8_R8_3PLANE_422_UNORM";
        case (1000156005): return "FORMAT_G8_B8R8_2PLANE_422_UNORM";
        case (1000156006): return "FORMAT_G8_B8_R8_3PLANE_444_UNORM";
        case (1000156007): return "FORMAT_R10X6_UNORM_PACK16";
        case (1000156008): return "FORMAT_R10X6G10X6_UNORM_2PACK16";
        case (1000156009): return "FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16";
        case (1000156010): return "FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16";
        case (1000156011): return "FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16";
        case (1000156012): return "FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16";
        case (1000156013): return "FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16";
        case (1000156014): return "FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16";
        case (1000156015): return "FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16";
        case (1000156016): return "FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16";
        case (1000156017): return "FORMAT_R12X4_UNORM_PACK16";
        case (1000156018): return "FORMAT_R12X4G12X4_UNORM_2PACK16";
        case (1000156019): return "FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16";
        case (1000156020): return "FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16";
        case (1000156021): return "FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16";
        case (1000156022): return "FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16";
        case (1000156023): return "FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16";
        case (1000156024): return "FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16";
        case (1000156025): return "FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16";
        case (1000156026): return "FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16";
        case (1000156027): return "FORMAT_G16B16G16R16_422_UNORM";
        case (1000156028): return "FORMAT_B16G16R16G16_422_UNORM";
        case (1000156029): return "FORMAT_G16_B16_R16_3PLANE_420_UNORM";
        case (1000156030): return "FORMAT_G16_B16R16_2PLANE_420_UNORM";
        case (1000156031): return "FORMAT_G16_B16_R16_3PLANE_422_UNORM";
        case (1000156032): return "FORMAT_G16_B16R16_2PLANE_422_UNORM";
        case (1000156033): return "FORMAT_G16_B16_R16_3PLANE_444_UNORM";
        case (1000054000): return "FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG";
        case (1000054001): return "FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG";
        case (1000054002): return "FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG";
        case (1000054003): return "FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG";
        case (1000054004): return "FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG";
        case (1000054005): return "FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG";
        case (1000054006): return "FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG";
        case (1000054007): return "FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG";
        case (1000066000): return "FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT";
        case (1000066001): return "FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT";
        case (1000066002): return "FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT";
        case (1000066003): return "FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT";
        case (1000066004): return "FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT";
        case (1000066005): return "FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT";
        case (1000066006): return "FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT";
        case (1000066007): return "FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT";
        case (1000066008): return "FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT";
        case (1000066009): return "FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT";
        case (1000066010): return "FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT";
        case (1000066011): return "FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT";
        case (1000066012): return "FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT";
        case (1000066013): return "FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT";
        default: return "UNKNOWN_VkFormat";
    }
}
void DumpVkFormat(Printer &p, std::string name, VkFormat value, int width = 0) {
    if (p.Type() == OutputType::json) {
        p.PrintKeyValue(name, value, width);
        return;
    } else {
        p.PrintKeyValue(name, VkFormatString(value), width);
    }
}
static const char *VkImageTilingString(VkImageTiling value) {
    switch (value) {
        case (0): return "IMAGE_TILING_OPTIMAL";
        case (1): return "IMAGE_TILING_LINEAR";
        case (1000158000): return "IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT";
        default: return "UNKNOWN_VkImageTiling";
    }
}
void DumpVkImageTiling(Printer &p, std::string name, VkImageTiling value, int width = 0) {
    if (p.Type() == OutputType::json) {
        p.PrintKeyValue(name, value, width);
        return;
    } else {
        p.PrintKeyValue(name, VkImageTilingString(value), width);
    }
}
static const char *VkPhysicalDeviceTypeString(VkPhysicalDeviceType value) {
    switch (value) {
        case (0): return "PHYSICAL_DEVICE_TYPE_OTHER";
        case (1): return "PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
        case (2): return "PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
        case (3): return "PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
        case (4): return "PHYSICAL_DEVICE_TYPE_CPU";
        default: return "UNKNOWN_VkPhysicalDeviceType";
    }
}
void DumpVkPhysicalDeviceType(Printer &p, std::string name, VkPhysicalDeviceType value, int width = 0) {
    if (p.Type() == OutputType::json) {
        p.PrintKeyValue(name, value, width);
        return;
    } else {
        p.PrintKeyValue(name, VkPhysicalDeviceTypeString(value), width);
    }
}
static const char *VkPointClippingBehaviorString(VkPointClippingBehavior value) {
    switch (value) {
        case (0): return "POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES";
        case (1): return "POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY";
        default: return "UNKNOWN_VkPointClippingBehavior";
    }
}
void DumpVkPointClippingBehavior(Printer &p, std::string name, VkPointClippingBehavior value, int width = 0) {
    if (p.Type() == OutputType::json) {
        p.PrintKeyValue(name, value, width);
        return;
    } else {
        p.PrintKeyValue(name, VkPointClippingBehaviorString(value), width);
    }
}
static const char *VkPresentModeKHRString(VkPresentModeKHR value) {
    switch (value) {
        case (0): return "PRESENT_MODE_IMMEDIATE_KHR";
        case (1): return "PRESENT_MODE_MAILBOX_KHR";
        case (2): return "PRESENT_MODE_FIFO_KHR";
        case (3): return "PRESENT_MODE_FIFO_RELAXED_KHR";
        case (1000111000): return "PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR";
        case (1000111001): return "PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR";
        default: return "UNKNOWN_VkPresentModeKHR";
    }
}
void DumpVkPresentModeKHR(Printer &p, std::string name, VkPresentModeKHR value, int width = 0) {
    if (p.Type() == OutputType::json) {
        p.PrintKeyValue(name, value, width);
        return;
    } else {
        p.PrintKeyValue(name, VkPresentModeKHRString(value), width);
    }
}
static const char *VkResultString(VkResult value) {
    switch (value) {
        case (0): return "SUCCESS";
        case (1): return "NOT_READY";
        case (2): return "TIMEOUT";
        case (3): return "EVENT_SET";
        case (4): return "EVENT_RESET";
        case (5): return "INCOMPLETE";
        case (-1): return "ERROR_OUT_OF_HOST_MEMORY";
        case (-2): return "ERROR_OUT_OF_DEVICE_MEMORY";
        case (-3): return "ERROR_INITIALIZATION_FAILED";
        case (-4): return "ERROR_DEVICE_LOST";
        case (-5): return "ERROR_MEMORY_MAP_FAILED";
        case (-6): return "ERROR_LAYER_NOT_PRESENT";
        case (-7): return "ERROR_EXTENSION_NOT_PRESENT";
        case (-8): return "ERROR_FEATURE_NOT_PRESENT";
        case (-9): return "ERROR_INCOMPATIBLE_DRIVER";
        case (-10): return "ERROR_TOO_MANY_OBJECTS";
        case (-11): return "ERROR_FORMAT_NOT_SUPPORTED";
        case (-12): return "ERROR_FRAGMENTED_POOL";
        case (-1000069000): return "ERROR_OUT_OF_POOL_MEMORY";
        case (-1000072003): return "ERROR_INVALID_EXTERNAL_HANDLE";
        case (-1000000000): return "ERROR_SURFACE_LOST_KHR";
        case (-1000000001): return "ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case (1000001003): return "SUBOPTIMAL_KHR";
        case (-1000001004): return "ERROR_OUT_OF_DATE_KHR";
        case (-1000003001): return "ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case (-1000011001): return "ERROR_VALIDATION_FAILED_EXT";
        case (-1000012000): return "ERROR_INVALID_SHADER_NV";
        case (-1000158000): return "ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case (-1000161000): return "ERROR_FRAGMENTATION_EXT";
        case (-1000174001): return "ERROR_NOT_PERMITTED_EXT";
        case (-1000244000): return "ERROR_INVALID_DEVICE_ADDRESS_EXT";
        case (-1000255000): return "ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        default: return "UNKNOWN_VkResult";
    }
}
void DumpVkResult(Printer &p, std::string name, VkResult value, int width = 0) {
    if (p.Type() == OutputType::json) {
        p.PrintKeyValue(name, value, width);
        return;
    } else {
        p.PrintKeyValue(name, VkResultString(value), width);
    }
}
static const char *VkShaderFloatControlsIndependenceKHRString(VkShaderFloatControlsIndependenceKHR value) {
    switch (value) {
        case (0): return "SHADER_FLOAT_CONTROLS_INDEPENDENCE_32_BIT_ONLY_KHR";
        case (1): return "SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL_KHR";
        case (2): return "SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE_KHR";
        default: return "UNKNOWN_VkShaderFloatControlsIndependenceKHR";
    }
}
void DumpVkShaderFloatControlsIndependenceKHR(Printer &p, std::string name, VkShaderFloatControlsIndependenceKHR value, int width = 0) {
    if (p.Type() == OutputType::json) {
        p.PrintKeyValue(name, value, width);
        return;
    } else {
        p.PrintKeyValue(name, VkShaderFloatControlsIndependenceKHRString(value), width);
    }
}
void DumpVkCompositeAlphaFlagsKHR(Printer &p, std::string name, VkCompositeAlphaFlagBitsKHR value, int width = 0) {
    if (value == 0) p.PrintElement("None");
    if (1 & value) p.SetAsType().PrintElement("COMPOSITE_ALPHA_OPAQUE_BIT_KHR");
    if (2 & value) p.SetAsType().PrintElement("COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR");
    if (4 & value) p.SetAsType().PrintElement("COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR");
    if (8 & value) p.SetAsType().PrintElement("COMPOSITE_ALPHA_INHERIT_BIT_KHR");
}
void DumpVkCompositeAlphaFlagsKHR(Printer &p, std::string name, VkCompositeAlphaFlagsKHR value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkCompositeAlphaFlagsKHR(p, name, static_cast<VkCompositeAlphaFlagBitsKHR>(value), width);
    p.ObjectEnd();
}
void DumpVkCompositeAlphaFlagBitsKHR(Printer &p, std::string name, VkCompositeAlphaFlagBitsKHR value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkCompositeAlphaFlagsKHR(p, name, value, width);
    p.ObjectEnd();
}
void DumpVkDeviceGroupPresentModeFlagsKHR(Printer &p, std::string name, VkDeviceGroupPresentModeFlagBitsKHR value, int width = 0) {
    if (value == 0) p.PrintElement("None");
    if (1 & value) p.SetAsType().PrintElement("DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR");
    if (2 & value) p.SetAsType().PrintElement("DEVICE_GROUP_PRESENT_MODE_REMOTE_BIT_KHR");
    if (4 & value) p.SetAsType().PrintElement("DEVICE_GROUP_PRESENT_MODE_SUM_BIT_KHR");
    if (8 & value) p.SetAsType().PrintElement("DEVICE_GROUP_PRESENT_MODE_LOCAL_MULTI_DEVICE_BIT_KHR");
}
void DumpVkDeviceGroupPresentModeFlagsKHR(Printer &p, std::string name, VkDeviceGroupPresentModeFlagsKHR value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkDeviceGroupPresentModeFlagsKHR(p, name, static_cast<VkDeviceGroupPresentModeFlagBitsKHR>(value), width);
    p.ObjectEnd();
}
void DumpVkDeviceGroupPresentModeFlagBitsKHR(Printer &p, std::string name, VkDeviceGroupPresentModeFlagBitsKHR value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkDeviceGroupPresentModeFlagsKHR(p, name, value, width);
    p.ObjectEnd();
}
void DumpVkFormatFeatureFlags(Printer &p, std::string name, VkFormatFeatureFlagBits value, int width = 0) {
    if (value == 0) p.PrintElement("None");
    if (1 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_SAMPLED_IMAGE_BIT");
    if (2 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_STORAGE_IMAGE_BIT");
    if (4 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT");
    if (8 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT");
    if (16 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT");
    if (32 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT");
    if (64 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_VERTEX_BUFFER_BIT");
    if (128 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_COLOR_ATTACHMENT_BIT");
    if (256 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT");
    if (512 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT");
    if (1024 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_BLIT_SRC_BIT");
    if (2048 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_BLIT_DST_BIT");
    if (4096 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT");
    if (16384 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_TRANSFER_SRC_BIT");
    if (32768 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_TRANSFER_DST_BIT");
    if (131072 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT");
    if (262144 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT");
    if (524288 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT");
    if (1048576 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT");
    if (2097152 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT");
    if (4194304 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_DISJOINT_BIT");
    if (8388608 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT");
    if (8192 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG");
    if (134217728 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_RESERVED_27_BIT_KHR");
    if (268435456 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_RESERVED_28_BIT_KHR");
    if (33554432 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_RESERVED_25_BIT_KHR");
    if (67108864 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_RESERVED_26_BIT_KHR");
    if (65536 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT");
    if (16777216 & value) p.SetAsType().PrintElement("FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT");
}
void DumpVkFormatFeatureFlags(Printer &p, std::string name, VkFormatFeatureFlags value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkFormatFeatureFlags(p, name, static_cast<VkFormatFeatureFlagBits>(value), width);
    p.ObjectEnd();
}
void DumpVkFormatFeatureFlagBits(Printer &p, std::string name, VkFormatFeatureFlagBits value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkFormatFeatureFlags(p, name, value, width);
    p.ObjectEnd();
}
void DumpVkImageUsageFlags(Printer &p, std::string name, VkImageUsageFlagBits value, int width = 0) {
    if (value == 0) p.PrintElement("None");
    if (1 & value) p.SetAsType().PrintElement("IMAGE_USAGE_TRANSFER_SRC_BIT");
    if (2 & value) p.SetAsType().PrintElement("IMAGE_USAGE_TRANSFER_DST_BIT");
    if (4 & value) p.SetAsType().PrintElement("IMAGE_USAGE_SAMPLED_BIT");
    if (8 & value) p.SetAsType().PrintElement("IMAGE_USAGE_STORAGE_BIT");
    if (16 & value) p.SetAsType().PrintElement("IMAGE_USAGE_COLOR_ATTACHMENT_BIT");
    if (32 & value) p.SetAsType().PrintElement("IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT");
    if (64 & value) p.SetAsType().PrintElement("IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT");
    if (128 & value) p.SetAsType().PrintElement("IMAGE_USAGE_INPUT_ATTACHMENT_BIT");
    if (8192 & value) p.SetAsType().PrintElement("IMAGE_USAGE_RESERVED_13_BIT_KHR");
    if (16384 & value) p.SetAsType().PrintElement("IMAGE_USAGE_RESERVED_14_BIT_KHR");
    if (32768 & value) p.SetAsType().PrintElement("IMAGE_USAGE_RESERVED_15_BIT_KHR");
    if (1024 & value) p.SetAsType().PrintElement("IMAGE_USAGE_RESERVED_10_BIT_KHR");
    if (2048 & value) p.SetAsType().PrintElement("IMAGE_USAGE_RESERVED_11_BIT_KHR");
    if (4096 & value) p.SetAsType().PrintElement("IMAGE_USAGE_RESERVED_12_BIT_KHR");
    if (256 & value) p.SetAsType().PrintElement("IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV");
    if (512 & value) p.SetAsType().PrintElement("IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT");
}
void DumpVkImageUsageFlags(Printer &p, std::string name, VkImageUsageFlags value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkImageUsageFlags(p, name, static_cast<VkImageUsageFlagBits>(value), width);
    p.ObjectEnd();
}
void DumpVkImageUsageFlagBits(Printer &p, std::string name, VkImageUsageFlagBits value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkImageUsageFlags(p, name, value, width);
    p.ObjectEnd();
}
void DumpVkMemoryHeapFlags(Printer &p, std::string name, VkMemoryHeapFlagBits value, int width = 0) {
    if (value == 0) p.PrintElement("None");
    if (1 & value) p.SetAsType().PrintElement("MEMORY_HEAP_DEVICE_LOCAL_BIT");
    if (2 & value) p.SetAsType().PrintElement("MEMORY_HEAP_MULTI_INSTANCE_BIT");
}
void DumpVkMemoryHeapFlags(Printer &p, std::string name, VkMemoryHeapFlags value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkMemoryHeapFlags(p, name, static_cast<VkMemoryHeapFlagBits>(value), width);
    p.ObjectEnd();
}
void DumpVkMemoryHeapFlagBits(Printer &p, std::string name, VkMemoryHeapFlagBits value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkMemoryHeapFlags(p, name, value, width);
    p.ObjectEnd();
}
void DumpVkMemoryPropertyFlags(Printer &p, std::string name, VkMemoryPropertyFlagBits value, int width = 0) {
    if (value == 0) p.PrintElement("None");
    if (1 & value) p.SetAsType().PrintElement("MEMORY_PROPERTY_DEVICE_LOCAL_BIT");
    if (2 & value) p.SetAsType().PrintElement("MEMORY_PROPERTY_HOST_VISIBLE_BIT");
    if (4 & value) p.SetAsType().PrintElement("MEMORY_PROPERTY_HOST_COHERENT_BIT");
    if (8 & value) p.SetAsType().PrintElement("MEMORY_PROPERTY_HOST_CACHED_BIT");
    if (16 & value) p.SetAsType().PrintElement("MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT");
    if (32 & value) p.SetAsType().PrintElement("MEMORY_PROPERTY_PROTECTED_BIT");
    if (64 & value) p.SetAsType().PrintElement("MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD");
    if (128 & value) p.SetAsType().PrintElement("MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD");
}
void DumpVkMemoryPropertyFlags(Printer &p, std::string name, VkMemoryPropertyFlags value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkMemoryPropertyFlags(p, name, static_cast<VkMemoryPropertyFlagBits>(value), width);
    p.ObjectEnd();
}
void DumpVkMemoryPropertyFlagBits(Printer &p, std::string name, VkMemoryPropertyFlagBits value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkMemoryPropertyFlags(p, name, value, width);
    p.ObjectEnd();
}
std::string VkQueueFlagsString(VkQueueFlags value, int width = 0) {
    std::string out;
    bool is_first = true;
    if (1 & value) {
        if (is_first) { is_first = false; } else { out += " | "; }
        out += "QUEUE_GRAPHICS";
    }
    if (2 & value) {
        if (is_first) { is_first = false; } else { out += " | "; }
        out += "QUEUE_COMPUTE";
    }
    if (4 & value) {
        if (is_first) { is_first = false; } else { out += " | "; }
        out += "QUEUE_TRANSFER";
    }
    if (8 & value) {
        if (is_first) { is_first = false; } else { out += " | "; }
        out += "QUEUE_SPARSE_BINDING";
    }
    if (16 & value) {
        if (is_first) { is_first = false; } else { out += " | "; }
        out += "QUEUE_PROTECTED";
    }
    if (64 & value) {
        if (is_first) { is_first = false; } else { out += " | "; }
        out += "QUEUE_RESERVED_6_BIT_KHR";
    }
    if (32 & value) {
        if (is_first) { is_first = false; } else { out += " | "; }
        out += "QUEUE_RESERVED_5_BIT_KHR";
    }
    return out;
}
void DumpVkResolveModeFlagsKHR(Printer &p, std::string name, VkResolveModeFlagBitsKHR value, int width = 0) {
    if (value == 0) p.PrintElement("None");
    if (0 & value) p.SetAsType().PrintElement("RESOLVE_MODE_NONE_KHR");
    if (1 & value) p.SetAsType().PrintElement("RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR");
    if (2 & value) p.SetAsType().PrintElement("RESOLVE_MODE_AVERAGE_BIT_KHR");
    if (4 & value) p.SetAsType().PrintElement("RESOLVE_MODE_MIN_BIT_KHR");
    if (8 & value) p.SetAsType().PrintElement("RESOLVE_MODE_MAX_BIT_KHR");
}
void DumpVkResolveModeFlagsKHR(Printer &p, std::string name, VkResolveModeFlagsKHR value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkResolveModeFlagsKHR(p, name, static_cast<VkResolveModeFlagBitsKHR>(value), width);
    p.ObjectEnd();
}
void DumpVkResolveModeFlagBitsKHR(Printer &p, std::string name, VkResolveModeFlagBitsKHR value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkResolveModeFlagsKHR(p, name, value, width);
    p.ObjectEnd();
}
void DumpVkSampleCountFlags(Printer &p, std::string name, VkSampleCountFlagBits value, int width = 0) {
    if (value == 0) p.PrintElement("None");
    if (1 & value) p.SetAsType().PrintElement("SAMPLE_COUNT_1_BIT");
    if (2 & value) p.SetAsType().PrintElement("SAMPLE_COUNT_2_BIT");
    if (4 & value) p.SetAsType().PrintElement("SAMPLE_COUNT_4_BIT");
    if (8 & value) p.SetAsType().PrintElement("SAMPLE_COUNT_8_BIT");
    if (16 & value) p.SetAsType().PrintElement("SAMPLE_COUNT_16_BIT");
    if (32 & value) p.SetAsType().PrintElement("SAMPLE_COUNT_32_BIT");
    if (64 & value) p.SetAsType().PrintElement("SAMPLE_COUNT_64_BIT");
}
void DumpVkSampleCountFlags(Printer &p, std::string name, VkSampleCountFlags value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkSampleCountFlags(p, name, static_cast<VkSampleCountFlagBits>(value), width);
    p.ObjectEnd();
}
void DumpVkSampleCountFlagBits(Printer &p, std::string name, VkSampleCountFlagBits value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkSampleCountFlags(p, name, value, width);
    p.ObjectEnd();
}
void DumpVkShaderStageFlags(Printer &p, std::string name, VkShaderStageFlagBits value, int width = 0) {
    if (value == 0) p.PrintElement("None");
    if (1 & value) p.SetAsType().PrintElement("SHADER_STAGE_VERTEX_BIT");
    if (2 & value) p.SetAsType().PrintElement("SHADER_STAGE_TESSELLATION_CONTROL_BIT");
    if (4 & value) p.SetAsType().PrintElement("SHADER_STAGE_TESSELLATION_EVALUATION_BIT");
    if (8 & value) p.SetAsType().PrintElement("SHADER_STAGE_GEOMETRY_BIT");
    if (16 & value) p.SetAsType().PrintElement("SHADER_STAGE_FRAGMENT_BIT");
    if (32 & value) p.SetAsType().PrintElement("SHADER_STAGE_COMPUTE_BIT");
    if (0x0000001F & value) p.SetAsType().PrintElement("SHADER_STAGE_ALL_GRAPHICS");
    if (0x7FFFFFFF & value) p.SetAsType().PrintElement("SHADER_STAGE_ALL");
    if (256 & value) p.SetAsType().PrintElement("SHADER_STAGE_RAYGEN_BIT_NV");
    if (512 & value) p.SetAsType().PrintElement("SHADER_STAGE_ANY_HIT_BIT_NV");
    if (1024 & value) p.SetAsType().PrintElement("SHADER_STAGE_CLOSEST_HIT_BIT_NV");
    if (2048 & value) p.SetAsType().PrintElement("SHADER_STAGE_MISS_BIT_NV");
    if (4096 & value) p.SetAsType().PrintElement("SHADER_STAGE_INTERSECTION_BIT_NV");
    if (8192 & value) p.SetAsType().PrintElement("SHADER_STAGE_CALLABLE_BIT_NV");
    if (64 & value) p.SetAsType().PrintElement("SHADER_STAGE_TASK_BIT_NV");
    if (128 & value) p.SetAsType().PrintElement("SHADER_STAGE_MESH_BIT_NV");
}
void DumpVkShaderStageFlags(Printer &p, std::string name, VkShaderStageFlags value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkShaderStageFlags(p, name, static_cast<VkShaderStageFlagBits>(value), width);
    p.ObjectEnd();
}
void DumpVkShaderStageFlagBits(Printer &p, std::string name, VkShaderStageFlagBits value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkShaderStageFlags(p, name, value, width);
    p.ObjectEnd();
}
void DumpVkSubgroupFeatureFlags(Printer &p, std::string name, VkSubgroupFeatureFlagBits value, int width = 0) {
    if (value == 0) p.PrintElement("None");
    if (1 & value) p.SetAsType().PrintElement("SUBGROUP_FEATURE_BASIC_BIT");
    if (2 & value) p.SetAsType().PrintElement("SUBGROUP_FEATURE_VOTE_BIT");
    if (4 & value) p.SetAsType().PrintElement("SUBGROUP_FEATURE_ARITHMETIC_BIT");
    if (8 & value) p.SetAsType().PrintElement("SUBGROUP_FEATURE_BALLOT_BIT");
    if (16 & value) p.SetAsType().PrintElement("SUBGROUP_FEATURE_SHUFFLE_BIT");
    if (32 & value) p.SetAsType().PrintElement("SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT");
    if (64 & value) p.SetAsType().PrintElement("SUBGROUP_FEATURE_CLUSTERED_BIT");
    if (128 & value) p.SetAsType().PrintElement("SUBGROUP_FEATURE_QUAD_BIT");
    if (256 & value) p.SetAsType().PrintElement("SUBGROUP_FEATURE_PARTITIONED_BIT_NV");
}
void DumpVkSubgroupFeatureFlags(Printer &p, std::string name, VkSubgroupFeatureFlags value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkSubgroupFeatureFlags(p, name, static_cast<VkSubgroupFeatureFlagBits>(value), width);
    p.ObjectEnd();
}
void DumpVkSubgroupFeatureFlagBits(Printer &p, std::string name, VkSubgroupFeatureFlagBits value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkSubgroupFeatureFlags(p, name, value, width);
    p.ObjectEnd();
}
void DumpVkSurfaceTransformFlagsKHR(Printer &p, std::string name, VkSurfaceTransformFlagBitsKHR value, int width = 0) {
    if (value == 0) p.PrintElement("None");
    if (1 & value) p.SetAsType().PrintElement("SURFACE_TRANSFORM_IDENTITY_BIT_KHR");
    if (2 & value) p.SetAsType().PrintElement("SURFACE_TRANSFORM_ROTATE_90_BIT_KHR");
    if (4 & value) p.SetAsType().PrintElement("SURFACE_TRANSFORM_ROTATE_180_BIT_KHR");
    if (8 & value) p.SetAsType().PrintElement("SURFACE_TRANSFORM_ROTATE_270_BIT_KHR");
    if (16 & value) p.SetAsType().PrintElement("SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR");
    if (32 & value) p.SetAsType().PrintElement("SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR");
    if (64 & value) p.SetAsType().PrintElement("SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR");
    if (128 & value) p.SetAsType().PrintElement("SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR");
    if (256 & value) p.SetAsType().PrintElement("SURFACE_TRANSFORM_INHERIT_BIT_KHR");
}
void DumpVkSurfaceTransformFlagsKHR(Printer &p, std::string name, VkSurfaceTransformFlagsKHR value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkSurfaceTransformFlagsKHR(p, name, static_cast<VkSurfaceTransformFlagBitsKHR>(value), width);
    p.ObjectEnd();
}
void DumpVkSurfaceTransformFlagBitsKHR(Printer &p, std::string name, VkSurfaceTransformFlagBitsKHR value, int width = 0) {
    if (p.Type() == OutputType::json) { p.PrintKeyValue(name, value); return; }
    p.ObjectStart(name);
    DumpVkSurfaceTransformFlagsKHR(p, name, value, width);
    p.ObjectEnd();
}
void DumpVkDrmFormatModifierPropertiesEXT(Printer &p, std::string name, VkDrmFormatModifierPropertiesEXT &obj);
void DumpVkDrmFormatModifierPropertiesListEXT(Printer &p, std::string name, VkDrmFormatModifierPropertiesListEXT &obj);
void DumpVkExtent2D(Printer &p, std::string name, VkExtent2D &obj);
void DumpVkExtent3D(Printer &p, std::string name, VkExtent3D &obj);
void DumpVkLayerProperties(Printer &p, std::string name, VkLayerProperties &obj);
void DumpVkPhysicalDevice16BitStorageFeatures(Printer &p, std::string name, VkPhysicalDevice16BitStorageFeatures &obj);
void DumpVkPhysicalDevice8BitStorageFeaturesKHR(Printer &p, std::string name, VkPhysicalDevice8BitStorageFeaturesKHR &obj);
void DumpVkPhysicalDeviceASTCDecodeFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceASTCDecodeFeaturesEXT &obj);
void DumpVkPhysicalDeviceBlendOperationAdvancedFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT &obj);
void DumpVkPhysicalDeviceBlendOperationAdvancedPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT &obj);
void DumpVkPhysicalDeviceBufferDeviceAddressFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceBufferDeviceAddressFeaturesEXT &obj);
void DumpVkPhysicalDeviceConditionalRenderingFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceConditionalRenderingFeaturesEXT &obj);
void DumpVkPhysicalDeviceConservativeRasterizationPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceConservativeRasterizationPropertiesEXT &obj);
void DumpVkPhysicalDeviceDepthClipEnableFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceDepthClipEnableFeaturesEXT &obj);
void DumpVkPhysicalDeviceDepthStencilResolvePropertiesKHR(Printer &p, std::string name, VkPhysicalDeviceDepthStencilResolvePropertiesKHR &obj);
void DumpVkPhysicalDeviceDescriptorIndexingFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceDescriptorIndexingFeaturesEXT &obj);
void DumpVkPhysicalDeviceDescriptorIndexingPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceDescriptorIndexingPropertiesEXT &obj);
void DumpVkPhysicalDeviceDiscardRectanglePropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceDiscardRectanglePropertiesEXT &obj);
void DumpVkPhysicalDeviceDriverPropertiesKHR(Printer &p, std::string name, VkPhysicalDeviceDriverPropertiesKHR &obj);
void DumpVkPhysicalDeviceExternalMemoryHostPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceExternalMemoryHostPropertiesEXT &obj);
void DumpVkPhysicalDeviceFeatures(Printer &p, std::string name, VkPhysicalDeviceFeatures &obj);
void DumpVkPhysicalDeviceFloatControlsPropertiesKHR(Printer &p, std::string name, VkPhysicalDeviceFloatControlsPropertiesKHR &obj);
void DumpVkPhysicalDeviceFragmentDensityMapFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceFragmentDensityMapFeaturesEXT &obj);
void DumpVkPhysicalDeviceFragmentDensityMapPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceFragmentDensityMapPropertiesEXT &obj);
void DumpVkPhysicalDeviceFragmentShaderInterlockFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT &obj);
void DumpVkPhysicalDeviceHostQueryResetFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceHostQueryResetFeaturesEXT &obj);
void DumpVkPhysicalDeviceIDProperties(Printer &p, std::string name, VkPhysicalDeviceIDProperties &obj);
void DumpVkPhysicalDeviceImagelessFramebufferFeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceImagelessFramebufferFeaturesKHR &obj);
void DumpVkPhysicalDeviceIndexTypeUint8FeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceIndexTypeUint8FeaturesEXT &obj);
void DumpVkPhysicalDeviceInlineUniformBlockFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceInlineUniformBlockFeaturesEXT &obj);
void DumpVkPhysicalDeviceInlineUniformBlockPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceInlineUniformBlockPropertiesEXT &obj);
void DumpVkPhysicalDeviceLimits(Printer &p, std::string name, VkPhysicalDeviceLimits &obj);
void DumpVkPhysicalDeviceLineRasterizationFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceLineRasterizationFeaturesEXT &obj);
void DumpVkPhysicalDeviceLineRasterizationPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceLineRasterizationPropertiesEXT &obj);
void DumpVkPhysicalDeviceMaintenance3Properties(Printer &p, std::string name, VkPhysicalDeviceMaintenance3Properties &obj);
void DumpVkPhysicalDeviceMemoryBudgetPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceMemoryBudgetPropertiesEXT &obj);
void DumpVkPhysicalDeviceMemoryPriorityFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceMemoryPriorityFeaturesEXT &obj);
void DumpVkPhysicalDeviceMultiviewFeatures(Printer &p, std::string name, VkPhysicalDeviceMultiviewFeatures &obj);
void DumpVkPhysicalDeviceMultiviewProperties(Printer &p, std::string name, VkPhysicalDeviceMultiviewProperties &obj);
void DumpVkPhysicalDevicePCIBusInfoPropertiesEXT(Printer &p, std::string name, VkPhysicalDevicePCIBusInfoPropertiesEXT &obj);
void DumpVkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR(Printer &p, std::string name, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR &obj);
void DumpVkPhysicalDevicePointClippingProperties(Printer &p, std::string name, VkPhysicalDevicePointClippingProperties &obj);
void DumpVkPhysicalDeviceProtectedMemoryFeatures(Printer &p, std::string name, VkPhysicalDeviceProtectedMemoryFeatures &obj);
void DumpVkPhysicalDeviceProtectedMemoryProperties(Printer &p, std::string name, VkPhysicalDeviceProtectedMemoryProperties &obj);
void DumpVkPhysicalDevicePushDescriptorPropertiesKHR(Printer &p, std::string name, VkPhysicalDevicePushDescriptorPropertiesKHR &obj);
void DumpVkPhysicalDeviceSampleLocationsPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceSampleLocationsPropertiesEXT &obj);
void DumpVkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT &obj);
void DumpVkPhysicalDeviceSamplerYcbcrConversionFeatures(Printer &p, std::string name, VkPhysicalDeviceSamplerYcbcrConversionFeatures &obj);
void DumpVkPhysicalDeviceScalarBlockLayoutFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT &obj);
void DumpVkPhysicalDeviceShaderAtomicInt64FeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR &obj);
void DumpVkPhysicalDeviceShaderClockFeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceShaderClockFeaturesKHR &obj);
void DumpVkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT &obj);
void DumpVkPhysicalDeviceShaderDrawParametersFeatures(Printer &p, std::string name, VkPhysicalDeviceShaderDrawParametersFeatures &obj);
void DumpVkPhysicalDeviceShaderFloat16Int8FeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR &obj);
void DumpVkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR &obj);
void DumpVkPhysicalDeviceSparseProperties(Printer &p, std::string name, VkPhysicalDeviceSparseProperties &obj);
void DumpVkPhysicalDeviceSubgroupProperties(Printer &p, std::string name, VkPhysicalDeviceSubgroupProperties &obj);
void DumpVkPhysicalDeviceSubgroupSizeControlFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT &obj);
void DumpVkPhysicalDeviceSubgroupSizeControlPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceSubgroupSizeControlPropertiesEXT &obj);
void DumpVkPhysicalDeviceTexelBufferAlignmentFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT &obj);
void DumpVkPhysicalDeviceTexelBufferAlignmentPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT &obj);
void DumpVkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT &obj);
void DumpVkPhysicalDeviceTimelineSemaphoreFeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR &obj);
void DumpVkPhysicalDeviceTimelineSemaphorePropertiesKHR(Printer &p, std::string name, VkPhysicalDeviceTimelineSemaphorePropertiesKHR &obj);
void DumpVkPhysicalDeviceTransformFeedbackFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceTransformFeedbackFeaturesEXT &obj);
void DumpVkPhysicalDeviceTransformFeedbackPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceTransformFeedbackPropertiesEXT &obj);
void DumpVkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR &obj);
void DumpVkPhysicalDeviceVariablePointersFeatures(Printer &p, std::string name, VkPhysicalDeviceVariablePointersFeatures &obj);
void DumpVkPhysicalDeviceVertexAttributeDivisorFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT &obj);
void DumpVkPhysicalDeviceVertexAttributeDivisorPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT &obj);
void DumpVkPhysicalDeviceVulkanMemoryModelFeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR &obj);
void DumpVkPhysicalDeviceYcbcrImageArraysFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT &obj);
void DumpVkSharedPresentSurfaceCapabilitiesKHR(Printer &p, std::string name, VkSharedPresentSurfaceCapabilitiesKHR &obj);
#ifdef VK_USE_PLATFORM_WIN32_KHR
void DumpVkSurfaceCapabilitiesFullScreenExclusiveEXT(Printer &p, std::string name, VkSurfaceCapabilitiesFullScreenExclusiveEXT &obj);
#endif  // VK_USE_PLATFORM_WIN32_KHR
void DumpVkSurfaceCapabilitiesKHR(Printer &p, std::string name, VkSurfaceCapabilitiesKHR &obj);
void DumpVkSurfaceFormatKHR(Printer &p, std::string name, VkSurfaceFormatKHR &obj);
void DumpVkSurfaceProtectedCapabilitiesKHR(Printer &p, std::string name, VkSurfaceProtectedCapabilitiesKHR &obj);
void DumpVkDrmFormatModifierPropertiesEXT(Printer &p, std::string name, VkDrmFormatModifierPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("drmFormatModifier", obj.drmFormatModifier, 27);
    p.PrintKeyValue("drmFormatModifierPlaneCount", obj.drmFormatModifierPlaneCount, 27);
    DumpVkFormatFeatureFlags(p, "drmFormatModifierTilingFeatures", obj.drmFormatModifierTilingFeatures, 27);
    p.ObjectEnd();
}
void DumpVkDrmFormatModifierPropertiesListEXT(Printer &p, std::string name, VkDrmFormatModifierPropertiesListEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("drmFormatModifierCount", obj.drmFormatModifierCount, 52);
    p.ArrayStart("pDrmFormatModifierProperties", obj.drmFormatModifierCount);
    for (uint32_t i = 0; i < obj.drmFormatModifierCount; i++) {
        if (obj.pDrmFormatModifierProperties != nullptr) {
            p.SetElementIndex(i);
            DumpVkDrmFormatModifierPropertiesEXT(p, "pDrmFormatModifierProperties", obj.pDrmFormatModifierProperties[i]);
        }
    }
    p.ArrayEnd();
    p.ObjectEnd();
}
void DumpVkExtent2D(Printer &p, std::string name, VkExtent2D &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("width", obj.width, 6);
    p.PrintKeyValue("height", obj.height, 6);
    p.ObjectEnd();
}
void DumpVkExtent3D(Printer &p, std::string name, VkExtent3D &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("width", obj.width, 6);
    p.PrintKeyValue("height", obj.height, 6);
    p.PrintKeyValue("depth", obj.depth, 6);
    p.ObjectEnd();
}
void DumpVkLayerProperties(Printer &p, std::string name, VkLayerProperties &obj) {
    p.ObjectStart(name);
    p.PrintKeyString("layerName", obj.layerName, 21);
    p.PrintKeyValue("specVersion", obj.specVersion, 21);
    p.PrintKeyValue("implementationVersion", obj.implementationVersion, 21);
    p.PrintKeyString("description", obj.description, 21);
    p.ObjectEnd();
}
void DumpVkPhysicalDevice16BitStorageFeatures(Printer &p, std::string name, VkPhysicalDevice16BitStorageFeatures &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("storageBuffer16BitAccess", obj.storageBuffer16BitAccess, 34);
    p.PrintKeyBool("uniformAndStorageBuffer16BitAccess", obj.uniformAndStorageBuffer16BitAccess, 34);
    p.PrintKeyBool("storagePushConstant16", obj.storagePushConstant16, 34);
    p.PrintKeyBool("storageInputOutput16", obj.storageInputOutput16, 34);
    p.ObjectEnd();
}
void DumpVkPhysicalDevice8BitStorageFeaturesKHR(Printer &p, std::string name, VkPhysicalDevice8BitStorageFeaturesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("storageBuffer8BitAccess", obj.storageBuffer8BitAccess, 33);
    p.PrintKeyBool("uniformAndStorageBuffer8BitAccess", obj.uniformAndStorageBuffer8BitAccess, 33);
    p.PrintKeyBool("storagePushConstant8", obj.storagePushConstant8, 33);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceASTCDecodeFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceASTCDecodeFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("decodeModeSharedExponent", obj.decodeModeSharedExponent, 24);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceBlendOperationAdvancedFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("advancedBlendCoherentOperations", obj.advancedBlendCoherentOperations, 31);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceBlendOperationAdvancedPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("advancedBlendMaxColorAttachments", obj.advancedBlendMaxColorAttachments, 37);
    p.PrintKeyBool("advancedBlendIndependentBlend", obj.advancedBlendIndependentBlend, 37);
    p.PrintKeyBool("advancedBlendNonPremultipliedSrcColor", obj.advancedBlendNonPremultipliedSrcColor, 37);
    p.PrintKeyBool("advancedBlendNonPremultipliedDstColor", obj.advancedBlendNonPremultipliedDstColor, 37);
    p.PrintKeyBool("advancedBlendCorrelatedOverlap", obj.advancedBlendCorrelatedOverlap, 37);
    p.PrintKeyBool("advancedBlendAllOperations", obj.advancedBlendAllOperations, 37);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceBufferDeviceAddressFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceBufferDeviceAddressFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("bufferDeviceAddress", obj.bufferDeviceAddress, 32);
    p.PrintKeyBool("bufferDeviceAddressCaptureReplay", obj.bufferDeviceAddressCaptureReplay, 32);
    p.PrintKeyBool("bufferDeviceAddressMultiDevice", obj.bufferDeviceAddressMultiDevice, 32);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceConditionalRenderingFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceConditionalRenderingFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("conditionalRendering", obj.conditionalRendering, 29);
    p.PrintKeyBool("inheritedConditionalRendering", obj.inheritedConditionalRendering, 29);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceConservativeRasterizationPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceConservativeRasterizationPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("primitiveOverestimationSize", obj.primitiveOverestimationSize, 43);
    p.PrintKeyValue("maxExtraPrimitiveOverestimationSize", obj.maxExtraPrimitiveOverestimationSize, 43);
    p.PrintKeyValue("extraPrimitiveOverestimationSizeGranularity", obj.extraPrimitiveOverestimationSizeGranularity, 43);
    p.PrintKeyBool("primitiveUnderestimation", obj.primitiveUnderestimation, 43);
    p.PrintKeyBool("conservativePointAndLineRasterization", obj.conservativePointAndLineRasterization, 43);
    p.PrintKeyBool("degenerateTrianglesRasterized", obj.degenerateTrianglesRasterized, 43);
    p.PrintKeyBool("degenerateLinesRasterized", obj.degenerateLinesRasterized, 43);
    p.PrintKeyBool("fullyCoveredFragmentShaderInputVariable", obj.fullyCoveredFragmentShaderInputVariable, 43);
    p.PrintKeyBool("conservativeRasterizationPostDepthCoverage", obj.conservativeRasterizationPostDepthCoverage, 43);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceDepthClipEnableFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceDepthClipEnableFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("depthClipEnable", obj.depthClipEnable, 15);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceDepthStencilResolvePropertiesKHR(Printer &p, std::string name, VkPhysicalDeviceDepthStencilResolvePropertiesKHR &obj) {
    p.ObjectStart(name);
    DumpVkResolveModeFlagsKHR(p, "supportedDepthResolveModes", obj.supportedDepthResolveModes, 22);
    DumpVkResolveModeFlagsKHR(p, "supportedStencilResolveModes", obj.supportedStencilResolveModes, 22);
    p.PrintKeyBool("independentResolveNone", obj.independentResolveNone, 22);
    p.PrintKeyBool("independentResolve", obj.independentResolve, 22);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceDescriptorIndexingFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceDescriptorIndexingFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("shaderInputAttachmentArrayDynamicIndexing", obj.shaderInputAttachmentArrayDynamicIndexing, 50);
    p.PrintKeyBool("shaderUniformTexelBufferArrayDynamicIndexing", obj.shaderUniformTexelBufferArrayDynamicIndexing, 50);
    p.PrintKeyBool("shaderStorageTexelBufferArrayDynamicIndexing", obj.shaderStorageTexelBufferArrayDynamicIndexing, 50);
    p.PrintKeyBool("shaderUniformBufferArrayNonUniformIndexing", obj.shaderUniformBufferArrayNonUniformIndexing, 50);
    p.PrintKeyBool("shaderSampledImageArrayNonUniformIndexing", obj.shaderSampledImageArrayNonUniformIndexing, 50);
    p.PrintKeyBool("shaderStorageBufferArrayNonUniformIndexing", obj.shaderStorageBufferArrayNonUniformIndexing, 50);
    p.PrintKeyBool("shaderStorageImageArrayNonUniformIndexing", obj.shaderStorageImageArrayNonUniformIndexing, 50);
    p.PrintKeyBool("shaderInputAttachmentArrayNonUniformIndexing", obj.shaderInputAttachmentArrayNonUniformIndexing, 50);
    p.PrintKeyBool("shaderUniformTexelBufferArrayNonUniformIndexing", obj.shaderUniformTexelBufferArrayNonUniformIndexing, 50);
    p.PrintKeyBool("shaderStorageTexelBufferArrayNonUniformIndexing", obj.shaderStorageTexelBufferArrayNonUniformIndexing, 50);
    p.PrintKeyBool("descriptorBindingUniformBufferUpdateAfterBind", obj.descriptorBindingUniformBufferUpdateAfterBind, 50);
    p.PrintKeyBool("descriptorBindingSampledImageUpdateAfterBind", obj.descriptorBindingSampledImageUpdateAfterBind, 50);
    p.PrintKeyBool("descriptorBindingStorageImageUpdateAfterBind", obj.descriptorBindingStorageImageUpdateAfterBind, 50);
    p.PrintKeyBool("descriptorBindingStorageBufferUpdateAfterBind", obj.descriptorBindingStorageBufferUpdateAfterBind, 50);
    p.PrintKeyBool("descriptorBindingUniformTexelBufferUpdateAfterBind", obj.descriptorBindingUniformTexelBufferUpdateAfterBind, 50);
    p.PrintKeyBool("descriptorBindingStorageTexelBufferUpdateAfterBind", obj.descriptorBindingStorageTexelBufferUpdateAfterBind, 50);
    p.PrintKeyBool("descriptorBindingUpdateUnusedWhilePending", obj.descriptorBindingUpdateUnusedWhilePending, 50);
    p.PrintKeyBool("descriptorBindingPartiallyBound", obj.descriptorBindingPartiallyBound, 50);
    p.PrintKeyBool("descriptorBindingVariableDescriptorCount", obj.descriptorBindingVariableDescriptorCount, 50);
    p.PrintKeyBool("runtimeDescriptorArray", obj.runtimeDescriptorArray, 50);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceDescriptorIndexingPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceDescriptorIndexingPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("maxUpdateAfterBindDescriptorsInAllPools", obj.maxUpdateAfterBindDescriptorsInAllPools, 52);
    p.PrintKeyBool("shaderUniformBufferArrayNonUniformIndexingNative", obj.shaderUniformBufferArrayNonUniformIndexingNative, 52);
    p.PrintKeyBool("shaderSampledImageArrayNonUniformIndexingNative", obj.shaderSampledImageArrayNonUniformIndexingNative, 52);
    p.PrintKeyBool("shaderStorageBufferArrayNonUniformIndexingNative", obj.shaderStorageBufferArrayNonUniformIndexingNative, 52);
    p.PrintKeyBool("shaderStorageImageArrayNonUniformIndexingNative", obj.shaderStorageImageArrayNonUniformIndexingNative, 52);
    p.PrintKeyBool("shaderInputAttachmentArrayNonUniformIndexingNative", obj.shaderInputAttachmentArrayNonUniformIndexingNative, 52);
    p.PrintKeyBool("robustBufferAccessUpdateAfterBind", obj.robustBufferAccessUpdateAfterBind, 52);
    p.PrintKeyBool("quadDivergentImplicitLod", obj.quadDivergentImplicitLod, 52);
    p.PrintKeyValue("maxPerStageDescriptorUpdateAfterBindSamplers", obj.maxPerStageDescriptorUpdateAfterBindSamplers, 52);
    p.PrintKeyValue("maxPerStageDescriptorUpdateAfterBindUniformBuffers", obj.maxPerStageDescriptorUpdateAfterBindUniformBuffers, 52);
    p.PrintKeyValue("maxPerStageDescriptorUpdateAfterBindStorageBuffers", obj.maxPerStageDescriptorUpdateAfterBindStorageBuffers, 52);
    p.PrintKeyValue("maxPerStageDescriptorUpdateAfterBindSampledImages", obj.maxPerStageDescriptorUpdateAfterBindSampledImages, 52);
    p.PrintKeyValue("maxPerStageDescriptorUpdateAfterBindStorageImages", obj.maxPerStageDescriptorUpdateAfterBindStorageImages, 52);
    p.PrintKeyValue("maxPerStageDescriptorUpdateAfterBindInputAttachments", obj.maxPerStageDescriptorUpdateAfterBindInputAttachments, 52);
    p.PrintKeyValue("maxPerStageUpdateAfterBindResources", obj.maxPerStageUpdateAfterBindResources, 52);
    p.PrintKeyValue("maxDescriptorSetUpdateAfterBindSamplers", obj.maxDescriptorSetUpdateAfterBindSamplers, 52);
    p.PrintKeyValue("maxDescriptorSetUpdateAfterBindUniformBuffers", obj.maxDescriptorSetUpdateAfterBindUniformBuffers, 52);
    p.PrintKeyValue("maxDescriptorSetUpdateAfterBindUniformBuffersDynamic", obj.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic, 52);
    p.PrintKeyValue("maxDescriptorSetUpdateAfterBindStorageBuffers", obj.maxDescriptorSetUpdateAfterBindStorageBuffers, 52);
    p.PrintKeyValue("maxDescriptorSetUpdateAfterBindStorageBuffersDynamic", obj.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic, 52);
    p.PrintKeyValue("maxDescriptorSetUpdateAfterBindSampledImages", obj.maxDescriptorSetUpdateAfterBindSampledImages, 52);
    p.PrintKeyValue("maxDescriptorSetUpdateAfterBindStorageImages", obj.maxDescriptorSetUpdateAfterBindStorageImages, 52);
    p.PrintKeyValue("maxDescriptorSetUpdateAfterBindInputAttachments", obj.maxDescriptorSetUpdateAfterBindInputAttachments, 52);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceDiscardRectanglePropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceDiscardRectanglePropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("maxDiscardRectangles", obj.maxDiscardRectangles, 20);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceDriverPropertiesKHR(Printer &p, std::string name, VkPhysicalDeviceDriverPropertiesKHR &obj) {
    p.ObjectStart(name);
    DumpVkDriverIdKHR(p, "driverID", obj.driverID, 18);
    p.PrintKeyString("driverName", obj.driverName, 18);
    p.PrintKeyString("driverInfo", obj.driverInfo, 18);
    p.PrintKeyValue("conformanceVersion", obj.conformanceVersion, 18);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceExternalMemoryHostPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceExternalMemoryHostPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("minImportedHostPointerAlignment", to_hex_str(p, obj.minImportedHostPointerAlignment), 31);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceFeatures(Printer &p, std::string name, VkPhysicalDeviceFeatures &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("robustBufferAccess", obj.robustBufferAccess, 39);
    p.PrintKeyBool("fullDrawIndexUint32", obj.fullDrawIndexUint32, 39);
    p.PrintKeyBool("imageCubeArray", obj.imageCubeArray, 39);
    p.PrintKeyBool("independentBlend", obj.independentBlend, 39);
    p.PrintKeyBool("geometryShader", obj.geometryShader, 39);
    p.PrintKeyBool("tessellationShader", obj.tessellationShader, 39);
    p.PrintKeyBool("sampleRateShading", obj.sampleRateShading, 39);
    p.PrintKeyBool("dualSrcBlend", obj.dualSrcBlend, 39);
    p.PrintKeyBool("logicOp", obj.logicOp, 39);
    p.PrintKeyBool("multiDrawIndirect", obj.multiDrawIndirect, 39);
    p.PrintKeyBool("drawIndirectFirstInstance", obj.drawIndirectFirstInstance, 39);
    p.PrintKeyBool("depthClamp", obj.depthClamp, 39);
    p.PrintKeyBool("depthBiasClamp", obj.depthBiasClamp, 39);
    p.PrintKeyBool("fillModeNonSolid", obj.fillModeNonSolid, 39);
    p.PrintKeyBool("depthBounds", obj.depthBounds, 39);
    p.PrintKeyBool("wideLines", obj.wideLines, 39);
    p.PrintKeyBool("largePoints", obj.largePoints, 39);
    p.PrintKeyBool("alphaToOne", obj.alphaToOne, 39);
    p.PrintKeyBool("multiViewport", obj.multiViewport, 39);
    p.PrintKeyBool("samplerAnisotropy", obj.samplerAnisotropy, 39);
    p.PrintKeyBool("textureCompressionETC2", obj.textureCompressionETC2, 39);
    p.PrintKeyBool("textureCompressionASTC_LDR", obj.textureCompressionASTC_LDR, 39);
    p.PrintKeyBool("textureCompressionBC", obj.textureCompressionBC, 39);
    p.PrintKeyBool("occlusionQueryPrecise", obj.occlusionQueryPrecise, 39);
    p.PrintKeyBool("pipelineStatisticsQuery", obj.pipelineStatisticsQuery, 39);
    p.PrintKeyBool("vertexPipelineStoresAndAtomics", obj.vertexPipelineStoresAndAtomics, 39);
    p.PrintKeyBool("fragmentStoresAndAtomics", obj.fragmentStoresAndAtomics, 39);
    p.PrintKeyBool("shaderTessellationAndGeometryPointSize", obj.shaderTessellationAndGeometryPointSize, 39);
    p.PrintKeyBool("shaderImageGatherExtended", obj.shaderImageGatherExtended, 39);
    p.PrintKeyBool("shaderStorageImageExtendedFormats", obj.shaderStorageImageExtendedFormats, 39);
    p.PrintKeyBool("shaderStorageImageMultisample", obj.shaderStorageImageMultisample, 39);
    p.PrintKeyBool("shaderStorageImageReadWithoutFormat", obj.shaderStorageImageReadWithoutFormat, 39);
    p.PrintKeyBool("shaderStorageImageWriteWithoutFormat", obj.shaderStorageImageWriteWithoutFormat, 39);
    p.PrintKeyBool("shaderUniformBufferArrayDynamicIndexing", obj.shaderUniformBufferArrayDynamicIndexing, 39);
    p.PrintKeyBool("shaderSampledImageArrayDynamicIndexing", obj.shaderSampledImageArrayDynamicIndexing, 39);
    p.PrintKeyBool("shaderStorageBufferArrayDynamicIndexing", obj.shaderStorageBufferArrayDynamicIndexing, 39);
    p.PrintKeyBool("shaderStorageImageArrayDynamicIndexing", obj.shaderStorageImageArrayDynamicIndexing, 39);
    p.PrintKeyBool("shaderClipDistance", obj.shaderClipDistance, 39);
    p.PrintKeyBool("shaderCullDistance", obj.shaderCullDistance, 39);
    p.PrintKeyBool("shaderFloat64", obj.shaderFloat64, 39);
    p.PrintKeyBool("shaderInt64", obj.shaderInt64, 39);
    p.PrintKeyBool("shaderInt16", obj.shaderInt16, 39);
    p.PrintKeyBool("shaderResourceResidency", obj.shaderResourceResidency, 39);
    p.PrintKeyBool("shaderResourceMinLod", obj.shaderResourceMinLod, 39);
    p.PrintKeyBool("sparseBinding", obj.sparseBinding, 39);
    p.PrintKeyBool("sparseResidencyBuffer", obj.sparseResidencyBuffer, 39);
    p.PrintKeyBool("sparseResidencyImage2D", obj.sparseResidencyImage2D, 39);
    p.PrintKeyBool("sparseResidencyImage3D", obj.sparseResidencyImage3D, 39);
    p.PrintKeyBool("sparseResidency2Samples", obj.sparseResidency2Samples, 39);
    p.PrintKeyBool("sparseResidency4Samples", obj.sparseResidency4Samples, 39);
    p.PrintKeyBool("sparseResidency8Samples", obj.sparseResidency8Samples, 39);
    p.PrintKeyBool("sparseResidency16Samples", obj.sparseResidency16Samples, 39);
    p.PrintKeyBool("sparseResidencyAliased", obj.sparseResidencyAliased, 39);
    p.PrintKeyBool("variableMultisampleRate", obj.variableMultisampleRate, 39);
    p.PrintKeyBool("inheritedQueries", obj.inheritedQueries, 39);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceFloatControlsPropertiesKHR(Printer &p, std::string name, VkPhysicalDeviceFloatControlsPropertiesKHR &obj) {
    p.ObjectStart(name);
    DumpVkShaderFloatControlsIndependenceKHR(p, "denormBehaviorIndependence", obj.denormBehaviorIndependence, 37);
    DumpVkShaderFloatControlsIndependenceKHR(p, "roundingModeIndependence", obj.roundingModeIndependence, 37);
    p.PrintKeyBool("shaderSignedZeroInfNanPreserveFloat16", obj.shaderSignedZeroInfNanPreserveFloat16, 37);
    p.PrintKeyBool("shaderSignedZeroInfNanPreserveFloat32", obj.shaderSignedZeroInfNanPreserveFloat32, 37);
    p.PrintKeyBool("shaderSignedZeroInfNanPreserveFloat64", obj.shaderSignedZeroInfNanPreserveFloat64, 37);
    p.PrintKeyBool("shaderDenormPreserveFloat16", obj.shaderDenormPreserveFloat16, 37);
    p.PrintKeyBool("shaderDenormPreserveFloat32", obj.shaderDenormPreserveFloat32, 37);
    p.PrintKeyBool("shaderDenormPreserveFloat64", obj.shaderDenormPreserveFloat64, 37);
    p.PrintKeyBool("shaderDenormFlushToZeroFloat16", obj.shaderDenormFlushToZeroFloat16, 37);
    p.PrintKeyBool("shaderDenormFlushToZeroFloat32", obj.shaderDenormFlushToZeroFloat32, 37);
    p.PrintKeyBool("shaderDenormFlushToZeroFloat64", obj.shaderDenormFlushToZeroFloat64, 37);
    p.PrintKeyBool("shaderRoundingModeRTEFloat16", obj.shaderRoundingModeRTEFloat16, 37);
    p.PrintKeyBool("shaderRoundingModeRTEFloat32", obj.shaderRoundingModeRTEFloat32, 37);
    p.PrintKeyBool("shaderRoundingModeRTEFloat64", obj.shaderRoundingModeRTEFloat64, 37);
    p.PrintKeyBool("shaderRoundingModeRTZFloat16", obj.shaderRoundingModeRTZFloat16, 37);
    p.PrintKeyBool("shaderRoundingModeRTZFloat32", obj.shaderRoundingModeRTZFloat32, 37);
    p.PrintKeyBool("shaderRoundingModeRTZFloat64", obj.shaderRoundingModeRTZFloat64, 37);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceFragmentDensityMapFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceFragmentDensityMapFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("fragmentDensityMap", obj.fragmentDensityMap, 37);
    p.PrintKeyBool("fragmentDensityMapDynamic", obj.fragmentDensityMapDynamic, 37);
    p.PrintKeyBool("fragmentDensityMapNonSubsampledImages", obj.fragmentDensityMapNonSubsampledImages, 37);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceFragmentDensityMapPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceFragmentDensityMapPropertiesEXT &obj) {
    p.ObjectStart(name);
    DumpVkExtent2D(p, "minFragmentDensityTexelSize", obj.minFragmentDensityTexelSize);
    DumpVkExtent2D(p, "maxFragmentDensityTexelSize", obj.maxFragmentDensityTexelSize);
    p.PrintKeyBool("fragmentDensityInvocations", obj.fragmentDensityInvocations, 26);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceFragmentShaderInterlockFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("fragmentShaderSampleInterlock", obj.fragmentShaderSampleInterlock, 34);
    p.PrintKeyBool("fragmentShaderPixelInterlock", obj.fragmentShaderPixelInterlock, 34);
    p.PrintKeyBool("fragmentShaderShadingRateInterlock", obj.fragmentShaderShadingRateInterlock, 34);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceHostQueryResetFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceHostQueryResetFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("hostQueryReset", obj.hostQueryReset, 14);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceIDProperties(Printer &p, std::string name, VkPhysicalDeviceIDProperties &obj) {
    p.ObjectStart(name);
    p.PrintKeyString("deviceUUID", to_string_16(obj.deviceUUID), 15);
    p.PrintKeyString("driverUUID", to_string_16(obj.driverUUID), 15);
    if (obj.deviceLUIDValid) p.PrintKeyString("deviceLUID", to_string_8(obj.deviceLUID), 15);
    p.PrintKeyValue("deviceNodeMask", obj.deviceNodeMask, 15);
    p.PrintKeyBool("deviceLUIDValid", obj.deviceLUIDValid, 15);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceImagelessFramebufferFeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceImagelessFramebufferFeaturesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("imagelessFramebuffer", obj.imagelessFramebuffer, 20);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceIndexTypeUint8FeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceIndexTypeUint8FeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("indexTypeUint8", obj.indexTypeUint8, 14);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceInlineUniformBlockFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceInlineUniformBlockFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("inlineUniformBlock", obj.inlineUniformBlock, 50);
    p.PrintKeyBool("descriptorBindingInlineUniformBlockUpdateAfterBind", obj.descriptorBindingInlineUniformBlockUpdateAfterBind, 50);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceInlineUniformBlockPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceInlineUniformBlockPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("maxInlineUniformBlockSize", obj.maxInlineUniformBlockSize, 55);
    p.PrintKeyValue("maxPerStageDescriptorInlineUniformBlocks", obj.maxPerStageDescriptorInlineUniformBlocks, 55);
    p.PrintKeyValue("maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks", obj.maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks, 55);
    p.PrintKeyValue("maxDescriptorSetInlineUniformBlocks", obj.maxDescriptorSetInlineUniformBlocks, 55);
    p.PrintKeyValue("maxDescriptorSetUpdateAfterBindInlineUniformBlocks", obj.maxDescriptorSetUpdateAfterBindInlineUniformBlocks, 55);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceLimits(Printer &p, std::string name, VkPhysicalDeviceLimits &obj) {
    if (p.Type() == OutputType::json)
        p.ObjectStart("limits");
    else
        p.SetSubHeader().ObjectStart(name);
    p.PrintKeyValue("maxImageDimension1D", obj.maxImageDimension1D, 47);
    p.PrintKeyValue("maxImageDimension2D", obj.maxImageDimension2D, 47);
    p.PrintKeyValue("maxImageDimension3D", obj.maxImageDimension3D, 47);
    p.PrintKeyValue("maxImageDimensionCube", obj.maxImageDimensionCube, 47);
    p.PrintKeyValue("maxImageArrayLayers", obj.maxImageArrayLayers, 47);
    p.PrintKeyValue("maxTexelBufferElements", obj.maxTexelBufferElements, 47);
    p.PrintKeyValue("maxUniformBufferRange", obj.maxUniformBufferRange, 47);
    p.PrintKeyValue("maxStorageBufferRange", obj.maxStorageBufferRange, 47);
    p.PrintKeyValue("maxPushConstantsSize", obj.maxPushConstantsSize, 47);
    p.PrintKeyValue("maxMemoryAllocationCount", obj.maxMemoryAllocationCount, 47);
    p.PrintKeyValue("maxSamplerAllocationCount", obj.maxSamplerAllocationCount, 47);
    p.PrintKeyValue("bufferImageGranularity", to_hex_str(p, obj.bufferImageGranularity), 47);
    p.PrintKeyValue("sparseAddressSpaceSize", to_hex_str(p, obj.sparseAddressSpaceSize), 47);
    p.PrintKeyValue("maxBoundDescriptorSets", obj.maxBoundDescriptorSets, 47);
    p.PrintKeyValue("maxPerStageDescriptorSamplers", obj.maxPerStageDescriptorSamplers, 47);
    p.PrintKeyValue("maxPerStageDescriptorUniformBuffers", obj.maxPerStageDescriptorUniformBuffers, 47);
    p.PrintKeyValue("maxPerStageDescriptorStorageBuffers", obj.maxPerStageDescriptorStorageBuffers, 47);
    p.PrintKeyValue("maxPerStageDescriptorSampledImages", obj.maxPerStageDescriptorSampledImages, 47);
    p.PrintKeyValue("maxPerStageDescriptorStorageImages", obj.maxPerStageDescriptorStorageImages, 47);
    p.PrintKeyValue("maxPerStageDescriptorInputAttachments", obj.maxPerStageDescriptorInputAttachments, 47);
    p.PrintKeyValue("maxPerStageResources", obj.maxPerStageResources, 47);
    p.PrintKeyValue("maxDescriptorSetSamplers", obj.maxDescriptorSetSamplers, 47);
    p.PrintKeyValue("maxDescriptorSetUniformBuffers", obj.maxDescriptorSetUniformBuffers, 47);
    p.PrintKeyValue("maxDescriptorSetUniformBuffersDynamic", obj.maxDescriptorSetUniformBuffersDynamic, 47);
    p.PrintKeyValue("maxDescriptorSetStorageBuffers", obj.maxDescriptorSetStorageBuffers, 47);
    p.PrintKeyValue("maxDescriptorSetStorageBuffersDynamic", obj.maxDescriptorSetStorageBuffersDynamic, 47);
    p.PrintKeyValue("maxDescriptorSetSampledImages", obj.maxDescriptorSetSampledImages, 47);
    p.PrintKeyValue("maxDescriptorSetStorageImages", obj.maxDescriptorSetStorageImages, 47);
    p.PrintKeyValue("maxDescriptorSetInputAttachments", obj.maxDescriptorSetInputAttachments, 47);
    p.PrintKeyValue("maxVertexInputAttributes", obj.maxVertexInputAttributes, 47);
    p.PrintKeyValue("maxVertexInputBindings", obj.maxVertexInputBindings, 47);
    p.PrintKeyValue("maxVertexInputAttributeOffset", obj.maxVertexInputAttributeOffset, 47);
    p.PrintKeyValue("maxVertexInputBindingStride", obj.maxVertexInputBindingStride, 47);
    p.PrintKeyValue("maxVertexOutputComponents", obj.maxVertexOutputComponents, 47);
    p.PrintKeyValue("maxTessellationGenerationLevel", obj.maxTessellationGenerationLevel, 47);
    p.PrintKeyValue("maxTessellationPatchSize", obj.maxTessellationPatchSize, 47);
    p.PrintKeyValue("maxTessellationControlPerVertexInputComponents", obj.maxTessellationControlPerVertexInputComponents, 47);
    p.PrintKeyValue("maxTessellationControlPerVertexOutputComponents", obj.maxTessellationControlPerVertexOutputComponents, 47);
    p.PrintKeyValue("maxTessellationControlPerPatchOutputComponents", obj.maxTessellationControlPerPatchOutputComponents, 47);
    p.PrintKeyValue("maxTessellationControlTotalOutputComponents", obj.maxTessellationControlTotalOutputComponents, 47);
    p.PrintKeyValue("maxTessellationEvaluationInputComponents", obj.maxTessellationEvaluationInputComponents, 47);
    p.PrintKeyValue("maxTessellationEvaluationOutputComponents", obj.maxTessellationEvaluationOutputComponents, 47);
    p.PrintKeyValue("maxGeometryShaderInvocations", obj.maxGeometryShaderInvocations, 47);
    p.PrintKeyValue("maxGeometryInputComponents", obj.maxGeometryInputComponents, 47);
    p.PrintKeyValue("maxGeometryOutputComponents", obj.maxGeometryOutputComponents, 47);
    p.PrintKeyValue("maxGeometryOutputVertices", obj.maxGeometryOutputVertices, 47);
    p.PrintKeyValue("maxGeometryTotalOutputComponents", obj.maxGeometryTotalOutputComponents, 47);
    p.PrintKeyValue("maxFragmentInputComponents", obj.maxFragmentInputComponents, 47);
    p.PrintKeyValue("maxFragmentOutputAttachments", obj.maxFragmentOutputAttachments, 47);
    p.PrintKeyValue("maxFragmentDualSrcAttachments", obj.maxFragmentDualSrcAttachments, 47);
    p.PrintKeyValue("maxFragmentCombinedOutputResources", obj.maxFragmentCombinedOutputResources, 47);
    p.PrintKeyValue("maxComputeSharedMemorySize", obj.maxComputeSharedMemorySize, 47);
    p.ArrayStart("maxComputeWorkGroupCount", 3);
    p.PrintElement(obj.maxComputeWorkGroupCount[0]);
    p.PrintElement(obj.maxComputeWorkGroupCount[1]);
    p.PrintElement(obj.maxComputeWorkGroupCount[2]);
    p.ArrayEnd();
    p.PrintKeyValue("maxComputeWorkGroupInvocations", obj.maxComputeWorkGroupInvocations, 47);
    p.ArrayStart("maxComputeWorkGroupSize", 3);
    p.PrintElement(obj.maxComputeWorkGroupSize[0]);
    p.PrintElement(obj.maxComputeWorkGroupSize[1]);
    p.PrintElement(obj.maxComputeWorkGroupSize[2]);
    p.ArrayEnd();
    p.PrintKeyValue("subPixelPrecisionBits", obj.subPixelPrecisionBits, 47);
    p.PrintKeyValue("subTexelPrecisionBits", obj.subTexelPrecisionBits, 47);
    p.PrintKeyValue("mipmapPrecisionBits", obj.mipmapPrecisionBits, 47);
    p.PrintKeyValue("maxDrawIndexedIndexValue", obj.maxDrawIndexedIndexValue, 47);
    p.PrintKeyValue("maxDrawIndirectCount", obj.maxDrawIndirectCount, 47);
    p.PrintKeyValue("maxSamplerLodBias", obj.maxSamplerLodBias, 47);
    p.PrintKeyValue("maxSamplerAnisotropy", obj.maxSamplerAnisotropy, 47);
    p.PrintKeyValue("maxViewports", obj.maxViewports, 47);
    p.ArrayStart("maxViewportDimensions", 2);
    p.PrintElement(obj.maxViewportDimensions[0]);
    p.PrintElement(obj.maxViewportDimensions[1]);
    p.ArrayEnd();
    p.ArrayStart("viewportBoundsRange", 2);
    p.PrintElement(obj.viewportBoundsRange[0]);
    p.PrintElement(obj.viewportBoundsRange[1]);
    p.ArrayEnd();
    p.PrintKeyValue("viewportSubPixelBits", obj.viewportSubPixelBits, 47);
    p.PrintKeyValue("minMemoryMapAlignment", obj.minMemoryMapAlignment, 47);
    p.PrintKeyValue("minTexelBufferOffsetAlignment", to_hex_str(p, obj.minTexelBufferOffsetAlignment), 47);
    p.PrintKeyValue("minUniformBufferOffsetAlignment", to_hex_str(p, obj.minUniformBufferOffsetAlignment), 47);
    p.PrintKeyValue("minStorageBufferOffsetAlignment", to_hex_str(p, obj.minStorageBufferOffsetAlignment), 47);
    p.PrintKeyValue("minTexelOffset", obj.minTexelOffset, 47);
    p.PrintKeyValue("maxTexelOffset", obj.maxTexelOffset, 47);
    p.PrintKeyValue("minTexelGatherOffset", obj.minTexelGatherOffset, 47);
    p.PrintKeyValue("maxTexelGatherOffset", obj.maxTexelGatherOffset, 47);
    p.PrintKeyValue("minInterpolationOffset", obj.minInterpolationOffset, 47);
    p.PrintKeyValue("maxInterpolationOffset", obj.maxInterpolationOffset, 47);
    p.PrintKeyValue("subPixelInterpolationOffsetBits", obj.subPixelInterpolationOffsetBits, 47);
    p.PrintKeyValue("maxFramebufferWidth", obj.maxFramebufferWidth, 47);
    p.PrintKeyValue("maxFramebufferHeight", obj.maxFramebufferHeight, 47);
    p.PrintKeyValue("maxFramebufferLayers", obj.maxFramebufferLayers, 47);
    DumpVkSampleCountFlags(p, "framebufferColorSampleCounts", obj.framebufferColorSampleCounts, 47);
    DumpVkSampleCountFlags(p, "framebufferDepthSampleCounts", obj.framebufferDepthSampleCounts, 47);
    DumpVkSampleCountFlags(p, "framebufferStencilSampleCounts", obj.framebufferStencilSampleCounts, 47);
    DumpVkSampleCountFlags(p, "framebufferNoAttachmentsSampleCounts", obj.framebufferNoAttachmentsSampleCounts, 47);
    p.PrintKeyValue("maxColorAttachments", obj.maxColorAttachments, 47);
    DumpVkSampleCountFlags(p, "sampledImageColorSampleCounts", obj.sampledImageColorSampleCounts, 47);
    DumpVkSampleCountFlags(p, "sampledImageIntegerSampleCounts", obj.sampledImageIntegerSampleCounts, 47);
    DumpVkSampleCountFlags(p, "sampledImageDepthSampleCounts", obj.sampledImageDepthSampleCounts, 47);
    DumpVkSampleCountFlags(p, "sampledImageStencilSampleCounts", obj.sampledImageStencilSampleCounts, 47);
    DumpVkSampleCountFlags(p, "storageImageSampleCounts", obj.storageImageSampleCounts, 47);
    p.PrintKeyValue("maxSampleMaskWords", obj.maxSampleMaskWords, 47);
    p.PrintKeyBool("timestampComputeAndGraphics", obj.timestampComputeAndGraphics, 47);
    p.PrintKeyValue("timestampPeriod", obj.timestampPeriod, 47);
    p.PrintKeyValue("maxClipDistances", obj.maxClipDistances, 47);
    p.PrintKeyValue("maxCullDistances", obj.maxCullDistances, 47);
    p.PrintKeyValue("maxCombinedClipAndCullDistances", obj.maxCombinedClipAndCullDistances, 47);
    p.PrintKeyValue("discreteQueuePriorities", obj.discreteQueuePriorities, 47);
    p.ArrayStart("pointSizeRange", 2);
    p.PrintElement(obj.pointSizeRange[0]);
    p.PrintElement(obj.pointSizeRange[1]);
    p.ArrayEnd();
    p.ArrayStart("lineWidthRange", 2);
    p.PrintElement(obj.lineWidthRange[0]);
    p.PrintElement(obj.lineWidthRange[1]);
    p.ArrayEnd();
    p.PrintKeyValue("pointSizeGranularity", obj.pointSizeGranularity, 47);
    p.PrintKeyValue("lineWidthGranularity", obj.lineWidthGranularity, 47);
    p.PrintKeyBool("strictLines", obj.strictLines, 47);
    p.PrintKeyBool("standardSampleLocations", obj.standardSampleLocations, 47);
    p.PrintKeyValue("optimalBufferCopyOffsetAlignment", to_hex_str(p, obj.optimalBufferCopyOffsetAlignment), 47);
    p.PrintKeyValue("optimalBufferCopyRowPitchAlignment", to_hex_str(p, obj.optimalBufferCopyRowPitchAlignment), 47);
    p.PrintKeyValue("nonCoherentAtomSize", to_hex_str(p, obj.nonCoherentAtomSize), 47);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceLineRasterizationFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceLineRasterizationFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("rectangularLines", obj.rectangularLines, 24);
    p.PrintKeyBool("bresenhamLines", obj.bresenhamLines, 24);
    p.PrintKeyBool("smoothLines", obj.smoothLines, 24);
    p.PrintKeyBool("stippledRectangularLines", obj.stippledRectangularLines, 24);
    p.PrintKeyBool("stippledBresenhamLines", obj.stippledBresenhamLines, 24);
    p.PrintKeyBool("stippledSmoothLines", obj.stippledSmoothLines, 24);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceLineRasterizationPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceLineRasterizationPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("lineSubPixelPrecisionBits", obj.lineSubPixelPrecisionBits, 25);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceMaintenance3Properties(Printer &p, std::string name, VkPhysicalDeviceMaintenance3Properties &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("maxPerSetDescriptors", obj.maxPerSetDescriptors, 23);
    p.PrintKeyValue("maxMemoryAllocationSize", to_hex_str(p, obj.maxMemoryAllocationSize), 23);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceMemoryBudgetPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceMemoryBudgetPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.ArrayStart("heapBudget", 16);
    p.PrintElement(obj.heapBudget[0]);
    p.PrintElement(obj.heapBudget[1]);
    p.PrintElement(obj.heapBudget[2]);
    p.PrintElement(obj.heapBudget[3]);
    p.PrintElement(obj.heapBudget[4]);
    p.PrintElement(obj.heapBudget[5]);
    p.PrintElement(obj.heapBudget[6]);
    p.PrintElement(obj.heapBudget[7]);
    p.PrintElement(obj.heapBudget[8]);
    p.PrintElement(obj.heapBudget[9]);
    p.PrintElement(obj.heapBudget[10]);
    p.PrintElement(obj.heapBudget[11]);
    p.PrintElement(obj.heapBudget[12]);
    p.PrintElement(obj.heapBudget[13]);
    p.PrintElement(obj.heapBudget[14]);
    p.PrintElement(obj.heapBudget[15]);
    p.ArrayEnd();
    p.ArrayStart("heapUsage", 16);
    p.PrintElement(obj.heapUsage[0]);
    p.PrintElement(obj.heapUsage[1]);
    p.PrintElement(obj.heapUsage[2]);
    p.PrintElement(obj.heapUsage[3]);
    p.PrintElement(obj.heapUsage[4]);
    p.PrintElement(obj.heapUsage[5]);
    p.PrintElement(obj.heapUsage[6]);
    p.PrintElement(obj.heapUsage[7]);
    p.PrintElement(obj.heapUsage[8]);
    p.PrintElement(obj.heapUsage[9]);
    p.PrintElement(obj.heapUsage[10]);
    p.PrintElement(obj.heapUsage[11]);
    p.PrintElement(obj.heapUsage[12]);
    p.PrintElement(obj.heapUsage[13]);
    p.PrintElement(obj.heapUsage[14]);
    p.PrintElement(obj.heapUsage[15]);
    p.ArrayEnd();
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceMemoryPriorityFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceMemoryPriorityFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("memoryPriority", obj.memoryPriority, 14);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceMultiviewFeatures(Printer &p, std::string name, VkPhysicalDeviceMultiviewFeatures &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("multiview", obj.multiview, 27);
    p.PrintKeyBool("multiviewGeometryShader", obj.multiviewGeometryShader, 27);
    p.PrintKeyBool("multiviewTessellationShader", obj.multiviewTessellationShader, 27);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceMultiviewProperties(Printer &p, std::string name, VkPhysicalDeviceMultiviewProperties &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("maxMultiviewViewCount", obj.maxMultiviewViewCount, 25);
    p.PrintKeyValue("maxMultiviewInstanceIndex", obj.maxMultiviewInstanceIndex, 25);
    p.ObjectEnd();
}
void DumpVkPhysicalDevicePCIBusInfoPropertiesEXT(Printer &p, std::string name, VkPhysicalDevicePCIBusInfoPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("pciDomain", obj.pciDomain, 11);
    p.PrintKeyValue("pciBus", obj.pciBus, 11);
    p.PrintKeyValue("pciDevice", obj.pciDevice, 11);
    p.PrintKeyValue("pciFunction", obj.pciFunction, 11);
    p.ObjectEnd();
}
void DumpVkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR(Printer &p, std::string name, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("pipelineExecutableInfo", obj.pipelineExecutableInfo, 22);
    p.ObjectEnd();
}
void DumpVkPhysicalDevicePointClippingProperties(Printer &p, std::string name, VkPhysicalDevicePointClippingProperties &obj) {
    p.ObjectStart(name);
    DumpVkPointClippingBehavior(p, "pointClippingBehavior", obj.pointClippingBehavior, 5);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceProtectedMemoryFeatures(Printer &p, std::string name, VkPhysicalDeviceProtectedMemoryFeatures &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("protectedMemory", obj.protectedMemory, 15);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceProtectedMemoryProperties(Printer &p, std::string name, VkPhysicalDeviceProtectedMemoryProperties &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("protectedNoFault", obj.protectedNoFault, 16);
    p.ObjectEnd();
}
void DumpVkPhysicalDevicePushDescriptorPropertiesKHR(Printer &p, std::string name, VkPhysicalDevicePushDescriptorPropertiesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("maxPushDescriptors", obj.maxPushDescriptors, 18);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceSampleLocationsPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceSampleLocationsPropertiesEXT &obj) {
    p.ObjectStart(name);
    DumpVkSampleCountFlags(p, "sampleLocationSampleCounts", obj.sampleLocationSampleCounts, 32);
    DumpVkExtent2D(p, "maxSampleLocationGridSize", obj.maxSampleLocationGridSize);
    p.ArrayStart("sampleLocationCoordinateRange", 2);
    p.PrintElement(obj.sampleLocationCoordinateRange[0]);
    p.PrintElement(obj.sampleLocationCoordinateRange[1]);
    p.ArrayEnd();
    p.PrintKeyValue("sampleLocationSubPixelBits", obj.sampleLocationSubPixelBits, 32);
    p.PrintKeyBool("variableSampleLocations", obj.variableSampleLocations, 32);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("filterMinmaxSingleComponentFormats", obj.filterMinmaxSingleComponentFormats, 34);
    p.PrintKeyBool("filterMinmaxImageComponentMapping", obj.filterMinmaxImageComponentMapping, 34);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceSamplerYcbcrConversionFeatures(Printer &p, std::string name, VkPhysicalDeviceSamplerYcbcrConversionFeatures &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("samplerYcbcrConversion", obj.samplerYcbcrConversion, 22);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceScalarBlockLayoutFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("scalarBlockLayout", obj.scalarBlockLayout, 17);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceShaderAtomicInt64FeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("shaderBufferInt64Atomics", obj.shaderBufferInt64Atomics, 24);
    p.PrintKeyBool("shaderSharedInt64Atomics", obj.shaderSharedInt64Atomics, 24);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceShaderClockFeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceShaderClockFeaturesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("shaderSubgroupClock", obj.shaderSubgroupClock, 19);
    p.PrintKeyBool("shaderDeviceClock", obj.shaderDeviceClock, 19);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("shaderDemoteToHelperInvocation", obj.shaderDemoteToHelperInvocation, 30);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceShaderDrawParametersFeatures(Printer &p, std::string name, VkPhysicalDeviceShaderDrawParametersFeatures &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("shaderDrawParameters", obj.shaderDrawParameters, 20);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceShaderFloat16Int8FeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("shaderFloat16", obj.shaderFloat16, 13);
    p.PrintKeyBool("shaderInt8", obj.shaderInt8, 13);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("shaderSubgroupExtendedTypes", obj.shaderSubgroupExtendedTypes, 27);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceSparseProperties(Printer &p, std::string name, VkPhysicalDeviceSparseProperties &obj) {
    if (p.Type() == OutputType::json)
        p.ObjectStart("sparseProperties");
    else
        p.SetSubHeader().ObjectStart(name);
    p.PrintKeyBool("residencyStandard2DBlockShape", obj.residencyStandard2DBlockShape, 40);
    p.PrintKeyBool("residencyStandard2DMultisampleBlockShape", obj.residencyStandard2DMultisampleBlockShape, 40);
    p.PrintKeyBool("residencyStandard3DBlockShape", obj.residencyStandard3DBlockShape, 40);
    p.PrintKeyBool("residencyAlignedMipSize", obj.residencyAlignedMipSize, 40);
    p.PrintKeyBool("residencyNonResidentStrict", obj.residencyNonResidentStrict, 40);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceSubgroupProperties(Printer &p, std::string name, VkPhysicalDeviceSubgroupProperties &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("subgroupSize", obj.subgroupSize, 25);
    DumpVkShaderStageFlags(p, "supportedStages", obj.supportedStages, 25);
    DumpVkSubgroupFeatureFlags(p, "supportedOperations", obj.supportedOperations, 25);
    p.PrintKeyBool("quadOperationsInAllStages", obj.quadOperationsInAllStages, 25);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceSubgroupSizeControlFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("subgroupSizeControl", obj.subgroupSizeControl, 20);
    p.PrintKeyBool("computeFullSubgroups", obj.computeFullSubgroups, 20);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceSubgroupSizeControlPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceSubgroupSizeControlPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("minSubgroupSize", obj.minSubgroupSize, 28);
    p.PrintKeyValue("maxSubgroupSize", obj.maxSubgroupSize, 28);
    p.PrintKeyValue("maxComputeWorkgroupSubgroups", obj.maxComputeWorkgroupSubgroups, 28);
    DumpVkShaderStageFlags(p, "requiredSubgroupSizeStages", obj.requiredSubgroupSizeStages, 28);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceTexelBufferAlignmentFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("texelBufferAlignment", obj.texelBufferAlignment, 20);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceTexelBufferAlignmentPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("storageTexelBufferOffsetAlignmentBytes", to_hex_str(p, obj.storageTexelBufferOffsetAlignmentBytes), 44);
    p.PrintKeyBool("storageTexelBufferOffsetSingleTexelAlignment", obj.storageTexelBufferOffsetSingleTexelAlignment, 44);
    p.PrintKeyValue("uniformTexelBufferOffsetAlignmentBytes", to_hex_str(p, obj.uniformTexelBufferOffsetAlignmentBytes), 44);
    p.PrintKeyBool("uniformTexelBufferOffsetSingleTexelAlignment", obj.uniformTexelBufferOffsetSingleTexelAlignment, 44);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("textureCompressionASTC_HDR", obj.textureCompressionASTC_HDR, 26);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceTimelineSemaphoreFeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("timelineSemaphore", obj.timelineSemaphore, 17);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceTimelineSemaphorePropertiesKHR(Printer &p, std::string name, VkPhysicalDeviceTimelineSemaphorePropertiesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("maxTimelineSemaphoreValueDifference", obj.maxTimelineSemaphoreValueDifference, 35);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceTransformFeedbackFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceTransformFeedbackFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("transformFeedback", obj.transformFeedback, 17);
    p.PrintKeyBool("geometryStreams", obj.geometryStreams, 17);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceTransformFeedbackPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceTransformFeedbackPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("maxTransformFeedbackStreams", obj.maxTransformFeedbackStreams, 42);
    p.PrintKeyValue("maxTransformFeedbackBuffers", obj.maxTransformFeedbackBuffers, 42);
    p.PrintKeyValue("maxTransformFeedbackBufferSize", to_hex_str(p, obj.maxTransformFeedbackBufferSize), 42);
    p.PrintKeyValue("maxTransformFeedbackStreamDataSize", obj.maxTransformFeedbackStreamDataSize, 42);
    p.PrintKeyValue("maxTransformFeedbackBufferDataSize", obj.maxTransformFeedbackBufferDataSize, 42);
    p.PrintKeyValue("maxTransformFeedbackBufferDataStride", obj.maxTransformFeedbackBufferDataStride, 42);
    p.PrintKeyBool("transformFeedbackQueries", obj.transformFeedbackQueries, 42);
    p.PrintKeyBool("transformFeedbackStreamsLinesTriangles", obj.transformFeedbackStreamsLinesTriangles, 42);
    p.PrintKeyBool("transformFeedbackRasterizationStreamSelect", obj.transformFeedbackRasterizationStreamSelect, 42);
    p.PrintKeyBool("transformFeedbackDraw", obj.transformFeedbackDraw, 42);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("uniformBufferStandardLayout", obj.uniformBufferStandardLayout, 27);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceVariablePointersFeatures(Printer &p, std::string name, VkPhysicalDeviceVariablePointersFeatures &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("variablePointersStorageBuffer", obj.variablePointersStorageBuffer, 29);
    p.PrintKeyBool("variablePointers", obj.variablePointers, 29);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceVertexAttributeDivisorFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("vertexAttributeInstanceRateDivisor", obj.vertexAttributeInstanceRateDivisor, 38);
    p.PrintKeyBool("vertexAttributeInstanceRateZeroDivisor", obj.vertexAttributeInstanceRateZeroDivisor, 38);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceVertexAttributeDivisorPropertiesEXT(Printer &p, std::string name, VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("maxVertexAttribDivisor", obj.maxVertexAttribDivisor, 22);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceVulkanMemoryModelFeaturesKHR(Printer &p, std::string name, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("vulkanMemoryModel", obj.vulkanMemoryModel, 45);
    p.PrintKeyBool("vulkanMemoryModelDeviceScope", obj.vulkanMemoryModelDeviceScope, 45);
    p.PrintKeyBool("vulkanMemoryModelAvailabilityVisibilityChains", obj.vulkanMemoryModelAvailabilityVisibilityChains, 45);
    p.ObjectEnd();
}
void DumpVkPhysicalDeviceYcbcrImageArraysFeaturesEXT(Printer &p, std::string name, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("ycbcrImageArrays", obj.ycbcrImageArrays, 16);
    p.ObjectEnd();
}
void DumpVkSharedPresentSurfaceCapabilitiesKHR(Printer &p, std::string name, VkSharedPresentSurfaceCapabilitiesKHR &obj) {
    p.ObjectStart(name);
    DumpVkImageUsageFlags(p, "sharedPresentSupportedUsageFlags", obj.sharedPresentSupportedUsageFlags, 5);
    p.ObjectEnd();
}
#ifdef VK_USE_PLATFORM_WIN32_KHR
void DumpVkSurfaceCapabilitiesFullScreenExclusiveEXT(Printer &p, std::string name, VkSurfaceCapabilitiesFullScreenExclusiveEXT &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("fullScreenExclusiveSupported", obj.fullScreenExclusiveSupported, 28);
    p.ObjectEnd();
}
#endif  // VK_USE_PLATFORM_WIN32_KHR
void DumpVkSurfaceCapabilitiesKHR(Printer &p, std::string name, VkSurfaceCapabilitiesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyValue("minImageCount", obj.minImageCount, 19);
    p.PrintKeyValue("maxImageCount", obj.maxImageCount, 19);
    DumpVkExtent2D(p, "currentExtent", obj.currentExtent);
    DumpVkExtent2D(p, "minImageExtent", obj.minImageExtent);
    DumpVkExtent2D(p, "maxImageExtent", obj.maxImageExtent);
    p.PrintKeyValue("maxImageArrayLayers", obj.maxImageArrayLayers, 19);
    DumpVkSurfaceTransformFlagsKHR(p, "supportedTransforms", obj.supportedTransforms, 19);
    DumpVkSurfaceTransformFlagBitsKHR(p, "currentTransform", obj.currentTransform, 19);
    DumpVkCompositeAlphaFlagsKHR(p, "supportedCompositeAlpha", obj.supportedCompositeAlpha, 19);
    DumpVkImageUsageFlags(p, "supportedUsageFlags", obj.supportedUsageFlags, 19);
    p.ObjectEnd();
}
void DumpVkSurfaceFormatKHR(Printer &p, std::string name, VkSurfaceFormatKHR &obj) {
    p.ObjectStart(name);
    DumpVkFormat(p, "format", obj.format, 6);
    DumpVkColorSpaceKHR(p, "colorSpace", obj.colorSpace, 6);
    p.ObjectEnd();
}
void DumpVkSurfaceProtectedCapabilitiesKHR(Printer &p, std::string name, VkSurfaceProtectedCapabilitiesKHR &obj) {
    p.ObjectStart(name);
    p.PrintKeyBool("supportsProtected", obj.supportsProtected, 17);
    p.ObjectEnd();
}
pNextChainInfos get_chain_infos() {
    pNextChainInfos infos;
    infos.phys_device_props2 = {
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT, sizeof(VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT, sizeof(VkPhysicalDeviceConservativeRasterizationPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR, sizeof(VkPhysicalDeviceDepthStencilResolvePropertiesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT, sizeof(VkPhysicalDeviceDescriptorIndexingPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT, sizeof(VkPhysicalDeviceDiscardRectanglePropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR, sizeof(VkPhysicalDeviceDriverPropertiesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT, sizeof(VkPhysicalDeviceExternalMemoryHostPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR, sizeof(VkPhysicalDeviceFloatControlsPropertiesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT, sizeof(VkPhysicalDeviceFragmentDensityMapPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES, sizeof(VkPhysicalDeviceIDProperties)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT, sizeof(VkPhysicalDeviceInlineUniformBlockPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT, sizeof(VkPhysicalDeviceLineRasterizationPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES, sizeof(VkPhysicalDeviceMaintenance3Properties)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES, sizeof(VkPhysicalDeviceMultiviewProperties)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT, sizeof(VkPhysicalDevicePCIBusInfoPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES, sizeof(VkPhysicalDevicePointClippingProperties)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES, sizeof(VkPhysicalDeviceProtectedMemoryProperties)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR, sizeof(VkPhysicalDevicePushDescriptorPropertiesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT, sizeof(VkPhysicalDeviceSampleLocationsPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT, sizeof(VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES, sizeof(VkPhysicalDeviceSubgroupProperties)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT, sizeof(VkPhysicalDeviceSubgroupSizeControlPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT, sizeof(VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR, sizeof(VkPhysicalDeviceTimelineSemaphorePropertiesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT, sizeof(VkPhysicalDeviceTransformFeedbackPropertiesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT, sizeof(VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT)},
    };
    infos.phys_device_mem_props2 = {
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT, sizeof(VkPhysicalDeviceMemoryBudgetPropertiesEXT)},
    };
    infos.phys_device_features2 = {
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES, sizeof(VkPhysicalDevice16BitStorageFeatures)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR, sizeof(VkPhysicalDevice8BitStorageFeaturesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT, sizeof(VkPhysicalDeviceASTCDecodeFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT, sizeof(VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT, sizeof(VkPhysicalDeviceBufferDeviceAddressFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT, sizeof(VkPhysicalDeviceConditionalRenderingFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT, sizeof(VkPhysicalDeviceDepthClipEnableFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, sizeof(VkPhysicalDeviceDescriptorIndexingFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT, sizeof(VkPhysicalDeviceFragmentDensityMapFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT, sizeof(VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT, sizeof(VkPhysicalDeviceHostQueryResetFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR, sizeof(VkPhysicalDeviceImagelessFramebufferFeaturesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT, sizeof(VkPhysicalDeviceIndexTypeUint8FeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT, sizeof(VkPhysicalDeviceInlineUniformBlockFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT, sizeof(VkPhysicalDeviceLineRasterizationFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT, sizeof(VkPhysicalDeviceMemoryPriorityFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES, sizeof(VkPhysicalDeviceMultiviewFeatures)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR, sizeof(VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES, sizeof(VkPhysicalDeviceProtectedMemoryFeatures)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES, sizeof(VkPhysicalDeviceSamplerYcbcrConversionFeatures)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT, sizeof(VkPhysicalDeviceScalarBlockLayoutFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR, sizeof(VkPhysicalDeviceShaderAtomicInt64FeaturesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR, sizeof(VkPhysicalDeviceShaderClockFeaturesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT, sizeof(VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES, sizeof(VkPhysicalDeviceShaderDrawParametersFeatures)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR, sizeof(VkPhysicalDeviceShaderFloat16Int8FeaturesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR, sizeof(VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT, sizeof(VkPhysicalDeviceSubgroupSizeControlFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT, sizeof(VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT, sizeof(VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR, sizeof(VkPhysicalDeviceTimelineSemaphoreFeaturesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT, sizeof(VkPhysicalDeviceTransformFeedbackFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR, sizeof(VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES, sizeof(VkPhysicalDeviceVariablePointersFeatures)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT, sizeof(VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR, sizeof(VkPhysicalDeviceVulkanMemoryModelFeaturesKHR)},
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT, sizeof(VkPhysicalDeviceYcbcrImageArraysFeaturesEXT)},
    };
    infos.surface_capabilities2 = {
        {VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR, sizeof(VkSharedPresentSurfaceCapabilitiesKHR)},
#ifdef VK_USE_PLATFORM_WIN32_KHR
        {VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_FULL_SCREEN_EXCLUSIVE_EXT, sizeof(VkSurfaceCapabilitiesFullScreenExclusiveEXT)},
#endif  // VK_USE_PLATFORM_WIN32_KHR
        {VK_STRUCTURE_TYPE_SURFACE_PROTECTED_CAPABILITIES_KHR, sizeof(VkSurfaceProtectedCapabilitiesKHR)},
    };
    infos.format_properties2 = {
        {VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_EXT, sizeof(VkDrmFormatModifierPropertiesListEXT)},
    };
    return infos;
}
void chain_iterator_phys_device_props2(Printer &p, AppGpu &gpu, void * place) {
    while (place) {
        struct VkStructureHeader *structure = (struct VkStructureHeader *)place;
        p.SetSubHeader();
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
            VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT* props = (VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT*)structure;
            DumpVkPhysicalDeviceBlendOperationAdvancedPropertiesEXT(p, "VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME)) {
            VkPhysicalDeviceConservativeRasterizationPropertiesEXT* props = (VkPhysicalDeviceConservativeRasterizationPropertiesEXT*)structure;
            DumpVkPhysicalDeviceConservativeRasterizationPropertiesEXT(p, "VkPhysicalDeviceConservativeRasterizationPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)) {
            VkPhysicalDeviceDepthStencilResolvePropertiesKHR* props = (VkPhysicalDeviceDepthStencilResolvePropertiesKHR*)structure;
            DumpVkPhysicalDeviceDepthStencilResolvePropertiesKHR(p, "VkPhysicalDeviceDepthStencilResolvePropertiesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
            VkPhysicalDeviceDescriptorIndexingPropertiesEXT* props = (VkPhysicalDeviceDescriptorIndexingPropertiesEXT*)structure;
            DumpVkPhysicalDeviceDescriptorIndexingPropertiesEXT(p, "VkPhysicalDeviceDescriptorIndexingPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME)) {
            VkPhysicalDeviceDiscardRectanglePropertiesEXT* props = (VkPhysicalDeviceDiscardRectanglePropertiesEXT*)structure;
            DumpVkPhysicalDeviceDiscardRectanglePropertiesEXT(p, "VkPhysicalDeviceDiscardRectanglePropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME)) {
            VkPhysicalDeviceDriverPropertiesKHR* props = (VkPhysicalDeviceDriverPropertiesKHR*)structure;
            DumpVkPhysicalDeviceDriverPropertiesKHR(p, "VkPhysicalDeviceDriverPropertiesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME)) {
            VkPhysicalDeviceExternalMemoryHostPropertiesEXT* props = (VkPhysicalDeviceExternalMemoryHostPropertiesEXT*)structure;
            DumpVkPhysicalDeviceExternalMemoryHostPropertiesEXT(p, "VkPhysicalDeviceExternalMemoryHostPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME)) {
            VkPhysicalDeviceFloatControlsPropertiesKHR* props = (VkPhysicalDeviceFloatControlsPropertiesKHR*)structure;
            DumpVkPhysicalDeviceFloatControlsPropertiesKHR(p, "VkPhysicalDeviceFloatControlsPropertiesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
            VkPhysicalDeviceFragmentDensityMapPropertiesEXT* props = (VkPhysicalDeviceFragmentDensityMapPropertiesEXT*)structure;
            DumpVkPhysicalDeviceFragmentDensityMapPropertiesEXT(p, "VkPhysicalDeviceFragmentDensityMapPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES) {
            VkPhysicalDeviceIDProperties* props = (VkPhysicalDeviceIDProperties*)structure;
            DumpVkPhysicalDeviceIDProperties(p, "VkPhysicalDeviceIDProperties", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
            VkPhysicalDeviceInlineUniformBlockPropertiesEXT* props = (VkPhysicalDeviceInlineUniformBlockPropertiesEXT*)structure;
            DumpVkPhysicalDeviceInlineUniformBlockPropertiesEXT(p, "VkPhysicalDeviceInlineUniformBlockPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
            VkPhysicalDeviceLineRasterizationPropertiesEXT* props = (VkPhysicalDeviceLineRasterizationPropertiesEXT*)structure;
            DumpVkPhysicalDeviceLineRasterizationPropertiesEXT(p, "VkPhysicalDeviceLineRasterizationPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES) {
            VkPhysicalDeviceMaintenance3Properties* props = (VkPhysicalDeviceMaintenance3Properties*)structure;
            DumpVkPhysicalDeviceMaintenance3Properties(p, "VkPhysicalDeviceMaintenance3Properties", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES) {
            VkPhysicalDeviceMultiviewProperties* props = (VkPhysicalDeviceMultiviewProperties*)structure;
            DumpVkPhysicalDeviceMultiviewProperties(p, "VkPhysicalDeviceMultiviewProperties", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_PCI_BUS_INFO_EXTENSION_NAME)) {
            VkPhysicalDevicePCIBusInfoPropertiesEXT* props = (VkPhysicalDevicePCIBusInfoPropertiesEXT*)structure;
            DumpVkPhysicalDevicePCIBusInfoPropertiesEXT(p, "VkPhysicalDevicePCIBusInfoPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES) {
            VkPhysicalDevicePointClippingProperties* props = (VkPhysicalDevicePointClippingProperties*)structure;
            DumpVkPhysicalDevicePointClippingProperties(p, "VkPhysicalDevicePointClippingProperties", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES) {
            VkPhysicalDeviceProtectedMemoryProperties* props = (VkPhysicalDeviceProtectedMemoryProperties*)structure;
            DumpVkPhysicalDeviceProtectedMemoryProperties(p, "VkPhysicalDeviceProtectedMemoryProperties", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)) {
            VkPhysicalDevicePushDescriptorPropertiesKHR* props = (VkPhysicalDevicePushDescriptorPropertiesKHR*)structure;
            DumpVkPhysicalDevicePushDescriptorPropertiesKHR(p, "VkPhysicalDevicePushDescriptorPropertiesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME)) {
            VkPhysicalDeviceSampleLocationsPropertiesEXT* props = (VkPhysicalDeviceSampleLocationsPropertiesEXT*)structure;
            DumpVkPhysicalDeviceSampleLocationsPropertiesEXT(p, "VkPhysicalDeviceSampleLocationsPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME)) {
            VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT* props = (VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT*)structure;
            DumpVkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT(p, "VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES) {
            VkPhysicalDeviceSubgroupProperties* props = (VkPhysicalDeviceSubgroupProperties*)structure;
            DumpVkPhysicalDeviceSubgroupProperties(p, "VkPhysicalDeviceSubgroupProperties", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
            VkPhysicalDeviceSubgroupSizeControlPropertiesEXT* props = (VkPhysicalDeviceSubgroupSizeControlPropertiesEXT*)structure;
            DumpVkPhysicalDeviceSubgroupSizeControlPropertiesEXT(p, "VkPhysicalDeviceSubgroupSizeControlPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
            VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT* props = (VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT*)structure;
            DumpVkPhysicalDeviceTexelBufferAlignmentPropertiesEXT(p, "VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
            VkPhysicalDeviceTimelineSemaphorePropertiesKHR* props = (VkPhysicalDeviceTimelineSemaphorePropertiesKHR*)structure;
            DumpVkPhysicalDeviceTimelineSemaphorePropertiesKHR(p, "VkPhysicalDeviceTimelineSemaphorePropertiesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
            VkPhysicalDeviceTransformFeedbackPropertiesEXT* props = (VkPhysicalDeviceTransformFeedbackPropertiesEXT*)structure;
            DumpVkPhysicalDeviceTransformFeedbackPropertiesEXT(p, "VkPhysicalDeviceTransformFeedbackPropertiesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
            VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT* props = (VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT*)structure;
            DumpVkPhysicalDeviceVertexAttributeDivisorPropertiesEXT(p, "VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT", *props);
            p.AddNewline();
        }
        place = structure->pNext;
    }
}
void chain_iterator_phys_device_mem_props2(Printer &p, AppGpu &gpu, void * place) {
    while (place) {
        struct VkStructureHeader *structure = (struct VkStructureHeader *)place;
        p.SetSubHeader();
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME)) {
            VkPhysicalDeviceMemoryBudgetPropertiesEXT* props = (VkPhysicalDeviceMemoryBudgetPropertiesEXT*)structure;
            DumpVkPhysicalDeviceMemoryBudgetPropertiesEXT(p, "VkPhysicalDeviceMemoryBudgetPropertiesEXT", *props);
            p.AddNewline();
        }
        place = structure->pNext;
    }
}
void chain_iterator_phys_device_features2(Printer &p, AppGpu &gpu, void * place) {
    while (place) {
        struct VkStructureHeader *structure = (struct VkStructureHeader *)place;
        p.SetSubHeader();
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES) {
            VkPhysicalDevice16BitStorageFeatures* props = (VkPhysicalDevice16BitStorageFeatures*)structure;
            DumpVkPhysicalDevice16BitStorageFeatures(p, "VkPhysicalDevice16BitStorageFeatures", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_8BIT_STORAGE_EXTENSION_NAME)) {
            VkPhysicalDevice8BitStorageFeaturesKHR* props = (VkPhysicalDevice8BitStorageFeaturesKHR*)structure;
            DumpVkPhysicalDevice8BitStorageFeaturesKHR(p, "VkPhysicalDevice8BitStorageFeaturesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME)) {
            VkPhysicalDeviceASTCDecodeFeaturesEXT* props = (VkPhysicalDeviceASTCDecodeFeaturesEXT*)structure;
            DumpVkPhysicalDeviceASTCDecodeFeaturesEXT(p, "VkPhysicalDeviceASTCDecodeFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
            VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT* props = (VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT*)structure;
            DumpVkPhysicalDeviceBlendOperationAdvancedFeaturesEXT(p, "VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
            VkPhysicalDeviceBufferDeviceAddressFeaturesEXT* props = (VkPhysicalDeviceBufferDeviceAddressFeaturesEXT*)structure;
            DumpVkPhysicalDeviceBufferDeviceAddressFeaturesEXT(p, "VkPhysicalDeviceBufferDeviceAddressFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME)) {
            VkPhysicalDeviceConditionalRenderingFeaturesEXT* props = (VkPhysicalDeviceConditionalRenderingFeaturesEXT*)structure;
            DumpVkPhysicalDeviceConditionalRenderingFeaturesEXT(p, "VkPhysicalDeviceConditionalRenderingFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME)) {
            VkPhysicalDeviceDepthClipEnableFeaturesEXT* props = (VkPhysicalDeviceDepthClipEnableFeaturesEXT*)structure;
            DumpVkPhysicalDeviceDepthClipEnableFeaturesEXT(p, "VkPhysicalDeviceDepthClipEnableFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
            VkPhysicalDeviceDescriptorIndexingFeaturesEXT* props = (VkPhysicalDeviceDescriptorIndexingFeaturesEXT*)structure;
            DumpVkPhysicalDeviceDescriptorIndexingFeaturesEXT(p, "VkPhysicalDeviceDescriptorIndexingFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
            VkPhysicalDeviceFragmentDensityMapFeaturesEXT* props = (VkPhysicalDeviceFragmentDensityMapFeaturesEXT*)structure;
            DumpVkPhysicalDeviceFragmentDensityMapFeaturesEXT(p, "VkPhysicalDeviceFragmentDensityMapFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME)) {
            VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT* props = (VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT*)structure;
            DumpVkPhysicalDeviceFragmentShaderInterlockFeaturesEXT(p, "VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)) {
            VkPhysicalDeviceHostQueryResetFeaturesEXT* props = (VkPhysicalDeviceHostQueryResetFeaturesEXT*)structure;
            DumpVkPhysicalDeviceHostQueryResetFeaturesEXT(p, "VkPhysicalDeviceHostQueryResetFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME)) {
            VkPhysicalDeviceImagelessFramebufferFeaturesKHR* props = (VkPhysicalDeviceImagelessFramebufferFeaturesKHR*)structure;
            DumpVkPhysicalDeviceImagelessFramebufferFeaturesKHR(p, "VkPhysicalDeviceImagelessFramebufferFeaturesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME)) {
            VkPhysicalDeviceIndexTypeUint8FeaturesEXT* props = (VkPhysicalDeviceIndexTypeUint8FeaturesEXT*)structure;
            DumpVkPhysicalDeviceIndexTypeUint8FeaturesEXT(p, "VkPhysicalDeviceIndexTypeUint8FeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
            VkPhysicalDeviceInlineUniformBlockFeaturesEXT* props = (VkPhysicalDeviceInlineUniformBlockFeaturesEXT*)structure;
            DumpVkPhysicalDeviceInlineUniformBlockFeaturesEXT(p, "VkPhysicalDeviceInlineUniformBlockFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
            VkPhysicalDeviceLineRasterizationFeaturesEXT* props = (VkPhysicalDeviceLineRasterizationFeaturesEXT*)structure;
            DumpVkPhysicalDeviceLineRasterizationFeaturesEXT(p, "VkPhysicalDeviceLineRasterizationFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)) {
            VkPhysicalDeviceMemoryPriorityFeaturesEXT* props = (VkPhysicalDeviceMemoryPriorityFeaturesEXT*)structure;
            DumpVkPhysicalDeviceMemoryPriorityFeaturesEXT(p, "VkPhysicalDeviceMemoryPriorityFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES) {
            VkPhysicalDeviceMultiviewFeatures* props = (VkPhysicalDeviceMultiviewFeatures*)structure;
            DumpVkPhysicalDeviceMultiviewFeatures(p, "VkPhysicalDeviceMultiviewFeatures", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME)) {
            VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR* props = (VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR*)structure;
            DumpVkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR(p, "VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES) {
            VkPhysicalDeviceProtectedMemoryFeatures* props = (VkPhysicalDeviceProtectedMemoryFeatures*)structure;
            DumpVkPhysicalDeviceProtectedMemoryFeatures(p, "VkPhysicalDeviceProtectedMemoryFeatures", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES) {
            VkPhysicalDeviceSamplerYcbcrConversionFeatures* props = (VkPhysicalDeviceSamplerYcbcrConversionFeatures*)structure;
            DumpVkPhysicalDeviceSamplerYcbcrConversionFeatures(p, "VkPhysicalDeviceSamplerYcbcrConversionFeatures", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME)) {
            VkPhysicalDeviceScalarBlockLayoutFeaturesEXT* props = (VkPhysicalDeviceScalarBlockLayoutFeaturesEXT*)structure;
            DumpVkPhysicalDeviceScalarBlockLayoutFeaturesEXT(p, "VkPhysicalDeviceScalarBlockLayoutFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME)) {
            VkPhysicalDeviceShaderAtomicInt64FeaturesKHR* props = (VkPhysicalDeviceShaderAtomicInt64FeaturesKHR*)structure;
            DumpVkPhysicalDeviceShaderAtomicInt64FeaturesKHR(p, "VkPhysicalDeviceShaderAtomicInt64FeaturesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_SHADER_CLOCK_EXTENSION_NAME)) {
            VkPhysicalDeviceShaderClockFeaturesKHR* props = (VkPhysicalDeviceShaderClockFeaturesKHR*)structure;
            DumpVkPhysicalDeviceShaderClockFeaturesKHR(p, "VkPhysicalDeviceShaderClockFeaturesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME)) {
            VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT* props = (VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT*)structure;
            DumpVkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT(p, "VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES) {
            VkPhysicalDeviceShaderDrawParametersFeatures* props = (VkPhysicalDeviceShaderDrawParametersFeatures*)structure;
            DumpVkPhysicalDeviceShaderDrawParametersFeatures(p, "VkPhysicalDeviceShaderDrawParametersFeatures", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME)) {
            VkPhysicalDeviceShaderFloat16Int8FeaturesKHR* props = (VkPhysicalDeviceShaderFloat16Int8FeaturesKHR*)structure;
            DumpVkPhysicalDeviceShaderFloat16Int8FeaturesKHR(p, "VkPhysicalDeviceShaderFloat16Int8FeaturesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME)) {
            VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR* props = (VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR*)structure;
            DumpVkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR(p, "VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
            VkPhysicalDeviceSubgroupSizeControlFeaturesEXT* props = (VkPhysicalDeviceSubgroupSizeControlFeaturesEXT*)structure;
            DumpVkPhysicalDeviceSubgroupSizeControlFeaturesEXT(p, "VkPhysicalDeviceSubgroupSizeControlFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
            VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT* props = (VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT*)structure;
            DumpVkPhysicalDeviceTexelBufferAlignmentFeaturesEXT(p, "VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME)) {
            VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT* props = (VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT*)structure;
            DumpVkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT(p, "VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
            VkPhysicalDeviceTimelineSemaphoreFeaturesKHR* props = (VkPhysicalDeviceTimelineSemaphoreFeaturesKHR*)structure;
            DumpVkPhysicalDeviceTimelineSemaphoreFeaturesKHR(p, "VkPhysicalDeviceTimelineSemaphoreFeaturesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
            VkPhysicalDeviceTransformFeedbackFeaturesEXT* props = (VkPhysicalDeviceTransformFeedbackFeaturesEXT*)structure;
            DumpVkPhysicalDeviceTransformFeedbackFeaturesEXT(p, "VkPhysicalDeviceTransformFeedbackFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME)) {
            VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR* props = (VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR*)structure;
            DumpVkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR(p, "VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES) {
            VkPhysicalDeviceVariablePointersFeatures* props = (VkPhysicalDeviceVariablePointersFeatures*)structure;
            DumpVkPhysicalDeviceVariablePointersFeatures(p, "VkPhysicalDeviceVariablePointersFeatures", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
            VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT* props = (VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT*)structure;
            DumpVkPhysicalDeviceVertexAttributeDivisorFeaturesEXT(p, "VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME)) {
            VkPhysicalDeviceVulkanMemoryModelFeaturesKHR* props = (VkPhysicalDeviceVulkanMemoryModelFeaturesKHR*)structure;
            DumpVkPhysicalDeviceVulkanMemoryModelFeaturesKHR(p, "VkPhysicalDeviceVulkanMemoryModelFeaturesKHR", *props);
            p.AddNewline();
        }
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_YCBCR_IMAGE_ARRAYS_EXTENSION_NAME)) {
            VkPhysicalDeviceYcbcrImageArraysFeaturesEXT* props = (VkPhysicalDeviceYcbcrImageArraysFeaturesEXT*)structure;
            DumpVkPhysicalDeviceYcbcrImageArraysFeaturesEXT(p, "VkPhysicalDeviceYcbcrImageArraysFeaturesEXT", *props);
            p.AddNewline();
        }
        place = structure->pNext;
    }
}
void chain_iterator_surface_capabilities2(Printer &p, AppInstance &inst, AppGpu &gpu, void * place) {
    while (place) {
        struct VkStructureHeader *structure = (struct VkStructureHeader *)place;
        p.SetSubHeader();
        if (structure->sType == VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)) {
            VkSharedPresentSurfaceCapabilitiesKHR* props = (VkSharedPresentSurfaceCapabilitiesKHR*)structure;
            DumpVkSharedPresentSurfaceCapabilitiesKHR(p, "VkSharedPresentSurfaceCapabilitiesKHR", *props);
            p.AddNewline();
        }
#ifdef VK_USE_PLATFORM_WIN32_KHR
        if (structure->sType == VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_FULL_SCREEN_EXCLUSIVE_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME)) {
            VkSurfaceCapabilitiesFullScreenExclusiveEXT* props = (VkSurfaceCapabilitiesFullScreenExclusiveEXT*)structure;
            DumpVkSurfaceCapabilitiesFullScreenExclusiveEXT(p, "VkSurfaceCapabilitiesFullScreenExclusiveEXT", *props);
            p.AddNewline();
        }
#endif  // VK_USE_PLATFORM_WIN32_KHR
        if (structure->sType == VK_STRUCTURE_TYPE_SURFACE_PROTECTED_CAPABILITIES_KHR &&
            inst.CheckExtensionEnabled(VK_KHR_SURFACE_PROTECTED_CAPABILITIES_EXTENSION_NAME)) {
            VkSurfaceProtectedCapabilitiesKHR* props = (VkSurfaceProtectedCapabilitiesKHR*)structure;
            DumpVkSurfaceProtectedCapabilitiesKHR(p, "VkSurfaceProtectedCapabilitiesKHR", *props);
            p.AddNewline();
        }
        place = structure->pNext;
    }
}
void chain_iterator_format_properties2(Printer &p, AppGpu &gpu, void * place) {
    while (place) {
        struct VkStructureHeader *structure = (struct VkStructureHeader *)place;
        p.SetSubHeader();
        if (structure->sType == VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_EXT &&
            gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_IMAGE_DRM_FORMAT_MODIFIER_EXTENSION_NAME)) {
            VkDrmFormatModifierPropertiesListEXT* props = (VkDrmFormatModifierPropertiesListEXT*)structure;
            DumpVkDrmFormatModifierPropertiesListEXT(p, "VkDrmFormatModifierPropertiesListEXT", *props);
            p.AddNewline();
        }
        place = structure->pNext;
    }
}
bool operator==(const VkExtent2D & a, const VkExtent2D b);
bool operator==(const VkSurfaceCapabilities2EXT & a, const VkSurfaceCapabilities2EXT b);
bool operator==(const VkSurfaceCapabilities2KHR & a, const VkSurfaceCapabilities2KHR b);
bool operator==(const VkSurfaceCapabilitiesKHR & a, const VkSurfaceCapabilitiesKHR b);
bool operator==(const VkSurfaceFormat2KHR & a, const VkSurfaceFormat2KHR b);
bool operator==(const VkSurfaceFormatKHR & a, const VkSurfaceFormatKHR b);
bool operator==(const VkExtent2D & a, const VkExtent2D b) {
    return a.width == b.width
        && a.height == b.height;
}
bool operator==(const VkSurfaceCapabilities2EXT & a, const VkSurfaceCapabilities2EXT b) {
    return a.minImageCount == b.minImageCount
        && a.maxImageCount == b.maxImageCount
        && a.currentExtent == b.currentExtent
        && a.minImageExtent == b.minImageExtent
        && a.maxImageExtent == b.maxImageExtent
        && a.maxImageArrayLayers == b.maxImageArrayLayers
        && a.supportedTransforms == b.supportedTransforms
        && a.currentTransform == b.currentTransform
        && a.supportedCompositeAlpha == b.supportedCompositeAlpha
        && a.supportedUsageFlags == b.supportedUsageFlags
        && a.supportedSurfaceCounters == b.supportedSurfaceCounters;
}
bool operator==(const VkSurfaceCapabilities2KHR & a, const VkSurfaceCapabilities2KHR b) {
    return a.surfaceCapabilities == b.surfaceCapabilities;
}
bool operator==(const VkSurfaceCapabilitiesKHR & a, const VkSurfaceCapabilitiesKHR b) {
    return a.minImageCount == b.minImageCount
        && a.maxImageCount == b.maxImageCount
        && a.currentExtent == b.currentExtent
        && a.minImageExtent == b.minImageExtent
        && a.maxImageExtent == b.maxImageExtent
        && a.maxImageArrayLayers == b.maxImageArrayLayers
        && a.supportedTransforms == b.supportedTransforms
        && a.currentTransform == b.currentTransform
        && a.supportedCompositeAlpha == b.supportedCompositeAlpha
        && a.supportedUsageFlags == b.supportedUsageFlags;
}
bool operator==(const VkSurfaceFormat2KHR & a, const VkSurfaceFormat2KHR b) {
    return a.surfaceFormat == b.surfaceFormat;
}
bool operator==(const VkSurfaceFormatKHR & a, const VkSurfaceFormatKHR b) {
    return a.format == b.format
        && a.colorSpace == b.colorSpace;
}

