/*
 * Copyright (c) 2015-2019 The Khronos Group Inc.
 * Copyright (c) 2015-2019 Valve Corporation
 * Copyright (c) 2015-2019 LunarG, Inc.
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
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: David Pinedo <david@lunarg.com>
 * Author: Mark Lobodzinski <mark@lunarg.com>
 * Author: Rene Lindsay <rene@lunarg.com>
 * Author: Jeremy Kniager <jeremyk@lunarg.com>
 * Author: Shannon McPherson <shannon@lunarg.com>
 * Author: Bob Ellison <bob@lunarg.com>
 */

#ifdef __GNUC__
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#else
#define strndup(p, n) strdup(p)
#endif

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif  // _WIN32

#if defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_XCB_KHR)
#include <X11/Xutil.h>
#endif

#if defined(VK_USE_PLATFORM_MACOS_MVK)
#include "metal_view.h"
#endif

#include <vulkan/vulkan.h>

#define ERR(err) fprintf(stderr, "%s:%d: failed with %s\n", __FILE__, __LINE__, VkResultString(err));

#ifdef _WIN32

#define strdup _strdup

// Returns nonzero if the console is used only for this process. Will return
// zero if another process (such as cmd.exe) is also attached.
static int ConsoleIsExclusive(void) {
    DWORD pids[2];
    DWORD num_pids = GetConsoleProcessList(pids, ARRAYSIZE(pids));
    return num_pids <= 1;
}

#define WAIT_FOR_CONSOLE_DESTROY                   \
    do {                                           \
        if (ConsoleIsExclusive()) Sleep(INFINITE); \
    } while (0)
#else
#define WAIT_FOR_CONSOLE_DESTROY
#endif

#define ERR_EXIT(err)             \
    do {                          \
        ERR(err);                 \
        fflush(stdout);           \
        fflush(stderr);           \
        WAIT_FOR_CONSOLE_DESTROY; \
        exit(-1);                 \
    } while (0)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define MAX_QUEUE_TYPES 5
#define APP_SHORT_NAME "vulkaninfo"

static bool html_output = false;
static bool human_readable_output = true;
static bool json_output = false;
static uint32_t selected_gpu = 0;

struct VkStructureHeader {
    VkStructureType sType;
    void *pNext;
};

struct pNextChainBuildingBlockInfo {
    VkStructureType sType;
    uint32_t mem_size;
};

struct LayerExtensionList {
    VkLayerProperties layer_properties;
    uint32_t extension_count;
    VkExtensionProperties *extension_properties;
};

struct AppInstance;

struct SurfaceExtensionNode {
    struct SurfaceExtensionNode *next;

    const char *name;
    void (*create_window)(struct AppInstance *);
    VkSurfaceKHR (*create_surface)(struct AppInstance *);
    void (*destroy_window)(struct AppInstance *);
    VkSurfaceKHR surface;
    VkBool32 supports_present;
};

struct AppInstance {
    VkInstance instance;
    uint32_t instance_version;
    uint32_t vulkan_major;
    uint32_t vulkan_minor;
    uint32_t vulkan_patch;

    uint32_t global_layer_count;
    struct LayerExtensionList *global_layers;
    uint32_t global_extension_count;
    VkExtensionProperties *global_extensions;  // Instance Extensions

    const char **inst_extensions;
    uint32_t inst_extensions_count;

    // Functions from vkGetInstanceProcAddress
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormats2KHR vkGetPhysicalDeviceSurfaceFormats2KHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR;
    PFN_vkGetPhysicalDeviceFormatProperties2KHR vkGetPhysicalDeviceFormatProperties2KHR;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR vkGetPhysicalDeviceQueueFamilyProperties2KHR;
    PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR;
    PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vkGetPhysicalDeviceSurfaceCapabilities2EXT;

    struct SurfaceExtensionNode *surface_ext_infos_root;

    int width, height;

#ifdef VK_USE_PLATFORM_WIN32_KHR
    HINSTANCE h_instance;  // Windows Instance
    HWND h_wnd;            // window handle
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    xcb_connection_t *xcb_connection;
    xcb_screen_t *xcb_screen;
    xcb_window_t xcb_window;
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
    Display *xlib_display;
    Window xlib_window;
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR  // TODO
    struct ANativeWindow *window;
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
    void *window;
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    struct wl_display *wayland_display;
    struct wl_surface *wayland_surface;
#endif
};

struct MemResSupport {
    struct MemImageSupport {
        bool regular_supported, sparse_supported, transient_supported;
        VkFormat format;
        uint32_t regular_memtypes, sparse_memtypes, transient_memtypes;
    } image[2][1 + 7];
    // TODO: buffers
};

struct AppGpu {
    uint32_t id;
    VkPhysicalDevice obj;

    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceProperties2KHR props2;

    uint32_t queue_count;
    VkQueueFamilyProperties *queue_props;
    VkQueueFamilyProperties2KHR *queue_props2;
    VkDeviceQueueCreateInfo *queue_reqs;

    struct AppInstance *inst;

    VkPhysicalDeviceMemoryProperties memory_props;
    VkPhysicalDeviceMemoryProperties2KHR memory_props2;

    struct MemResSupport mem_type_res_support;

    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceFeatures2KHR features2;
    VkPhysicalDevice limits;

    uint32_t device_extension_count;
    VkExtensionProperties *device_extensions;

    VkDevice dev;
    VkPhysicalDeviceFeatures enabled_features;
};

// return most severe flag only
static const char *DebugReportFlagString(const VkDebugReportFlagsEXT flags) {
    switch (flags) {
        case VK_DEBUG_REPORT_ERROR_BIT_EXT:
            return "ERROR";
        case VK_DEBUG_REPORT_WARNING_BIT_EXT:
            return "WARNING";
        case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
            return "PERF";
        case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
            return "INFO";
        case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DbgCallback(VkDebugReportFlagsEXT msgFlags, VkDebugReportObjectTypeEXT objType,
                                                  uint64_t srcObject, size_t location, int32_t msgCode, const char *pLayerPrefix,
                                                  const char *pMsg, void *pUserData) {
    fprintf(stderr, "%s: [%s] Code %d : %s\n", DebugReportFlagString(msgFlags), pLayerPrefix, msgCode, pMsg);
    fflush(stderr);

    // True is reserved for layer developers, and MAY mean calls are not distributed down the layer chain after validation error.
    // False SHOULD always be returned by apps:
    return VK_FALSE;
}

static const char *VkResultString(VkResult err) {
    switch (err) {
#define STR(r) \
    case r:    \
        return #r
        STR(VK_SUCCESS);
        STR(VK_NOT_READY);
        STR(VK_TIMEOUT);
        STR(VK_EVENT_SET);
        STR(VK_EVENT_RESET);
        STR(VK_INCOMPLETE);
        STR(VK_ERROR_OUT_OF_HOST_MEMORY);
        STR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        STR(VK_ERROR_INITIALIZATION_FAILED);
        STR(VK_ERROR_DEVICE_LOST);
        STR(VK_ERROR_MEMORY_MAP_FAILED);
        STR(VK_ERROR_LAYER_NOT_PRESENT);
        STR(VK_ERROR_EXTENSION_NOT_PRESENT);
        STR(VK_ERROR_FEATURE_NOT_PRESENT);
        STR(VK_ERROR_INCOMPATIBLE_DRIVER);
        STR(VK_ERROR_TOO_MANY_OBJECTS);
        STR(VK_ERROR_FORMAT_NOT_SUPPORTED);
        STR(VK_ERROR_FRAGMENTED_POOL);
        STR(VK_ERROR_OUT_OF_POOL_MEMORY);
        STR(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        STR(VK_ERROR_SURFACE_LOST_KHR);
        STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        STR(VK_SUBOPTIMAL_KHR);
        STR(VK_ERROR_OUT_OF_DATE_KHR);
        STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        STR(VK_ERROR_VALIDATION_FAILED_EXT);
        STR(VK_ERROR_INVALID_SHADER_NV);
        STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        STR(VK_ERROR_FRAGMENTATION_EXT);
        STR(VK_ERROR_NOT_PERMITTED_EXT);
#undef STR
        default:
            return "UNKNOWN_RESULT";
    }
}

static const char *VkPhysicalDeviceTypeString(VkPhysicalDeviceType type) {
    switch (type) {
#define STR(r)                        \
    case VK_PHYSICAL_DEVICE_TYPE_##r: \
        return #r
        STR(OTHER);
        STR(INTEGRATED_GPU);
        STR(DISCRETE_GPU);
        STR(VIRTUAL_GPU);
        STR(CPU);
#undef STR
        default:
            return "UNKNOWN_DEVICE";
    }
}

static const char *VkTilingString(const VkImageTiling tiling) {
    switch (tiling) {
#define STR(r)                \
    case VK_IMAGE_TILING_##r: \
        return #r
        STR(OPTIMAL);
        STR(LINEAR);
        STR(DRM_FORMAT_MODIFIER_EXT);
#undef STR
        default:
            return "UNKNOWN_TILING";
    }
}

static const char *VkColorSpaceString(VkColorSpaceKHR cs) {
    switch (cs) {
#define STR(r)               \
    case VK_COLOR_SPACE_##r: \
        return #r
        STR(SRGB_NONLINEAR_KHR);
        STR(DISPLAY_P3_NONLINEAR_EXT);
        STR(EXTENDED_SRGB_LINEAR_EXT);
        STR(DISPLAY_P3_LINEAR_EXT);
        STR(DCI_P3_NONLINEAR_EXT);
        STR(BT709_LINEAR_EXT);
        STR(BT709_NONLINEAR_EXT);
        STR(BT2020_LINEAR_EXT);
        STR(HDR10_ST2084_EXT);
        STR(DOLBYVISION_EXT);
        STR(HDR10_HLG_EXT);
        STR(ADOBERGB_LINEAR_EXT);
        STR(ADOBERGB_NONLINEAR_EXT);
        STR(PASS_THROUGH_EXT);
        STR(EXTENDED_SRGB_NONLINEAR_EXT);
        STR(DISPLAY_NATIVE_AMD);
#undef STR
        default:
            return "UNKNOWN_COLOR_SPACE";
    }
}

static const char *VkFormatString(VkFormat fmt) {
    switch (fmt) {
#define STR(r)          \
    case VK_FORMAT_##r: \
        return #r
        STR(UNDEFINED);
        STR(R4G4_UNORM_PACK8);
        STR(R4G4B4A4_UNORM_PACK16);
        STR(B4G4R4A4_UNORM_PACK16);
        STR(R5G6B5_UNORM_PACK16);
        STR(B5G6R5_UNORM_PACK16);
        STR(R5G5B5A1_UNORM_PACK16);
        STR(B5G5R5A1_UNORM_PACK16);
        STR(A1R5G5B5_UNORM_PACK16);
        STR(R8_UNORM);
        STR(R8_SNORM);
        STR(R8_USCALED);
        STR(R8_SSCALED);
        STR(R8_UINT);
        STR(R8_SINT);
        STR(R8_SRGB);
        STR(R8G8_UNORM);
        STR(R8G8_SNORM);
        STR(R8G8_USCALED);
        STR(R8G8_SSCALED);
        STR(R8G8_UINT);
        STR(R8G8_SINT);
        STR(R8G8_SRGB);
        STR(R8G8B8_UNORM);
        STR(R8G8B8_SNORM);
        STR(R8G8B8_USCALED);
        STR(R8G8B8_SSCALED);
        STR(R8G8B8_UINT);
        STR(R8G8B8_SINT);
        STR(R8G8B8_SRGB);
        STR(B8G8R8_UNORM);
        STR(B8G8R8_SNORM);
        STR(B8G8R8_USCALED);
        STR(B8G8R8_SSCALED);
        STR(B8G8R8_UINT);
        STR(B8G8R8_SINT);
        STR(B8G8R8_SRGB);
        STR(R8G8B8A8_UNORM);
        STR(R8G8B8A8_SNORM);
        STR(R8G8B8A8_USCALED);
        STR(R8G8B8A8_SSCALED);
        STR(R8G8B8A8_UINT);
        STR(R8G8B8A8_SINT);
        STR(R8G8B8A8_SRGB);
        STR(B8G8R8A8_UNORM);
        STR(B8G8R8A8_SNORM);
        STR(B8G8R8A8_USCALED);
        STR(B8G8R8A8_SSCALED);
        STR(B8G8R8A8_UINT);
        STR(B8G8R8A8_SINT);
        STR(B8G8R8A8_SRGB);
        STR(A8B8G8R8_UNORM_PACK32);
        STR(A8B8G8R8_SNORM_PACK32);
        STR(A8B8G8R8_USCALED_PACK32);
        STR(A8B8G8R8_SSCALED_PACK32);
        STR(A8B8G8R8_UINT_PACK32);
        STR(A8B8G8R8_SINT_PACK32);
        STR(A8B8G8R8_SRGB_PACK32);
        STR(A2R10G10B10_UNORM_PACK32);
        STR(A2R10G10B10_SNORM_PACK32);
        STR(A2R10G10B10_USCALED_PACK32);
        STR(A2R10G10B10_SSCALED_PACK32);
        STR(A2R10G10B10_UINT_PACK32);
        STR(A2R10G10B10_SINT_PACK32);
        STR(A2B10G10R10_UNORM_PACK32);
        STR(A2B10G10R10_SNORM_PACK32);
        STR(A2B10G10R10_USCALED_PACK32);
        STR(A2B10G10R10_SSCALED_PACK32);
        STR(A2B10G10R10_UINT_PACK32);
        STR(A2B10G10R10_SINT_PACK32);
        STR(R16_UNORM);
        STR(R16_SNORM);
        STR(R16_USCALED);
        STR(R16_SSCALED);
        STR(R16_UINT);
        STR(R16_SINT);
        STR(R16_SFLOAT);
        STR(R16G16_UNORM);
        STR(R16G16_SNORM);
        STR(R16G16_USCALED);
        STR(R16G16_SSCALED);
        STR(R16G16_UINT);
        STR(R16G16_SINT);
        STR(R16G16_SFLOAT);
        STR(R16G16B16_UNORM);
        STR(R16G16B16_SNORM);
        STR(R16G16B16_USCALED);
        STR(R16G16B16_SSCALED);
        STR(R16G16B16_UINT);
        STR(R16G16B16_SINT);
        STR(R16G16B16_SFLOAT);
        STR(R16G16B16A16_UNORM);
        STR(R16G16B16A16_SNORM);
        STR(R16G16B16A16_USCALED);
        STR(R16G16B16A16_SSCALED);
        STR(R16G16B16A16_UINT);
        STR(R16G16B16A16_SINT);
        STR(R16G16B16A16_SFLOAT);
        STR(R32_UINT);
        STR(R32_SINT);
        STR(R32_SFLOAT);
        STR(R32G32_UINT);
        STR(R32G32_SINT);
        STR(R32G32_SFLOAT);
        STR(R32G32B32_UINT);
        STR(R32G32B32_SINT);
        STR(R32G32B32_SFLOAT);
        STR(R32G32B32A32_UINT);
        STR(R32G32B32A32_SINT);
        STR(R32G32B32A32_SFLOAT);
        STR(R64_UINT);
        STR(R64_SINT);
        STR(R64_SFLOAT);
        STR(R64G64_UINT);
        STR(R64G64_SINT);
        STR(R64G64_SFLOAT);
        STR(R64G64B64_UINT);
        STR(R64G64B64_SINT);
        STR(R64G64B64_SFLOAT);
        STR(R64G64B64A64_UINT);
        STR(R64G64B64A64_SINT);
        STR(R64G64B64A64_SFLOAT);
        STR(B10G11R11_UFLOAT_PACK32);
        STR(E5B9G9R9_UFLOAT_PACK32);
        STR(D16_UNORM);
        STR(X8_D24_UNORM_PACK32);
        STR(D32_SFLOAT);
        STR(S8_UINT);
        STR(D16_UNORM_S8_UINT);
        STR(D24_UNORM_S8_UINT);
        STR(D32_SFLOAT_S8_UINT);
        STR(BC1_RGB_UNORM_BLOCK);
        STR(BC1_RGB_SRGB_BLOCK);
        STR(BC1_RGBA_UNORM_BLOCK);
        STR(BC1_RGBA_SRGB_BLOCK);
        STR(BC2_UNORM_BLOCK);
        STR(BC2_SRGB_BLOCK);
        STR(BC3_UNORM_BLOCK);
        STR(BC3_SRGB_BLOCK);
        STR(BC4_UNORM_BLOCK);
        STR(BC4_SNORM_BLOCK);
        STR(BC5_UNORM_BLOCK);
        STR(BC5_SNORM_BLOCK);
        STR(BC6H_UFLOAT_BLOCK);
        STR(BC6H_SFLOAT_BLOCK);
        STR(BC7_UNORM_BLOCK);
        STR(BC7_SRGB_BLOCK);
        STR(ETC2_R8G8B8_UNORM_BLOCK);
        STR(ETC2_R8G8B8_SRGB_BLOCK);
        STR(ETC2_R8G8B8A1_UNORM_BLOCK);
        STR(ETC2_R8G8B8A1_SRGB_BLOCK);
        STR(ETC2_R8G8B8A8_UNORM_BLOCK);
        STR(ETC2_R8G8B8A8_SRGB_BLOCK);
        STR(EAC_R11_UNORM_BLOCK);
        STR(EAC_R11_SNORM_BLOCK);
        STR(EAC_R11G11_UNORM_BLOCK);
        STR(EAC_R11G11_SNORM_BLOCK);
        STR(ASTC_4x4_UNORM_BLOCK);
        STR(ASTC_4x4_SRGB_BLOCK);
        STR(ASTC_5x4_UNORM_BLOCK);
        STR(ASTC_5x4_SRGB_BLOCK);
        STR(ASTC_5x5_UNORM_BLOCK);
        STR(ASTC_5x5_SRGB_BLOCK);
        STR(ASTC_6x5_UNORM_BLOCK);
        STR(ASTC_6x5_SRGB_BLOCK);
        STR(ASTC_6x6_UNORM_BLOCK);
        STR(ASTC_6x6_SRGB_BLOCK);
        STR(ASTC_8x5_UNORM_BLOCK);
        STR(ASTC_8x5_SRGB_BLOCK);
        STR(ASTC_8x6_UNORM_BLOCK);
        STR(ASTC_8x6_SRGB_BLOCK);
        STR(ASTC_8x8_UNORM_BLOCK);
        STR(ASTC_8x8_SRGB_BLOCK);
        STR(ASTC_10x5_UNORM_BLOCK);
        STR(ASTC_10x5_SRGB_BLOCK);
        STR(ASTC_10x6_UNORM_BLOCK);
        STR(ASTC_10x6_SRGB_BLOCK);
        STR(ASTC_10x8_UNORM_BLOCK);
        STR(ASTC_10x8_SRGB_BLOCK);
        STR(ASTC_10x10_UNORM_BLOCK);
        STR(ASTC_10x10_SRGB_BLOCK);
        STR(ASTC_12x10_UNORM_BLOCK);
        STR(ASTC_12x10_SRGB_BLOCK);
        STR(ASTC_12x12_UNORM_BLOCK);
        STR(ASTC_12x12_SRGB_BLOCK);
        STR(G8B8G8R8_422_UNORM);
        STR(B8G8R8G8_422_UNORM);
        STR(G8_B8_R8_3PLANE_420_UNORM);
        STR(G8_B8R8_2PLANE_420_UNORM);
        STR(G8_B8_R8_3PLANE_422_UNORM);
        STR(G8_B8R8_2PLANE_422_UNORM);
        STR(G8_B8_R8_3PLANE_444_UNORM);
        STR(R10X6_UNORM_PACK16);
        STR(R10X6G10X6_UNORM_2PACK16);
        STR(R10X6G10X6B10X6A10X6_UNORM_4PACK16);
        STR(G10X6B10X6G10X6R10X6_422_UNORM_4PACK16);
        STR(B10X6G10X6R10X6G10X6_422_UNORM_4PACK16);
        STR(G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16);
        STR(G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16);
        STR(G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16);
        STR(G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16);
        STR(G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16);
        STR(R12X4_UNORM_PACK16);
        STR(R12X4G12X4_UNORM_2PACK16);
        STR(R12X4G12X4B12X4A12X4_UNORM_4PACK16);
        STR(G12X4B12X4G12X4R12X4_422_UNORM_4PACK16);
        STR(B12X4G12X4R12X4G12X4_422_UNORM_4PACK16);
        STR(G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16);
        STR(G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16);
        STR(G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16);
        STR(G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16);
        STR(G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16);
        STR(G16B16G16R16_422_UNORM);
        STR(B16G16R16G16_422_UNORM);
        STR(G16_B16_R16_3PLANE_420_UNORM);
        STR(G16_B16R16_2PLANE_420_UNORM);
        STR(G16_B16_R16_3PLANE_422_UNORM);
        STR(G16_B16R16_2PLANE_422_UNORM);
        STR(G16_B16_R16_3PLANE_444_UNORM);
        STR(PVRTC1_2BPP_UNORM_BLOCK_IMG);
        STR(PVRTC1_4BPP_UNORM_BLOCK_IMG);
        STR(PVRTC2_2BPP_UNORM_BLOCK_IMG);
        STR(PVRTC2_4BPP_UNORM_BLOCK_IMG);
        STR(PVRTC1_2BPP_SRGB_BLOCK_IMG);
        STR(PVRTC1_4BPP_SRGB_BLOCK_IMG);
        STR(PVRTC2_2BPP_SRGB_BLOCK_IMG);
        STR(PVRTC2_4BPP_SRGB_BLOCK_IMG);
#undef STR
        default:
            return "UNKNOWN_FORMAT";
    }
}

#if defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WIN32_KHR) || \
    defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_WAYLAND_KHR)
static const char *VkPresentModeString(VkPresentModeKHR mode) {
    switch (mode) {
#define STR(r)                \
    case VK_PRESENT_MODE_##r: \
        return #r
        STR(IMMEDIATE_KHR);
        STR(MAILBOX_KHR);
        STR(FIFO_KHR);
        STR(FIFO_RELAXED_KHR);
        STR(SHARED_DEMAND_REFRESH_KHR);
        STR(SHARED_CONTINUOUS_REFRESH_KHR);
#undef STR
        default:
            return "UNKNOWN_FORMAT";
    }
}
#endif

static const char *VkShaderFloatControlsIndependenceString(const VkShaderFloatControlsIndependenceKHR indep) {
    switch (indep) {
#define STR(r)                                      \
    case VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_##r: \
        return #r
        STR(32_BIT_ONLY_KHR);
        STR(ALL_KHR);
        STR(NONE_KHR);
#undef STR
        default:
            return "UNKNOWN_INDEPENDENCE";
    }
}

static bool CheckExtensionEnabled(const char *extension_to_check, const char **extension_list, uint32_t extension_count) {
    for (uint32_t i = 0; i < extension_count; ++i) {
        if (!strncmp(extension_to_check, extension_list[i], VK_MAX_EXTENSION_NAME_SIZE)) {
            return true;
        }
    }
    return false;
}

static bool CheckPhysicalDeviceExtensionIncluded(const char *extension_to_check, VkExtensionProperties *extension_list,
                                                 uint32_t extension_count) {
    for (uint32_t i = 0; i < extension_count; ++i) {
        if (!strncmp(extension_to_check, extension_list[i].extensionName, VK_MAX_EXTENSION_NAME_SIZE)) {
            return true;
        }
    }
    return false;
}

static void buildpNextChain(struct VkStructureHeader *first, const struct pNextChainBuildingBlockInfo *chain_info,
                            uint32_t chain_info_len) {
    struct VkStructureHeader *place = first;

    for (uint32_t i = 0; i < chain_info_len; i++) {
        place->pNext = malloc(chain_info[i].mem_size);
        if (!place->pNext) {
            ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
        }
        memset(place->pNext, 0, chain_info[i].mem_size);
        place = place->pNext;
        place->sType = chain_info[i].sType;
    }

    place->pNext = NULL;
}

static void freepNextChain(struct VkStructureHeader *first) {
    struct VkStructureHeader *place = first;
    struct VkStructureHeader *next = NULL;

    while (place) {
        next = place->pNext;
        free(place);
        place = next;
    }
}

static void ExtractVersion(uint32_t version, uint32_t *major, uint32_t *minor, uint32_t *patch) {
    *major = version >> 22;
    *minor = (version >> 12) & 0x3ff;
    *patch = version & 0xfff;
}

static void AppGetPhysicalDeviceLayerExtensions(struct AppGpu *gpu, char *layer_name, uint32_t *extension_count,
                                                VkExtensionProperties **extension_properties) {
    VkResult err;
    uint32_t ext_count = 0;
    VkExtensionProperties *ext_ptr = NULL;

    /* repeat get until VK_INCOMPLETE goes away */
    do {
        err = vkEnumerateDeviceExtensionProperties(gpu->obj, layer_name, &ext_count, NULL);
        if (err) ERR_EXIT(err);

        if (ext_ptr) {
            free(ext_ptr);
        }
        ext_ptr = malloc(ext_count * sizeof(VkExtensionProperties));
        err = vkEnumerateDeviceExtensionProperties(gpu->obj, layer_name, &ext_count, ext_ptr);
    } while (err == VK_INCOMPLETE);
    if (err) ERR_EXIT(err);

    *extension_count = ext_count;
    *extension_properties = ext_ptr;
}

static void AppGetGlobalLayerExtensions(char *layer_name, uint32_t *extension_count, VkExtensionProperties **extension_properties) {
    VkResult err;
    uint32_t ext_count = 0;
    VkExtensionProperties *ext_ptr = NULL;

    /* repeat get until VK_INCOMPLETE goes away */
    do {
        // gets the extension count if the last parameter is NULL
        err = vkEnumerateInstanceExtensionProperties(layer_name, &ext_count, NULL);
        if (err) ERR_EXIT(err);

        if (ext_ptr) {
            free(ext_ptr);
        }
        ext_ptr = malloc(ext_count * sizeof(VkExtensionProperties));
        // gets the extension properties if the last parameter is not NULL
        err = vkEnumerateInstanceExtensionProperties(layer_name, &ext_count, ext_ptr);
    } while (err == VK_INCOMPLETE);
    if (err) ERR_EXIT(err);
    *extension_count = ext_count;
    *extension_properties = ext_ptr;
}

/* Gets a list of layer and instance extensions */
static void AppGetInstanceExtensions(struct AppInstance *inst) {
    VkResult err;

    uint32_t count = 0;

    /* Scan layers */
    VkLayerProperties *global_layer_properties = NULL;
    struct LayerExtensionList *global_layers = NULL;

    do {
        err = vkEnumerateInstanceLayerProperties(&count, NULL);
        if (err) ERR_EXIT(err);

        if (global_layer_properties) {
            free(global_layer_properties);
        }
        global_layer_properties = malloc(sizeof(VkLayerProperties) * count);
        assert(global_layer_properties);

        if (global_layers) {
            free(global_layers);
        }
        global_layers = malloc(sizeof(struct LayerExtensionList) * count);
        assert(global_layers);

        err = vkEnumerateInstanceLayerProperties(&count, global_layer_properties);
    } while (err == VK_INCOMPLETE);
    if (err) ERR_EXIT(err);

    inst->global_layer_count = count;
    inst->global_layers = global_layers;

    for (uint32_t i = 0; i < inst->global_layer_count; ++i) {
        VkLayerProperties *src_info = &global_layer_properties[i];
        struct LayerExtensionList *dst_info = &inst->global_layers[i];
        memcpy(&dst_info->layer_properties, src_info, sizeof(VkLayerProperties));

        // Save away layer extension info for report
        // Gets layer extensions, if first parameter is not NULL
        AppGetGlobalLayerExtensions(src_info->layerName, &dst_info->extension_count, &dst_info->extension_properties);
    }
    free(global_layer_properties);

    // Collect global extensions
    inst->global_extension_count = 0;
    // Gets instance extensions, if no layer was specified in the first
    // paramteter
    AppGetGlobalLayerExtensions(NULL, &inst->global_extension_count, &inst->global_extensions);
}

// Prints opening code for html output file
void PrintHtmlHeader(FILE *out) {
    fprintf(out, "<!doctype html>\n");
    fprintf(out, "<html lang='en'>\n");
    fprintf(out, "\t<head>\n");
    fprintf(out, "\t\t<title>vulkaninfo</title>\n");
    fprintf(out, "\t\t<style>\n");
    fprintf(out, "\t\thtml {\n");
    fprintf(out, "\t\t\tbackground-color: #0b1e48;\n");
    fprintf(out, "\t\t\tbackground-image: url(\"https://vulkan.lunarg.com/img/bg-starfield.jpg\");\n");
    fprintf(out, "\t\t\tbackground-position: center;\n");
    fprintf(out, "\t\t\t-webkit-background-size: cover;\n");
    fprintf(out, "\t\t\t-moz-background-size: cover;\n");
    fprintf(out, "\t\t\t-o-background-size: cover;\n");
    fprintf(out, "\t\t\tbackground-size: cover;\n");
    fprintf(out, "\t\t\tbackground-attachment: fixed;\n");
    fprintf(out, "\t\t\tbackground-repeat: no-repeat;\n");
    fprintf(out, "\t\t\theight: 100%%;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t#header {\n");
    fprintf(out, "\t\t\tz-index: -1;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t#header>img {\n");
    fprintf(out, "\t\t\tposition: absolute;\n");
    fprintf(out, "\t\t\twidth: 160px;\n");
    fprintf(out, "\t\t\tmargin-left: -280px;\n");
    fprintf(out, "\t\t\ttop: -10px;\n");
    fprintf(out, "\t\t\tleft: 50%%;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t#header>h1 {\n");
    fprintf(out, "\t\t\tfont-family: Arial, \"Helvetica Neue\", Helvetica, sans-serif;\n");
    fprintf(out, "\t\t\tfont-size: 44px;\n");
    fprintf(out, "\t\t\tfont-weight: 200;\n");
    fprintf(out, "\t\t\ttext-shadow: 4px 4px 5px #000;\n");
    fprintf(out, "\t\t\tcolor: #eee;\n");
    fprintf(out, "\t\t\tposition: absolute;\n");
    fprintf(out, "\t\t\twidth: 400px;\n");
    fprintf(out, "\t\t\tmargin-left: -80px;\n");
    fprintf(out, "\t\t\ttop: 8px;\n");
    fprintf(out, "\t\t\tleft: 50%%;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\tbody {\n");
    fprintf(out, "\t\t\tfont-family: Consolas, monaco, monospace;\n");
    fprintf(out, "\t\t\tfont-size: 14px;\n");
    fprintf(out, "\t\t\tline-height: 20px;\n");
    fprintf(out, "\t\t\tcolor: #eee;\n");
    fprintf(out, "\t\t\theight: 100%%;\n");
    fprintf(out, "\t\t\tmargin: 0;\n");
    fprintf(out, "\t\t\toverflow: hidden;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t#wrapper {\n");
    fprintf(out, "\t\t\tbackground-color: rgba(0, 0, 0, 0.7);\n");
    fprintf(out, "\t\t\tborder: 1px solid #446;\n");
    fprintf(out, "\t\t\tbox-shadow: 0px 0px 10px #000;\n");
    fprintf(out, "\t\t\tpadding: 8px 12px;\n\n");
    fprintf(out, "\t\t\tdisplay: inline-block;\n");
    fprintf(out, "\t\t\tposition: absolute;\n");
    fprintf(out, "\t\t\ttop: 80px;\n");
    fprintf(out, "\t\t\tbottom: 25px;\n");
    fprintf(out, "\t\t\tleft: 50px;\n");
    fprintf(out, "\t\t\tright: 50px;\n");
    fprintf(out, "\t\t\toverflow: auto;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\tdetails>details {\n");
    fprintf(out, "\t\t\tmargin-left: 22px;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\tdetails>summary:only-child::-webkit-details-marker {\n");
    fprintf(out, "\t\t\tdisplay: none;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t.var, .type, .val {\n");
    fprintf(out, "\t\t\tdisplay: inline;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t.var {\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t.type {\n");
    fprintf(out, "\t\t\tcolor: #acf;\n");
    fprintf(out, "\t\t\tmargin: 0 12px;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t.val {\n");
    fprintf(out, "\t\t\tcolor: #afa;\n");
    fprintf(out, "\t\t\tbackground: #222;\n");
    fprintf(out, "\t\t\ttext-align: right;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t</style>\n");
    fprintf(out, "\t</head>\n");
    fprintf(out, "\t<body>\n");
    fprintf(out, "\t\t<div id='header'>\n");
    fprintf(out, "\t\t\t<h1>vulkaninfo</h1>\n");
    fprintf(out, "\t\t</div>\n");
    fprintf(out, "\t\t<div id='wrapper'>\n");
}

// Prints closing code for html output file
void PrintHtmlFooter(FILE *out) {
    fprintf(out, "\t\t</div>\n");
    fprintf(out, "\t</body>\n");
    fprintf(out, "</html>");
}

// Prints opening code for json output file
void PrintJsonHeader(const int vulkan_major, const int vulkan_minor, const int vulkan_patch) {
    printf("{\n");
    printf("\t\"$schema\": \"https://schema.khronos.org/vulkan/devsim_1_0_0.json#\",\n");
    printf("\t\"comments\": {\n");
    printf("\t\t\"desc\": \"JSON configuration file describing GPU %u. Generated using the vulkaninfo program.\",\n", selected_gpu);
    printf("\t\t\"vulkanApiVersion\": \"%d.%d.%d\"\n", vulkan_major, vulkan_minor, vulkan_patch);
    printf("\t}");
}

// Checks if current argument specifies json output, interprets/updates gpu selection
bool CheckForJsonOption(const char *arg) {
    if (strncmp("--json", arg, 6) == 0 || strcmp(arg, "-j") == 0) {
        if (strlen(arg) > 7 && strncmp("--json=", arg, 7) == 0) {
            selected_gpu = strtol(arg + 7, NULL, 10);
        }
        human_readable_output = false;
        json_output = true;
        return true;
    } else {
        return false;
    }
}

static void AppCompileInstanceExtensionsToEnable(struct AppInstance *inst) {
    // Get all supported Instance extensions (excl. layer-provided ones)
    inst->inst_extensions_count = inst->global_extension_count;
    inst->inst_extensions = malloc(sizeof(char *) * inst->inst_extensions_count);
    if (!inst->inst_extensions) ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);

    for (uint32_t i = 0; i < inst->global_extension_count; ++i) {
        inst->inst_extensions[i] = inst->global_extensions[i].extensionName;
    }
}

static void AppLoadInstanceCommands(struct AppInstance *inst) {
#define LOAD_INSTANCE_VK_CMD(cmd) inst->cmd = (PFN_##cmd)vkGetInstanceProcAddr(inst->instance, #cmd)

    LOAD_INSTANCE_VK_CMD(vkGetPhysicalDeviceSurfaceSupportKHR);
    LOAD_INSTANCE_VK_CMD(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    LOAD_INSTANCE_VK_CMD(vkGetPhysicalDeviceSurfaceFormatsKHR);
    LOAD_INSTANCE_VK_CMD(vkGetPhysicalDeviceSurfaceFormats2KHR);
    LOAD_INSTANCE_VK_CMD(vkGetPhysicalDeviceSurfacePresentModesKHR);
    LOAD_INSTANCE_VK_CMD(vkGetPhysicalDeviceProperties2KHR);
    LOAD_INSTANCE_VK_CMD(vkGetPhysicalDeviceFormatProperties2KHR);
    LOAD_INSTANCE_VK_CMD(vkGetPhysicalDeviceQueueFamilyProperties2KHR);
    LOAD_INSTANCE_VK_CMD(vkGetPhysicalDeviceFeatures2KHR);
    LOAD_INSTANCE_VK_CMD(vkGetPhysicalDeviceMemoryProperties2KHR);
    LOAD_INSTANCE_VK_CMD(vkGetPhysicalDeviceSurfaceCapabilities2KHR);
    LOAD_INSTANCE_VK_CMD(vkGetPhysicalDeviceSurfaceCapabilities2EXT);

#undef LOAD_INSTANCE_VK_CMD
}

static void AppCreateInstance(struct AppInstance *inst) {
    PFN_vkEnumerateInstanceVersion enumerate_instance_version =
        (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");

    if (!enumerate_instance_version) {
        inst->instance_version = VK_API_VERSION_1_0;
    } else {
        const VkResult err = enumerate_instance_version(&inst->instance_version);
        if (err) ERR_EXIT(err);
    }

    inst->vulkan_major = VK_VERSION_MAJOR(inst->instance_version);
    inst->vulkan_minor = VK_VERSION_MINOR(inst->instance_version);
    inst->vulkan_patch = VK_VERSION_PATCH(VK_HEADER_VERSION);

    AppGetInstanceExtensions(inst);

    const VkDebugReportCallbackCreateInfoEXT dbg_info = {.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
                                                         .flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
                                                         .pfnCallback = DbgCallback};

    const VkApplicationInfo app_info = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                        .pApplicationName = APP_SHORT_NAME,
                                        .applicationVersion = 1,
                                        .apiVersion = VK_API_VERSION_1_0};

    AppCompileInstanceExtensionsToEnable(inst);
    const VkInstanceCreateInfo inst_info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                            .pNext = &dbg_info,
                                            .pApplicationInfo = &app_info,
                                            .enabledExtensionCount = inst->inst_extensions_count,
                                            .ppEnabledExtensionNames = inst->inst_extensions};

    VkResult err = vkCreateInstance(&inst_info, NULL, &inst->instance);
    if (err == VK_ERROR_INCOMPATIBLE_DRIVER) {
        fprintf(stderr, "Cannot create Vulkan instance.\n");
        fprintf(stderr,
                "This problem is often caused by a faulty installation of the Vulkan driver or attempting to use a GPU that does "
                "not support Vulkan.\n");
        ERR_EXIT(err);
    } else if (err) {
        ERR_EXIT(err);
    }

    AppLoadInstanceCommands(inst);

    inst->surface_ext_infos_root = NULL;
}

static void AppDestroyInstance(struct AppInstance *inst) {
    free(inst->global_extensions);
    for (uint32_t i = 0; i < inst->global_layer_count; ++i) {
        free(inst->global_layers[i].extension_properties);
    }
    free(inst->global_layers);
    free((char **)inst->inst_extensions);
    vkDestroyInstance(inst->instance, NULL);
}

static void AppGpuInit(struct AppGpu *gpu, struct AppInstance *inst, uint32_t id, VkPhysicalDevice obj) {
    uint32_t i;

    memset(gpu, 0, sizeof(*gpu));

    gpu->id = id;
    gpu->obj = obj;
    gpu->inst = inst;

    vkGetPhysicalDeviceProperties(gpu->obj, &gpu->props);

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        struct pNextChainBuildingBlockInfo chain_info[] = {
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR,
             .mem_size = sizeof(VkPhysicalDevicePointClippingPropertiesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR,
             .mem_size = sizeof(VkPhysicalDevicePushDescriptorPropertiesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceDiscardRectanglePropertiesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceMultiviewPropertiesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceMaintenance3PropertiesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES_KHR, .mem_size = sizeof(VkPhysicalDeviceIDPropertiesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceDriverPropertiesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceFloatControlsPropertiesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT,
             .mem_size = sizeof(VkPhysicalDevicePCIBusInfoPropertiesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceTransformFeedbackPropertiesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceFragmentDensityMapPropertiesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceDescriptorIndexingPropertiesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceDepthStencilResolvePropertiesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceSubgroupSizeControlPropertiesEXT)}};

        uint32_t chain_info_len = ARRAY_SIZE(chain_info);

        gpu->props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
        buildpNextChain((struct VkStructureHeader *)&gpu->props2, chain_info, chain_info_len);

        inst->vkGetPhysicalDeviceProperties2KHR(gpu->obj, &gpu->props2);
    }

    /* get queue count */
    vkGetPhysicalDeviceQueueFamilyProperties(gpu->obj, &gpu->queue_count, NULL);

    gpu->queue_props = malloc(sizeof(gpu->queue_props[0]) * gpu->queue_count);

    if (!gpu->queue_props) {
        ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    }
    vkGetPhysicalDeviceQueueFamilyProperties(gpu->obj, &gpu->queue_count, gpu->queue_props);

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        gpu->queue_props2 = malloc(sizeof(gpu->queue_props2[0]) * gpu->queue_count);

        if (!gpu->queue_props2) {
            ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
        }

        for (i = 0; i < gpu->queue_count; ++i) {
            gpu->queue_props2[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR;
            gpu->queue_props2[i].pNext = NULL;
        }

        inst->vkGetPhysicalDeviceQueueFamilyProperties2KHR(gpu->obj, &gpu->queue_count, gpu->queue_props2);
    }

    /* set up queue requests */
    gpu->queue_reqs = malloc(sizeof(*gpu->queue_reqs) * gpu->queue_count);
    if (!gpu->queue_reqs) {
        ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    }
    for (i = 0; i < gpu->queue_count; ++i) {
        float *queue_priorities = NULL;
        if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                                  gpu->inst->inst_extensions_count)) {
            queue_priorities = malloc(gpu->queue_props2[i].queueFamilyProperties.queueCount * sizeof(float));
        } else {
            queue_priorities = malloc(gpu->queue_props[i].queueCount * sizeof(float));
        }
        if (!queue_priorities) {
            ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
        }

        if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                                  gpu->inst->inst_extensions_count)) {
            memset(queue_priorities, 0, gpu->queue_props2[i].queueFamilyProperties.queueCount * sizeof(float));
        } else {
            memset(queue_priorities, 0, gpu->queue_props[i].queueCount * sizeof(float));
        }

        gpu->queue_reqs[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        gpu->queue_reqs[i].pNext = NULL;
        gpu->queue_reqs[i].flags = 0;
        gpu->queue_reqs[i].queueFamilyIndex = i;

        if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                                  gpu->inst->inst_extensions_count)) {
            gpu->queue_reqs[i].queueCount = gpu->queue_props2[i].queueFamilyProperties.queueCount;
        } else {
            gpu->queue_reqs[i].queueCount = gpu->queue_props[i].queueCount;
        }

        gpu->queue_reqs[i].pQueuePriorities = queue_priorities;
    }

    vkGetPhysicalDeviceMemoryProperties(gpu->obj, &gpu->memory_props);

    vkGetPhysicalDeviceFeatures(gpu->obj, &gpu->features);

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        struct pNextChainBuildingBlockInfo mem_prop_chain_info[] = {
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceMemoryBudgetPropertiesEXT)}};

        uint32_t mem_prop_chain_info_len = ARRAY_SIZE(mem_prop_chain_info);

        gpu->memory_props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2_KHR;
        buildpNextChain((struct VkStructureHeader *)&gpu->memory_props2, mem_prop_chain_info, mem_prop_chain_info_len);

        inst->vkGetPhysicalDeviceMemoryProperties2KHR(gpu->obj, &gpu->memory_props2);

        struct pNextChainBuildingBlockInfo chain_info[] = {
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDevice8BitStorageFeaturesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDevice16BitStorageFeaturesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceVariablePointerFeaturesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceMultiviewFeaturesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceFloat16Int8FeaturesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceShaderAtomicInt64FeaturesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceTransformFeedbackFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceScalarBlockLayoutFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceFragmentDensityMapFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceMemoryPriorityFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_ADDRESS_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceBufferAddressFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceDescriptorIndexingFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceYcbcrImageArraysFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceHostQueryResetFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceImagelessFramebufferFeaturesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceIndexTypeUint8FeaturesEXT)}};

        uint32_t chain_info_len = ARRAY_SIZE(chain_info);

        gpu->features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
        buildpNextChain((struct VkStructureHeader *)&gpu->features2, chain_info, chain_info_len);

        inst->vkGetPhysicalDeviceFeatures2KHR(gpu->obj, &gpu->features2);
    }

    AppGetPhysicalDeviceLayerExtensions(gpu, NULL, &gpu->device_extension_count, &gpu->device_extensions);

    const float queue_priority = 1.0f;
    const VkDeviceQueueCreateInfo q_ci = {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                          .queueFamilyIndex = 0,  // just pick the first one and hope for the best
                                          .queueCount = 1,
                                          .pQueuePriorities = &queue_priority};
    VkPhysicalDeviceFeatures features = {0};
    // if (gpu->features.sparseBinding ) features.sparseBinding = VK_TRUE;
    gpu->enabled_features = features;
    const VkDeviceCreateInfo device_ci = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                          .queueCreateInfoCount = 1,
                                          .pQueueCreateInfos = &q_ci,
                                          // TODO: relevant extensions
                                          .pEnabledFeatures = &gpu->enabled_features};

    VkResult err = vkCreateDevice(gpu->obj, &device_ci, NULL, &gpu->dev);
    if (err) ERR_EXIT(err);

    const VkFormat color_format = VK_FORMAT_R8G8B8A8_UNORM;
    const VkFormat formats[] = {
        color_format,      VK_FORMAT_D16_UNORM,         VK_FORMAT_X8_D24_UNORM_PACK32, VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT,   VK_FORMAT_D32_SFLOAT_S8_UINT};
    assert(ARRAY_SIZE(gpu->mem_type_res_support.image[0]) == ARRAY_SIZE(formats));
    const VkImageUsageFlags usages[] = {0, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT};
    const VkImageCreateFlags flagss[] = {0, VK_IMAGE_CREATE_SPARSE_BINDING_BIT};

    for (size_t fmt_i = 0; fmt_i < ARRAY_SIZE(formats); ++fmt_i) {
        for (VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL; tiling <= VK_IMAGE_TILING_LINEAR; ++tiling) {
            gpu->mem_type_res_support.image[tiling][fmt_i].format = formats[fmt_i];
            gpu->mem_type_res_support.image[tiling][fmt_i].regular_supported = true;
            gpu->mem_type_res_support.image[tiling][fmt_i].sparse_supported = true;
            gpu->mem_type_res_support.image[tiling][fmt_i].transient_supported = true;

            VkFormatProperties fmt_props;
            vkGetPhysicalDeviceFormatProperties(gpu->obj, formats[fmt_i], &fmt_props);
            if ((tiling == VK_IMAGE_TILING_OPTIMAL && fmt_props.optimalTilingFeatures == 0) ||
                (tiling == VK_IMAGE_TILING_LINEAR && fmt_props.linearTilingFeatures == 0)) {
                gpu->mem_type_res_support.image[tiling][fmt_i].regular_supported = false;
                gpu->mem_type_res_support.image[tiling][fmt_i].sparse_supported = false;
                gpu->mem_type_res_support.image[tiling][fmt_i].transient_supported = false;
                continue;
            }

            for (size_t u_i = 0; u_i < ARRAY_SIZE(usages); ++u_i) {
                for (size_t flg_i = 0; flg_i < ARRAY_SIZE(flagss); ++flg_i) {
                    VkImageCreateInfo image_ci = {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                        .flags = flagss[flg_i],
                        .imageType = VK_IMAGE_TYPE_2D,
                        .format = formats[fmt_i],
                        .extent = {8, 8, 1},
                        .mipLevels = 1,
                        .arrayLayers = 1,
                        .samples = VK_SAMPLE_COUNT_1_BIT,
                        .tiling = tiling,
                        .usage = usages[u_i],
                    };

                    if ((image_ci.flags & VK_IMAGE_CREATE_SPARSE_BINDING_BIT) &&
                        (image_ci.usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)) {
                        continue;
                    }

                    if (image_ci.usage == 0 || (image_ci.usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)) {
                        if (image_ci.format == color_format)
                            image_ci.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                        else
                            image_ci.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                    }

                    if (!gpu->enabled_features.sparseBinding && (image_ci.flags & VK_IMAGE_CREATE_SPARSE_BINDING_BIT)) {
                        gpu->mem_type_res_support.image[tiling][fmt_i].sparse_supported = false;
                        continue;
                    }

                    VkImageFormatProperties img_props;
                    err = vkGetPhysicalDeviceImageFormatProperties(gpu->obj, image_ci.format, image_ci.imageType, image_ci.tiling,
                                                                   image_ci.usage, image_ci.flags, &img_props);

                    uint32_t *memtypes;
                    bool *support;

                    if (image_ci.flags == 0 && !(image_ci.usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)) {
                        memtypes = &gpu->mem_type_res_support.image[tiling][fmt_i].regular_memtypes;
                        support = &gpu->mem_type_res_support.image[tiling][fmt_i].regular_supported;
                    } else if ((image_ci.flags & VK_IMAGE_CREATE_SPARSE_BINDING_BIT) &&
                               !(image_ci.usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)) {
                        memtypes = &gpu->mem_type_res_support.image[tiling][fmt_i].sparse_memtypes;
                        support = &gpu->mem_type_res_support.image[tiling][fmt_i].sparse_supported;
                    } else if (image_ci.flags == 0 && (image_ci.usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)) {
                        memtypes = &gpu->mem_type_res_support.image[tiling][fmt_i].transient_memtypes;
                        support = &gpu->mem_type_res_support.image[tiling][fmt_i].transient_supported;
                    } else {
                        assert(false);
                    }

                    if (err == VK_ERROR_FORMAT_NOT_SUPPORTED) {
                        *support = false;
                    } else {
                        if (err) ERR_EXIT(err);

                        VkImage dummy_img;
                        err = vkCreateImage(gpu->dev, &image_ci, NULL, &dummy_img);
                        if (err) ERR_EXIT(err);

                        VkMemoryRequirements mem_req;
                        vkGetImageMemoryRequirements(gpu->dev, dummy_img, &mem_req);
                        *memtypes = mem_req.memoryTypeBits;

                        vkDestroyImage(gpu->dev, dummy_img, NULL);
                    }
                }
            }
        }
    }
    // TODO buffer - memory type compatibility
}

static void AppGpuDestroy(struct AppGpu *gpu) {
    vkDestroyDevice(gpu->dev, NULL);

    free(gpu->device_extensions);

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        freepNextChain(gpu->features2.pNext);
    }

    for (uint32_t i = 0; i < gpu->queue_count; ++i) {
        free((void *)gpu->queue_reqs[i].pQueuePriorities);
    }
    free(gpu->queue_reqs);

    free(gpu->queue_props);
    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        free(gpu->queue_props2);

        freepNextChain(gpu->props2.pNext);
        freepNextChain(gpu->memory_props2.pNext);
    }
}

//-----------------------------------------------------------

//---------------------------Win32---------------------------
#ifdef VK_USE_PLATFORM_WIN32_KHR

// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return (DefWindowProc(hWnd, uMsg, wParam, lParam)); }

static void AppCreateWin32Window(struct AppInstance *inst) {
    inst->h_instance = GetModuleHandle(NULL);

    WNDCLASSEX win_class;

    // Initialize the window class structure:
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = WndProc;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = inst->h_instance;
    win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    win_class.lpszMenuName = NULL;
    win_class.lpszClassName = APP_SHORT_NAME;
    win_class.hInstance = inst->h_instance;
    win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    // Register window class:
    if (!RegisterClassEx(&win_class)) {
        // It didn't work, so try to give a useful error:
        fprintf(stderr, "Failed to register the window class!\n");
        exit(1);
    }
    // Create window with the registered class:
    RECT wr = {0, 0, inst->width, inst->height};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    inst->h_wnd = CreateWindowEx(0,
                                 APP_SHORT_NAME,  // class name
                                 APP_SHORT_NAME,  // app name
                                 // WS_VISIBLE | WS_SYSMENU |
                                 WS_OVERLAPPEDWINDOW,  // window style
                                 100, 100,             // x/y coords
                                 wr.right - wr.left,   // width
                                 wr.bottom - wr.top,   // height
                                 NULL,                 // handle to parent
                                 NULL,                 // handle to menu
                                 inst->h_instance,     // hInstance
                                 NULL);                // no extra parameters
    if (!inst->h_wnd) {
        // It didn't work, so try to give a useful error:
        fprintf(stderr, "Failed to create a window!\n");
        exit(1);
    }
}

static VkSurfaceKHR AppCreateWin32Surface(struct AppInstance *inst) {
    VkWin32SurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.hinstance = inst->h_instance;
    createInfo.hwnd = inst->h_wnd;

    VkSurfaceKHR surface;
    VkResult err = vkCreateWin32SurfaceKHR(inst->instance, &createInfo, NULL, &surface);
    if (err) ERR_EXIT(err);
    return surface;
}

static void AppDestroyWin32Window(struct AppInstance *inst) { DestroyWindow(inst->h_wnd); }
#endif  // VK_USE_PLATFORM_WIN32_KHR
//-----------------------------------------------------------

#if defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WIN32_KHR) || \
    defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_WAYLAND_KHR) || defined(VK_USE_PLATFORM_ANDROID_KHR)
static void AppDestroySurface(struct AppInstance *inst, VkSurfaceKHR surface) {  // same for all platforms
    vkDestroySurfaceKHR(inst->instance, surface, NULL);
}
#endif

//----------------------------XCB----------------------------

#ifdef VK_USE_PLATFORM_XCB_KHR
static void AppCreateXcbWindow(struct AppInstance *inst) {
    //--Init Connection--
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    // API guarantees non-null xcb_connection
    inst->xcb_connection = xcb_connect(NULL, &scr);
    int conn_error = xcb_connection_has_error(inst->xcb_connection);
    if (conn_error) {
        fprintf(stderr, "XCB failed to connect to the X server due to error:%d.\n", conn_error);
        fflush(stderr);
        inst->xcb_connection = NULL;
    }

    setup = xcb_get_setup(inst->xcb_connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0) {
        xcb_screen_next(&iter);
    }

    inst->xcb_screen = iter.data;
    //-------------------

    inst->xcb_window = xcb_generate_id(inst->xcb_connection);
    xcb_create_window(inst->xcb_connection, XCB_COPY_FROM_PARENT, inst->xcb_window, inst->xcb_screen->root, 0, 0, inst->width,
                      inst->height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, inst->xcb_screen->root_visual, 0, NULL);

    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(inst->xcb_connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(inst->xcb_connection, cookie, 0);
    free(reply);
}

static VkSurfaceKHR AppCreateXcbSurface(struct AppInstance *inst) {
    if (!inst->xcb_connection) {
        ERR_EXIT(VK_ERROR_INITIALIZATION_FAILED);
    }

    VkXcbSurfaceCreateInfoKHR xcb_createInfo;
    xcb_createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    xcb_createInfo.pNext = NULL;
    xcb_createInfo.flags = 0;
    xcb_createInfo.connection = inst->xcb_connection;
    xcb_createInfo.window = inst->xcb_window;

    VkSurfaceKHR surface;
    VkResult err = vkCreateXcbSurfaceKHR(inst->instance, &xcb_createInfo, NULL, &surface);
    if (err) ERR_EXIT(err);
    return surface;
}

static void AppDestroyXcbWindow(struct AppInstance *inst) {
    if (!inst->xcb_connection) {
        return;  // Nothing to destroy
    }

    xcb_destroy_window(inst->xcb_connection, inst->xcb_window);
    xcb_disconnect(inst->xcb_connection);
}
#endif  // VK_USE_PLATFORM_XCB_KHR
//-----------------------------------------------------------

//----------------------------XLib---------------------------
#ifdef VK_USE_PLATFORM_XLIB_KHR
static void AppCreateXlibWindow(struct AppInstance *inst) {
    long visualMask = VisualScreenMask;
    int numberOfVisuals;

    inst->xlib_display = XOpenDisplay(NULL);
    if (inst->xlib_display == NULL) {
        fprintf(stderr, "XLib failed to connect to the X server.\nExiting ...\n");
        exit(1);
    }

    XVisualInfo vInfoTemplate = {};
    vInfoTemplate.screen = DefaultScreen(inst->xlib_display);
    XVisualInfo *visualInfo = XGetVisualInfo(inst->xlib_display, visualMask, &vInfoTemplate, &numberOfVisuals);
    inst->xlib_window = XCreateWindow(inst->xlib_display, RootWindow(inst->xlib_display, vInfoTemplate.screen), 0, 0, inst->width,
                                      inst->height, 0, visualInfo->depth, InputOutput, visualInfo->visual, 0, NULL);

    XSync(inst->xlib_display, false);
    XFree(visualInfo);
}

static VkSurfaceKHR AppCreateXlibSurface(struct AppInstance *inst) {
    VkXlibSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.dpy = inst->xlib_display;
    createInfo.window = inst->xlib_window;

    VkSurfaceKHR surface;
    VkResult err = vkCreateXlibSurfaceKHR(inst->instance, &createInfo, NULL, &surface);
    if (err) ERR_EXIT(err);
    return surface;
}

static void AppDestroyXlibWindow(struct AppInstance *inst) {
    XDestroyWindow(inst->xlib_display, inst->xlib_window);
    XCloseDisplay(inst->xlib_display);
}
#endif  // VK_USE_PLATFORM_XLIB_KHR
//-----------------------------------------------------------

#ifdef VK_USE_PLATFORM_MACOS_MVK

static void AppCreateMacOSWindow(struct AppInstance *inst) {
    inst->window = CreateMetalView(inst->width, inst->height);
    if (inst->window == NULL) {
        fprintf(stderr, "Could not create a native Metal view.\nExiting...\n");
        exit(1);
    }
}

static VkSurfaceKHR AppCreateMacOSSurface(struct AppInstance *inst) {
    VkMacOSSurfaceCreateInfoMVK createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.pView = inst->window;

    VkSurfaceKHR surface;
    VkResult err = vkCreateMacOSSurfaceMVK(inst->instance, &createInfo, NULL, &surface);
    if (err) ERR_EXIT(err);
    return surface;
}

static void AppDestroyMacOSWindow(struct AppInstance *inst) { DestroyMetalView(inst->window); }
#endif  // VK_USE_PLATFORM_MACOS_MVK
//-----------------------------------------------------------

#ifdef VK_USE_PLATFORM_WAYLAND_KHR

static void wayland_registry_global(void *data, struct wl_registry *registry, uint32_t id, const char *interface,
                                    uint32_t version) {
    struct AppInstance *inst = (struct AppInstance *)data;
    if (strcmp(interface, "wl_compositor") == 0) {
        struct wl_compositor *compositor = (struct wl_compositor *)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
        inst->wayland_surface = wl_compositor_create_surface(compositor);
    }
}
static void wayland_registry_global_remove(void *data, struct wl_registry *registry, uint32_t id) {}
static const struct wl_registry_listener wayland_registry_listener = {wayland_registry_global, wayland_registry_global_remove};

static void AppCreateWaylandWindow(struct AppInstance *inst) {
    inst->wayland_display = wl_display_connect(NULL);
    struct wl_registry *registry = wl_display_get_registry(inst->wayland_display);
    wl_registry_add_listener(wl_display_get_registry(inst->wayland_display), &wayland_registry_listener, inst);
    wl_display_roundtrip(inst->wayland_display);
    wl_registry_destroy(registry);
}

static VkSurfaceKHR AppCreateWaylandSurface(struct AppInstance *inst) {
    VkWaylandSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.display = inst->wayland_display;
    createInfo.surface = inst->wayland_surface;

    VkSurfaceKHR surface;
    VkResult err = vkCreateWaylandSurfaceKHR(inst->instance, &createInfo, NULL, &surface);
    if (err) ERR_EXIT(err);
    return surface;
}

static void AppDestroyWaylandWindow(struct AppInstance *inst) { wl_display_disconnect(inst->wayland_display); }
#endif  // VK_USE_PLATFORM_WAYLAND_KHR

#if defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WIN32_KHR) || \
    defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_WAYLAND_KHR)
static int AppDumpSurfaceFormats(struct AppInstance *inst, struct AppGpu *gpu, VkSurfaceKHR surface, FILE *out) {
    // Get the list of VkFormat's that are supported
    VkResult err;
    uint32_t format_count = 0;
    VkSurfaceFormatKHR *surf_formats = NULL;
    VkSurfaceFormat2KHR *surf_formats2 = NULL;

    const VkPhysicalDeviceSurfaceInfo2KHR surface_info2 = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR,
                                                           .surface = surface};

    if (CheckExtensionEnabled(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        err = inst->vkGetPhysicalDeviceSurfaceFormats2KHR(gpu->obj, &surface_info2, &format_count, NULL);
        if (err) ERR_EXIT(err);
        surf_formats2 = (VkSurfaceFormat2KHR *)malloc(format_count * sizeof(VkSurfaceFormat2KHR));
        if (!surf_formats2) ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
        for (uint32_t i = 0; i < format_count; ++i) {
            surf_formats2[i].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
            surf_formats2[i].pNext = NULL;
        }
        err = inst->vkGetPhysicalDeviceSurfaceFormats2KHR(gpu->obj, &surface_info2, &format_count, surf_formats2);
        if (err) ERR_EXIT(err);
    } else {
        err = inst->vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->obj, surface, &format_count, NULL);
        if (err) ERR_EXIT(err);
        surf_formats = (VkSurfaceFormatKHR *)malloc(format_count * sizeof(VkSurfaceFormatKHR));
        if (!surf_formats) ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
        err = inst->vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->obj, surface, &format_count, surf_formats);
        if (err) ERR_EXIT(err);
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>Formats: count = <span class='val'>%d</span></summary>", format_count);
        if (format_count > 0) {
            fprintf(out, "\n");
        } else {
            fprintf(out, "</details>\n");
        }
    } else if (human_readable_output) {
        printf("Formats:\t\tcount = %d\n", format_count);
    }
    for (uint32_t i = 0; i < format_count; ++i) {
        if (html_output) {
            fprintf(out, "\t\t\t\t\t\t<details><summary>SurfaceFormat[%d]:</summary>\n", i);
            if (CheckExtensionEnabled(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                                      gpu->inst->inst_extensions_count)) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary>format = <span class='type'>%s</span></summary></details>\n",
                        VkFormatString(surf_formats2[i].surfaceFormat.format));
                fprintf(out, "\t\t\t\t\t\t\t<details><summary>colorSpace = <span class='type'>%s</span></summary></details>\n",
                        VkColorSpaceString(surf_formats2[i].surfaceFormat.colorSpace));
            } else {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary>format = <span class='type'>%s</span></summary></details>\n",
                        VkFormatString(surf_formats[i].format));
                fprintf(out, "\t\t\t\t\t\t\t<details><summary>colorSpace = <span class='type'>%s</span></summary></details>\n",
                        VkColorSpaceString(surf_formats[i].colorSpace));
            }
            fprintf(out, "\t\t\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            printf("\tSurfaceFormats[%d]\n", i);
            if (CheckExtensionEnabled(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                                      gpu->inst->inst_extensions_count)) {
                printf("\t\tformat = %s\n", VkFormatString(surf_formats2[i].surfaceFormat.format));
                printf("\t\tcolorSpace = %s\n", VkColorSpaceString(surf_formats2[i].surfaceFormat.colorSpace));
            } else {
                printf("\t\tformat = %s\n", VkFormatString(surf_formats[i].format));
                printf("\t\tcolorSpace = %s\n", VkColorSpaceString(surf_formats[i].colorSpace));
            }
        }
    }
    if (format_count > 0 && html_output) {
        fprintf(out, "\t\t\t\t\t</details>\n");
    }

    fflush(out);
    fflush(stdout);
    if (CheckExtensionEnabled(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        free(surf_formats2);
    } else {
        free(surf_formats);
    }

    return format_count;
}

static int AppDumpSurfacePresentModes(struct AppInstance *inst, struct AppGpu *gpu, VkSurfaceKHR surface, FILE *out) {
    // Get the list of VkPresentMode's that are supported:
    VkResult err;
    uint32_t present_mode_count = 0;
    err = inst->vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->obj, surface, &present_mode_count, NULL);
    if (err) ERR_EXIT(err);

    VkPresentModeKHR *surf_present_modes = (VkPresentModeKHR *)malloc(present_mode_count * sizeof(VkPresentModeKHR));
    if (!surf_present_modes) ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    err = inst->vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->obj, surface, &present_mode_count, surf_present_modes);
    if (err) ERR_EXIT(err);

    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>Present Modes: count = <span class='val'>%d</span></summary>",
                present_mode_count);
        if (present_mode_count > 0) {
            fprintf(out, "\n");
        } else {
            fprintf(out, "</details>");
        }
    } else if (human_readable_output) {
        printf("Present Modes:\t\tcount = %d\n", present_mode_count);
    }
    for (uint32_t i = 0; i < present_mode_count; ++i) {
        if (html_output) {
            fprintf(out, "\t\t\t\t\t\t<details><summary><span class='type'>%s</span></summary></details>\n",
                    VkPresentModeString(surf_present_modes[i]));
        } else if (human_readable_output) {
            printf("\t%s\n", VkPresentModeString(surf_present_modes[i]));
        }
    }
    if (present_mode_count > 0 && html_output) {
        fprintf(out, "\t\t\t\t\t</details>\n");
    }

    fflush(out);
    fflush(stdout);
    free(surf_present_modes);

    return present_mode_count;
}

static void AppDumpSurfaceCapabilities(struct AppInstance *inst, struct AppGpu *gpu, VkSurfaceKHR surface, FILE *out) {
    if (CheckExtensionEnabled(VK_KHR_SURFACE_EXTENSION_NAME, gpu->inst->inst_extensions, gpu->inst->inst_extensions_count)) {
        VkSurfaceCapabilitiesKHR surface_capabilities;
        VkResult err;
        err = inst->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu->obj, surface, &surface_capabilities);
        if (err) ERR_EXIT(err);

        if (html_output) {
            fprintf(out, "\t\t\t\t\t<details><summary>VkSurfaceCapabilitiesKHR</summary>\n");
            fprintf(out, "\t\t\t\t\t\t<details><summary>minImageCount = <span class='val'>%u</span></summary></details>\n",
                    surface_capabilities.minImageCount);
            fprintf(out, "\t\t\t\t\t\t<details><summary>maxImageCount = <span class='val'>%u</span></summary></details>\n",
                    surface_capabilities.maxImageCount);
            fprintf(out, "\t\t\t\t\t\t<details><summary>currentExtent</summary>\n");
            fprintf(out, "\t\t\t\t\t\t\t<details><summary>width = <span class='val'>%u</span></summary></details>\n",
                    surface_capabilities.currentExtent.width);
            fprintf(out, "\t\t\t\t\t\t\t<details><summary>height = <span class='val'>%u</span></summary></details>\n",
                    surface_capabilities.currentExtent.height);
            fprintf(out, "\t\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t\t<details><summary>minImageExtent</summary>\n");
            fprintf(out, "\t\t\t\t\t\t\t<details><summary>width = <span class='val'>%u</span></summary></details>\n",
                    surface_capabilities.minImageExtent.width);
            fprintf(out, "\t\t\t\t\t\t\t<details><summary>height = <span class='val'>%u</span></summary></details>\n",
                    surface_capabilities.minImageExtent.height);
            fprintf(out, "\t\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t\t<details><summary>maxImageExtent</summary>\n");
            fprintf(out, "\t\t\t\t\t\t\t<details><summary>width = <span class='val'>%u</span></summary></details>\n",
                    surface_capabilities.maxImageExtent.width);
            fprintf(out, "\t\t\t\t\t\t\t<details><summary>height = <span class='val'>%u</span></summary></details>\n",
                    surface_capabilities.maxImageExtent.height);
            fprintf(out, "\t\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t\t<details><summary>maxImageArrayLayers = <span class='val'>%u</span></summary></details>\n",
                    surface_capabilities.maxImageArrayLayers);
            fprintf(out, "\t\t\t\t\t\t<details><summary>supportedTransform</summary>\n");
            if (surface_capabilities.supportedTransforms == 0) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary>None</summary></details>\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR</span></summary></details>\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR</span></summary></details>\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR</span></summary></details>\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR</span></summary></details>\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR</span></summary></details>\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR</span></summary></details>\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR</span></summary></details>\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR</span></summary></details>\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR</span></summary></details>\n");
            }
            fprintf(out, "\t\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t\t<details><summary>currentTransform</summary>\n");
            if (surface_capabilities.currentTransform == 0) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary>None</summary></details>\n");
            }
            if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR</span></summary></details>\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR</span></summary></details>\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR</span></summary></details>\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR</span></summary></details>\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR</span></summary></details>\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR</span></summary></details>\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR</span></summary></details>\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR</span></summary></details>\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR</span></summary></details>\n");
            }
            fprintf(out, "\t\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t\t<details><summary>supportedCompositeAlpha</summary>\n");
            if (surface_capabilities.supportedCompositeAlpha == 0) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary>None</summary></details>\n");
            }
            if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR</span></summary></details>\n");
            }
            if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR</span></summary></details>\n");
            }
            if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR</span></summary></details>\n");
            }
            if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR</span></summary></details>\n");
            }
            fprintf(out, "\t\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t\t<details><summary>supportedUsageFlags</summary>\n");
            if (surface_capabilities.supportedUsageFlags == 0) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary>None</summary></details>\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_IMAGE_USAGE_TRANSFER_SRC_BIT</span></summary></details>\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_IMAGE_USAGE_TRANSFER_DST_BIT</span></summary></details>\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT) {
                fprintf(
                    out,
                    "\t\t\t\t\t\t\t<details><summary><span class='type'>VK_IMAGE_USAGE_SAMPLED_BIT</span></summary></details>\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT) {
                fprintf(
                    out,
                    "\t\t\t\t\t\t\t<details><summary><span class='type'>VK_IMAGE_USAGE_STORAGE_BIT</span></summary></details>\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT</span></summary></details>\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT</span></summary></details>\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT</span></summary></details>\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) {
                fprintf(out,
                        "\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT</span></summary></details>\n");
            }
            fprintf(out, "\t\t\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            printf("VkSurfaceCapabilitiesKHR:\n");
            printf("\tminImageCount       = %u\n", surface_capabilities.minImageCount);
            printf("\tmaxImageCount       = %u\n", surface_capabilities.maxImageCount);
            printf("\tcurrentExtent:\n");
            printf("\t\twidth       = %u\n", surface_capabilities.currentExtent.width);
            printf("\t\theight      = %u\n", surface_capabilities.currentExtent.height);
            printf("\tminImageExtent:\n");
            printf("\t\twidth       = %u\n", surface_capabilities.minImageExtent.width);
            printf("\t\theight      = %u\n", surface_capabilities.minImageExtent.height);
            printf("\tmaxImageExtent:\n");
            printf("\t\twidth       = %u\n", surface_capabilities.maxImageExtent.width);
            printf("\t\theight      = %u\n", surface_capabilities.maxImageExtent.height);
            printf("\tmaxImageArrayLayers = %u\n", surface_capabilities.maxImageArrayLayers);
            printf("\tsupportedTransform:\n");
            if (surface_capabilities.supportedTransforms == 0) {
                printf("\t\tNone\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR\n");
            }
            if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_INHERIT_BIT_KHR\n");
            }
            printf("\tcurrentTransform:\n");
            if (surface_capabilities.currentTransform == 0) {
                printf("\t\tNone\n");
            }
            if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR\n");
            } else if (surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) {
                printf("\t\tVK_SURFACE_TRANSFORM_INHERIT_BIT_KHR\n");
            }
            printf("\tsupportedCompositeAlpha:\n");
            if (surface_capabilities.supportedCompositeAlpha == 0) {
                printf("\t\tNone\n");
            }
            if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
                printf("\t\tVK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR\n");
            }
            if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
                printf("\t\tVK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR\n");
            }
            if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
                printf("\t\tVK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR\n");
            }
            if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
                printf("\t\tVK_COMPOSITE_ALPHA_INHERIT_BIT_KHR\n");
            }
            printf("\tsupportedUsageFlags:\n");
            if (surface_capabilities.supportedUsageFlags == 0) {
                printf("\t\tNone\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
                printf("\t\tVK_IMAGE_USAGE_TRANSFER_SRC_BIT\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
                printf("\t\tVK_IMAGE_USAGE_TRANSFER_DST_BIT\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT) {
                printf("\t\tVK_IMAGE_USAGE_SAMPLED_BIT\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT) {
                printf("\t\tVK_IMAGE_USAGE_STORAGE_BIT\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
                printf("\t\tVK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                printf("\t\tVK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
                printf("\t\tVK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT\n");
            }
            if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) {
                printf("\t\tVK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT\n");
            }
        }

        // Get additional surface capability information from vkGetPhysicalDeviceSurfaceCapabilities2EXT
        if (CheckExtensionEnabled(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME, gpu->inst->inst_extensions,
                                  gpu->inst->inst_extensions_count)) {
            VkSurfaceCapabilities2EXT surface_capabilities2_ext = {VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_EXT};

            err = inst->vkGetPhysicalDeviceSurfaceCapabilities2EXT(gpu->obj, surface, &surface_capabilities2_ext);
            if (err) ERR_EXIT(err);

            if (html_output) {
                fprintf(out, "\t\t\t\t\t\t<details><summary>VkSurfaceCapabilities2EXT</summary>\n");
                fprintf(out, "\t\t\t\t\t\t\t<details><summary>supportedSurfaceCounters</summary>\n");
                if (surface_capabilities2_ext.supportedSurfaceCounters == 0) {
                    fprintf(out, "\t\t\t\t\t\t\t\t<details><summary>None</summary></details>\n");
                }
                if (surface_capabilities2_ext.supportedSurfaceCounters & VK_SURFACE_COUNTER_VBLANK_EXT) {
                    fprintf(out,
                            "\t\t\t\t\t\t\t\t<details><summary><span "
                            "class='type'>VK_SURFACE_COUNTER_VBLANK_EXT</span></summary></details>\n");
                }
                fprintf(out, "\t\t\t\t\t\t\t</details>\n");
                fprintf(out, "\t\t\t\t\t\t</details>\n");
            } else if (human_readable_output) {
                printf("VkSurfaceCapabilities2EXT:\n");
                printf("\tsupportedSurfaceCounters:\n");
                if (surface_capabilities2_ext.supportedSurfaceCounters == 0) {
                    printf("\t\tNone\n");
                }
                if (surface_capabilities2_ext.supportedSurfaceCounters & VK_SURFACE_COUNTER_VBLANK_EXT) {
                    printf("\t\tVK_SURFACE_COUNTER_VBLANK_EXT\n");
                }
            }
        }

        // Get additional surface capability information from vkGetPhysicalDeviceSurfaceCapabilities2KHR
        if (CheckExtensionEnabled(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                                  gpu->inst->inst_extensions_count)) {
            VkSurfaceCapabilities2KHR surface_capabilities2;

            struct pNextChainBuildingBlockInfo sur_cap2_chain_info[] = {
                {.sType = VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR,
                 .mem_size = sizeof(VkSharedPresentSurfaceCapabilitiesKHR)},
                {.sType = VK_STRUCTURE_TYPE_SURFACE_PROTECTED_CAPABILITIES_KHR,
                 .mem_size = sizeof(VkSurfaceProtectedCapabilitiesKHR)}
#ifdef VK_USE_PLATFORM_WIN32_KHR
                ,
                {.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_FULL_SCREEN_EXCLUSIVE_EXT,
                 .mem_size = sizeof(VkSurfaceCapabilitiesFullScreenExclusiveEXT)}
#endif
            };

            uint32_t sur_cap2_chain_info_len = ARRAY_SIZE(sur_cap2_chain_info);

            surface_capabilities2.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;
            buildpNextChain((struct VkStructureHeader *)&surface_capabilities2, sur_cap2_chain_info, sur_cap2_chain_info_len);

            VkPhysicalDeviceSurfaceInfo2KHR surface_info;
            surface_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
            surface_info.pNext = NULL;
            surface_info.surface = surface;

            err = inst->vkGetPhysicalDeviceSurfaceCapabilities2KHR(gpu->obj, &surface_info, &surface_capabilities2);
            if (err) ERR_EXIT(err);

            void *place = surface_capabilities2.pNext;
            while (place) {
                struct VkStructureHeader *work = (struct VkStructureHeader *)place;
                if (work->sType == VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR &&
                    CheckExtensionEnabled(VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME, gpu->inst->inst_extensions,
                                          gpu->inst->inst_extensions_count)) {
                    VkSharedPresentSurfaceCapabilitiesKHR *shared_surface_capabilities =
                        (VkSharedPresentSurfaceCapabilitiesKHR *)place;
                    if (html_output) {
                        fprintf(out, "\t\t\t\t\t\t<details><summary>VkSharedPresentSurfaceCapabilitiesKHR</summary>\n");
                        fprintf(out, "\t\t\t\t\t\t\t<details><summary>sharedPresentSupportedUsageFlags</summary>\n");
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags == 0) {
                            fprintf(out, "\t\t\t\t\t\t\t\t<details><summary>None</summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
                            fprintf(out,
                                    "\t\t\t\t\t\t\t\t<details><summary><span "
                                    "class='type'>VK_IMAGE_USAGE_TRANSFER_SRC_BIT</span></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
                            fprintf(out,
                                    "\t\t\t\t\t\t\t\t<details><summary><span "
                                    "class='type'>VK_IMAGE_USAGE_TRANSFER_DST_BIT</span></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT) {
                            fprintf(out,
                                    "\t\t\t\t\t\t\t\t<details><summary><span "
                                    "class='type'>VK_IMAGE_USAGE_SAMPLED_BIT</span></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT) {
                            fprintf(out,
                                    "\t\t\t\t\t\t\t\t<details><summary><span "
                                    "class='type'>VK_IMAGE_USAGE_STORAGE_BIT</span></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
                            fprintf(out,
                                    "\t\t\t\t\t\t\t\t<details><summary><span "
                                    "class='type'>VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT</span></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags &
                            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                            fprintf(out,
                                    "\t\t\t\t\t\t\t\t<details><summary><span "
                                    "class='type'>VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT</span></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags &
                            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
                            fprintf(out,
                                    "\t\t\t\t\t\t\t\t<details><summary><span "
                                    "class='type'>VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT</span></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) {
                            fprintf(out,
                                    "\t\t\t\t\t\t\t\t<details><summary><span "
                                    "class='type'>VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT</span></summary></details>\n");
                        }
                        fprintf(out, "\t\t\t\t\t\t\t</details>\n");
                        fprintf(out, "\t\t\t\t\t\t</details>\n");
                    } else if (human_readable_output) {
                        printf("VkSharedPresentSurfaceCapabilitiesKHR:\n");
                        printf("\tsharedPresentSupportedUsageFlags:\n");
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags == 0) {
                            printf("\t\tNone\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
                            printf("\t\tVK_IMAGE_USAGE_TRANSFER_SRC_BIT\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
                            printf("\t\tVK_IMAGE_USAGE_TRANSFER_DST_BIT\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT) {
                            printf("\t\tVK_IMAGE_USAGE_SAMPLED_BIT\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT) {
                            printf("\t\tVK_IMAGE_USAGE_STORAGE_BIT\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
                            printf("\t\tVK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags &
                            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                            printf("\t\tVK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags &
                            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
                            printf("\t\tVK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) {
                            printf("\t\tVK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT\n");
                        }
                    }
                } else if (work->sType == VK_STRUCTURE_TYPE_SURFACE_PROTECTED_CAPABILITIES_KHR &&
                           CheckExtensionEnabled(VK_KHR_SURFACE_PROTECTED_CAPABILITIES_EXTENSION_NAME, gpu->inst->inst_extensions,
                                                 gpu->inst->inst_extensions_count)) {
                    VkSurfaceProtectedCapabilitiesKHR *protected_surface_capabilities = (VkSurfaceProtectedCapabilitiesKHR *)place;
                    if (html_output) {
                        fprintf(out, "\t\t\t\t\t\t<details><summary>VkSurfaceProtectedCapabilities</summary>\n");
                        fprintf(out,
                                "\t\t\t\t\t\t\t<details><summary>supportsProtected = <span class='val'>%" PRIuLEAST32
                                "</span></summary></details>\n",
                                protected_surface_capabilities->supportsProtected);
                        fprintf(out, "\t\t\t\t\t\t</details>\n");
                    } else if (human_readable_output) {
                        printf("VkSurfaceProtectedCapabilities\n");
                        printf("\tsupportsProtected = %" PRIuLEAST32 "\n", protected_surface_capabilities->supportsProtected);
                    }
                }
#ifdef VK_USE_PLATFORM_WIN32_KHR
                else if (work->sType == VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_FULL_SCREEN_EXCLUSIVE_EXT &&
                         CheckPhysicalDeviceExtensionIncluded(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME, gpu->device_extensions,
                                                              gpu->device_extension_count)) {
                    VkSurfaceCapabilitiesFullScreenExclusiveEXT *full_screen_exclusive =
                        (VkSurfaceCapabilitiesFullScreenExclusiveEXT *)place;
                    if (html_output) {
                        fprintf(out, "\t\t\t\t\t\t<details><summary>VkSurfaceCapabilitiesFullScreenExclusiveEXT</summary>\n");
                        fprintf(out,
                                "\t\t\t\t\t\t\t<details><summary>fullScreenExclusive = <span class='val'>%" PRIuLEAST32
                                "</span></summary></details>\n",
                                full_screen_exclusive->fullScreenExclusiveSupported);
                        fprintf(out, "\t\t\t\t\t\t</details>\n");
                    } else if (human_readable_output) {
                        printf("VkSurfaceCapabilitiesFullScreenExclusiveEXT\n");
                        printf("\tfullScreenExclusive = %" PRIuLEAST32 "\n", full_screen_exclusive->fullScreenExclusiveSupported);
                    }
                }
#endif
                place = work->pNext;
            }
            freepNextChain(surface_capabilities2.pNext);
        }
        if (html_output) {
            fprintf(out, "\t\t\t\t\t</details>\n");
        }
    }
}

static void AppDumpSurfaceExtension(struct AppInstance *inst, struct AppGpu *gpus, uint32_t gpu_count,
                                    struct SurfaceExtensionNode *surface_extension, int *format_count, int *present_mode_count,
                                    FILE *out) {
    if (!CheckExtensionEnabled(surface_extension->name, inst->inst_extensions, inst->inst_extensions_count)) {
        return;
    }

    for (uint32_t i = 0; i < gpu_count; ++i) {
        if (html_output) {
            fprintf(out, "\t\t\t\t<details><summary>GPU id : <span class='val'>%u</span> (%s)</summary>\n", i,
                    gpus[i].props.deviceName);
            fprintf(out, "\t\t\t\t\t<details><summary>Surface type : <span class='type'>%s</span></summary></details>\n",
                    surface_extension->name);
        } else if (human_readable_output) {
            printf("GPU id       : %u (%s)\n", i, gpus[i].props.deviceName);
            printf("Surface type : %s\n", surface_extension->name);
        }
        *format_count += AppDumpSurfaceFormats(inst, &gpus[i], surface_extension->surface, out);
        *present_mode_count += AppDumpSurfacePresentModes(inst, &gpus[i], surface_extension->surface, out);
        AppDumpSurfaceCapabilities(inst, &gpus[i], surface_extension->surface, out);

        if (html_output) {
            fprintf(out, "\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            printf("\n");
        }
    }
}

#endif

static void AppDevDumpFormatProps(const struct AppGpu *gpu, VkFormat fmt, bool *first_in_list, FILE *out) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(gpu->obj, fmt, &props);
    struct {
        const char *name;
        VkFlags flags;
    } features[3];

    features[0].name = "linearTiling   FormatFeatureFlags";
    features[0].flags = props.linearTilingFeatures;
    features[1].name = "optimalTiling  FormatFeatureFlags";
    features[1].flags = props.optimalTilingFeatures;
    features[2].name = "bufferFeatures FormatFeatureFlags";
    features[2].flags = props.bufferFeatures;

    if (html_output) {
        fprintf(out, "\t\t\t\t\t\t<details><summary><span class='type'>FORMAT_%s</span></summary>\n", VkFormatString(fmt));
    } else if (human_readable_output) {
        printf("\nFORMAT_%s:", VkFormatString(fmt));
    }
    for (uint32_t i = 0; i < ARRAY_SIZE(features); ++i) {
        if (html_output) {
            fprintf(out, "\t\t\t\t\t\t\t<details open><summary>%s</summary>\n", features[i].name);
            if (features[i].flags == 0) {
                fprintf(out, "\t\t\t\t\t\t\t\t<details><summary>None</summary></details>\n");
            } else {
                fprintf(
                    out, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                    ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) ? "\t\t\t\t\t\t\t\t<details><summary><span "
                                                                                 "class='type'>VK_FORMAT_FEATURE_SAMPLED_IMAGE_"
                                                                                 "BIT</span></summary></details>\n"
                                                                               : ""),  // 0x0001
                    ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) ? "\t\t\t\t\t\t\t\t<details><summary><span "
                                                                                 "class='type'>VK_FORMAT_FEATURE_STORAGE_IMAGE_"
                                                                                 "BIT</span></summary></details>\n"
                                                                               : ""),  // 0x0002
                    ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT)
                         ? "\t\t\t\t\t\t\t\t<details><summary><span "
                           "class='type'>VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT</span></summary></details>\n"
                         : ""),  // 0x0004
                    ((features[i].flags & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT)
                         ? "\t\t\t\t\t\t\t\t<details><summary><span "
                           "class='type'>VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT</span></summary></details>\n"
                         : ""),  // 0x0008
                    ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT)
                         ? "\t\t\t\t\t\t\t\t<details><summary><span "
                           "class='type'>VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT</span></summary></details>\n"
                         : ""),  // 0x0010
                    ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT)
                         ? "\t\t\t\t\t\t\t\t<details><summary><span "
                           "class='type'>VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT</span></summary></details>\n"
                         : ""),  // 0x0020
                    ((features[i].flags & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) ? "\t\t\t\t\t\t\t\t<details><summary><span "
                                                                                 "class='type'>VK_FORMAT_FEATURE_VERTEX_BUFFER_"
                                                                                 "BIT</span></summary></details>\n"
                                                                               : ""),  // 0x0040
                    ((features[i].flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) ? "\t\t\t\t\t\t\t\t<details><summary><span "
                                                                                    "class='type'>VK_FORMAT_FEATURE_COLOR_"
                                                                                    "ATTACHMENT_BIT</span></summary></details>\n"
                                                                                  : ""),  // 0x0080
                    ((features[i].flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT)
                         ? "\t\t\t\t\t\t\t\t<details><summary><span "
                           "class='type'>VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT</span></summary></details>\n"
                         : ""),  // 0x0100
                    ((features[i].flags & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                         ? "\t\t\t\t\t\t\t\t<details><summary><span "
                           "class='type'>VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT</span></summary></details>\n"
                         : ""),  // 0x0200
                    ((features[i].flags & VK_FORMAT_FEATURE_BLIT_SRC_BIT) ? "\t\t\t\t\t\t\t\t<details><summary><span "
                                                                            "class='type'>VK_FORMAT_FEATURE_BLIT_SRC_BIT</"
                                                                            "span></summary></details>\n"
                                                                          : ""),  // 0x0400
                    ((features[i].flags & VK_FORMAT_FEATURE_BLIT_DST_BIT) ? "\t\t\t\t\t\t\t\t<details><summary><span "
                                                                            "class='type'>VK_FORMAT_FEATURE_BLIT_DST_BIT</"
                                                                            "span></summary></details>\n"
                                                                          : ""),  // 0x0800
                    ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)
                         ? "\t\t\t\t\t\t\t\t<details><summary><span "
                           "class='type'>VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT</span></summary></details>\n"
                         : ""),  // 0x1000
                    ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG)
                         ? "\t\t\t\t\t\t\t\t<details><summary><span "
                           "class='type'>VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG</span></summary></details>\n"
                         : ""),  // 0x2000
                    ((features[i].flags & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR) ? "\t\t\t\t\t\t\t\t<details><summary><span "
                                                                                    "class='type'>VK_FORMAT_FEATURE_TRANSFER_"
                                                                                    "SRC_BIT_KHR</span></summary></details>\n"
                                                                                  : ""),  // 0x4000
                    ((features[i].flags & VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR) ? "\t\t\t\t\t\t\t\t<details><summary><span "
                                                                                    "class='type'>VK_FORMAT_FEATURE_TRANSFER_"
                                                                                    "DST_BIT_KHR</span></summary></details>\n"
                                                                                  : ""));  // 0x8000
            }
            fprintf(out, "\t\t\t\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            printf("\n\t%s:", features[i].name);
            if (features[i].flags == 0) {
                printf("\n\t\tNone");
            } else {
                printf(
                    "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                    ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) ? "\n\t\tVK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"
                                                                               : ""),  // 0x0001
                    ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) ? "\n\t\tVK_FORMAT_FEATURE_STORAGE_IMAGE_BIT"
                                                                               : ""),  // 0x0002
                    ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT"
                         : ""),  // 0x0004
                    ((features[i].flags & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT"
                         : ""),  // 0x0008
                    ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT"
                         : ""),  // 0x0010
                    ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT"
                         : ""),  // 0x0020
                    ((features[i].flags & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) ? "\n\t\tVK_FORMAT_FEATURE_VERTEX_BUFFER_BIT"
                                                                               : ""),  // 0x0040
                    ((features[i].flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) ? "\n\t\tVK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT"
                                                                                  : ""),  // 0x0080
                    ((features[i].flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT"
                         : ""),  // 0x0100
                    ((features[i].flags & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT"
                         : ""),                                                                                            // 0x0200
                    ((features[i].flags & VK_FORMAT_FEATURE_BLIT_SRC_BIT) ? "\n\t\tVK_FORMAT_FEATURE_BLIT_SRC_BIT" : ""),  // 0x0400
                    ((features[i].flags & VK_FORMAT_FEATURE_BLIT_DST_BIT) ? "\n\t\tVK_FORMAT_FEATURE_BLIT_DST_BIT" : ""),  // 0x0800
                    ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT"
                         : ""),  // 0x1000
                    ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG)
                         ? "\n\t\tVK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG"
                         : ""),  // 0x2000
                    ((features[i].flags & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR) ? "\n\t\tVK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR"
                                                                                  : ""),  // 0x4000
                    ((features[i].flags & VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR) ? "\n\t\tVK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR"
                                                                                  : ""));  // 0x8000
            }
        }
    }
    if (html_output) {
        fprintf(out, "\t\t\t\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("\n");
    }
    if (json_output && (props.linearTilingFeatures || props.optimalTilingFeatures || props.bufferFeatures)) {
        if (!(*first_in_list)) {
            printf(",");
        } else {
            *first_in_list = false;
        }
        printf("\n");
        printf("\t\t{\n");
        printf("\t\t\t\"formatID\": %d,\n", fmt);
        printf("\t\t\t\"linearTilingFeatures\": %u,\n", props.linearTilingFeatures);
        printf("\t\t\t\"optimalTilingFeatures\": %u,\n", props.optimalTilingFeatures);
        printf("\t\t\t\"bufferFeatures\": %u\n", props.bufferFeatures);
        printf("\t\t}");
    }
}

/* This structure encodes all the format ranges to be queried.
 * It ensures that a format is not queried if the instance
 * doesn't support it (either through the instance version or
 * through extensions).
 */
static struct FormatRange {
    // the Vulkan standard version that supports this format range, or 0 if non-standard
    uint32_t minimum_instance_version;

    // The name of the extension that supports this format range, or NULL if the range
    // is only part of the standard
    char *extension_name;

    // The first and last supported formats within this range.
    VkFormat first_format;
    VkFormat last_format;
} supported_format_ranges[] = {
    {
        // Standard formats in Vulkan 1.0
        VK_MAKE_VERSION(1, 0, 0),
        NULL,
        VK_FORMAT_BEGIN_RANGE,
        VK_FORMAT_END_RANGE,
    },
    {
        // YCBCR extension, standard in Vulkan 1.1
        VK_MAKE_VERSION(1, 1, 0),
        VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
        VK_FORMAT_G8B8G8R8_422_UNORM,
        VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
    },
    {
        // PVRTC extension, not standardized
        0,
        VK_IMG_FORMAT_PVRTC_EXTENSION_NAME,
        VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG,
        VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG,
    },
};

// Helper function to determine whether a format range is currently supported.
bool FormatRangeSupported(const struct FormatRange *format_range, const struct AppGpu *gpu) {
    // True if standard and supported by both this instance and this GPU
    if (format_range->minimum_instance_version > 0 && gpu->inst->instance_version >= format_range->minimum_instance_version &&
        gpu->props.apiVersion >= format_range->minimum_instance_version) {
        return true;
    }

    // True if this extension is present
    if (format_range->extension_name != NULL) {
        return CheckExtensionEnabled(format_range->extension_name, gpu->inst->inst_extensions, gpu->inst->inst_extensions_count);
    }

    // Otherwise, not supported.
    return false;
}

bool FormatPropsEq(const VkFormatProperties *props1, const VkFormatProperties *props2) {
    if (props1->bufferFeatures == props2->bufferFeatures && props1->linearTilingFeatures == props2->linearTilingFeatures &&
        props1->optimalTilingFeatures == props2->optimalTilingFeatures) {
        return true;
    } else {
        return false;
    }
}

struct PropFormats {
    VkFormatProperties props;

    uint32_t format_count;
    uint32_t format_reserve;
    VkFormat *formats;
};

void FormatPropsShortenedDump(const struct AppGpu *gpu) {
    const VkFormatProperties unsupported_prop = {0};
    uint32_t unique_props_count = 1;
    uint32_t unique_props_reserve = 50;
    struct PropFormats *prop_map = malloc(sizeof(struct PropFormats) * unique_props_reserve);
    if (!prop_map) ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    prop_map[0].props = unsupported_prop;
    prop_map[0].format_count = 0;
    prop_map[0].format_reserve = 20;
    prop_map[0].formats = malloc(sizeof(VkFormat) * prop_map[0].format_reserve);
    if (!prop_map[0].formats) ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);

    for (uint32_t ri = 0; ri < ARRAY_SIZE(supported_format_ranges); ++ri) {
        struct FormatRange format_range = supported_format_ranges[ri];
        if (FormatRangeSupported(&format_range, gpu)) {
            for (VkFormat fmt = format_range.first_format; fmt <= format_range.last_format; ++fmt) {
                VkFormatProperties props;
                vkGetPhysicalDeviceFormatProperties(gpu->obj, fmt, &props);

                uint32_t formats_prop_i = 0;
                for (; formats_prop_i < unique_props_count; ++formats_prop_i) {
                    if (FormatPropsEq(&prop_map[formats_prop_i].props, &props)) break;
                }

                if (formats_prop_i < unique_props_count) {
                    struct PropFormats *propFormats = &prop_map[formats_prop_i];
                    ++propFormats->format_count;

                    if (propFormats->format_count > propFormats->format_reserve) {
                        propFormats->format_reserve *= 2;
                        propFormats->formats = realloc(propFormats->formats, sizeof(VkFormat) * propFormats->format_reserve);
                        if (!propFormats->formats) ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
                    }

                    propFormats->formats[propFormats->format_count - 1] = fmt;
                } else {
                    assert(formats_prop_i == unique_props_count);
                    ++unique_props_count;

                    if (unique_props_count > unique_props_reserve) {
                        unique_props_reserve *= 2;
                        prop_map = realloc(prop_map, sizeof(struct PropFormats) * unique_props_reserve);
                        if (!prop_map) ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
                    }

                    struct PropFormats *propFormats = &prop_map[formats_prop_i];
                    propFormats->props = props;
                    propFormats->format_count = 1;
                    propFormats->format_reserve = 20;
                    propFormats->formats = malloc(sizeof(VkFormat) * propFormats->format_reserve);
                    if (!propFormats->formats) ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
                    propFormats->formats[0] = fmt;
                }
            }
        }
    }

    for (uint32_t pi = 1; pi < unique_props_count; ++pi) {
        struct PropFormats *propFormats = &prop_map[pi];

        for (uint32_t fi = 0; fi < propFormats->format_count; ++fi) {
            const VkFormat fmt = propFormats->formats[fi];

            printf("\nFORMAT_%s", VkFormatString(fmt));

            if (fi < propFormats->format_count - 1)
                printf(",");
            else
                printf(":");
        }

        struct {
            const char *name;
            VkFlags flags;
        } features[3];

        features[0].name = "linearTiling   FormatFeatureFlags";
        features[0].flags = propFormats->props.linearTilingFeatures;
        features[1].name = "optimalTiling  FormatFeatureFlags";
        features[1].flags = propFormats->props.optimalTilingFeatures;
        features[2].name = "bufferFeatures FormatFeatureFlags";
        features[2].flags = propFormats->props.bufferFeatures;

        for (uint32_t i = 0; i < ARRAY_SIZE(features); ++i) {
            printf("\n\t%s:", features[i].name);
            if (features[i].flags == 0) {
                printf("\n\t\tNone");
            } else {
                printf(
                    "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                    ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) ? "\n\t\tVK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"
                                                                               : ""),  // 0x0001
                    ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) ? "\n\t\tVK_FORMAT_FEATURE_STORAGE_IMAGE_BIT"
                                                                               : ""),  // 0x0002
                    ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT"
                         : ""),  // 0x0004
                    ((features[i].flags & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT"
                         : ""),  // 0x0008
                    ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT"
                         : ""),  // 0x0010
                    ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT"
                         : ""),  // 0x0020
                    ((features[i].flags & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) ? "\n\t\tVK_FORMAT_FEATURE_VERTEX_BUFFER_BIT"
                                                                               : ""),  // 0x0040
                    ((features[i].flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) ? "\n\t\tVK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT"
                                                                                  : ""),  // 0x0080
                    ((features[i].flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT"
                         : ""),  // 0x0100
                    ((features[i].flags & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT"
                         : ""),                                                                                            // 0x0200
                    ((features[i].flags & VK_FORMAT_FEATURE_BLIT_SRC_BIT) ? "\n\t\tVK_FORMAT_FEATURE_BLIT_SRC_BIT" : ""),  // 0x0400
                    ((features[i].flags & VK_FORMAT_FEATURE_BLIT_DST_BIT) ? "\n\t\tVK_FORMAT_FEATURE_BLIT_DST_BIT" : ""),  // 0x0800
                    ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)
                         ? "\n\t\tVK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT"
                         : ""),  // 0x1000
                    ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG)
                         ? "\n\t\tVK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG"
                         : ""),  // 0x2000
                    ((features[i].flags & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR) ? "\n\t\tVK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR"
                                                                                  : ""),  // 0x4000
                    ((features[i].flags & VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR) ? "\n\t\tVK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR"
                                                                                  : ""));  // 0x8000
            }

            printf("\n");
        }
    }

    printf("\nUnsupported formats:");
    if (prop_map[0].format_count == 0) printf("\nNone");
    for (uint32_t fi = 0; fi < prop_map[0].format_count; ++fi) {
        const VkFormat fmt = prop_map[0].formats[fi];

        printf("\nFORMAT_%s", VkFormatString(fmt));
    }

    // cleanup
    for (uint32_t pi = 0; pi < unique_props_count; ++pi) free(prop_map[pi].formats);
    free(prop_map);
}

static void AppDevDump(const struct AppGpu *gpu, FILE *out) {
    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>Format Properties</summary>\n");
    } else if (human_readable_output) {
        printf("Format Properties:\n");
        printf("==================\n");
    }
    if (json_output) {
        printf(",\n");
        printf("\t\"ArrayOfVkFormatProperties\": [");
    }

    if (human_readable_output) {
        FormatPropsShortenedDump(gpu);
    } else {
        bool first_in_list = true;  // Used for commas in json output
        for (uint32_t i = 0; i < ARRAY_SIZE(supported_format_ranges); ++i) {
            struct FormatRange format_range = supported_format_ranges[i];
            if (FormatRangeSupported(&format_range, gpu)) {
                for (VkFormat fmt = format_range.first_format; fmt <= format_range.last_format; ++fmt) {
                    AppDevDumpFormatProps(gpu, fmt, &first_in_list, out);
                }
            }
        }
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t</details>\n");
    }
    if (json_output) {
        printf("\n\t]");
    }
}

#ifdef _WIN32
#define PRINTF_SIZE_T_SPECIFIER "%Iu"
#else
#define PRINTF_SIZE_T_SPECIFIER "%zu"
#endif

static void AppGpuDumpFeatures(const struct AppGpu *gpu, FILE *out) {
    VkPhysicalDeviceFeatures features;

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        const VkPhysicalDeviceFeatures *features2_const = &gpu->features2.features;
        features = *features2_const;
    } else {
        const VkPhysicalDeviceFeatures *features_const = &gpu->features;
        features = *features_const;
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceFeatures</summary>\n");
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>robustBufferAccess                      = <span "
                "class='val'>%u</span></summary></details>\n",
                features.robustBufferAccess);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>fullDrawIndexUint32                     = <span "
                "class='val'>%u</span></summary></details>\n",
                features.fullDrawIndexUint32);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>imageCubeArray                          = <span "
                "class='val'>%u</span></summary></details>\n",
                features.imageCubeArray);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>independentBlend                        = <span "
                "class='val'>%u</span></summary></details>\n",
                features.independentBlend);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>geometryShader                          = <span "
                "class='val'>%u</span></summary></details>\n",
                features.geometryShader);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>tessellationShader                      = <span "
                "class='val'>%u</span></summary></details>\n",
                features.tessellationShader);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sampleRateShading                       = <span "
                "class='val'>%u</span></summary></details>\n",
                features.sampleRateShading);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>dualSrcBlend                            = <span "
                "class='val'>%u</span></summary></details>\n",
                features.dualSrcBlend);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>logicOp                                 = <span "
                "class='val'>%u</span></summary></details>\n",
                features.logicOp);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>multiDrawIndirect                       = <span "
                "class='val'>%u</span></summary></details>\n",
                features.multiDrawIndirect);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>drawIndirectFirstInstance               = <span "
                "class='val'>%u</span></summary></details>\n",
                features.drawIndirectFirstInstance);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>depthClamp                              = <span "
                "class='val'>%u</span></summary></details>\n",
                features.depthClamp);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>depthBiasClamp                          = <span "
                "class='val'>%u</span></summary></details>\n",
                features.depthBiasClamp);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>fillModeNonSolid                        = <span "
                "class='val'>%u</span></summary></details>\n",
                features.fillModeNonSolid);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>depthBounds                             = <span "
                "class='val'>%u</span></summary></details>\n",
                features.depthBounds);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>wideLines                               = <span "
                "class='val'>%u</span></summary></details>\n",
                features.wideLines);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>largePoints                             = <span "
                "class='val'>%u</span></summary></details>\n",
                features.largePoints);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>alphaToOne                              = <span "
                "class='val'>%u</span></summary></details>\n",
                features.alphaToOne);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>multiViewport                           = <span "
                "class='val'>%u</span></summary></details>\n",
                features.multiViewport);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>samplerAnisotropy                       = <span "
                "class='val'>%u</span></summary></details>\n",
                features.samplerAnisotropy);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>textureCompressionETC2                  = <span "
                "class='val'>%u</span></summary></details>\n",
                features.textureCompressionETC2);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>textureCompressionASTC_LDR              = <span "
                "class='val'>%u</span></summary></details>\n",
                features.textureCompressionASTC_LDR);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>textureCompressionBC                    = <span "
                "class='val'>%u</span></summary></details>\n",
                features.textureCompressionBC);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>occlusionQueryPrecise                   = <span "
                "class='val'>%u</span></summary></details>\n",
                features.occlusionQueryPrecise);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>pipelineStatisticsQuery                 = <span "
                "class='val'>%u</span></summary></details>\n",
                features.pipelineStatisticsQuery);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>vertexPipelineStoresAndAtomics          = <span "
                "class='val'>%u</span></summary></details>\n",
                features.vertexPipelineStoresAndAtomics);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>fragmentStoresAndAtomics                = <span "
                "class='val'>%u</span></summary></details>\n",
                features.fragmentStoresAndAtomics);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderTessellationAndGeometryPointSize  = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderTessellationAndGeometryPointSize);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderImageGatherExtended               = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderImageGatherExtended);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderStorageImageExtendedFormats       = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderStorageImageExtendedFormats);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderStorageImageMultisample           = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderStorageImageMultisample);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderStorageImageReadWithoutFormat     = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderStorageImageReadWithoutFormat);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderStorageImageWriteWithoutFormat    = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderStorageImageWriteWithoutFormat);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderUniformBufferArrayDynamicIndexing = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderUniformBufferArrayDynamicIndexing);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderSampledImageArrayDynamicIndexing  = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderSampledImageArrayDynamicIndexing);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderStorageBufferArrayDynamicIndexing = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderStorageBufferArrayDynamicIndexing);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderStorageImageArrayDynamicIndexing  = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderStorageImageArrayDynamicIndexing);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderClipDistance                      = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderClipDistance);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderCullDistance                      = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderCullDistance);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderFloat64                           = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderFloat64);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderInt64                             = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderInt64);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderInt16                             = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderInt16);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderResourceResidency                 = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderResourceResidency);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>shaderResourceMinLod                    = <span "
                "class='val'>%u</span></summary></details>\n",
                features.shaderResourceMinLod);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sparseBinding                           = <span "
                "class='val'>%u</span></summary></details>\n",
                features.sparseBinding);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sparseResidencyBuffer                   = <span "
                "class='val'>%u</span></summary></details>\n",
                features.sparseResidencyBuffer);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sparseResidencyImage2D                  = <span "
                "class='val'>%u</span></summary></details>\n",
                features.sparseResidencyImage2D);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sparseResidencyImage3D                  = <span "
                "class='val'>%u</span></summary></details>\n",
                features.sparseResidencyImage3D);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sparseResidency2Samples                 = <span "
                "class='val'>%u</span></summary></details>\n",
                features.sparseResidency2Samples);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sparseResidency4Samples                 = <span "
                "class='val'>%u</span></summary></details>\n",
                features.sparseResidency4Samples);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sparseResidency8Samples                 = <span "
                "class='val'>%u</span></summary></details>\n",
                features.sparseResidency8Samples);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sparseResidency16Samples                = <span "
                "class='val'>%u</span></summary></details>\n",
                features.sparseResidency16Samples);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sparseResidencyAliased                  = <span "
                "class='val'>%u</span></summary></details>\n",
                features.sparseResidencyAliased);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>variableMultisampleRate                 = <span "
                "class='val'>%u</span></summary></details>\n",
                features.variableMultisampleRate);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>inheritedQueries                        = <span "
                "class='val'>%u</span></summary></details>\n",
                features.inheritedQueries);
        fprintf(out, "\t\t\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("VkPhysicalDeviceFeatures:\n");
        printf("=========================\n");
        printf("\trobustBufferAccess                      = %u\n", features.robustBufferAccess);
        printf("\tfullDrawIndexUint32                     = %u\n", features.fullDrawIndexUint32);
        printf("\timageCubeArray                          = %u\n", features.imageCubeArray);
        printf("\tindependentBlend                        = %u\n", features.independentBlend);
        printf("\tgeometryShader                          = %u\n", features.geometryShader);
        printf("\ttessellationShader                      = %u\n", features.tessellationShader);
        printf("\tsampleRateShading                       = %u\n", features.sampleRateShading);
        printf("\tdualSrcBlend                            = %u\n", features.dualSrcBlend);
        printf("\tlogicOp                                 = %u\n", features.logicOp);
        printf("\tmultiDrawIndirect                       = %u\n", features.multiDrawIndirect);
        printf("\tdrawIndirectFirstInstance               = %u\n", features.drawIndirectFirstInstance);
        printf("\tdepthClamp                              = %u\n", features.depthClamp);
        printf("\tdepthBiasClamp                          = %u\n", features.depthBiasClamp);
        printf("\tfillModeNonSolid                        = %u\n", features.fillModeNonSolid);
        printf("\tdepthBounds                             = %u\n", features.depthBounds);
        printf("\twideLines                               = %u\n", features.wideLines);
        printf("\tlargePoints                             = %u\n", features.largePoints);
        printf("\talphaToOne                              = %u\n", features.alphaToOne);
        printf("\tmultiViewport                           = %u\n", features.multiViewport);
        printf("\tsamplerAnisotropy                       = %u\n", features.samplerAnisotropy);
        printf("\ttextureCompressionETC2                  = %u\n", features.textureCompressionETC2);
        printf("\ttextureCompressionASTC_LDR              = %u\n", features.textureCompressionASTC_LDR);
        printf("\ttextureCompressionBC                    = %u\n", features.textureCompressionBC);
        printf("\tocclusionQueryPrecise                   = %u\n", features.occlusionQueryPrecise);
        printf("\tpipelineStatisticsQuery                 = %u\n", features.pipelineStatisticsQuery);
        printf("\tvertexPipelineStoresAndAtomics          = %u\n", features.vertexPipelineStoresAndAtomics);
        printf("\tfragmentStoresAndAtomics                = %u\n", features.fragmentStoresAndAtomics);
        printf("\tshaderTessellationAndGeometryPointSize  = %u\n", features.shaderTessellationAndGeometryPointSize);
        printf("\tshaderImageGatherExtended               = %u\n", features.shaderImageGatherExtended);
        printf("\tshaderStorageImageExtendedFormats       = %u\n", features.shaderStorageImageExtendedFormats);
        printf("\tshaderStorageImageMultisample           = %u\n", features.shaderStorageImageMultisample);
        printf("\tshaderStorageImageReadWithoutFormat     = %u\n", features.shaderStorageImageReadWithoutFormat);
        printf("\tshaderStorageImageWriteWithoutFormat    = %u\n", features.shaderStorageImageWriteWithoutFormat);
        printf("\tshaderUniformBufferArrayDynamicIndexing = %u\n", features.shaderUniformBufferArrayDynamicIndexing);
        printf("\tshaderSampledImageArrayDynamicIndexing  = %u\n", features.shaderSampledImageArrayDynamicIndexing);
        printf("\tshaderStorageBufferArrayDynamicIndexing = %u\n", features.shaderStorageBufferArrayDynamicIndexing);
        printf("\tshaderStorageImageArrayDynamicIndexing  = %u\n", features.shaderStorageImageArrayDynamicIndexing);
        printf("\tshaderClipDistance                      = %u\n", features.shaderClipDistance);
        printf("\tshaderCullDistance                      = %u\n", features.shaderCullDistance);
        printf("\tshaderFloat64                           = %u\n", features.shaderFloat64);
        printf("\tshaderInt64                             = %u\n", features.shaderInt64);
        printf("\tshaderInt16                             = %u\n", features.shaderInt16);
        printf("\tshaderResourceResidency                 = %u\n", features.shaderResourceResidency);
        printf("\tshaderResourceMinLod                    = %u\n", features.shaderResourceMinLod);
        printf("\tsparseBinding                           = %u\n", features.sparseBinding);
        printf("\tsparseResidencyBuffer                   = %u\n", features.sparseResidencyBuffer);
        printf("\tsparseResidencyImage2D                  = %u\n", features.sparseResidencyImage2D);
        printf("\tsparseResidencyImage3D                  = %u\n", features.sparseResidencyImage3D);
        printf("\tsparseResidency2Samples                 = %u\n", features.sparseResidency2Samples);
        printf("\tsparseResidency4Samples                 = %u\n", features.sparseResidency4Samples);
        printf("\tsparseResidency8Samples                 = %u\n", features.sparseResidency8Samples);
        printf("\tsparseResidency16Samples                = %u\n", features.sparseResidency16Samples);
        printf("\tsparseResidencyAliased                  = %u\n", features.sparseResidencyAliased);
        printf("\tvariableMultisampleRate                 = %u\n", features.variableMultisampleRate);
        printf("\tinheritedQueries                        = %u\n", features.inheritedQueries);
    }
    if (json_output) {
        printf(",\n");
        printf("\t\"VkPhysicalDeviceFeatures\": {\n");
        printf("\t\t\"robustBufferAccess\": %u,\n", features.robustBufferAccess);
        printf("\t\t\"fullDrawIndexUint32\": %u,\n", features.fullDrawIndexUint32);
        printf("\t\t\"imageCubeArray\": %u,\n", features.imageCubeArray);
        printf("\t\t\"independentBlend\": %u,\n", features.independentBlend);
        printf("\t\t\"geometryShader\": %u,\n", features.geometryShader);
        printf("\t\t\"tessellationShader\": %u,\n", features.tessellationShader);
        printf("\t\t\"sampleRateShading\": %u,\n", features.sampleRateShading);
        printf("\t\t\"dualSrcBlend\": %u,\n", features.dualSrcBlend);
        printf("\t\t\"logicOp\": %u,\n", features.logicOp);
        printf("\t\t\"multiDrawIndirect\": %u,\n", features.multiDrawIndirect);
        printf("\t\t\"drawIndirectFirstInstance\": %u,\n", features.drawIndirectFirstInstance);
        printf("\t\t\"depthClamp\": %u,\n", features.depthClamp);
        printf("\t\t\"depthBiasClamp\": %u,\n", features.depthBiasClamp);
        printf("\t\t\"fillModeNonSolid\": %u,\n", features.fillModeNonSolid);
        printf("\t\t\"depthBounds\": %u,\n", features.depthBounds);
        printf("\t\t\"wideLines\": %u,\n", features.wideLines);
        printf("\t\t\"largePoints\": %u,\n", features.largePoints);
        printf("\t\t\"alphaToOne\": %u,\n", features.alphaToOne);
        printf("\t\t\"multiViewport\": %u,\n", features.multiViewport);
        printf("\t\t\"samplerAnisotropy\": %u,\n", features.samplerAnisotropy);
        printf("\t\t\"textureCompressionETC2\": %u,\n", features.textureCompressionETC2);
        printf("\t\t\"textureCompressionASTC_LDR\": %u,\n", features.textureCompressionASTC_LDR);
        printf("\t\t\"textureCompressionBC\": %u,\n", features.textureCompressionBC);
        printf("\t\t\"occlusionQueryPrecise\": %u,\n", features.occlusionQueryPrecise);
        printf("\t\t\"pipelineStatisticsQuery\": %u,\n", features.pipelineStatisticsQuery);
        printf("\t\t\"vertexPipelineStoresAndAtomics\": %u,\n", features.vertexPipelineStoresAndAtomics);
        printf("\t\t\"fragmentStoresAndAtomics\": %u,\n", features.fragmentStoresAndAtomics);
        printf("\t\t\"shaderTessellationAndGeometryPointSize\": %u,\n", features.shaderTessellationAndGeometryPointSize);
        printf("\t\t\"shaderImageGatherExtended\": %u,\n", features.shaderImageGatherExtended);
        printf("\t\t\"shaderStorageImageExtendedFormats\": %u,\n", features.shaderStorageImageExtendedFormats);
        printf("\t\t\"shaderStorageImageMultisample\": %u,\n", features.shaderStorageImageMultisample);
        printf("\t\t\"shaderStorageImageReadWithoutFormat\": %u,\n", features.shaderStorageImageReadWithoutFormat);
        printf("\t\t\"shaderStorageImageWriteWithoutFormat\": %u,\n", features.shaderStorageImageWriteWithoutFormat);
        printf("\t\t\"shaderUniformBufferArrayDynamicIndexing\": %u,\n", features.shaderUniformBufferArrayDynamicIndexing);
        printf("\t\t\"shaderSampledImageArrayDynamicIndexing\": %u,\n", features.shaderSampledImageArrayDynamicIndexing);
        printf("\t\t\"shaderStorageBufferArrayDynamicIndexing\": %u,\n", features.shaderStorageBufferArrayDynamicIndexing);
        printf("\t\t\"shaderStorageImageArrayDynamicIndexing\": %u,\n", features.shaderStorageImageArrayDynamicIndexing);
        printf("\t\t\"shaderClipDistance\": %u,\n", features.shaderClipDistance);
        printf("\t\t\"shaderCullDistance\": %u,\n", features.shaderCullDistance);
        printf("\t\t\"shaderFloat64\": %u,\n", features.shaderFloat64);
        printf("\t\t\"shaderInt64\": %u,\n", features.shaderInt64);
        printf("\t\t\"shaderInt16\": %u,\n", features.shaderInt16);
        printf("\t\t\"shaderResourceResidency\": %u,\n", features.shaderResourceResidency);
        printf("\t\t\"shaderResourceMinLod\": %u,\n", features.shaderResourceMinLod);
        printf("\t\t\"sparseBinding\": %u,\n", features.sparseBinding);
        printf("\t\t\"sparseResidencyBuffer\": %u,\n", features.sparseResidencyBuffer);
        printf("\t\t\"sparseResidencyImage2D\": %u,\n", features.sparseResidencyImage2D);
        printf("\t\t\"sparseResidencyImage3D\": %u,\n", features.sparseResidencyImage3D);
        printf("\t\t\"sparseResidency2Samples\": %u,\n", features.sparseResidency2Samples);
        printf("\t\t\"sparseResidency4Samples\": %u,\n", features.sparseResidency4Samples);
        printf("\t\t\"sparseResidency8Samples\": %u,\n", features.sparseResidency8Samples);
        printf("\t\t\"sparseResidency16Samples\": %u,\n", features.sparseResidency16Samples);
        printf("\t\t\"sparseResidencyAliased\": %u,\n", features.sparseResidencyAliased);
        printf("\t\t\"variableMultisampleRate\": %u,\n", features.variableMultisampleRate);
        printf("\t\t\"inheritedQueries\": %u\n", features.inheritedQueries);
        printf("\t}");
    }

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        void *place = gpu->features2.pNext;
        while (place) {
            struct VkStructureHeader *structure = (struct VkStructureHeader *)place;
            if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR &&
                CheckPhysicalDeviceExtensionIncluded(VK_KHR_8BIT_STORAGE_EXTENSION_NAME, gpu->device_extensions,
                                                     gpu->device_extension_count)) {
                VkPhysicalDevice8BitStorageFeaturesKHR *b8_store_features = (VkPhysicalDevice8BitStorageFeaturesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDevice8BitStorageFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>storageBuffer8BitAccess           = <span "
                            "class='val'>%u</span></summary></details>\n",
                            b8_store_features->storageBuffer8BitAccess);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>uniformAndStorageBuffer8BitAccess = <span "
                            "class='val'>%u</span></summary></details>\n",
                            b8_store_features->uniformAndStorageBuffer8BitAccess);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>storagePushConstant8              = <span "
                            "class='val'>%u</span></summary></details>\n",
                            b8_store_features->storagePushConstant8);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDevice8BitStorageFeatures:\n");
                    printf("=====================================\n");
                    printf("\tstorageBuffer8BitAccess           = %u\n", b8_store_features->storageBuffer8BitAccess);
                    printf("\tuniformAndStorageBuffer8BitAccess = %u\n", b8_store_features->uniformAndStorageBuffer8BitAccess);
                    printf("\tstoragePushConstant8              = %u\n", b8_store_features->storagePushConstant8);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_16BIT_STORAGE_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDevice16BitStorageFeaturesKHR *b16_store_features = (VkPhysicalDevice16BitStorageFeaturesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDevice16BitStorageFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>storageBuffer16BitAccess           = <span "
                            "class='val'>%u</span></summary></details>\n",
                            b16_store_features->storageBuffer16BitAccess);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>uniformAndStorageBuffer16BitAccess = <span "
                            "class='val'>%u</span></summary></details>\n",
                            b16_store_features->uniformAndStorageBuffer16BitAccess);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>storagePushConstant16              = <span "
                            "class='val'>%u</span></summary></details>\n",
                            b16_store_features->storagePushConstant16);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>storageInputOutput16               = <span "
                            "class='val'>%u</span></summary></details>\n",
                            b16_store_features->storageInputOutput16);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDevice16BitStorageFeatures:\n");
                    printf("=====================================\n");
                    printf("\tstorageBuffer16BitAccess           = %u\n", b16_store_features->storageBuffer16BitAccess);
                    printf("\tuniformAndStorageBuffer16BitAccess = %u\n", b16_store_features->uniformAndStorageBuffer16BitAccess);
                    printf("\tstoragePushConstant16              = %u\n", b16_store_features->storagePushConstant16);
                    printf("\tstorageInputOutput16               = %u\n", b16_store_features->storageInputOutput16);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *sampler_ycbcr_features =
                    (VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceSamplerYcbcrConversionFeatures</summary>\n");
                    fprintf(
                        out,
                        "\t\t\t\t\t\t<details><summary>samplerYcbcrConversion = <span class='val'>%u</span></summary></details>\n",
                        sampler_ycbcr_features->samplerYcbcrConversion);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceSamplerYcbcrConversionFeatures:\n");
                    printf("===============================================\n");
                    printf("\tsamplerYcbcrConversion = %u\n", sampler_ycbcr_features->samplerYcbcrConversion);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceVariablePointerFeaturesKHR *var_pointer_features =
                    (VkPhysicalDeviceVariablePointerFeaturesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceVariablePointerFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>variablePointersStorageBuffer = <span "
                            "class='val'>%u</span></summary></details>\n",
                            var_pointer_features->variablePointersStorageBuffer);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>variablePointers              = <span "
                            "class='val'>%u</span></summary></details>\n",
                            var_pointer_features->variablePointers);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceVariablePointerFeatures:\n");
                    printf("========================================\n");
                    printf("\tvariablePointersStorageBuffer = %u\n", var_pointer_features->variablePointersStorageBuffer);
                    printf("\tvariablePointers              = %u\n", var_pointer_features->variablePointers);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *blend_op_adv_features =
                    (VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceBlendOperationAdvancedFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>advancedBlendCoherentOperations = <span "
                            "class='val'>%u</span></summary></details>\n",
                            blend_op_adv_features->advancedBlendCoherentOperations);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceBlendOperationAdvancedFeatures:\n");
                    printf("===============================================\n");
                    printf("\tadvancedBlendCoherentOperations = %u\n", blend_op_adv_features->advancedBlendCoherentOperations);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_MULTIVIEW_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceMultiviewFeaturesKHR *multiview_features = (VkPhysicalDeviceMultiviewFeaturesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceMultiviewFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>multiview                   = <span "
                            "class='val'>%u</span></summary></details>\n",
                            multiview_features->multiview);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>multiviewGeometryShader     = <span "
                            "class='val'>%u</span></summary></details>\n",
                            multiview_features->multiviewGeometryShader);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>multiviewTessellationShader = <span "
                            "class='val'>%u</span></summary></details>\n",
                            multiview_features->multiviewTessellationShader);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceMultiviewFeatures:\n");
                    printf("==================================\n");
                    printf("\tmultiview                   = %u\n", multiview_features->multiview);
                    printf("\tmultiviewGeometryShader     = %u\n", multiview_features->multiviewGeometryShader);
                    printf("\tmultiviewTessellationShader = %u\n", multiview_features->multiviewTessellationShader);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceFloat16Int8FeaturesKHR *float_int_features = (VkPhysicalDeviceFloat16Int8FeaturesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceFloat16Int8Features</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderFloat16 = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_int_features->shaderFloat16);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderInt8    = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_int_features->shaderInt8);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceFloat16Int8Features:\n");
                    printf("====================================\n");
                    printf("\tshaderFloat16 = %" PRIuLEAST32 "\n", float_int_features->shaderFloat16);
                    printf("\tshaderInt8    = %" PRIuLEAST32 "\n", float_int_features->shaderInt8);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *shader_atomic_int64_features =
                    (VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceShaderAtomicInt64Features</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderBufferInt64Atomics = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            shader_atomic_int64_features->shaderBufferInt64Atomics);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderSharedInt64Atomics = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            shader_atomic_int64_features->shaderSharedInt64Atomics);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceShaderAtomicInt64Features:\n");
                    printf("==========================================\n");
                    printf("\tshaderBufferInt64Atomics = %" PRIuLEAST32 "\n",
                           shader_atomic_int64_features->shaderBufferInt64Atomics);
                    printf("\tshaderSharedInt64Atomics = %" PRIuLEAST32 "\n",
                           shader_atomic_int64_features->shaderSharedInt64Atomics);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceTransformFeedbackFeaturesEXT *transform_feedback_features =
                    (VkPhysicalDeviceTransformFeedbackFeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceTransformFeedbackFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>transformFeedback = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            transform_feedback_features->transformFeedback);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>geometryStreams   = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            transform_feedback_features->geometryStreams);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceTransformFeedbackFeatures:\n");
                    printf("==========================================\n");
                    printf("\ttransformFeedback = %" PRIuLEAST32 "\n", transform_feedback_features->transformFeedback);
                    printf("\tgeometryStreams   = %" PRIuLEAST32 "\n", transform_feedback_features->geometryStreams);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *scalar_block_layout_features =
                    (VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceScalarBlockLayoutFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>scalarBlockLayout = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            scalar_block_layout_features->scalarBlockLayout);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceScalarBlockLayoutFeatures:\n");
                    printf("==========================================\n");
                    printf("\tscalarBlockLayout = %" PRIuLEAST32 "\n", scalar_block_layout_features->scalarBlockLayout);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceFragmentDensityMapFeaturesEXT *fragment_density_map_features =
                    (VkPhysicalDeviceFragmentDensityMapFeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceFragmentDensityMapFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>fragmentDensityMap                    = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            fragment_density_map_features->fragmentDensityMap);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>fragmentDensityMapDynamic             = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            fragment_density_map_features->fragmentDensityMapDynamic);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>fragmentDensityMapNonSubsampledImages = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            fragment_density_map_features->fragmentDensityMapNonSubsampledImages);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceFragmentDensityMapFeatures:\n");
                    printf("==========================================\n");
                    printf("\tfragmentDensityMap                    = %" PRIuLEAST32 "\n",
                           fragment_density_map_features->fragmentDensityMap);
                    printf("\tfragmentDensityMapDynamic             = %" PRIuLEAST32 "\n",
                           fragment_density_map_features->fragmentDensityMapDynamic);
                    printf("\tfragmentDensityMapNonSubsampledImages = %" PRIuLEAST32 "\n",
                           fragment_density_map_features->fragmentDensityMapNonSubsampledImages);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceMemoryPriorityFeaturesEXT *memory_priority_features =
                    (VkPhysicalDeviceMemoryPriorityFeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceMemoryPriorityFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>memoryPriority = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            memory_priority_features->memoryPriority);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceMemoryPriorityFeatures:\n");
                    printf("======================================\n");
                    printf("\tmemoryPriority = %" PRIuLEAST32 "\n", memory_priority_features->memoryPriority);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_ADDRESS_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceBufferAddressFeaturesEXT *buffer_address_features =
                    (VkPhysicalDeviceBufferAddressFeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceBufferAddressFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>bufferDeviceAddress = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            buffer_address_features->bufferDeviceAddress);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>bufferDeviceAddressCaptureReplay = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            buffer_address_features->bufferDeviceAddressCaptureReplay);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>bufferDeviceAddressMultiDevice = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            buffer_address_features->bufferDeviceAddressMultiDevice);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceBufferAddressFeatures:\n");
                    printf("======================================\n");
                    printf("\tbufferDeviceAddress = %" PRIuLEAST32 "\n", buffer_address_features->bufferDeviceAddress);
                    printf("\tbufferDeviceAddressCaptureReplay = %" PRIuLEAST32 "\n",
                           buffer_address_features->bufferDeviceAddressCaptureReplay);
                    printf("\tbufferDeviceAddressMultiDevice = %" PRIuLEAST32 "\n",
                           buffer_address_features->bufferDeviceAddressMultiDevice);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_YCBCR_IMAGE_ARRAYS_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *ycbcr_image_arrays_features =
                    (VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceYcbcrImageArraysFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>ycbcrImageArrays = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            ycbcr_image_arrays_features->ycbcrImageArrays);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceYcbcrImageArraysFeatures:\n");
                    printf("=========================================\n");
                    printf("\tycbcrImageArrays = %" PRIuLEAST32 "\n", ycbcr_image_arrays_features->ycbcrImageArrays);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceHostQueryResetFeaturesEXT *host_query_reset_features =
                    (VkPhysicalDeviceHostQueryResetFeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceHostQueryResetFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>hostQueryReset = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            host_query_reset_features->hostQueryReset);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceHostQueryResetFeatures:\n");
                    printf("=======================================\n");
                    printf("\thostQueryReset = %" PRIuLEAST32 "\n", host_query_reset_features->hostQueryReset);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceDescriptorIndexingFeaturesEXT *indexing_features =
                    (VkPhysicalDeviceDescriptorIndexingFeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceDescriptorIndexingFeatures</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderInputAttachmentArrayDynamicIndexing    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->shaderInputAttachmentArrayDynamicIndexing);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderUniformTexelBufferArrayDynamicIndexing    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->shaderUniformTexelBufferArrayDynamicIndexing);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderStorageTexelBufferArrayDynamicIndexing    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->shaderStorageTexelBufferArrayDynamicIndexing);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderUniformBufferArrayNonUniformIndexing    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->shaderUniformBufferArrayNonUniformIndexing);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderSampledImageArrayNonUniformIndexing    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->shaderSampledImageArrayNonUniformIndexing);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderStorageBufferArrayNonUniformIndexing    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->shaderStorageBufferArrayNonUniformIndexing);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderStorageImageArrayNonUniformIndexing    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->shaderStorageImageArrayNonUniformIndexing);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderInputAttachmentArrayNonUniformIndexing    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->shaderInputAttachmentArrayNonUniformIndexing);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderUniformTexelBufferArrayNonUniformIndexing    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->shaderUniformTexelBufferArrayNonUniformIndexing);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderStorageTexelBufferArrayNonUniformIndexing    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->shaderStorageTexelBufferArrayNonUniformIndexing);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>descriptorBindingUniformBufferUpdateAfterBind    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->descriptorBindingUniformBufferUpdateAfterBind);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>descriptorBindingSampledImageUpdateAfterBind    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->descriptorBindingSampledImageUpdateAfterBind);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>descriptorBindingStorageImageUpdateAfterBind    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->descriptorBindingStorageImageUpdateAfterBind);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>descriptorBindingStorageBufferUpdateAfterBind    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->descriptorBindingStorageBufferUpdateAfterBind);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>descriptorBindingUniformTexelBufferUpdateAfterBind    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->descriptorBindingUniformTexelBufferUpdateAfterBind);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>descriptorBindingStorageTexelBufferUpdateAfterBind    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->descriptorBindingStorageTexelBufferUpdateAfterBind);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>descriptorBindingUpdateUnusedWhilePending    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->descriptorBindingUpdateUnusedWhilePending);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>descriptorBindingPartiallyBound    = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            indexing_features->descriptorBindingPartiallyBound);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>descriptorBindingVariableDescriptorCount    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_features->descriptorBindingVariableDescriptorCount);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>runtimeDescriptorArray    = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            indexing_features->runtimeDescriptorArray);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceDescriptorIndexingFeatures:\n");
                    printf("=======================================\n");
                    printf("\tshaderInputAttachmentArrayDynamicIndexing = %" PRIuLEAST32 "\n",
                           indexing_features->shaderInputAttachmentArrayDynamicIndexing);
                    printf("\tshaderUniformTexelBufferArrayDynamicIndexing = %" PRIuLEAST32 "\n",
                           indexing_features->shaderUniformTexelBufferArrayDynamicIndexing);
                    printf("\tshaderStorageTexelBufferArrayDynamicIndexing = %" PRIuLEAST32 "\n",
                           indexing_features->shaderStorageTexelBufferArrayDynamicIndexing);
                    printf("\tshaderUniformBufferArrayNonUniformIndexing = %" PRIuLEAST32 "\n",
                           indexing_features->shaderUniformBufferArrayNonUniformIndexing);
                    printf("\tshaderSampledImageArrayNonUniformIndexing = %" PRIuLEAST32 "\n",
                           indexing_features->shaderSampledImageArrayNonUniformIndexing);
                    printf("\tshaderStorageBufferArrayNonUniformIndexing = %" PRIuLEAST32 "\n",
                           indexing_features->shaderStorageBufferArrayNonUniformIndexing);
                    printf("\tshaderStorageImageArrayNonUniformIndexing = %" PRIuLEAST32 "\n",
                           indexing_features->shaderStorageImageArrayNonUniformIndexing);
                    printf("\tshaderInputAttachmentArrayNonUniformIndexing = %" PRIuLEAST32 "\n",
                           indexing_features->shaderInputAttachmentArrayNonUniformIndexing);
                    printf("\tshaderUniformTexelBufferArrayNonUniformIndexing = %" PRIuLEAST32 "\n",
                           indexing_features->shaderUniformTexelBufferArrayNonUniformIndexing);
                    printf("\tshaderStorageTexelBufferArrayNonUniformIndexing = %" PRIuLEAST32 "\n",
                           indexing_features->shaderStorageTexelBufferArrayNonUniformIndexing);
                    printf("\tdescriptorBindingUniformBufferUpdateAfterBind = %" PRIuLEAST32 "\n",
                           indexing_features->descriptorBindingUniformBufferUpdateAfterBind);
                    printf("\tdescriptorBindingSampledImageUpdateAfterBind = %" PRIuLEAST32 "\n",
                           indexing_features->descriptorBindingSampledImageUpdateAfterBind);
                    printf("\tdescriptorBindingStorageImageUpdateAfterBind = %" PRIuLEAST32 "\n",
                           indexing_features->descriptorBindingStorageImageUpdateAfterBind);
                    printf("\tdescriptorBindingStorageBufferUpdateAfterBind = %" PRIuLEAST32 "\n",
                           indexing_features->descriptorBindingStorageBufferUpdateAfterBind);
                    printf("\tdescriptorBindingUniformTexelBufferUpdateAfterBind = %" PRIuLEAST32 "\n",
                           indexing_features->descriptorBindingUniformTexelBufferUpdateAfterBind);
                    printf("\tdescriptorBindingStorageTexelBufferUpdateAfterBind = %" PRIuLEAST32 "\n",
                           indexing_features->descriptorBindingStorageTexelBufferUpdateAfterBind);
                    printf("\tdescriptorBindingUpdateUnusedWhilePending = %" PRIuLEAST32 "\n",
                           indexing_features->descriptorBindingUpdateUnusedWhilePending);
                    printf("\tdescriptorBindingPartiallyBound = %" PRIuLEAST32 "\n",
                           indexing_features->descriptorBindingPartiallyBound);
                    printf("\tdescriptorBindingVariableDescriptorCount = %" PRIuLEAST32 "\n",
                           indexing_features->descriptorBindingVariableDescriptorCount);
                    printf("\truntimeDescriptorArray = %" PRIuLEAST32 "\n", indexing_features->runtimeDescriptorArray);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME,
                                                            gpu->device_extensions, gpu->device_extension_count)) {
                VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *standard_features =
                    (VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *)structure;
                if (html_output) {
                    fprintf(out,
                            "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>uniformBufferStandardLayout = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            standard_features->uniformBufferStandardLayout);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR:\n");
                    printf("=======================================================\n");
                    printf("\tuniformBufferStandardLayout = %" PRIuLEAST32 "\n", standard_features->uniformBufferStandardLayout);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *fragment_shader_features =
                    (VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>fragmentShaderPixelInterlock    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            fragment_shader_features->fragmentShaderPixelInterlock);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>fragmentShaderSampleInterlock    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            fragment_shader_features->fragmentShaderSampleInterlock);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>fragmentShaderShadingRateInterlock    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            fragment_shader_features->fragmentShaderShadingRateInterlock);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceFragmentShaderInterlockFeaturesEXT:\n");
                    printf("===================================================\n");
                    printf("\tfragmentShaderPixelInterlock = %" PRIuLEAST32 "\n",
                           fragment_shader_features->fragmentShaderPixelInterlock);
                    printf("\tfragmentShaderSampleInterlock = %" PRIuLEAST32 "\n",
                           fragment_shader_features->fragmentShaderSampleInterlock);
                    printf("\tfragmentShaderShadingRateInterlock = %" PRIuLEAST32 "\n",
                           fragment_shader_features->fragmentShaderShadingRateInterlock);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceImagelessFramebufferFeaturesKHR *imageless_framebuffer =
                    (VkPhysicalDeviceImagelessFramebufferFeaturesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceImagelessFramebufferFeaturesKHR</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>imagelessFramebuffer = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            imageless_framebuffer->imagelessFramebuffer);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceImagelessFramebufferFeaturesKHR:\n");
                    printf("================================================\n");
                    printf("\timagelessFramebuffer = %" PRIuLEAST32 "\n", imageless_framebuffer->imagelessFramebuffer);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *texel_buffer_alignment =
                    (VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>texelBufferAlignment = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            texel_buffer_alignment->texelBufferAlignment);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceTexelBufferAlignmentFeaturesEXT:\n");
                    printf("================================================\n");
                    printf("\ttexelBufferAlignment = %" PRIuLEAST32 "\n", texel_buffer_alignment->texelBufferAlignment);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME,
                                                            gpu->device_extensions, gpu->device_extension_count)) {
                VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *shader_helper =
                    (VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out,
                            "\t\t\t\t\t<details><summary>VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderDemoteToHelperInvocation    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            shader_helper->shaderDemoteToHelperInvocation);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT:\n");
                    printf("===================================================\n");
                    printf("\tshaderDemoteToHelperInvocation = %" PRIuLEAST32 "\n", shader_helper->shaderDemoteToHelperInvocation);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceIndexTypeUint8FeaturesEXT *index_type_uint8_features =
                    (VkPhysicalDeviceIndexTypeUint8FeaturesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceIndexTypeUint8FeaturesEXT</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>indexTypeUint8    = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            index_type_uint8_features->indexTypeUint8);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceIndexTypeUint8FeaturesEXT:\n");
                    printf("===================================================\n");
                    printf("\tindexTypeUint8 = %" PRIuLEAST32 "\n", index_type_uint8_features->indexTypeUint8);
                }
            }
            place = structure->pNext;
        }
    }
}

static void AppDumpSparseProps(const VkPhysicalDeviceSparseProperties *sparse_props, FILE *out) {
    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceSparseProperties</summary>\n");
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>residencyStandard2DBlockShape            = <span "
                "class='val'>%u</span></summary></details>\n",
                sparse_props->residencyStandard2DBlockShape);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>residencyStandard2DMultisampleBlockShape = <span "
                "class='val'>%u</span></summary></details>\n",
                sparse_props->residencyStandard2DMultisampleBlockShape);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>residencyStandard3DBlockShape            = <span "
                "class='val'>%u</span></summary></details>\n",
                sparse_props->residencyStandard3DBlockShape);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>residencyAlignedMipSize                  = <span "
                "class='val'>%u</span></summary></details>\n",
                sparse_props->residencyAlignedMipSize);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>residencyNonResidentStrict               = <span "
                "class='val'>%u</span></summary></details>\n",
                sparse_props->residencyNonResidentStrict);
        fprintf(out, "\t\t\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("\tVkPhysicalDeviceSparseProperties:\n");
        printf("\t---------------------------------\n");
        printf("\t\tresidencyStandard2DBlockShape            = %u\n", sparse_props->residencyStandard2DBlockShape);
        printf("\t\tresidencyStandard2DMultisampleBlockShape = %u\n", sparse_props->residencyStandard2DMultisampleBlockShape);
        printf("\t\tresidencyStandard3DBlockShape            = %u\n", sparse_props->residencyStandard3DBlockShape);
        printf("\t\tresidencyAlignedMipSize                  = %u\n", sparse_props->residencyAlignedMipSize);
        printf("\t\tresidencyNonResidentStrict               = %u\n", sparse_props->residencyNonResidentStrict);
    }
    if (json_output) {
        printf(",\n");
        printf("\t\t\"sparseProperties\": {\n");
        printf("\t\t\t\"residencyStandard2DBlockShape\": %u,\n", sparse_props->residencyStandard2DBlockShape);
        printf("\t\t\t\"residencyStandard2DMultisampleBlockShape\": %u,\n", sparse_props->residencyStandard2DMultisampleBlockShape);
        printf("\t\t\t\"residencyStandard3DBlockShape\": %u,\n", sparse_props->residencyStandard3DBlockShape);
        printf("\t\t\t\"residencyAlignedMipSize\": %u,\n", sparse_props->residencyAlignedMipSize);
        printf("\t\t\t\"residencyNonResidentStrict\": %u\n", sparse_props->residencyNonResidentStrict);
        printf("\t\t}");
    }
}

static void AppDumpLimits(const VkPhysicalDeviceLimits *limits, FILE *out) {
    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceLimits</summary>\n");
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxImageDimension1D                     = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxImageDimension1D);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxImageDimension2D                     = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxImageDimension2D);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxImageDimension3D                     = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxImageDimension3D);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxImageDimensionCube                   = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxImageDimensionCube);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxImageArrayLayers                     = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxImageArrayLayers);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxTexelBufferElements                  = <span class='val'>0x%" PRIxLEAST32
                "</span></summary></details>\n",
                limits->maxTexelBufferElements);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxUniformBufferRange                   = <span class='val'>0x%" PRIxLEAST32
                "</span></summary></details>\n",
                limits->maxUniformBufferRange);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxStorageBufferRange                   = <span class='val'>0x%" PRIxLEAST32
                "</span></summary></details>\n",
                limits->maxStorageBufferRange);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxPushConstantsSize                    = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxPushConstantsSize);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxMemoryAllocationCount                = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxMemoryAllocationCount);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxSamplerAllocationCount               = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxSamplerAllocationCount);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>bufferImageGranularity                  = <span class='val'>0x%" PRIxLEAST64
                "</span></summary></details>\n",
                limits->bufferImageGranularity);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sparseAddressSpaceSize                  = <span class='val'>0x%" PRIxLEAST64
                "</span></summary></details>\n",
                limits->sparseAddressSpaceSize);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxBoundDescriptorSets                  = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxBoundDescriptorSets);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxPerStageDescriptorSamplers           = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxPerStageDescriptorSamplers);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxPerStageDescriptorUniformBuffers     = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxPerStageDescriptorUniformBuffers);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxPerStageDescriptorStorageBuffers     = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxPerStageDescriptorStorageBuffers);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxPerStageDescriptorSampledImages      = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxPerStageDescriptorSampledImages);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxPerStageDescriptorStorageImages      = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxPerStageDescriptorStorageImages);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxPerStageDescriptorInputAttachments   = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxPerStageDescriptorInputAttachments);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxPerStageResources                    = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxPerStageResources);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxDescriptorSetSamplers                = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxDescriptorSetSamplers);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxDescriptorSetUniformBuffers          = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxDescriptorSetUniformBuffers);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxDescriptorSetUniformBuffersDynamic   = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxDescriptorSetUniformBuffersDynamic);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxDescriptorSetStorageBuffers          = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxDescriptorSetStorageBuffers);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxDescriptorSetStorageBuffersDynamic   = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxDescriptorSetStorageBuffersDynamic);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxDescriptorSetSampledImages           = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxDescriptorSetSampledImages);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxDescriptorSetStorageImages           = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxDescriptorSetStorageImages);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxDescriptorSetInputAttachments        = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxDescriptorSetInputAttachments);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxVertexInputAttributes                = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxVertexInputAttributes);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxVertexInputBindings                  = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxVertexInputBindings);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxVertexInputAttributeOffset           = <span class='val'>0x%" PRIxLEAST32
                "</span></summary></details>\n",
                limits->maxVertexInputAttributeOffset);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxVertexInputBindingStride             = <span class='val'>0x%" PRIxLEAST32
                "</span></summary></details>\n",
                limits->maxVertexInputBindingStride);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxVertexOutputComponents               = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxVertexOutputComponents);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxTessellationGenerationLevel          = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxTessellationGenerationLevel);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxTessellationPatchSize                        = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxTessellationPatchSize);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxTessellationControlPerVertexInputComponents  = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxTessellationControlPerVertexInputComponents);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxTessellationControlPerVertexOutputComponents = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxTessellationControlPerVertexOutputComponents);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxTessellationControlPerPatchOutputComponents  = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxTessellationControlPerPatchOutputComponents);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxTessellationControlTotalOutputComponents     = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxTessellationControlTotalOutputComponents);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxTessellationEvaluationInputComponents        = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxTessellationEvaluationInputComponents);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxTessellationEvaluationOutputComponents       = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxTessellationEvaluationOutputComponents);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxGeometryShaderInvocations            = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxGeometryShaderInvocations);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxGeometryInputComponents              = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxGeometryInputComponents);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxGeometryOutputComponents             = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxGeometryOutputComponents);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxGeometryOutputVertices               = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxGeometryOutputVertices);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxGeometryTotalOutputComponents        = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxGeometryTotalOutputComponents);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxFragmentInputComponents              = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxFragmentInputComponents);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxFragmentOutputAttachments            = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxFragmentOutputAttachments);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxFragmentDualSrcAttachments           = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxFragmentDualSrcAttachments);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxFragmentCombinedOutputResources      = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxFragmentCombinedOutputResources);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxComputeSharedMemorySize              = <span class='val'>%" PRIuLEAST32
                "</span></summary></details>\n",
                limits->maxComputeSharedMemorySize);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupCount[0]             = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxComputeWorkGroupCount[0]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupCount[1]             = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxComputeWorkGroupCount[1]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupCount[2]             = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxComputeWorkGroupCount[2]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupInvocations          = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxComputeWorkGroupInvocations);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupSize[0]              = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxComputeWorkGroupSize[0]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupSize[1]              = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxComputeWorkGroupSize[1]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupSize[2]              = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxComputeWorkGroupSize[2]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>subPixelPrecisionBits                   = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->subPixelPrecisionBits);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>subTexelPrecisionBits                   = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->subTexelPrecisionBits);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>mipmapPrecisionBits                     = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->mipmapPrecisionBits);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxDrawIndexedIndexValue                = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxDrawIndexedIndexValue);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxDrawIndirectCount                    = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxDrawIndirectCount);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxSamplerLodBias                       = <span "
                "class='val'>%f</span></summary></details>\n",
                limits->maxSamplerLodBias);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxSamplerAnisotropy                    = <span "
                "class='val'>%f</span></summary></details>\n",
                limits->maxSamplerAnisotropy);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxViewports                            = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxViewports);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxViewportDimensions[0]                = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxViewportDimensions[0]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxViewportDimensions[1]                = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxViewportDimensions[1]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>viewportBoundsRange[0]                  = <span "
                "class='val'>%13f</span></summary></details>\n",
                limits->viewportBoundsRange[0]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>viewportBoundsRange[1]                  = <span "
                "class='val'>%13f</span></summary></details>\n",
                limits->viewportBoundsRange[1]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>viewportSubPixelBits                    = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->viewportSubPixelBits);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>minMemoryMapAlignment                   = <span class='val'>" PRINTF_SIZE_T_SPECIFIER
                "</span></summary></details>\n",
                limits->minMemoryMapAlignment);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>minTexelBufferOffsetAlignment           = <span class='val'>0x%" PRIxLEAST64
                "</span></summary></details>\n",
                limits->minTexelBufferOffsetAlignment);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>minUniformBufferOffsetAlignment         = <span class='val'>0x%" PRIxLEAST64
                "</span></summary></details>\n",
                limits->minUniformBufferOffsetAlignment);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>minStorageBufferOffsetAlignment         = <span class='val'>0x%" PRIxLEAST64
                "</span></summary></details>\n",
                limits->minStorageBufferOffsetAlignment);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>minTexelOffset                          = <span "
                "class='val'>%3d</span></summary></details>\n",
                limits->minTexelOffset);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxTexelOffset                          = <span "
                "class='val'>%3d</span></summary></details>\n",
                limits->maxTexelOffset);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>minTexelGatherOffset                    = <span "
                "class='val'>%3d</span></summary></details>\n",
                limits->minTexelGatherOffset);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxTexelGatherOffset                    = <span "
                "class='val'>%3d</span></summary></details>\n",
                limits->maxTexelGatherOffset);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>minInterpolationOffset                  = <span "
                "class='val'>%9f</span></summary></details>\n",
                limits->minInterpolationOffset);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxInterpolationOffset                  = <span "
                "class='val'>%9f</span></summary></details>\n",
                limits->maxInterpolationOffset);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>subPixelInterpolationOffsetBits         = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->subPixelInterpolationOffsetBits);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxFramebufferWidth                     = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxFramebufferWidth);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxFramebufferHeight                    = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxFramebufferHeight);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxFramebufferLayers                    = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxFramebufferLayers);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>framebufferColorSampleCounts            = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->framebufferColorSampleCounts);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>framebufferDepthSampleCounts            = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->framebufferDepthSampleCounts);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>framebufferStencilSampleCounts          = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->framebufferStencilSampleCounts);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>framebufferNoAttachmentsSampleCounts    = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->framebufferNoAttachmentsSampleCounts);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxColorAttachments                     = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxColorAttachments);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sampledImageColorSampleCounts           = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->sampledImageColorSampleCounts);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sampledImageDepthSampleCounts           = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->sampledImageDepthSampleCounts);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sampledImageStencilSampleCounts         = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->sampledImageStencilSampleCounts);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>sampledImageIntegerSampleCounts         = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->sampledImageIntegerSampleCounts);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>storageImageSampleCounts                = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->storageImageSampleCounts);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxSampleMaskWords                      = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxSampleMaskWords);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>timestampComputeAndGraphics             = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->timestampComputeAndGraphics);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>timestampPeriod                         = <span "
                "class='val'>%f</span></summary></details>\n",
                limits->timestampPeriod);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxClipDistances                        = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxClipDistances);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxCullDistances                        = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxCullDistances);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>maxCombinedClipAndCullDistances         = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->maxCombinedClipAndCullDistances);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>discreteQueuePriorities                 = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->discreteQueuePriorities);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>pointSizeRange[0]                       = <span "
                "class='val'>%f</span></summary></details>\n",
                limits->pointSizeRange[0]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>pointSizeRange[1]                       = <span "
                "class='val'>%f</span></summary></details>\n",
                limits->pointSizeRange[1]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>lineWidthRange[0]                       = <span "
                "class='val'>%f</span></summary></details>\n",
                limits->lineWidthRange[0]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>lineWidthRange[1]                       = <span "
                "class='val'>%f</span></summary></details>\n",
                limits->lineWidthRange[1]);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>pointSizeGranularity                    = <span "
                "class='val'>%f</span></summary></details>\n",
                limits->pointSizeGranularity);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>lineWidthGranularity                    = <span "
                "class='val'>%f</span></summary></details>\n",
                limits->lineWidthGranularity);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>strictLines                             = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->strictLines);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>standardSampleLocations                 = <span "
                "class='val'>%u</span></summary></details>\n",
                limits->standardSampleLocations);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>optimalBufferCopyOffsetAlignment        = <span class='val'>0x%" PRIxLEAST64
                "</span></summary></details>\n",
                limits->optimalBufferCopyOffsetAlignment);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>optimalBufferCopyRowPitchAlignment      = <span class='val'>0x%" PRIxLEAST64
                "</span></summary></details>\n",
                limits->optimalBufferCopyRowPitchAlignment);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>nonCoherentAtomSize                     = <span class='val'>0x%" PRIxLEAST64
                "</span></summary></details>\n",
                limits->nonCoherentAtomSize);
        fprintf(out, "\t\t\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("\tVkPhysicalDeviceLimits:\n");
        printf("\t-----------------------\n");
        printf("\t\tmaxImageDimension1D                     = %u\n", limits->maxImageDimension1D);
        printf("\t\tmaxImageDimension2D                     = %u\n", limits->maxImageDimension2D);
        printf("\t\tmaxImageDimension3D                     = %u\n", limits->maxImageDimension3D);
        printf("\t\tmaxImageDimensionCube                   = %u\n", limits->maxImageDimensionCube);
        printf("\t\tmaxImageArrayLayers                     = %u\n", limits->maxImageArrayLayers);
        printf("\t\tmaxTexelBufferElements                  = 0x%" PRIxLEAST32 "\n", limits->maxTexelBufferElements);
        printf("\t\tmaxUniformBufferRange                   = 0x%" PRIxLEAST32 "\n", limits->maxUniformBufferRange);
        printf("\t\tmaxStorageBufferRange                   = 0x%" PRIxLEAST32 "\n", limits->maxStorageBufferRange);
        printf("\t\tmaxPushConstantsSize                    = %u\n", limits->maxPushConstantsSize);
        printf("\t\tmaxMemoryAllocationCount                = %u\n", limits->maxMemoryAllocationCount);
        printf("\t\tmaxSamplerAllocationCount               = %u\n", limits->maxSamplerAllocationCount);
        printf("\t\tbufferImageGranularity                  = 0x%" PRIxLEAST64 "\n", limits->bufferImageGranularity);
        printf("\t\tsparseAddressSpaceSize                  = 0x%" PRIxLEAST64 "\n", limits->sparseAddressSpaceSize);
        printf("\t\tmaxBoundDescriptorSets                  = %u\n", limits->maxBoundDescriptorSets);
        printf("\t\tmaxPerStageDescriptorSamplers           = %u\n", limits->maxPerStageDescriptorSamplers);
        printf("\t\tmaxPerStageDescriptorUniformBuffers     = %u\n", limits->maxPerStageDescriptorUniformBuffers);
        printf("\t\tmaxPerStageDescriptorStorageBuffers     = %u\n", limits->maxPerStageDescriptorStorageBuffers);
        printf("\t\tmaxPerStageDescriptorSampledImages      = %u\n", limits->maxPerStageDescriptorSampledImages);
        printf("\t\tmaxPerStageDescriptorStorageImages      = %u\n", limits->maxPerStageDescriptorStorageImages);
        printf("\t\tmaxPerStageDescriptorInputAttachments   = %u\n", limits->maxPerStageDescriptorInputAttachments);
        printf("\t\tmaxPerStageResources                    = %u\n", limits->maxPerStageResources);
        printf("\t\tmaxDescriptorSetSamplers                = %u\n", limits->maxDescriptorSetSamplers);
        printf("\t\tmaxDescriptorSetUniformBuffers          = %u\n", limits->maxDescriptorSetUniformBuffers);
        printf("\t\tmaxDescriptorSetUniformBuffersDynamic   = %u\n", limits->maxDescriptorSetUniformBuffersDynamic);
        printf("\t\tmaxDescriptorSetStorageBuffers          = %u\n", limits->maxDescriptorSetStorageBuffers);
        printf("\t\tmaxDescriptorSetStorageBuffersDynamic   = %u\n", limits->maxDescriptorSetStorageBuffersDynamic);
        printf("\t\tmaxDescriptorSetSampledImages           = %u\n", limits->maxDescriptorSetSampledImages);
        printf("\t\tmaxDescriptorSetStorageImages           = %u\n", limits->maxDescriptorSetStorageImages);
        printf("\t\tmaxDescriptorSetInputAttachments        = %u\n", limits->maxDescriptorSetInputAttachments);
        printf("\t\tmaxVertexInputAttributes                = %u\n", limits->maxVertexInputAttributes);
        printf("\t\tmaxVertexInputBindings                  = %u\n", limits->maxVertexInputBindings);
        printf("\t\tmaxVertexInputAttributeOffset           = 0x%" PRIxLEAST32 "\n", limits->maxVertexInputAttributeOffset);
        printf("\t\tmaxVertexInputBindingStride             = 0x%" PRIxLEAST32 "\n", limits->maxVertexInputBindingStride);
        printf("\t\tmaxVertexOutputComponents               = %u\n", limits->maxVertexOutputComponents);
        printf("\t\tmaxTessellationGenerationLevel          = %u\n", limits->maxTessellationGenerationLevel);
        printf("\t\tmaxTessellationPatchSize                        = %u\n", limits->maxTessellationPatchSize);
        printf("\t\tmaxTessellationControlPerVertexInputComponents  = %u\n",
               limits->maxTessellationControlPerVertexInputComponents);
        printf("\t\tmaxTessellationControlPerVertexOutputComponents = %u\n",
               limits->maxTessellationControlPerVertexOutputComponents);
        printf("\t\tmaxTessellationControlPerPatchOutputComponents  = %u\n",
               limits->maxTessellationControlPerPatchOutputComponents);
        printf("\t\tmaxTessellationControlTotalOutputComponents     = %u\n", limits->maxTessellationControlTotalOutputComponents);
        printf("\t\tmaxTessellationEvaluationInputComponents        = %u\n", limits->maxTessellationEvaluationInputComponents);
        printf("\t\tmaxTessellationEvaluationOutputComponents       = %u\n", limits->maxTessellationEvaluationOutputComponents);
        printf("\t\tmaxGeometryShaderInvocations            = %u\n", limits->maxGeometryShaderInvocations);
        printf("\t\tmaxGeometryInputComponents              = %u\n", limits->maxGeometryInputComponents);
        printf("\t\tmaxGeometryOutputComponents             = %u\n", limits->maxGeometryOutputComponents);
        printf("\t\tmaxGeometryOutputVertices               = %u\n", limits->maxGeometryOutputVertices);
        printf("\t\tmaxGeometryTotalOutputComponents        = %u\n", limits->maxGeometryTotalOutputComponents);
        printf("\t\tmaxFragmentInputComponents              = %u\n", limits->maxFragmentInputComponents);
        printf("\t\tmaxFragmentOutputAttachments            = %u\n", limits->maxFragmentOutputAttachments);
        printf("\t\tmaxFragmentDualSrcAttachments           = %u\n", limits->maxFragmentDualSrcAttachments);
        printf("\t\tmaxFragmentCombinedOutputResources      = %u\n", limits->maxFragmentCombinedOutputResources);
        printf("\t\tmaxComputeSharedMemorySize              = %" PRIuLEAST32 "\n", limits->maxComputeSharedMemorySize);
        printf("\t\tmaxComputeWorkGroupCount[0]             = %u\n", limits->maxComputeWorkGroupCount[0]);
        printf("\t\tmaxComputeWorkGroupCount[1]             = %u\n", limits->maxComputeWorkGroupCount[1]);
        printf("\t\tmaxComputeWorkGroupCount[2]             = %u\n", limits->maxComputeWorkGroupCount[2]);
        printf("\t\tmaxComputeWorkGroupInvocations          = %u\n", limits->maxComputeWorkGroupInvocations);
        printf("\t\tmaxComputeWorkGroupSize[0]              = %u\n", limits->maxComputeWorkGroupSize[0]);
        printf("\t\tmaxComputeWorkGroupSize[1]              = %u\n", limits->maxComputeWorkGroupSize[1]);
        printf("\t\tmaxComputeWorkGroupSize[2]              = %u\n", limits->maxComputeWorkGroupSize[2]);
        printf("\t\tsubPixelPrecisionBits                   = %u\n", limits->subPixelPrecisionBits);
        printf("\t\tsubTexelPrecisionBits                   = %u\n", limits->subTexelPrecisionBits);
        printf("\t\tmipmapPrecisionBits                     = %u\n", limits->mipmapPrecisionBits);
        printf("\t\tmaxDrawIndexedIndexValue                = %u\n", limits->maxDrawIndexedIndexValue);
        printf("\t\tmaxDrawIndirectCount                    = %u\n", limits->maxDrawIndirectCount);
        printf("\t\tmaxSamplerLodBias                       = %f\n", limits->maxSamplerLodBias);
        printf("\t\tmaxSamplerAnisotropy                    = %f\n", limits->maxSamplerAnisotropy);
        printf("\t\tmaxViewports                            = %u\n", limits->maxViewports);
        printf("\t\tmaxViewportDimensions[0]                = %u\n", limits->maxViewportDimensions[0]);
        printf("\t\tmaxViewportDimensions[1]                = %u\n", limits->maxViewportDimensions[1]);
        printf("\t\tviewportBoundsRange[0]                  = %13f\n", limits->viewportBoundsRange[0]);
        printf("\t\tviewportBoundsRange[1]                  = %13f\n", limits->viewportBoundsRange[1]);
        printf("\t\tviewportSubPixelBits                    = %u\n", limits->viewportSubPixelBits);
        printf("\t\tminMemoryMapAlignment                   = " PRINTF_SIZE_T_SPECIFIER "\n", limits->minMemoryMapAlignment);
        printf("\t\tminTexelBufferOffsetAlignment           = 0x%" PRIxLEAST64 "\n", limits->minTexelBufferOffsetAlignment);
        printf("\t\tminUniformBufferOffsetAlignment         = 0x%" PRIxLEAST64 "\n", limits->minUniformBufferOffsetAlignment);
        printf("\t\tminStorageBufferOffsetAlignment         = 0x%" PRIxLEAST64 "\n", limits->minStorageBufferOffsetAlignment);
        printf("\t\tminTexelOffset                          = %3d\n", limits->minTexelOffset);
        printf("\t\tmaxTexelOffset                          = %3d\n", limits->maxTexelOffset);
        printf("\t\tminTexelGatherOffset                    = %3d\n", limits->minTexelGatherOffset);
        printf("\t\tmaxTexelGatherOffset                    = %3d\n", limits->maxTexelGatherOffset);
        printf("\t\tminInterpolationOffset                  = %9f\n", limits->minInterpolationOffset);
        printf("\t\tmaxInterpolationOffset                  = %9f\n", limits->maxInterpolationOffset);
        printf("\t\tsubPixelInterpolationOffsetBits         = %u\n", limits->subPixelInterpolationOffsetBits);
        printf("\t\tmaxFramebufferWidth                     = %u\n", limits->maxFramebufferWidth);
        printf("\t\tmaxFramebufferHeight                    = %u\n", limits->maxFramebufferHeight);
        printf("\t\tmaxFramebufferLayers                    = %u\n", limits->maxFramebufferLayers);
        printf("\t\tframebufferColorSampleCounts            = %u\n", limits->framebufferColorSampleCounts);
        printf("\t\tframebufferDepthSampleCounts            = %u\n", limits->framebufferDepthSampleCounts);
        printf("\t\tframebufferStencilSampleCounts          = %u\n", limits->framebufferStencilSampleCounts);
        printf("\t\tframebufferNoAttachmentsSampleCounts    = %u\n", limits->framebufferNoAttachmentsSampleCounts);
        printf("\t\tmaxColorAttachments                     = %u\n", limits->maxColorAttachments);
        printf("\t\tsampledImageColorSampleCounts           = %u\n", limits->sampledImageColorSampleCounts);
        printf("\t\tsampledImageDepthSampleCounts           = %u\n", limits->sampledImageDepthSampleCounts);
        printf("\t\tsampledImageStencilSampleCounts         = %u\n", limits->sampledImageStencilSampleCounts);
        printf("\t\tsampledImageIntegerSampleCounts         = %u\n", limits->sampledImageIntegerSampleCounts);
        printf("\t\tstorageImageSampleCounts                = %u\n", limits->storageImageSampleCounts);
        printf("\t\tmaxSampleMaskWords                      = %u\n", limits->maxSampleMaskWords);
        printf("\t\ttimestampComputeAndGraphics             = %u\n", limits->timestampComputeAndGraphics);
        printf("\t\ttimestampPeriod                         = %f\n", limits->timestampPeriod);
        printf("\t\tmaxClipDistances                        = %u\n", limits->maxClipDistances);
        printf("\t\tmaxCullDistances                        = %u\n", limits->maxCullDistances);
        printf("\t\tmaxCombinedClipAndCullDistances         = %u\n", limits->maxCombinedClipAndCullDistances);
        printf("\t\tdiscreteQueuePriorities                 = %u\n", limits->discreteQueuePriorities);
        printf("\t\tpointSizeRange[0]                       = %f\n", limits->pointSizeRange[0]);
        printf("\t\tpointSizeRange[1]                       = %f\n", limits->pointSizeRange[1]);
        printf("\t\tlineWidthRange[0]                       = %f\n", limits->lineWidthRange[0]);
        printf("\t\tlineWidthRange[1]                       = %f\n", limits->lineWidthRange[1]);
        printf("\t\tpointSizeGranularity                    = %f\n", limits->pointSizeGranularity);
        printf("\t\tlineWidthGranularity                    = %f\n", limits->lineWidthGranularity);
        printf("\t\tstrictLines                             = %u\n", limits->strictLines);
        printf("\t\tstandardSampleLocations                 = %u\n", limits->standardSampleLocations);
        printf("\t\toptimalBufferCopyOffsetAlignment        = 0x%" PRIxLEAST64 "\n", limits->optimalBufferCopyOffsetAlignment);
        printf("\t\toptimalBufferCopyRowPitchAlignment      = 0x%" PRIxLEAST64 "\n", limits->optimalBufferCopyRowPitchAlignment);
        printf("\t\tnonCoherentAtomSize                     = 0x%" PRIxLEAST64 "\n", limits->nonCoherentAtomSize);
    }
    if (json_output) {
        printf(",\n");
        printf("\t\t\"limits\": {\n");
        printf("\t\t\t\"maxImageDimension1D\": %u,\n", limits->maxImageDimension1D);
        printf("\t\t\t\"maxImageDimension2D\": %u,\n", limits->maxImageDimension2D);
        printf("\t\t\t\"maxImageDimension3D\": %u,\n", limits->maxImageDimension3D);
        printf("\t\t\t\"maxImageDimensionCube\": %u,\n", limits->maxImageDimensionCube);
        printf("\t\t\t\"maxImageArrayLayers\": %u,\n", limits->maxImageArrayLayers);
        printf("\t\t\t\"maxTexelBufferElements\": %u,\n", limits->maxTexelBufferElements);
        printf("\t\t\t\"maxUniformBufferRange\": %u,\n", limits->maxUniformBufferRange);
        printf("\t\t\t\"maxStorageBufferRange\": %u,\n", limits->maxStorageBufferRange);
        printf("\t\t\t\"maxPushConstantsSize\": %u,\n", limits->maxPushConstantsSize);
        printf("\t\t\t\"maxMemoryAllocationCount\": %u,\n", limits->maxMemoryAllocationCount);
        printf("\t\t\t\"maxSamplerAllocationCount\": %u,\n", limits->maxSamplerAllocationCount);
        printf("\t\t\t\"bufferImageGranularity\": %llu,\n", (unsigned long long)limits->bufferImageGranularity);
        printf("\t\t\t\"sparseAddressSpaceSize\": %llu,\n", (unsigned long long)limits->sparseAddressSpaceSize);
        printf("\t\t\t\"maxBoundDescriptorSets\": %u,\n", limits->maxBoundDescriptorSets);
        printf("\t\t\t\"maxPerStageDescriptorSamplers\": %u,\n", limits->maxPerStageDescriptorSamplers);
        printf("\t\t\t\"maxPerStageDescriptorUniformBuffers\": %u,\n", limits->maxPerStageDescriptorUniformBuffers);
        printf("\t\t\t\"maxPerStageDescriptorStorageBuffers\": %u,\n", limits->maxPerStageDescriptorStorageBuffers);
        printf("\t\t\t\"maxPerStageDescriptorSampledImages\": %u,\n", limits->maxPerStageDescriptorSampledImages);
        printf("\t\t\t\"maxPerStageDescriptorStorageImages\": %u,\n", limits->maxPerStageDescriptorStorageImages);
        printf("\t\t\t\"maxPerStageDescriptorInputAttachments\": %u,\n", limits->maxPerStageDescriptorInputAttachments);
        printf("\t\t\t\"maxPerStageResources\": %u,\n", limits->maxPerStageResources);
        printf("\t\t\t\"maxDescriptorSetSamplers\": %u,\n", limits->maxDescriptorSetSamplers);
        printf("\t\t\t\"maxDescriptorSetUniformBuffers\": %u,\n", limits->maxDescriptorSetUniformBuffers);
        printf("\t\t\t\"maxDescriptorSetUniformBuffersDynamic\": %u,\n", limits->maxDescriptorSetUniformBuffersDynamic);
        printf("\t\t\t\"maxDescriptorSetStorageBuffers\": %u,\n", limits->maxDescriptorSetStorageBuffers);
        printf("\t\t\t\"maxDescriptorSetStorageBuffersDynamic\": %u,\n", limits->maxDescriptorSetStorageBuffersDynamic);
        printf("\t\t\t\"maxDescriptorSetSampledImages\": %u,\n", limits->maxDescriptorSetSampledImages);
        printf("\t\t\t\"maxDescriptorSetStorageImages\": %u,\n", limits->maxDescriptorSetStorageImages);
        printf("\t\t\t\"maxDescriptorSetInputAttachments\": %u,\n", limits->maxDescriptorSetInputAttachments);
        printf("\t\t\t\"maxVertexInputAttributes\": %u,\n", limits->maxVertexInputAttributes);
        printf("\t\t\t\"maxVertexInputBindings\": %u,\n", limits->maxVertexInputBindings);
        printf("\t\t\t\"maxVertexInputAttributeOffset\": %u,\n", limits->maxVertexInputAttributeOffset);
        printf("\t\t\t\"maxVertexInputBindingStride\": %u,\n", limits->maxVertexInputBindingStride);
        printf("\t\t\t\"maxVertexOutputComponents\": %u,\n", limits->maxVertexOutputComponents);
        printf("\t\t\t\"maxTessellationGenerationLevel\": %u,\n", limits->maxTessellationGenerationLevel);
        printf("\t\t\t\"maxTessellationPatchSize\": %u,\n", limits->maxTessellationPatchSize);
        printf("\t\t\t\"maxTessellationControlPerVertexInputComponents\": %u,\n",
               limits->maxTessellationControlPerVertexInputComponents);
        printf("\t\t\t\"maxTessellationControlPerVertexOutputComponents\": %u,\n",
               limits->maxTessellationControlPerVertexOutputComponents);
        printf("\t\t\t\"maxTessellationControlPerPatchOutputComponents\": %u,\n",
               limits->maxTessellationControlPerPatchOutputComponents);
        printf("\t\t\t\"maxTessellationControlTotalOutputComponents\": %u,\n", limits->maxTessellationControlTotalOutputComponents);
        printf("\t\t\t\"maxTessellationEvaluationInputComponents\": %u,\n", limits->maxTessellationEvaluationInputComponents);
        printf("\t\t\t\"maxTessellationEvaluationOutputComponents\": %u,\n", limits->maxTessellationEvaluationOutputComponents);
        printf("\t\t\t\"maxGeometryShaderInvocations\": %u,\n", limits->maxGeometryShaderInvocations);
        printf("\t\t\t\"maxGeometryInputComponents\": %u,\n", limits->maxGeometryInputComponents);
        printf("\t\t\t\"maxGeometryOutputComponents\": %u,\n", limits->maxGeometryOutputComponents);
        printf("\t\t\t\"maxGeometryOutputVertices\": %u,\n", limits->maxGeometryOutputVertices);
        printf("\t\t\t\"maxGeometryTotalOutputComponents\": %u,\n", limits->maxGeometryTotalOutputComponents);
        printf("\t\t\t\"maxFragmentInputComponents\": %u,\n", limits->maxFragmentInputComponents);
        printf("\t\t\t\"maxFragmentOutputAttachments\": %u,\n", limits->maxFragmentOutputAttachments);
        printf("\t\t\t\"maxFragmentDualSrcAttachments\": %u,\n", limits->maxFragmentDualSrcAttachments);
        printf("\t\t\t\"maxFragmentCombinedOutputResources\": %u,\n", limits->maxFragmentCombinedOutputResources);
        printf("\t\t\t\"maxComputeSharedMemorySize\": %u,\n", limits->maxComputeSharedMemorySize);
        printf("\t\t\t\"maxComputeWorkGroupCount\": [\n");
        printf("\t\t\t\t%u,\n", limits->maxComputeWorkGroupCount[0]);
        printf("\t\t\t\t%u,\n", limits->maxComputeWorkGroupCount[1]);
        printf("\t\t\t\t%u\n", limits->maxComputeWorkGroupCount[2]);
        printf("\t\t\t],\n");
        printf("\t\t\t\"maxComputeWorkGroupInvocations\": %u,\n", limits->maxComputeWorkGroupInvocations);
        printf("\t\t\t\"maxComputeWorkGroupSize\": [\n");
        printf("\t\t\t\t%u,\n", limits->maxComputeWorkGroupSize[0]);
        printf("\t\t\t\t%u,\n", limits->maxComputeWorkGroupSize[1]);
        printf("\t\t\t\t%u\n", limits->maxComputeWorkGroupSize[2]);
        printf("\t\t\t],\n");
        printf("\t\t\t\"subPixelPrecisionBits\": %u,\n", limits->subPixelPrecisionBits);
        printf("\t\t\t\"subTexelPrecisionBits\": %u,\n", limits->subTexelPrecisionBits);
        printf("\t\t\t\"mipmapPrecisionBits\": %u,\n", limits->mipmapPrecisionBits);
        printf("\t\t\t\"maxDrawIndexedIndexValue\": %u,\n", limits->maxDrawIndexedIndexValue);
        printf("\t\t\t\"maxDrawIndirectCount\": %u,\n", limits->maxDrawIndirectCount);
        printf("\t\t\t\"maxSamplerLodBias\": %g,\n", limits->maxSamplerLodBias);
        printf("\t\t\t\"maxSamplerAnisotropy\": %g,\n", limits->maxSamplerAnisotropy);
        printf("\t\t\t\"maxViewports\": %u,\n", limits->maxViewports);
        printf("\t\t\t\"maxViewportDimensions\": [\n");
        printf("\t\t\t\t%u,\n", limits->maxViewportDimensions[0]);
        printf("\t\t\t\t%u\n", limits->maxViewportDimensions[1]);
        printf("\t\t\t],\n");
        printf("\t\t\t\"viewportBoundsRange\": [\n");
        printf("\t\t\t\t%g,\n", limits->viewportBoundsRange[0]);
        printf("\t\t\t\t%g\n", limits->viewportBoundsRange[1]);
        printf("\t\t\t],\n");
        printf("\t\t\t\"viewportSubPixelBits\": %u,\n", limits->viewportSubPixelBits);
        printf("\t\t\t\"minMemoryMapAlignment\": " PRINTF_SIZE_T_SPECIFIER ",\n", limits->minMemoryMapAlignment);
        printf("\t\t\t\"minTexelBufferOffsetAlignment\": %llu,\n", (unsigned long long)limits->minTexelBufferOffsetAlignment);
        printf("\t\t\t\"minUniformBufferOffsetAlignment\": %llu,\n", (unsigned long long)limits->minUniformBufferOffsetAlignment);
        printf("\t\t\t\"minStorageBufferOffsetAlignment\": %llu,\n", (unsigned long long)limits->minStorageBufferOffsetAlignment);
        printf("\t\t\t\"minTexelOffset\": %d,\n", limits->minTexelOffset);
        printf("\t\t\t\"maxTexelOffset\": %u,\n", limits->maxTexelOffset);
        printf("\t\t\t\"minTexelGatherOffset\": %d,\n", limits->minTexelGatherOffset);
        printf("\t\t\t\"maxTexelGatherOffset\": %u,\n", limits->maxTexelGatherOffset);
        printf("\t\t\t\"minInterpolationOffset\": %g,\n", limits->minInterpolationOffset);
        printf("\t\t\t\"maxInterpolationOffset\": %g,\n", limits->maxInterpolationOffset);
        printf("\t\t\t\"subPixelInterpolationOffsetBits\": %u,\n", limits->subPixelInterpolationOffsetBits);
        printf("\t\t\t\"maxFramebufferWidth\": %u,\n", limits->maxFramebufferWidth);
        printf("\t\t\t\"maxFramebufferHeight\": %u,\n", limits->maxFramebufferHeight);
        printf("\t\t\t\"maxFramebufferLayers\": %u,\n", limits->maxFramebufferLayers);
        printf("\t\t\t\"framebufferColorSampleCounts\": %u,\n", limits->framebufferColorSampleCounts);
        printf("\t\t\t\"framebufferDepthSampleCounts\": %u,\n", limits->framebufferDepthSampleCounts);
        printf("\t\t\t\"framebufferStencilSampleCounts\": %u,\n", limits->framebufferStencilSampleCounts);
        printf("\t\t\t\"framebufferNoAttachmentsSampleCounts\": %u,\n", limits->framebufferNoAttachmentsSampleCounts);
        printf("\t\t\t\"maxColorAttachments\": %u,\n", limits->maxColorAttachments);
        printf("\t\t\t\"sampledImageColorSampleCounts\": %u,\n", limits->sampledImageColorSampleCounts);
        printf("\t\t\t\"sampledImageIntegerSampleCounts\": %u,\n", limits->sampledImageIntegerSampleCounts);
        printf("\t\t\t\"sampledImageDepthSampleCounts\": %u,\n", limits->sampledImageDepthSampleCounts);
        printf("\t\t\t\"sampledImageStencilSampleCounts\": %u,\n", limits->sampledImageStencilSampleCounts);
        printf("\t\t\t\"storageImageSampleCounts\": %u,\n", limits->storageImageSampleCounts);
        printf("\t\t\t\"maxSampleMaskWords\": %u,\n", limits->maxSampleMaskWords);
        printf("\t\t\t\"timestampComputeAndGraphics\": %u,\n", limits->timestampComputeAndGraphics);
        printf("\t\t\t\"timestampPeriod\": %g,\n", limits->timestampPeriod);
        printf("\t\t\t\"maxClipDistances\": %u,\n", limits->maxClipDistances);
        printf("\t\t\t\"maxCullDistances\": %u,\n", limits->maxCullDistances);
        printf("\t\t\t\"maxCombinedClipAndCullDistances\": %u,\n", limits->maxCombinedClipAndCullDistances);
        printf("\t\t\t\"discreteQueuePriorities\": %u,\n", limits->discreteQueuePriorities);
        printf("\t\t\t\"pointSizeRange\": [\n");
        printf("\t\t\t\t%g,\n", limits->pointSizeRange[0]);
        printf("\t\t\t\t%g\n", limits->pointSizeRange[1]);
        printf("\t\t\t],\n");
        printf("\t\t\t\"lineWidthRange\": [\n");
        printf("\t\t\t\t%g,\n", limits->lineWidthRange[0]);
        printf("\t\t\t\t%g\n", limits->lineWidthRange[1]);
        printf("\t\t\t],\n");
        printf("\t\t\t\"pointSizeGranularity\": %g,\n", limits->pointSizeGranularity);
        printf("\t\t\t\"lineWidthGranularity\": %g,\n", limits->lineWidthGranularity);
        printf("\t\t\t\"strictLines\": %u,\n", limits->strictLines);
        printf("\t\t\t\"standardSampleLocations\": %u,\n", limits->standardSampleLocations);
        printf("\t\t\t\"optimalBufferCopyOffsetAlignment\": %llu,\n", (unsigned long long)limits->optimalBufferCopyOffsetAlignment);
        printf("\t\t\t\"optimalBufferCopyRowPitchAlignment\": %llu,\n",
               (unsigned long long)limits->optimalBufferCopyRowPitchAlignment);
        printf("\t\t\t\"nonCoherentAtomSize\": %llu\n", (unsigned long long)limits->nonCoherentAtomSize);
        printf("\t\t}");
    }
}

static void AppGpuDumpProps(const struct AppGpu *gpu, FILE *out) {
    VkPhysicalDeviceProperties props;

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        const VkPhysicalDeviceProperties *props2_const = &gpu->props2.properties;
        props = *props2_const;
    } else {
        const VkPhysicalDeviceProperties *props_const = &gpu->props;
        props = *props_const;
    }
    const uint32_t apiVersion = props.apiVersion;
    const uint32_t major = VK_VERSION_MAJOR(apiVersion);
    const uint32_t minor = VK_VERSION_MINOR(apiVersion);
    const uint32_t patch = VK_VERSION_PATCH(apiVersion);

    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceProperties</summary>\n");
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>apiVersion = <span class='val'>0x%" PRIxLEAST32
                "</span>  (<span class='val'>%d.%d.%d</span>)</summary></details>\n",
                apiVersion, major, minor, patch);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>driverVersion = <span class='val'>%u</span> (<span class='val'>0x%" PRIxLEAST32
                "</span>)</summary></details>\n",
                props.driverVersion, props.driverVersion);
        fprintf(out, "\t\t\t\t\t\t<details><summary>vendorID = <span class='val'>0x%04x</span></summary></details>\n",
                props.vendorID);
        fprintf(out, "\t\t\t\t\t\t<details><summary>deviceID = <span class='val'>0x%04x</span></summary></details>\n",
                props.deviceID);
        fprintf(out, "\t\t\t\t\t\t<details><summary>deviceType = %s</summary></details>\n",
                VkPhysicalDeviceTypeString(props.deviceType));
        fprintf(out, "\t\t\t\t\t\t<details><summary>deviceName = %s</summary></details>\n", props.deviceName);
        fprintf(out, "\t\t\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("VkPhysicalDeviceProperties:\n");
        printf("===========================\n");
        printf("\tapiVersion     = 0x%" PRIxLEAST32 "  (%d.%d.%d)\n", apiVersion, major, minor, patch);
        printf("\tdriverVersion  = %u (0x%" PRIxLEAST32 ")\n", props.driverVersion, props.driverVersion);
        printf("\tvendorID       = 0x%04x\n", props.vendorID);
        printf("\tdeviceID       = 0x%04x\n", props.deviceID);
        printf("\tdeviceType     = %s\n", VkPhysicalDeviceTypeString(props.deviceType));
        printf("\tdeviceName     = %s\n", props.deviceName);
    }
    if (json_output) {
        printf(",\n");
        printf("\t\"VkPhysicalDeviceProperties\": {\n");
        printf("\t\t\"apiVersion\": %u,\n", apiVersion);
        printf("\t\t\"driverVersion\": %u,\n", props.driverVersion);
        printf("\t\t\"vendorID\": %u,\n", props.vendorID);
        printf("\t\t\"deviceID\": %u,\n", props.deviceID);
        printf("\t\t\"deviceType\": %u,\n", props.deviceType);
        printf("\t\t\"deviceName\": \"%s\",\n", props.deviceName);
        printf("\t\t\"pipelineCacheUUID\": [");
        for (uint32_t i = 0; i < VK_UUID_SIZE; ++i) {
            if (i > 0) {
                printf(",");
            }
            printf("\n");
            printf("\t\t\t%u", props.pipelineCacheUUID[i]);
        }
        printf("\n");
        printf("\t\t]");
    }

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        AppDumpLimits(&gpu->props2.properties.limits, out);
    } else {
        AppDumpLimits(&gpu->props.limits, out);
    }

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        AppDumpSparseProps(&gpu->props2.properties.sparseProperties, out);
    } else {
        AppDumpSparseProps(&gpu->props.sparseProperties, out);
    }

    if (json_output) {
        printf("\n\t}");
    }

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        void *place = gpu->props2.pNext;
        while (place) {
            struct VkStructureHeader *structure = (struct VkStructureHeader *)place;
            if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT &&
                CheckPhysicalDeviceExtensionIncluded(VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME, gpu->device_extensions,
                                                     gpu->device_extension_count)) {
                VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *blend_op_adv_props =
                    (VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceBlendOperationAdvancedProperties</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>advancedBlendMaxColorAttachments                = <span "
                            "class='val'>%u</span></summary></details>\n",
                            blend_op_adv_props->advancedBlendMaxColorAttachments);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>advancedBlendIndependentBlend                   = <span "
                            "class='val'>%u</span></summary></details>\n",
                            blend_op_adv_props->advancedBlendIndependentBlend);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>advancedBlendNonPremultipliedSrcColor           = <span "
                            "class='val'>%u</span></summary></details>\n",
                            blend_op_adv_props->advancedBlendNonPremultipliedSrcColor);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>advancedBlendNonPremultipliedDstColor           = <span "
                            "class='val'>%u</span></summary></details>\n",
                            blend_op_adv_props->advancedBlendNonPremultipliedDstColor);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>advancedBlendCorrelatedOverlap                  = <span "
                            "class='val'>%u</span></summary></details>\n",
                            blend_op_adv_props->advancedBlendCorrelatedOverlap);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>advancedBlendAllOperations                      = <span "
                            "class='val'>%u</span></summary></details>\n",
                            blend_op_adv_props->advancedBlendAllOperations);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceBlendOperationAdvancedProperties:\n");
                    printf("=================================================\n");
                    printf("\tadvancedBlendMaxColorAttachments               = %u\n",
                           blend_op_adv_props->advancedBlendMaxColorAttachments);
                    printf("\tadvancedBlendIndependentBlend                  = %u\n",
                           blend_op_adv_props->advancedBlendIndependentBlend);
                    printf("\tadvancedBlendNonPremultipliedSrcColor          = %u\n",
                           blend_op_adv_props->advancedBlendNonPremultipliedSrcColor);
                    printf("\tadvancedBlendNonPremultipliedDstColor          = %u\n",
                           blend_op_adv_props->advancedBlendNonPremultipliedDstColor);
                    printf("\tadvancedBlendCorrelatedOverlap                 = %u\n",
                           blend_op_adv_props->advancedBlendCorrelatedOverlap);
                    printf("\tadvancedBlendAllOperations                     = %u\n",
                           blend_op_adv_props->advancedBlendAllOperations);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_MAINTENANCE2_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDevicePointClippingPropertiesKHR *pt_clip_props = (VkPhysicalDevicePointClippingPropertiesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDevicePointClippingProperties</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>pointClippingBehavior               = <span "
                            "class='val'>%u</span></summary></details>\n",
                            pt_clip_props->pointClippingBehavior);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDevicePointClippingProperties:\n");
                    printf("========================================\n");
                    printf("\tpointClippingBehavior               = %u\n", pt_clip_props->pointClippingBehavior);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDevicePushDescriptorPropertiesKHR *push_desc_props =
                    (VkPhysicalDevicePushDescriptorPropertiesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDevicePushDescriptorProperties</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>maxPushDescriptors                = <span "
                            "class='val'>%u</span></summary></details>\n",
                            push_desc_props->maxPushDescriptors);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDevicePushDescriptorProperties:\n");
                    printf("=========================================\n");
                    printf("\tmaxPushDescriptors               = %u\n", push_desc_props->maxPushDescriptors);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceDiscardRectanglePropertiesEXT *discard_rect_props =
                    (VkPhysicalDeviceDiscardRectanglePropertiesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceDiscardRectangleProperties</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>maxDiscardRectangles               = <span "
                            "class='val'>%u</span></summary></details>\n",
                            discard_rect_props->maxDiscardRectangles);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceDiscardRectangleProperties:\n");
                    printf("===========================================\n");
                    printf("\tmaxDiscardRectangles               = %u\n", discard_rect_props->maxDiscardRectangles);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_MULTIVIEW_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceMultiviewPropertiesKHR *multiview_props = (VkPhysicalDeviceMultiviewPropertiesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceMultiviewProperties</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>maxMultiviewViewCount     = <span "
                            "class='val'>%u</span></summary></details>\n",
                            multiview_props->maxMultiviewViewCount);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>maxMultiviewInstanceIndex = <span "
                            "class='val'>%u</span></summary></details>\n",
                            multiview_props->maxMultiviewInstanceIndex);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceMultiviewProperties:\n");
                    printf("====================================\n");
                    printf("\tmaxMultiviewViewCount     = %u\n", multiview_props->maxMultiviewViewCount);
                    printf("\tmaxMultiviewInstanceIndex = %u\n", multiview_props->maxMultiviewInstanceIndex);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR) {
                VkPhysicalDeviceMaintenance3PropertiesKHR *maintenance3_props =
                    (VkPhysicalDeviceMaintenance3PropertiesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceMaintenance3Properties</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>maxPerSetDescriptors    = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            maintenance3_props->maxPerSetDescriptors);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>maxMemoryAllocationSize = <span class='val'>%" PRIuLEAST64
                            "</span></summary></details>\n",
                            maintenance3_props->maxMemoryAllocationSize);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceMaintenance3Properties:\n");
                    printf("=======================================\n");
                    printf("\tmaxPerSetDescriptors    = %" PRIuLEAST32 "\n", maintenance3_props->maxPerSetDescriptors);
                    printf("\tmaxMemoryAllocationSize = %" PRIuLEAST64 "\n", maintenance3_props->maxMemoryAllocationSize);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES_KHR) {
                const VkPhysicalDeviceIDPropertiesKHR *id_props = (VkPhysicalDeviceIDPropertiesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceIDProperties</summary>\n");
                    // Visual Studio 2013's printf does not support the "hh"
                    // length modifier so cast the operands and use field width
                    // "2" to fake it.
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>deviceUUID      = <span "
                            "class='val'>%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x</span></summary></"
                            "details>\n",
                            (uint32_t)id_props->deviceUUID[0], (uint32_t)id_props->deviceUUID[1], (uint32_t)id_props->deviceUUID[2],
                            (uint32_t)id_props->deviceUUID[3], (uint32_t)id_props->deviceUUID[4], (uint32_t)id_props->deviceUUID[5],
                            (uint32_t)id_props->deviceUUID[6], (uint32_t)id_props->deviceUUID[7], (uint32_t)id_props->deviceUUID[8],
                            (uint32_t)id_props->deviceUUID[9], (uint32_t)id_props->deviceUUID[10],
                            (uint32_t)id_props->deviceUUID[11], (uint32_t)id_props->deviceUUID[12],
                            (uint32_t)id_props->deviceUUID[13], (uint32_t)id_props->deviceUUID[14],
                            (uint32_t)id_props->deviceUUID[15]);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>driverUUID      = <span "
                            "class='val'>%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x</span></summary></"
                            "details>\n",
                            (uint32_t)id_props->driverUUID[0], (uint32_t)id_props->driverUUID[1], (uint32_t)id_props->driverUUID[2],
                            (uint32_t)id_props->driverUUID[3], (uint32_t)id_props->driverUUID[4], (uint32_t)id_props->driverUUID[5],
                            (uint32_t)id_props->driverUUID[6], (uint32_t)id_props->driverUUID[7], (uint32_t)id_props->driverUUID[8],
                            (uint32_t)id_props->driverUUID[9], (uint32_t)id_props->driverUUID[10],
                            (uint32_t)id_props->driverUUID[11], (uint32_t)id_props->driverUUID[12],
                            (uint32_t)id_props->driverUUID[13], (uint32_t)id_props->driverUUID[14],
                            (uint32_t)id_props->driverUUID[15]);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>deviceLUIDValid = <span class='val'>%s</span></summary></details>\n",
                            id_props->deviceLUIDValid ? "true" : "false");
                    if (id_props->deviceLUIDValid) {
                        fprintf(out,
                                "\t\t\t\t\t\t<details><summary>deviceLUID      = <span "
                                "class='val'>%02x%02x%02x%02x-%02x%02x%02x%02x</span></summary></details>\n",
                                (uint32_t)id_props->deviceLUID[0], (uint32_t)id_props->deviceLUID[1],
                                (uint32_t)id_props->deviceLUID[2], (uint32_t)id_props->deviceLUID[3],
                                (uint32_t)id_props->deviceLUID[4], (uint32_t)id_props->deviceLUID[5],
                                (uint32_t)id_props->deviceLUID[6], (uint32_t)id_props->deviceLUID[7]);
                        fprintf(out,
                                "\t\t\t\t\t\t<details><summary>deviceNodeMask  = <span "
                                "class='val'>0x%08x</span></summary></details>\n",
                                id_props->deviceNodeMask);
                    }
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceIDProperties:\n");
                    printf("=========================================\n");
                    printf("\tdeviceUUID      = %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
                           (uint32_t)id_props->deviceUUID[0], (uint32_t)id_props->deviceUUID[1], (uint32_t)id_props->deviceUUID[2],
                           (uint32_t)id_props->deviceUUID[3], (uint32_t)id_props->deviceUUID[4], (uint32_t)id_props->deviceUUID[5],
                           (uint32_t)id_props->deviceUUID[6], (uint32_t)id_props->deviceUUID[7], (uint32_t)id_props->deviceUUID[8],
                           (uint32_t)id_props->deviceUUID[9], (uint32_t)id_props->deviceUUID[10],
                           (uint32_t)id_props->deviceUUID[11], (uint32_t)id_props->deviceUUID[12],
                           (uint32_t)id_props->deviceUUID[13], (uint32_t)id_props->deviceUUID[14],
                           (uint32_t)id_props->deviceUUID[15]);
                    printf("\tdriverUUID      = %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
                           (uint32_t)id_props->driverUUID[0], (uint32_t)id_props->driverUUID[1], (uint32_t)id_props->driverUUID[2],
                           (uint32_t)id_props->driverUUID[3], (uint32_t)id_props->driverUUID[4], (uint32_t)id_props->driverUUID[5],
                           (uint32_t)id_props->driverUUID[6], (uint32_t)id_props->driverUUID[7], (uint32_t)id_props->driverUUID[8],
                           (uint32_t)id_props->driverUUID[9], (uint32_t)id_props->driverUUID[10],
                           (uint32_t)id_props->driverUUID[11], (uint32_t)id_props->driverUUID[12],
                           (uint32_t)id_props->driverUUID[13], (uint32_t)id_props->driverUUID[14],
                           (uint32_t)id_props->driverUUID[15]);
                    printf("\tdeviceLUIDValid = %s\n", id_props->deviceLUIDValid ? "true" : "false");
                    if (id_props->deviceLUIDValid) {
                        printf("\tdeviceLUID      = %02x%02x%02x%02x-%02x%02x%02x%02x\n", (uint32_t)id_props->deviceLUID[0],
                               (uint32_t)id_props->deviceLUID[1], (uint32_t)id_props->deviceLUID[2],
                               (uint32_t)id_props->deviceLUID[3], (uint32_t)id_props->deviceLUID[4],
                               (uint32_t)id_props->deviceLUID[5], (uint32_t)id_props->deviceLUID[6],
                               (uint32_t)id_props->deviceLUID[7]);
                        printf("\tdeviceNodeMask  = 0x%08x\n", id_props->deviceNodeMask);
                    }
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceDriverPropertiesKHR *driver_props = (VkPhysicalDeviceDriverPropertiesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceDriverProperties</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>driverID   = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            driver_props->driverID);
                    fprintf(out, "\t\t\t\t\t\t<details><summary>driverName = %s</summary></details>\n", driver_props->driverName);
                    fprintf(out, "\t\t\t\t\t\t<details><summary>driverInfo = %s</summary></details>\n", driver_props->driverInfo);
                    fprintf(out, "\t\t\t\t\t\t<details><summary>conformanceVersion:</summary></details>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>major    = <span class='val'>%" PRIuLEAST8
                            "</span></summary></details>\n",
                            driver_props->conformanceVersion.major);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>minor    = <span class='val'>%" PRIuLEAST8
                            "</span></summary></details>\n",
                            driver_props->conformanceVersion.minor);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>subminor = <span class='val'>%" PRIuLEAST8
                            "</span></summary></details>\n",
                            driver_props->conformanceVersion.subminor);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>patch    = <span class='val'>%" PRIuLEAST8
                            "</span></summary></details>\n",
                            driver_props->conformanceVersion.patch);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceDriverProperties:\n");
                    printf("=================================\n");
                    printf("\tdriverID   = %" PRIuLEAST32 "\n", driver_props->driverID);
                    printf("\tdriverName = %s\n", driver_props->driverName);
                    printf("\tdriverInfo = %s\n", driver_props->driverInfo);
                    printf("\tconformanceVersion:\n");
                    printf("\t\tmajor    = %" PRIuLEAST8 "\n", driver_props->conformanceVersion.major);
                    printf("\t\tminor    = %" PRIuLEAST8 "\n", driver_props->conformanceVersion.minor);
                    printf("\t\tsubminor = %" PRIuLEAST8 "\n", driver_props->conformanceVersion.subminor);
                    printf("\t\tpatch    = %" PRIuLEAST8 "\n", driver_props->conformanceVersion.patch);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceFloatControlsPropertiesKHR *float_control_props =
                    (VkPhysicalDeviceFloatControlsPropertiesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceFloatControlsProperties</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>denormBehaviorIndependence            = <span "
                            "class='val'>%s</span></summary></details>\n",
                            VkShaderFloatControlsIndependenceString(float_control_props->denormBehaviorIndependence));
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>roundingModeIndependence              = <span "
                            "class='val'>%s</span></summary></details>\n",
                            VkShaderFloatControlsIndependenceString(float_control_props->roundingModeIndependence));
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderSignedZeroInfNanPreserveFloat16 = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderSignedZeroInfNanPreserveFloat16);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderSignedZeroInfNanPreserveFloat32 = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderSignedZeroInfNanPreserveFloat32);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderSignedZeroInfNanPreserveFloat64 = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderSignedZeroInfNanPreserveFloat64);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderDenormPreserveFloat16           = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderDenormPreserveFloat16);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderDenormPreserveFloat32           = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderDenormPreserveFloat32);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderDenormPreserveFloat64           = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderDenormPreserveFloat64);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderDenormFlushToZeroFloat16        = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderDenormFlushToZeroFloat16);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderDenormFlushToZeroFloat32        = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderDenormFlushToZeroFloat32);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderDenormFlushToZeroFloat64        = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderDenormFlushToZeroFloat64);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderRoundingModeRTEFloat16          = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderRoundingModeRTEFloat16);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderRoundingModeRTEFloat32          = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderRoundingModeRTEFloat32);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderRoundingModeRTEFloat64          = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderRoundingModeRTEFloat64);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderRoundingModeRTZFloat16          = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderRoundingModeRTZFloat16);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderRoundingModeRTZFloat32          = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderRoundingModeRTZFloat32);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>shaderRoundingModeRTZFloat64          = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            float_control_props->shaderRoundingModeRTZFloat64);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceFloatControlsProperties:\n");
                    printf("========================================\n");
                    printf("\tdenormBehaviorIndependence            = %s\n",
                           VkShaderFloatControlsIndependenceString(float_control_props->denormBehaviorIndependence));
                    printf("\troundingModeIndependence              = %s\n",
                           VkShaderFloatControlsIndependenceString(float_control_props->roundingModeIndependence));
                    printf("\tshaderSignedZeroInfNanPreserveFloat16 = %" PRIuLEAST32 "\n",
                           float_control_props->shaderSignedZeroInfNanPreserveFloat16);
                    printf("\tshaderSignedZeroInfNanPreserveFloat32 = %" PRIuLEAST32 "\n",
                           float_control_props->shaderSignedZeroInfNanPreserveFloat32);
                    printf("\tshaderSignedZeroInfNanPreserveFloat64 = %" PRIuLEAST32 "\n",
                           float_control_props->shaderSignedZeroInfNanPreserveFloat64);
                    printf("\tshaderDenormPreserveFloat16           = %" PRIuLEAST32 "\n",
                           float_control_props->shaderDenormPreserveFloat16);
                    printf("\tshaderDenormPreserveFloat32           = %" PRIuLEAST32 "\n",
                           float_control_props->shaderDenormPreserveFloat32);
                    printf("\tshaderDenormPreserveFloat64           = %" PRIuLEAST32 "\n",
                           float_control_props->shaderDenormPreserveFloat64);
                    printf("\tshaderDenormFlushToZeroFloat16        = %" PRIuLEAST32 "\n",
                           float_control_props->shaderDenormFlushToZeroFloat16);
                    printf("\tshaderDenormFlushToZeroFloat32        = %" PRIuLEAST32 "\n",
                           float_control_props->shaderDenormFlushToZeroFloat32);
                    printf("\tshaderDenormFlushToZeroFloat64        = %" PRIuLEAST32 "\n",
                           float_control_props->shaderDenormFlushToZeroFloat64);
                    printf("\tshaderRoundingModeRTEFloat16          = %" PRIuLEAST32 "\n",
                           float_control_props->shaderRoundingModeRTEFloat16);
                    printf("\tshaderRoundingModeRTEFloat32          = %" PRIuLEAST32 "\n",
                           float_control_props->shaderRoundingModeRTEFloat32);
                    printf("\tshaderRoundingModeRTEFloat64          = %" PRIuLEAST32 "\n",
                           float_control_props->shaderRoundingModeRTEFloat64);
                    printf("\tshaderRoundingModeRTZFloat16          = %" PRIuLEAST32 "\n",
                           float_control_props->shaderRoundingModeRTZFloat16);
                    printf("\tshaderRoundingModeRTZFloat32          = %" PRIuLEAST32 "\n",
                           float_control_props->shaderRoundingModeRTZFloat32);
                    printf("\tshaderRoundingModeRTZFloat64          = %" PRIuLEAST32 "\n",
                           float_control_props->shaderRoundingModeRTZFloat64);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDevicePCIBusInfoPropertiesEXT *pci_bus_properties = (VkPhysicalDevicePCIBusInfoPropertiesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDevicePCIBusInfoProperties</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>pciDomain   = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            pci_bus_properties->pciDomain);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>pciBus      = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            pci_bus_properties->pciBus);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>pciDevice   = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            pci_bus_properties->pciDevice);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>pciFunction = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            pci_bus_properties->pciFunction);
                    fprintf(out, "\t\t\t\t\t</details>\n");

                } else if (human_readable_output) {
                    printf("\nVkPhysicalDevicePCIBusInfoProperties\n");
                    printf("====================================\n");
                    printf("\tpciDomain   = %" PRIuLEAST32 "\n", pci_bus_properties->pciDomain);
                    printf("\tpciBus      = %" PRIuLEAST32 "\n", pci_bus_properties->pciBus);
                    printf("\tpciDevice   = %" PRIuLEAST32 "\n", pci_bus_properties->pciDevice);
                    printf("\tpciFunction = %" PRIuLEAST32 "\n", pci_bus_properties->pciFunction);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceTransformFeedbackPropertiesEXT *transform_feedback_properties =
                    (VkPhysicalDeviceTransformFeedbackPropertiesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceTransformFeedbackProperties</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>maxTransformFeedbackStreams                = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            transform_feedback_properties->maxTransformFeedbackStreams);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>maxTransformFeedbackBuffers                = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            transform_feedback_properties->maxTransformFeedbackBuffers);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>maxTransformFeedbackBufferSize             = <span "
                            "class='val'>%" PRIuLEAST64 "</span></summary></details>\n",
                            transform_feedback_properties->maxTransformFeedbackBufferSize);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>maxTransformFeedbackStreamDataSize         = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            transform_feedback_properties->maxTransformFeedbackStreamDataSize);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>maxTransformFeedbackBufferDataSize         = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            transform_feedback_properties->maxTransformFeedbackBufferDataSize);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>maxTransformFeedbackBufferDataStride       = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            transform_feedback_properties->maxTransformFeedbackBufferDataStride);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>transformFeedbackQueries                   = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            transform_feedback_properties->transformFeedbackQueries);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>transformFeedbackStreamsLinesTriangles     = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            transform_feedback_properties->transformFeedbackStreamsLinesTriangles);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>transformFeedbackRasterizationStreamSelect = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            transform_feedback_properties->transformFeedbackRasterizationStreamSelect);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>transformFeedbackDraw                      = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            transform_feedback_properties->transformFeedbackDraw);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceTransformFeedbackProperties\n");
                    printf("===========================================\n");
                    printf("\tmaxTransformFeedbackStreams                = %" PRIuLEAST32 "\n",
                           transform_feedback_properties->maxTransformFeedbackStreams);
                    printf("\tmaxTransformFeedbackBuffers                = %" PRIuLEAST32 "\n",
                           transform_feedback_properties->maxTransformFeedbackBuffers);
                    printf("\tmaxTransformFeedbackBufferSize             = %" PRIuLEAST64 "\n",
                           transform_feedback_properties->maxTransformFeedbackBufferSize);
                    printf("\tmaxTransformFeedbackStreamDataSize         = %" PRIuLEAST32 "\n",
                           transform_feedback_properties->maxTransformFeedbackStreamDataSize);
                    printf("\tmaxTransformFeedbackBufferDataSize         = %" PRIuLEAST32 "\n",
                           transform_feedback_properties->maxTransformFeedbackBufferDataSize);
                    printf("\tmaxTransformFeedbackBufferDataStride       = %" PRIuLEAST32 "\n",
                           transform_feedback_properties->maxTransformFeedbackBufferDataStride);
                    printf("\ttransformFeedbackQueries                   = %" PRIuLEAST32 "\n",
                           transform_feedback_properties->transformFeedbackQueries);
                    printf("\ttransformFeedbackStreamsLinesTriangles     = %" PRIuLEAST32 "\n",
                           transform_feedback_properties->transformFeedbackStreamsLinesTriangles);
                    printf("\ttransformFeedbackRasterizationStreamSelect = %" PRIuLEAST32 "\n",
                           transform_feedback_properties->transformFeedbackRasterizationStreamSelect);
                    printf("\ttransformFeedbackDraw                      = %" PRIuLEAST32 "\n",
                           transform_feedback_properties->transformFeedbackDraw);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceFragmentDensityMapPropertiesEXT *fragment_density_map_properties =
                    (VkPhysicalDeviceFragmentDensityMapPropertiesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceFragmentDensityMapProperties</summary>\n");
                    fprintf(out, "\t\t\t\t\t\t<details><summary>minFragmentDensityTexelSize</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>width = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            fragment_density_map_properties->minFragmentDensityTexelSize.width);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>height = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            fragment_density_map_properties->minFragmentDensityTexelSize.height);
                    fprintf(out, "\t\t\t\t\t\t<details><summary>maxFragmentDensityTexelSize</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>width = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            fragment_density_map_properties->maxFragmentDensityTexelSize.width);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>height = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            fragment_density_map_properties->maxFragmentDensityTexelSize.height);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>fragmentDensityInvocations = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            fragment_density_map_properties->fragmentDensityInvocations);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceFragmentDensityMapProperties\n");
                    printf("============================================\n");
                    printf("\tminFragmentDensityTexelSize\n");
                    printf("\t\twidth = %" PRIuLEAST32 "\n", fragment_density_map_properties->minFragmentDensityTexelSize.width);
                    printf("\t\theight = %" PRIuLEAST32 "\n", fragment_density_map_properties->minFragmentDensityTexelSize.height);
                    printf("\tmaxFragmentDensityTexelSize\n");
                    printf("\t\twidth = %" PRIuLEAST32 "\n", fragment_density_map_properties->maxFragmentDensityTexelSize.width);
                    printf("\t\theight = %" PRIuLEAST32 "\n", fragment_density_map_properties->maxFragmentDensityTexelSize.height);
                    printf("\tfragmentDensityInvocations = %" PRIuLEAST32 "\n",
                           fragment_density_map_properties->fragmentDensityInvocations);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR &&
                       CheckPhysicalDeviceExtensionIncluded(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceDepthStencilResolvePropertiesKHR *depth_stencil_resolve_properties =
                    (VkPhysicalDeviceDepthStencilResolvePropertiesKHR *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceDepthStencilResolveProperties</summary>\n");
                    fprintf(out, "\t\t\t\t\t\t<details><summary>supportedDepthResolveModes</summary></details>\n");
                    if (depth_stencil_resolve_properties->supportedDepthResolveModes == 0) {
                        fprintf(out,
                                "\t\t\t\t\t\t<details><summary><span "
                                "class='val'>VK_RESOLVE_MODE_NONE_KHR</span></summary></details>\n");
                    } else {
                        if (depth_stencil_resolve_properties->supportedDepthResolveModes & VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR)
                            fprintf(out,
                                    "\t\t\t\t\t\t<details><summary><span "
                                    "class='val'>VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR</span></summary></details>\n");
                        if (depth_stencil_resolve_properties->supportedDepthResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT_KHR)
                            fprintf(out,
                                    "\t\t\t\t\t\t<details><summary><span "
                                    "class='val'>VK_RESOLVE_MODE_AVERAGE_BIT_KHR</span></summary></details>\n");
                        if (depth_stencil_resolve_properties->supportedDepthResolveModes & VK_RESOLVE_MODE_MIN_BIT_KHR)
                            fprintf(out,
                                    "\t\t\t\t\t\t<details><summary><span "
                                    "class='val'>VK_RESOLVE_MODE_MIN_BIT_KHR</span></summary></details>\n");
                        if (depth_stencil_resolve_properties->supportedDepthResolveModes & VK_RESOLVE_MODE_MAX_BIT_KHR)
                            fprintf(out,
                                    "\t\t\t\t\t\t<details><summary><span "
                                    "class='val'>VK_RESOLVE_MODE_MAX_BIT_KHR</span></summary></details>\n");
                    }
                    fprintf(out, "\t\t\t\t\t\t<details><summary>supportedStencilResolveModes</summary></details>\n");
                    if (depth_stencil_resolve_properties->supportedStencilResolveModes == 0) {
                        fprintf(out,
                                "\t\t\t\t\t\t<details><summary><span "
                                "class='val'>VK_RESOLVE_MODE_NONE_KHR</span></summary></details>\n");
                    } else {
                        if (depth_stencil_resolve_properties->supportedStencilResolveModes & VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR)
                            fprintf(out,
                                    "\t\t\t\t\t\t<details><summary><span "
                                    "class='val'>VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR</span></summary></details>\n");
                        if (depth_stencil_resolve_properties->supportedStencilResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT_KHR)
                            fprintf(out,
                                    "\t\t\t\t\t\t<details><summary><span "
                                    "class='val'>VK_RESOLVE_MODE_AVERAGE_BIT_KHR</span></summary></details>\n");
                        if (depth_stencil_resolve_properties->supportedStencilResolveModes & VK_RESOLVE_MODE_MIN_BIT_KHR)
                            fprintf(out,
                                    "\t\t\t\t\t\t<details><summary><span "
                                    "class='val'>VK_RESOLVE_MODE_MIN_BIT_KHR</span></summary></details>\n");
                        if (depth_stencil_resolve_properties->supportedStencilResolveModes & VK_RESOLVE_MODE_MAX_BIT_KHR)
                            fprintf(out,
                                    "\t\t\t\t\t\t<details><summary><span "
                                    "class='val'>VK_RESOLVE_MODE_MAX_BIT_KHR</span></summary></details>\n");
                    }
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>independentResolveNone = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            depth_stencil_resolve_properties->independentResolveNone);
                    fprintf(out,
                            "\t\t\t\t\t\t<details><summary>independentResolve = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            depth_stencil_resolve_properties->independentResolve);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceDepthStencilResolveProperties\n");
                    printf("============================================\n");
                    printf("\tsupportedDepthResolveModes:\n");
                    if (depth_stencil_resolve_properties->supportedDepthResolveModes == 0) {
                        printf("\t\tVK_RESOLVE_MODE_NONE_KHR\n");
                    } else {
                        if (depth_stencil_resolve_properties->supportedDepthResolveModes & VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR)
                            printf("\t\tVK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR\n");
                        if (depth_stencil_resolve_properties->supportedDepthResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT_KHR)
                            printf("\t\tVK_RESOLVE_MODE_AVERAGE_BIT_KHR\n");
                        if (depth_stencil_resolve_properties->supportedDepthResolveModes & VK_RESOLVE_MODE_MIN_BIT_KHR)
                            printf("\t\tVK_RESOLVE_MODE_MIN_BIT_KHR\n");
                        if (depth_stencil_resolve_properties->supportedDepthResolveModes & VK_RESOLVE_MODE_MAX_BIT_KHR)
                            printf("\t\tVK_RESOLVE_MODE_MAX_BIT_KHR\n");
                    }
                    printf("\tsupportedStencilResolveModes:\n");
                    if (depth_stencil_resolve_properties->supportedStencilResolveModes == 0) {
                        printf("\t\tVK_RESOLVE_MODE_NONE_KHR\n");
                    } else {
                        if (depth_stencil_resolve_properties->supportedStencilResolveModes & VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR)
                            printf("\t\tVK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR\n");
                        if (depth_stencil_resolve_properties->supportedStencilResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT_KHR)
                            printf("\t\tVK_RESOLVE_MODE_AVERAGE_BIT_KHR\n");
                        if (depth_stencil_resolve_properties->supportedStencilResolveModes & VK_RESOLVE_MODE_MIN_BIT_KHR)
                            printf("\t\tVK_RESOLVE_MODE_MIN_BIT_KHR\n");
                        if (depth_stencil_resolve_properties->supportedStencilResolveModes & VK_RESOLVE_MODE_MAX_BIT_KHR)
                            printf("\t\tVK_RESOLVE_MODE_MAX_BIT_KHR\n");
                    }
                    printf("\tindependentResolveNone = %" PRIuLEAST32 "\n",
                           depth_stencil_resolve_properties->independentResolveNone);
                    printf("\tindependentResolve     = %" PRIuLEAST32 "\n", depth_stencil_resolve_properties->independentResolve);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceDescriptorIndexingPropertiesEXT *indexing_properties =
                    (VkPhysicalDeviceDescriptorIndexingPropertiesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceDescriptorIndexingProperties</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxUpdateAfterBindDescriptorsInAllPools = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxUpdateAfterBindDescriptorsInAllPools);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>shaderUniformBufferArrayNonUniformIndexingNative = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->shaderUniformBufferArrayNonUniformIndexingNative);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>shaderSampledImageArrayNonUniformIndexingNative = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->shaderSampledImageArrayNonUniformIndexingNative);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>shaderStorageBufferArrayNonUniformIndexingNative = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->shaderStorageBufferArrayNonUniformIndexingNative);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>shaderStorageImageArrayNonUniformIndexingNative = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->shaderStorageImageArrayNonUniformIndexingNative);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>shaderInputAttachmentArrayNonUniformIndexingNative = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->shaderInputAttachmentArrayNonUniformIndexingNative);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>robustBufferAccessUpdateAfterBind = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            indexing_properties->robustBufferAccessUpdateAfterBind);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>quadDivergentImplicitLod = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            indexing_properties->quadDivergentImplicitLod);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxPerStageDescriptorUpdateAfterBindSamplers = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxPerStageDescriptorUpdateAfterBindSamplers);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxPerStageDescriptorUpdateAfterBindUniformBuffers = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxPerStageDescriptorUpdateAfterBindUniformBuffers);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxPerStageDescriptorUpdateAfterBindStorageBuffers = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxPerStageDescriptorUpdateAfterBindStorageBuffers);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxPerStageDescriptorUpdateAfterBindSampledImages = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxPerStageDescriptorUpdateAfterBindSampledImages);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxPerStageDescriptorUpdateAfterBindStorageImages = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxPerStageDescriptorUpdateAfterBindStorageImages);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxPerStageDescriptorUpdateAfterBindInputAttachments = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxPerStageDescriptorUpdateAfterBindInputAttachments);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxPerStageUpdateAfterBindResources = <span class='val'>%" PRIuLEAST32
                            "</span></summary></details>\n",
                            indexing_properties->maxPerStageUpdateAfterBindResources);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxDescriptorSetUpdateAfterBindSamplers = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxDescriptorSetUpdateAfterBindSamplers);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxDescriptorSetUpdateAfterBindUniformBuffers = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxDescriptorSetUpdateAfterBindUniformBuffers);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxDescriptorSetUpdateAfterBindUniformBuffersDynamic = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxDescriptorSetUpdateAfterBindUniformBuffersDynamic);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxDescriptorSetUpdateAfterBindStorageBuffers = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxDescriptorSetUpdateAfterBindStorageBuffers);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxDescriptorSetUpdateAfterBindStorageBuffersDynamic = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxDescriptorSetUpdateAfterBindStorageBuffersDynamic);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxDescriptorSetUpdateAfterBindSampledImages = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxDescriptorSetUpdateAfterBindSampledImages);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxDescriptorSetUpdateAfterBindStorageImages = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxDescriptorSetUpdateAfterBindStorageImages);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxDescriptorSetUpdateAfterBindInputAttachments = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            indexing_properties->maxDescriptorSetUpdateAfterBindInputAttachments);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceDescriptorIndexingProperties\n");
                    printf("============================================\n");
                    printf("\tmaxUpdateAfterBindDescriptorsInAllPools = %" PRIuLEAST32 "\n",
                           indexing_properties->maxUpdateAfterBindDescriptorsInAllPools);
                    printf("\tshaderUniformBufferArrayNonUniformIndexingNative = %" PRIuLEAST32 "\n",
                           indexing_properties->shaderUniformBufferArrayNonUniformIndexingNative);
                    printf("\tshaderSampledImageArrayNonUniformIndexingNative = %" PRIuLEAST32 "\n",
                           indexing_properties->shaderSampledImageArrayNonUniformIndexingNative);
                    printf("\tshaderStorageBufferArrayNonUniformIndexingNative = %" PRIuLEAST32 "\n",
                           indexing_properties->shaderStorageBufferArrayNonUniformIndexingNative);
                    printf("\tshaderStorageImageArrayNonUniformIndexingNative = %" PRIuLEAST32 "\n",
                           indexing_properties->shaderStorageImageArrayNonUniformIndexingNative);
                    printf("\tshaderInputAttachmentArrayNonUniformIndexingNative = %" PRIuLEAST32 "\n",
                           indexing_properties->shaderInputAttachmentArrayNonUniformIndexingNative);
                    printf("\trobustBufferAccessUpdateAfterBind = %" PRIuLEAST32 "\n",
                           indexing_properties->robustBufferAccessUpdateAfterBind);
                    printf("\tquadDivergentImplicitLod = %" PRIuLEAST32 "\n", indexing_properties->quadDivergentImplicitLod);
                    printf("\tmaxPerStageDescriptorUpdateAfterBindSamplers = %" PRIuLEAST32 "\n",
                           indexing_properties->maxPerStageDescriptorUpdateAfterBindSamplers);
                    printf("\tmaxPerStageDescriptorUpdateAfterBindUniformBuffers = %" PRIuLEAST32 "\n",
                           indexing_properties->maxPerStageDescriptorUpdateAfterBindUniformBuffers);
                    printf("\tmaxPerStageDescriptorUpdateAfterBindStorageBuffers = %" PRIuLEAST32 "\n",
                           indexing_properties->maxPerStageDescriptorUpdateAfterBindStorageBuffers);
                    printf("\tmaxPerStageDescriptorUpdateAfterBindSampledImages = %" PRIuLEAST32 "\n",
                           indexing_properties->maxPerStageDescriptorUpdateAfterBindSampledImages);
                    printf("\tmaxPerStageDescriptorUpdateAfterBindStorageImages = %" PRIuLEAST32 "\n",
                           indexing_properties->maxPerStageDescriptorUpdateAfterBindStorageImages);
                    printf("\tmaxPerStageDescriptorUpdateAfterBindInputAttachments = %" PRIuLEAST32 "\n",
                           indexing_properties->maxPerStageDescriptorUpdateAfterBindInputAttachments);
                    printf("\tmaxPerStageUpdateAfterBindResources = %" PRIuLEAST32 "\n",
                           indexing_properties->maxPerStageUpdateAfterBindResources);
                    printf("\tmaxDescriptorSetUpdateAfterBindSamplers = %" PRIuLEAST32 "\n",
                           indexing_properties->maxDescriptorSetUpdateAfterBindSamplers);
                    printf("\tmaxDescriptorSetUpdateAfterBindUniformBuffers = %" PRIuLEAST32 "\n",
                           indexing_properties->maxDescriptorSetUpdateAfterBindUniformBuffers);
                    printf("\tmaxDescriptorSetUpdateAfterBindUniformBuffersDynamic = %" PRIuLEAST32 "\n",
                           indexing_properties->maxDescriptorSetUpdateAfterBindUniformBuffersDynamic);
                    printf("\tmaxDescriptorSetUpdateAfterBindStorageBuffer = %" PRIuLEAST32 "\n",
                           indexing_properties->maxDescriptorSetUpdateAfterBindStorageBuffers);
                    printf("\tmaxDescriptorSetUpdateAfterBindStorageBuffersDynamic = %" PRIuLEAST32 "\n",
                           indexing_properties->maxDescriptorSetUpdateAfterBindStorageBuffersDynamic);
                    printf("\tmaxDescriptorSetUpdateAfterBindSampledImages = %" PRIuLEAST32 "\n",
                           indexing_properties->maxDescriptorSetUpdateAfterBindSampledImages);
                    printf("\tmaxDescriptorSetUpdateAfterBindStorageImages = %" PRIuLEAST32 "\n",
                           indexing_properties->maxDescriptorSetUpdateAfterBindStorageImages);
                    printf("\tmaxDescriptorSetUpdateAfterBindInputAttachments = %" PRIuLEAST32 "\n",
                           indexing_properties->maxDescriptorSetUpdateAfterBindInputAttachments);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *texel_buffer_alignment =
                    (VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>storageTexelBufferOffsetAlignmentBytes = <span "
                            "class='val'>%" PRIuLEAST64 "</span></summary></details>\n",
                            texel_buffer_alignment->storageTexelBufferOffsetAlignmentBytes);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>storageTexelBufferOffsetSingleTexelAlignment = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            texel_buffer_alignment->storageTexelBufferOffsetSingleTexelAlignment);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>uniformTexelBufferOffsetAlignmentBytes = <span "
                            "class='val'>%" PRIuLEAST64 "</span></summary></details>\n",
                            texel_buffer_alignment->uniformTexelBufferOffsetAlignmentBytes);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>uniformTexelBufferOffsetSingleTexelAlignment = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            texel_buffer_alignment->uniformTexelBufferOffsetSingleTexelAlignment);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceTexelBufferAlignmentPropertiesEXT\n");
                    printf("=================================================\n");
                    printf("\t\tstorageTexelBufferOffsetAlignmentBytes = %" PRIuLEAST64 "\n",
                           texel_buffer_alignment->storageTexelBufferOffsetAlignmentBytes);
                    printf("\t\tstorageTexelBufferOffsetSingleTexelAlignment = %" PRIuLEAST32 "\n",
                           texel_buffer_alignment->storageTexelBufferOffsetSingleTexelAlignment);
                    printf("\t\tuniformTexelBufferOffsetAlignmentBytes = %" PRIuLEAST64 "\n",
                           texel_buffer_alignment->uniformTexelBufferOffsetAlignmentBytes);
                    printf("\t\tuniformTexelBufferOffsetSingleTexelAlignment = %" PRIuLEAST32 "\n",
                           texel_buffer_alignment->uniformTexelBufferOffsetSingleTexelAlignment);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT &&
                       CheckPhysicalDeviceExtensionIncluded(VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME, gpu->device_extensions,
                                                            gpu->device_extension_count)) {
                VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *subgroup_properties =
                    (VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceSubgroupSizeControlPropertiesEXT</summary>\n");
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>minSubgroupSize = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            subgroup_properties->minSubgroupSize);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxSubgroupSize = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            subgroup_properties->maxSubgroupSize);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>maxComputeWorkgroupSubgroups = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            subgroup_properties->maxComputeWorkgroupSubgroups);
                    fprintf(out,
                            "\t\t\t\t\t\t\t<details><summary>requiredSubgroupSizeStages = <span "
                            "class='val'>%" PRIuLEAST32 "</span></summary></details>\n",
                            subgroup_properties->requiredSubgroupSizeStages);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceSubgroupSizeControlPropertiesEXT\n");
                    printf("=================================================\n");
                    printf("\tminSubgroupSize = %" PRIuLEAST32 "\n", subgroup_properties->minSubgroupSize);
                    printf("\tmaxSubgroupSize = %" PRIuLEAST32 "\n", subgroup_properties->maxSubgroupSize);
                    printf("\tmaxComputeWorkgroupSubgroups = %" PRIuLEAST32 "\n",
                           subgroup_properties->maxComputeWorkgroupSubgroups);
                    printf("\trequiredSubgroupSizeStages = %" PRIuLEAST32 "\n", subgroup_properties->requiredSubgroupSizeStages);
                }
            }
            place = structure->pNext;
        }
    }
    fflush(out);
    fflush(stdout);
}
// Compare function for sorting extensions by name
static int CompareExtensionName(const void *a, const void *b) {
    const char *this = ((const VkExtensionProperties *)a)->extensionName;
    const char *that = ((const VkExtensionProperties *)b)->extensionName;
    return strcmp(this, that);
}

// Compare function for sorting layers by name
static int CompareLayerName(const void *a, const void *b) {
    const char *this = ((const struct LayerExtensionList *)a)->layer_properties.layerName;
    const char *that = ((const struct LayerExtensionList *)b)->layer_properties.layerName;
    return strcmp(this, that);
}

static void AppDumpExtensions(const char *indent, const char *layer_name, const uint32_t extension_count,
                              VkExtensionProperties *extension_properties, FILE *out) {
    if (html_output) {
        fprintf(out, "\t\t\t%s<details><summary>", indent);
    }
    if (layer_name && (strlen(layer_name) > 0)) {
        if (html_output) {
            fprintf(out, "%s Extensions", layer_name);
        } else if (human_readable_output) {
            printf("%s%s Extensions", indent, layer_name);
        }
    } else {
        if (html_output) {
            fprintf(out, "Extensions");
        } else if (human_readable_output) {
            printf("%sExtensions", indent);
        }
    }
    if (html_output) {
        fprintf(out, "\tcount = <span class='val'>%d</span></summary>", extension_count);
        if (extension_count > 0) {
            fprintf(out, "\n");
        }
    } else if (human_readable_output) {
        printf("\tcount = %d\n", extension_count);
    }

    const bool is_device_type = layer_name && (strcmp(layer_name, "Device") == 0);
    if (is_device_type && json_output) {
        printf(",\n");
        printf("\t\"ArrayOfVkExtensionProperties\": [");
    }

    qsort(extension_properties, extension_count, sizeof(VkExtensionProperties), CompareExtensionName);

    for (uint32_t i = 0; i < extension_count; ++i) {
        VkExtensionProperties const *ext_prop = &extension_properties[i];
        if (html_output) {
            fprintf(out, "\t\t\t\t%s<details><summary>", indent);
            fprintf(out, "<span class='type'>%s</span>: extension revision <span class='val'>%d</span>", ext_prop->extensionName,
                    ext_prop->specVersion);
            fprintf(out, "</summary></details>\n");
        } else if (human_readable_output) {
            printf("%s\t", indent);
            printf("%-36s: extension revision %2d\n", ext_prop->extensionName, ext_prop->specVersion);
        }
        if (is_device_type && json_output) {
            if (i > 0) {
                printf(",");
            }
            printf("\n");
            printf("\t\t{\n");
            printf("\t\t\t\"extensionName\": \"%s\",\n", ext_prop->extensionName);
            printf("\t\t\t\"specVersion\": %u\n", ext_prop->specVersion);
            printf("\t\t}");
        }
    }
    if (html_output) {
        if (extension_count > 0) {
            fprintf(out, "\t\t\t%s</details>\n", indent);
        } else {
            fprintf(out, "</details>\n");
        }
    }
    if (is_device_type && json_output) {
        if (extension_count > 0) {
            printf("\n\t");
        }
        printf("]");
    }

    fflush(out);
    fflush(stdout);
}

static void AppGpuDumpQueueProps(const struct AppGpu *gpu, uint32_t id, FILE *out) {
    VkQueueFamilyProperties props;

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        const VkQueueFamilyProperties *props2_const = &gpu->queue_props2[id].queueFamilyProperties;
        props = *props2_const;
    } else {
        const VkQueueFamilyProperties *props_const = &gpu->queue_props[id];
        props = *props_const;
    }

    bool is_present_platform_agnostic = true;
    VkBool32 platforms_support_present = VK_FALSE;
    for (struct SurfaceExtensionNode *sen = gpu->inst->surface_ext_infos_root; sen != NULL; sen = sen->next) {
        VkResult err = vkGetPhysicalDeviceSurfaceSupportKHR(gpu->obj, id, sen->surface, &sen->supports_present);
        if (err) ERR_EXIT(err);

        const bool first = (sen == gpu->inst->surface_ext_infos_root);
        if (!first && platforms_support_present != sen->supports_present) is_present_platform_agnostic = false;
        platforms_support_present = sen->supports_present;
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>VkQueueFamilyProperties[<span class='val'>%d</span>]</summary>\n", id);
        fprintf(out, "\t\t\t\t\t\t<details><summary>queueFlags = ");
    } else if (human_readable_output) {
        printf("VkQueueFamilyProperties[%d]:\n", id);
        printf("===========================\n");
        printf("\tqueueFlags         = ");
    }
    if (html_output || human_readable_output) {
        char *sep = "";  // separator character
        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            fprintf(out, "GRAPHICS");
            sep = " | ";
        }
        if (props.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            fprintf(out, "%sCOMPUTE", sep);
            sep = " | ";
        }
        if (props.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            fprintf(out, "%sTRANSFER", sep);
            sep = " | ";
        }
        if (props.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
            fprintf(out, "%sSPARSE", sep);
        }
    }

    if (html_output) {
        fprintf(out, "</summary></details>\n");
        fprintf(out, "\t\t\t\t\t\t<details><summary>queueCount         = <span class='val'>%u</span></summary></details>\n",
                props.queueCount);
        fprintf(out, "\t\t\t\t\t\t<details><summary>timestampValidBits = <span class='val'>%u</span></summary></details>\n",
                props.timestampValidBits);
        fprintf(out,
                "\t\t\t\t\t\t<details><summary>minImageTransferGranularity = (<span class='val'>%d</span>, <span "
                "class='val'>%d</span>, <span class='val'>%d</span>)</summary></details>\n",
                props.minImageTransferGranularity.width, props.minImageTransferGranularity.height,
                props.minImageTransferGranularity.depth);
        if (is_present_platform_agnostic) {
            fprintf(out, "\t\t\t\t\t\t<details><summary>present support = <span class='val'>%s</span></summary></details>\n",
                    platforms_support_present ? "true" : "false");
        } else {
            fprintf(out, "\t\t\t\t\t\t<details open><summary>present support</summary>\n");
            for (struct SurfaceExtensionNode *sen = gpu->inst->surface_ext_infos_root; sen != NULL; sen = sen->next) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary>%s = <span class='val'>%s</span></summary></details>\n", sen->name,
                        sen->supports_present ? "true" : "false");
            }
            fprintf(out, "\t\t\t\t\t\t</details>\n");
        }
        fprintf(out, "\t\t\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("\n");
        printf("\tqueueCount         = %u\n", props.queueCount);
        printf("\ttimestampValidBits = %u\n", props.timestampValidBits);
        printf("\tminImageTransferGranularity = (%d, %d, %d)\n", props.minImageTransferGranularity.width,
               props.minImageTransferGranularity.height, props.minImageTransferGranularity.depth);
        if (is_present_platform_agnostic) {
            printf("\tpresent support    = %s\n", platforms_support_present ? "true" : "false");
        } else {
            printf("\tpresent support:\n");
            for (struct SurfaceExtensionNode *sen = gpu->inst->surface_ext_infos_root; sen != NULL; sen = sen->next) {
                printf("\t\t%s = %s\n", sen->name, sen->supports_present ? "true" : "false");
            }
        }
    }
    if (json_output) {
        printf("\t\t{\n");
        printf("\t\t\t\"minImageTransferGranularity\": {\n");
        printf("\t\t\t\t\"depth\": %u,\n", props.minImageTransferGranularity.depth);
        printf("\t\t\t\t\"height\": %u,\n", props.minImageTransferGranularity.height);
        printf("\t\t\t\t\"width\": %u\n", props.minImageTransferGranularity.width);
        printf("\t\t\t},\n");
        printf("\t\t\t\"queueCount\": %u,\n", props.queueCount);
        printf("\t\t\t\"queueFlags\": %u,\n", props.queueFlags);
        printf("\t\t\t\"timestampValidBits\": %u\n", props.timestampValidBits);
        printf("\t\t}");
    }

    fflush(out);
    fflush(stdout);
}

// This prints a number of bytes in a human-readable format according to prefixes of the International System of Quantities (ISQ),
// defined in ISO/IEC 80000. The prefixes used here are not SI prefixes, but rather the binary prefixes based on powers of 1024
// (kibi-, mebi-, gibi- etc.).
#define kBufferSize 32

static char *HumanReadable(const size_t sz) {
    const char prefixes[] = "KMGTPEZY";
    char buf[kBufferSize];
    int which = -1;
    double result = (double)sz;
    while (result > 1024 && which < 7) {
        result /= 1024;
        ++which;
    }

    char unit[] = "\0i";
    if (which >= 0) {
        unit[0] = prefixes[which];
    }
#ifdef _WIN32
    _snprintf_s(buf, kBufferSize * sizeof(char), kBufferSize, "%.2f %sB", result, unit);
#else
    snprintf(buf, kBufferSize, "%.2f %sB", result, unit);
#endif
    return strndup(buf, kBufferSize);
}

static void AppGpuDumpMemoryProps(const struct AppGpu *gpu, FILE *out) {
    VkPhysicalDeviceMemoryProperties props;
    struct VkStructureHeader *structure = NULL;

    VkDeviceSize *heapBudget = NULL;
    VkDeviceSize *heapUsage = NULL;

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        const VkPhysicalDeviceMemoryProperties *props2_const = &gpu->memory_props2.memoryProperties;
        props = *props2_const;
        structure = (struct VkStructureHeader *)gpu->memory_props2.pNext;
    } else {
        const VkPhysicalDeviceMemoryProperties *props_const = &gpu->memory_props;
        props = *props_const;
    }

    while (structure) {
        if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT &&
            CheckPhysicalDeviceExtensionIncluded(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME, gpu->device_extensions,
                                                 gpu->device_extension_count)) {
            VkPhysicalDeviceMemoryBudgetPropertiesEXT *mem_budget_props = (VkPhysicalDeviceMemoryBudgetPropertiesEXT *)structure;
            heapBudget = mem_budget_props->heapBudget;
            heapUsage = mem_budget_props->heapUsage;
        }

        structure = (struct VkStructureHeader *)structure->pNext;
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceMemoryProperties</summary>\n");
        fprintf(out, "\t\t\t\t\t\t<details><summary>memoryHeapCount = <span class='val'>%u</span></summary>",
                props.memoryHeapCount);
        if (props.memoryHeapCount > 0) {
            fprintf(out, "\n");
        }
    } else if (human_readable_output) {
        printf("VkPhysicalDeviceMemoryProperties:\n");
        printf("=================================\n");
        printf("\tmemoryHeapCount       = %u\n", props.memoryHeapCount);
    }
    if (json_output) {
        printf(",\n");
        printf("\t\"VkPhysicalDeviceMemoryProperties\": {\n");
        printf("\t\t\"memoryHeaps\": [");
    }
    for (uint32_t i = 0; i < props.memoryHeapCount; ++i) {
        const VkDeviceSize memSize = props.memoryHeaps[i].size;
        char *mem_size_human_readable = HumanReadable((const size_t)memSize);

        if (html_output) {
            fprintf(out, "\t\t\t\t\t\t\t<details><summary>memoryHeaps[<span class='val'>%u</span>]</summary>\n", i);
            fprintf(out,
                    "\t\t\t\t\t\t\t\t<details><summary>size = <span class='val'>" PRINTF_SIZE_T_SPECIFIER
                    "</span> (<span class='val'>0x%" PRIxLEAST64 "</span>) (<span class='val'>%s</span>)</summary></details>\n",
                    (size_t)memSize, memSize, mem_size_human_readable);
            if (heapBudget != NULL) {
                fprintf(out,
                        "\t\t\t\t\t\t\t\t<details><summary>budget = <span class='val'>%" PRIuLEAST64
                        "</span></summary></details>\n",
                        heapBudget[i]);
                fprintf(out,
                        "\t\t\t\t\t\t\t\t<details><summary>usage = <span class='val'>%" PRIuLEAST64 "</span></summary></details>\n",
                        heapUsage[i]);
            }
        } else if (human_readable_output) {
            printf("\tmemoryHeaps[%u] :\n", i);
            printf("\t\tsize          = " PRINTF_SIZE_T_SPECIFIER " (0x%" PRIxLEAST64 ") (%s)\n", (size_t)memSize, memSize,
                   mem_size_human_readable);
            if (heapBudget != NULL) {
                fprintf(out, "\t\tbudget        = %" PRIuLEAST64 "\n", heapBudget[i]);
                fprintf(out, "\t\tusage         = %" PRIuLEAST64 "\n", heapUsage[i]);
            }
        }
        free(mem_size_human_readable);

        const VkMemoryHeapFlags heap_flags = props.memoryHeaps[i].flags;
        if (html_output) {
            fprintf(out, "\t\t\t\t\t\t\t\t<details open><summary>flags</summary>\n");
            fprintf(out, "\t\t\t\t\t\t\t\t\t<details><summary>");
            fprintf(out, (heap_flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                             ? "<span class='type'>VK_MEMORY_HEAP_DEVICE_LOCAL_BIT</span>"
                             : "None");
            fprintf(out, "</summary></details>\n");
            fprintf(out, "\t\t\t\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            printf("\t\tflags:\n\t\t\t");
            printf((heap_flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ? "VK_MEMORY_HEAP_DEVICE_LOCAL_BIT\n" : "None\n");
        }
        if (json_output) {
            if (i > 0) {
                printf(",");
            }
            printf("\n");
            printf("\t\t\t{\n");
            printf("\t\t\t\t\"flags\": %u,\n", heap_flags);
            printf("\t\t\t\t\"size\": " PRINTF_SIZE_T_SPECIFIER "\n", (size_t)memSize);
            printf("\t\t\t}");
        }
    }
    if (html_output) {
        if (props.memoryHeapCount > 0) {
            fprintf(out, "\t\t\t\t\t\t");
        }
        fprintf(out, "</details>\n");
    }
    if (json_output) {
        if (props.memoryHeapCount > 0) {
            printf("\n\t\t");
        }
        printf("]");
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t\t<details><summary>memoryTypeCount = <span class='val'>%u</span></summary>",
                props.memoryTypeCount);
        if (props.memoryTypeCount > 0) {
            fprintf(out, "\n");
        }
    } else if (human_readable_output) {
        printf("\tmemoryTypeCount       = %u\n", props.memoryTypeCount);
    }
    if (json_output) {
        printf(",\n");
        printf("\t\t\"memoryTypes\": [");
    }
    for (uint32_t i = 0; i < props.memoryTypeCount; ++i) {
        if (html_output) {
            fprintf(out, "\t\t\t\t\t\t\t<details><summary>memoryTypes[<span class='val'>%u</span>]</summary>\n", i);
            fprintf(out, "\t\t\t\t\t\t\t\t<details><summary>heapIndex = <span class='val'>%u</span></summary></details>\n",
                    props.memoryTypes[i].heapIndex);
            fprintf(out,
                    "\t\t\t\t\t\t\t\t<details open><summary>propertyFlags = <span class='val'>0x%" PRIxLEAST32 "</span></summary>",
                    props.memoryTypes[i].propertyFlags);
            if (props.memoryTypes[i].propertyFlags == 0) {
                fprintf(out, "</details>\n");
            } else {
                fprintf(out, "\n");
            }
        } else if (human_readable_output) {
            printf("\tmemoryTypes[%u] :\n", i);
            printf("\t\theapIndex     = %u\n", props.memoryTypes[i].heapIndex);
            printf("\t\tpropertyFlags = 0x%" PRIxLEAST32 ":\n", props.memoryTypes[i].propertyFlags);
        }
        if (json_output) {
            if (i > 0) {
                printf(",");
            }
            printf("\n");
            printf("\t\t\t{\n");
            printf("\t\t\t\t\"heapIndex\": %u,\n", props.memoryTypes[i].heapIndex);
            printf("\t\t\t\t\"propertyFlags\": %u\n", props.memoryTypes[i].propertyFlags);
            printf("\t\t\t}");
        }

        // Print each named flag to html or std output if it is set
        const VkFlags flags = props.memoryTypes[i].propertyFlags;
        if (html_output) {
            if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                fprintf(out,
                        "\t\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT</span></summary></details>\n");
            if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
                fprintf(out,
                        "\t\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT</span></summary></details>\n");
            if (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
                fprintf(out,
                        "\t\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_MEMORY_PROPERTY_HOST_COHERENT_BIT</span></summary></details>\n");
            if (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
                fprintf(out,
                        "\t\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_MEMORY_PROPERTY_HOST_CACHED_BIT</span></summary></details>\n");
            if (flags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
                fprintf(out,
                        "\t\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT</span></summary></details>\n");
            if (flags & VK_MEMORY_PROPERTY_PROTECTED_BIT)
                fprintf(out,
                        "\t\t\t\t\t\t\t\t\t<details><summary><span "
                        "class='type'>VK_MEMORY_PROPERTY_PROTECTED_BIT</span></summary></details>\n");
            if (props.memoryTypes[i].propertyFlags > 0) fprintf(out, "\t\t\t\t\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) printf("\t\t\tVK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT\n");
            if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) printf("\t\t\tVK_MEMORY_PROPERTY_HOST_VISIBLE_BIT\n");
            if (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) printf("\t\t\tVK_MEMORY_PROPERTY_HOST_COHERENT_BIT\n");
            if (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) printf("\t\t\tVK_MEMORY_PROPERTY_HOST_CACHED_BIT\n");
            if (flags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) printf("\t\t\tVK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT\n");
            if (flags & VK_MEMORY_PROPERTY_PROTECTED_BIT) printf("\t\t\tVK_MEMORY_PROPERTY_PROTECTED_BIT\n");
        }

        if (human_readable_output) {
            printf("\t\tusable for:\n");
            const uint32_t memtype_bit = 1 << i;

            for (VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL; tiling < ARRAY_SIZE(gpu->mem_type_res_support.image); ++tiling) {
                printf("\t\t\t%s: ", VkTilingString(tiling));

                bool first = true;
                for (size_t fmt_i = 0; fmt_i < ARRAY_SIZE(gpu->mem_type_res_support.image[tiling]); ++fmt_i) {
                    const struct MemImageSupport *image_support = &gpu->mem_type_res_support.image[tiling][fmt_i];
                    const bool regular_compatible =
                        image_support->regular_supported && (image_support->regular_memtypes & memtype_bit);
                    const bool sparse_compatible =
                        image_support->sparse_supported && (image_support->sparse_memtypes & memtype_bit);
                    const bool transient_compatible =
                        image_support->transient_supported && (image_support->transient_memtypes & memtype_bit);

                    if (regular_compatible || sparse_compatible || transient_compatible) {
                        if (!first) printf(", ");
                        first = false;

                        if (fmt_i == 0) {
                            printf("color images");
                        } else {
                            printf("%s", VkFormatString(gpu->mem_type_res_support.image[tiling][fmt_i].format));
                        }

                        if (regular_compatible && !sparse_compatible && !transient_compatible && image_support->sparse_supported &&
                            image_support->transient_supported) {
                            printf("(non-sparse, non-transient)");
                        } else if (regular_compatible && !sparse_compatible && image_support->sparse_supported) {
                            if (image_support->sparse_supported) printf("(non-sparse)");
                        } else if (regular_compatible && !transient_compatible && image_support->transient_supported) {
                            if (image_support->transient_supported) printf("(non-transient)");
                        } else if (!regular_compatible && sparse_compatible && !transient_compatible &&
                                   image_support->sparse_supported) {
                            if (image_support->sparse_supported) printf("(sparse only)");
                        } else if (!regular_compatible && !sparse_compatible && transient_compatible &&
                                   image_support->transient_supported) {
                            if (image_support->transient_supported) printf("(transient only)");
                        } else if (!regular_compatible && sparse_compatible && transient_compatible &&
                                   image_support->sparse_supported && image_support->transient_supported) {
                            printf("(sparse and transient only)");
                        }
                    }
                }

                if (first) printf("None");

                printf("\n");
            }
        } else if (html_output) {
            fprintf(out, "\t\t\t\t\t\t\t\t<details><summary>usable for</summary>\n");
            const uint32_t memtype_bit = 1 << i;

            for (VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL; tiling < ARRAY_SIZE(gpu->mem_type_res_support.image); ++tiling) {
                fprintf(out, "\t\t\t\t\t\t\t\t\t<details><summary>%s</summary>\n", VkTilingString(tiling));

                bool first = true;
                for (size_t fmt_i = 0; fmt_i < ARRAY_SIZE(gpu->mem_type_res_support.image[tiling]); ++fmt_i) {
                    const struct MemImageSupport *image_support = &gpu->mem_type_res_support.image[tiling][fmt_i];
                    const bool regular_compatible =
                        image_support->regular_supported && (image_support->regular_memtypes & memtype_bit);
                    const bool sparse_compatible =
                        image_support->sparse_supported && (image_support->sparse_memtypes & memtype_bit);
                    const bool transient_compatible =
                        image_support->transient_supported && (image_support->transient_memtypes & memtype_bit);

                    if (regular_compatible || sparse_compatible || transient_compatible) {
                        first = false;

                        if (fmt_i == 0) {
                            fprintf(out, "\t\t\t\t\t\t\t\t\t\t<details><summary>color images</summary>\n");
                        } else {
                            fprintf(out, "\t\t\t\t\t\t\t\t\t\t<details><summary>%s</summary>\n",
                                    VkFormatString(gpu->mem_type_res_support.image[tiling][fmt_i].format));
                        }

                        fprintf(out,
                                "\t\t\t\t\t\t\t\t\t\t\t<details><summary><span class=\"type\">regular image</span> = <span "
                                "class=\"val\">%s</span></summary></details>\n",
                                regular_compatible ? "supported" : "not supported");
                        fprintf(out,
                                "\t\t\t\t\t\t\t\t\t\t\t<details><summary><span "
                                "class=\"type\">VK_IMAGE_CREATE_SPARSE_BINDING_BIT</span> "
                                "= <span class=\"val\">%s</span></summary></details>\n",
                                sparse_compatible ? "supported" : "not supported");
                        fprintf(out,
                                "\t\t\t\t\t\t\t\t\t\t\t<details><summary><span "
                                "class=\"type\">VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT</span> = <span "
                                "class=\"val\">%s</span></summary></details>\n",
                                transient_compatible ? "supported" : "not supported");

                        fprintf(out, "\t\t\t\t\t\t\t\t\t\t</details>\n");
                    }
                }

                if (first) fprintf(out, "<details><summary><span class=\"type\">None</span></summary></details>");

                fprintf(out, "\t\t\t\t\t\t\t\t\t</details>\n");
            }

            fprintf(out, "\t\t\t\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t\t\t</details>\n");
        }
    }

    if (html_output) {
        if (props.memoryTypeCount > 0) {
            fprintf(out, "\t\t\t\t\t\t");
        }
        fprintf(out, "</details>\n");
        fprintf(out, "\t\t\t\t\t</details>\n");
    }
    if (json_output) {
        if (props.memoryTypeCount > 0) {
            printf("\n\t\t");
        }
        printf("]\n");
        printf("\t}");
    }

    fflush(out);
    fflush(stdout);
}

static void AppGpuDump(const struct AppGpu *gpu, FILE *out) {
    if (html_output) {
        fprintf(out, "\t\t\t\t<details><summary>GPU%u</summary>\n", gpu->id);
    } else if (human_readable_output) {
        printf("\nDevice Properties and Extensions :\n");
        printf("==================================\n");
        printf("GPU%u\n", gpu->id);
    }

    AppGpuDumpProps(gpu, out);
    if (html_output) {
        AppDumpExtensions("\t\t", "Device", gpu->device_extension_count, gpu->device_extensions, out);
    } else if (human_readable_output) {
        printf("\n");
        AppDumpExtensions("", "Device", gpu->device_extension_count, gpu->device_extensions, out);
        printf("\n");
    }

    if (json_output) {
        printf(",\n");
        printf("\t\"ArrayOfVkQueueFamilyProperties\": [");
    }
    for (uint32_t i = 0; i < gpu->queue_count; ++i) {
        if (json_output) {
            if (i > 0) {
                printf(",");
            }
            printf("\n");
        }
        AppGpuDumpQueueProps(gpu, i, out);
        if (human_readable_output) {
            printf("\n");
        }
    }
    if (json_output) {
        if (gpu->queue_count > 0) {
            printf("\n\t");
        }
        printf("]");
    }

    AppGpuDumpMemoryProps(gpu, out);
    if (human_readable_output) {
        printf("\n");
    }

    AppGpuDumpFeatures(gpu, out);
    if (human_readable_output) {
        printf("\n");
    }

    AppDevDump(gpu, out);
    if (html_output) {
        fprintf(out, "\t\t\t\t</details>\n");
    }
}

static void AppGroupDump(const VkPhysicalDeviceGroupProperties *group, const uint32_t id, const struct AppInstance *inst,
                         FILE *out) {
    if (html_output) {
        fprintf(out, "\t\t\t\t<details><summary>Device Group Properties (Group %u)</summary>\n", id);
        fprintf(out, "\t\t\t\t\t<details><summary>physicalDeviceCount = <span class='val'>%u</span></summary>\n",
                group->physicalDeviceCount);
    } else if (human_readable_output) {
        printf("\tDevice Group Properties (Group %u) :\n", id);
        printf("\t\tphysicalDeviceCount = %u\n", group->physicalDeviceCount);
    }

    // Keep a record of all physical device properties to give the user clearer information as output.
    VkPhysicalDeviceProperties *props = malloc(sizeof(props[0]) * group->physicalDeviceCount);
    if (!props) {
        ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    }
    for (uint32_t i = 0; i < group->physicalDeviceCount; ++i) {
        vkGetPhysicalDeviceProperties(group->physicalDevices[i], &props[i]);
    }

    // Output information to the user.
    for (uint32_t i = 0; i < group->physicalDeviceCount; ++i) {
        if (html_output) {
            fprintf(out, "\t\t\t\t\t\t<details><summary>%s (ID: <span class='val'>%d</span>)</summary></details>\n",
                    props[i].deviceName, i);
        } else if (human_readable_output) {
            printf("\n\t\t\t%s (ID: %d)\n", props[i].deviceName, i);
        }
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t</details>\n");
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>subsetAllocation = <span class='val'>%u</span></summary></details>\n",
                group->subsetAllocation);
    } else if (human_readable_output) {
        printf("\n\t\tsubsetAllocation = %u\n", group->subsetAllocation);
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t</details>\n");
    }

    // Build create info for logical device made from all physical devices in this group.
    const char *extensions_list[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DEVICE_GROUP_EXTENSION_NAME};

    VkDeviceGroupDeviceCreateInfoKHR dg_ci = {.sType = VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO_KHR,
                                              .pNext = NULL,
                                              .physicalDeviceCount = group->physicalDeviceCount,
                                              .pPhysicalDevices = group->physicalDevices};

    float queue_priority = 1.0f;

    VkDeviceQueueCreateInfo q_ci = {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                    .pNext = NULL,
                                    .queueFamilyIndex = 0,
                                    .queueCount = 1,
                                    .pQueuePriorities = &queue_priority};

    VkDeviceCreateInfo device_ci = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                    .pNext = &dg_ci,
                                    .queueCreateInfoCount = 1,
                                    .pQueueCreateInfos = &q_ci,
                                    .enabledExtensionCount = ARRAY_SIZE(extensions_list),
                                    .ppEnabledExtensionNames = extensions_list};

    VkDevice logical_device = VK_NULL_HANDLE;

    VkResult err = vkCreateDevice(group->physicalDevices[0], &device_ci, NULL, &logical_device);
    if (err != VK_SUCCESS && err != VK_ERROR_EXTENSION_NOT_PRESENT) ERR_EXIT(err);

    if (!err) {
        if (html_output) {
            fprintf(out, "\t\t\t\t<details><summary>Device Group Present Capabilities (Group %d)</summary>\n", id);
        } else if (human_readable_output) {
            printf("\n\tDevice Group Present Capabilities (Group %d) :\n", id);
        }

        VkDeviceGroupPresentCapabilitiesKHR group_capabilities = {.sType = VK_STRUCTURE_TYPE_DEVICE_GROUP_PRESENT_CAPABILITIES_KHR,
                                                                  .pNext = NULL};

        // If the KHR_device_group extension is present, write the capabilities of the logical device into a struct for
        // later output to user.
        PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR =
            (PFN_vkGetDeviceGroupPresentCapabilitiesKHR)vkGetInstanceProcAddr(inst->instance,
                                                                              "vkGetDeviceGroupPresentCapabilitiesKHR");
        err = vkGetDeviceGroupPresentCapabilitiesKHR(logical_device, &group_capabilities);
        if (err) ERR_EXIT(err);

        for (uint32_t i = 0; i < group->physicalDeviceCount; i++) {
            if (html_output) {
                fprintf(out, "\t\t\t\t\t<details><summary>%s (ID: <span class='val'>%d</span>)</summary></details>\n",
                        props[i].deviceName, i);
                fprintf(out, "\t\t\t\t\t<details><summary>Can present images from the following devices:</summary>\n");
                if (group_capabilities.presentMask[i] != 0) {
                    for (uint32_t j = 0; j < group->physicalDeviceCount; ++j) {
                        uint32_t mask = 1 << j;
                        if (group_capabilities.presentMask[i] & mask) {
                            fprintf(out, "\t\t\t\t\t\t<details><summary>%s (ID: <span class='val'>%d</span>)</summary></details>\n",
                                    props[j].deviceName, j);
                        }
                    }
                } else {
                    fprintf(out, "\t\t\t\t\t\t<details><summary>None</summary></details>\n");
                }
                fprintf(out, "\t\t\t\t\t</details>\n");
            } else if (human_readable_output) {
                printf("\n\t\t%s (ID: %d)\n", props[i].deviceName, i);
                printf("\t\tCan present images from the following devices:\n");
                if (group_capabilities.presentMask[i] != 0) {
                    for (uint32_t j = 0; j < group->physicalDeviceCount; ++j) {
                        uint32_t mask = 1 << j;
                        if (group_capabilities.presentMask[i] & mask) {
                            printf("\t\t\t%s (ID: %d)\n", props[j].deviceName, j);
                        }
                    }
                } else {
                    printf("\t\t\tNone\n");
                }
                printf("\n");
            }
        }

        if (html_output) {
            fprintf(out, "\t\t\t\t\t<details><summary>Present modes</summary>\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR)
                fprintf(out, "\t\t\t\t\t\t<details><summary>VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR</summary></details>\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_REMOTE_BIT_KHR)
                fprintf(out, "\t\t\t\t\t\t<details><summary>VK_DEVICE_GROUP_PRESENT_MODE_REMOTE_BIT_KHR</summary></details>\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_SUM_BIT_KHR)
                fprintf(out, "\t\t\t\t\t\t<details><summary>VK_DEVICE_GROUP_PRESENT_MODE_SUM_BIT_KHR</summary></details>\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_MULTI_DEVICE_BIT_KHR)
                fprintf(out,
                        "\t\t\t\t\t\t<details><summary>VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_MULTI_DEVICE_BIT_KHR</summary></"
                        "details>\n");
            fprintf(out, "\t\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            printf("\t\tPresent modes:\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR)
                printf("\t\t\tVK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_REMOTE_BIT_KHR)
                printf("\t\t\tVK_DEVICE_GROUP_PRESENT_MODE_REMOTE_BIT_KHR\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_SUM_BIT_KHR)
                printf("\t\t\tVK_DEVICE_GROUP_PRESENT_MODE_SUM_BIT_KHR\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_MULTI_DEVICE_BIT_KHR)
                printf("\t\t\tVK_DEVICE_GROUP_PRESENT_MODE_LOCAL_MULTI_DEVICE_BIT_KHR\n");
        }

        if (html_output) {
            fprintf(out, "\t\t\t\t</details>\n");
        }
    }

    // Clean up after ourselves.
    free(props);
    vkDestroyDevice(logical_device, NULL);
}

#ifdef _WIN32
// Enlarges the console window to have a large scrollback size.
static void ConsoleEnlarge() {
    const HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    // make the console window bigger
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD buffer_size;
    if (GetConsoleScreenBufferInfo(console_handle, &csbi)) {
        buffer_size.X = csbi.dwSize.X + 30;
        buffer_size.Y = 20000;
        SetConsoleScreenBufferSize(console_handle, buffer_size);
    }

    SMALL_RECT r;
    r.Left = r.Top = 0;
    r.Right = csbi.dwSize.X - 1 + 30;
    r.Bottom = 50;
    SetConsoleWindowInfo(console_handle, true, &r);

    // change the console window title
    SetConsoleTitle(TEXT(APP_SHORT_NAME));
}
#endif

void print_usage(char *argv0) {
    printf("\nvulkaninfo - Summarize Vulkan information in relation to the current environment.\n\n");
    printf("USAGE: %s [options]\n\n", argv0);
    printf("OPTIONS:\n");
    printf("-h, --help            Print this help.\n");
    printf("--html                Produce an html version of vulkaninfo output, saved as\n");
    printf("                      \"vulkaninfo.html\" in the directory in which the command is\n");
    printf("                      run.\n");
    printf("-j, --json            Produce a json version of vulkaninfo output to standard\n");
    printf("                      output.\n");
    printf("--json=<gpu-number>   For a multi-gpu system, a single gpu can be targetted by\n");
    printf("                      specifying the gpu-number associated with the gpu of \n");
    printf("                      interest. This number can be determined by running\n");
    printf("                      vulkaninfo without any options specified.\n\n");
}

int main(int argc, char **argv) {
    uint32_t gpu_count;
    VkResult err;
    struct AppInstance inst = {0};
    FILE *out = stdout;

#ifdef _WIN32
    if (ConsoleIsExclusive()) ConsoleEnlarge();
#endif

    // Combinations of output: html only, html AND json, json only, human readable only
    for (int i = 1; i < argc; ++i) {
        if (!CheckForJsonOption(argv[i])) {
            if (strcmp(argv[i], "--html") == 0) {
                human_readable_output = false;
                html_output = true;
                continue;
            } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
                print_usage(argv[0]);
                return 1;
            } else {
                print_usage(argv[0]);
                return 1;
            }
        }
    }

    AppCreateInstance(&inst);

    if (html_output) {
#ifdef _WIN32
        if (fopen_s(&out, "vulkaninfo.html", "w") != 0) out = NULL;
#else
        out = fopen("vulkaninfo.html", "w");
#endif
        if (!out) {
            printf("Unable to open vulkaninfo.html for writing\n");
            return 1;
        }
        PrintHtmlHeader(out);
        fprintf(out, "\t\t\t<details><summary>");
    } else if (human_readable_output) {
        printf("==========\n");
        printf("VULKANINFO\n");
        printf("==========\n\n");
    }
    if (html_output || human_readable_output) {
        fprintf(out, "Vulkan Instance Version: ");
    }
    if (html_output) {
        fprintf(out, "<span class='val'>%d.%d.%d</span></summary></details>\n", inst.vulkan_major, inst.vulkan_minor,
                inst.vulkan_patch);
        fprintf(out, "\t\t\t<br />\n");
    } else if (human_readable_output) {
        printf("%d.%d.%d\n\n", inst.vulkan_major, inst.vulkan_minor, inst.vulkan_patch);
    }

    err = vkEnumeratePhysicalDevices(inst.instance, &gpu_count, NULL);
    if (err) {
        ERR_EXIT(err);
    }

    VkPhysicalDevice *objs = malloc(sizeof(objs[0]) * gpu_count);
    if (!objs) {
        ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    }

    err = vkEnumeratePhysicalDevices(inst.instance, &gpu_count, objs);
    if (err) {
        ERR_EXIT(err);
    }

    struct AppGpu *gpus = malloc(sizeof(gpus[0]) * gpu_count);
    if (!gpus) {
        ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    }

    for (uint32_t i = 0; i < gpu_count; ++i) {
        AppGpuInit(&gpus[i], &inst, i, objs[i]);
        if (human_readable_output) {
            printf("\n\n");
        }
    }

    // If json output, confirm the desired gpu exists
    if (json_output) {
        if (selected_gpu >= gpu_count) {
            selected_gpu = 0;
        }
        PrintJsonHeader(inst.vulkan_major, inst.vulkan_minor, inst.vulkan_patch);
    }

    if (human_readable_output) {
        printf("Instance Extensions:\n");
        printf("====================\n");
    }
    AppDumpExtensions("", "Instance", inst.global_extension_count, inst.global_extensions, out);

    //---Layer-Device-Extensions---
    if (html_output) {
        fprintf(out, "\t\t\t<details><summary>Layers: count = <span class='val'>%d</span></summary>", inst.global_layer_count);
        if (inst.global_layer_count > 0) {
            fprintf(out, "\n");
        }
    } else if (human_readable_output) {
        printf("Layers: count = %d\n", inst.global_layer_count);
        printf("=======\n");
    }
    if (json_output && (inst.global_layer_count > 0)) {
        printf(",\n");
        printf("\t\"ArrayOfVkLayerProperties\": [");
    }

    qsort(inst.global_layers, inst.global_layer_count, sizeof(struct LayerExtensionList), CompareLayerName);

    for (uint32_t i = 0; i < inst.global_layer_count; ++i) {
        uint32_t layer_major, layer_minor, layer_patch;
        char spec_version[64], layer_version[64];
        VkLayerProperties const *layer_prop = &inst.global_layers[i].layer_properties;

        ExtractVersion(layer_prop->specVersion, &layer_major, &layer_minor, &layer_patch);
#ifdef _WIN32
        _snprintf_s(spec_version, sizeof(spec_version), 64, "%d.%d.%d", layer_major, layer_minor, layer_patch);
        _snprintf_s(layer_version, sizeof(layer_version), 64, "%d", layer_prop->implementationVersion);
#else
        snprintf(spec_version, sizeof(spec_version), "%d.%d.%d", layer_major, layer_minor, layer_patch);
        snprintf(layer_version, sizeof(layer_version), "%d", layer_prop->implementationVersion);
#endif

        if (html_output) {
            fprintf(out, "\t\t\t\t<details><summary>");
            fprintf(out, "<span class='type'>%s</span> (%s) Vulkan version <span class='val'>%s</span>, ", layer_prop->layerName,
                    (char *)layer_prop->description, spec_version);
            fprintf(out, "layer version <span class='val'>%s</span></summary>\n", layer_version);
            AppDumpExtensions("\t\t", "Layer", inst.global_layers[i].extension_count, inst.global_layers[i].extension_properties,
                              out);
        } else if (human_readable_output) {
            printf("%s (%s) Vulkan version %s, layer version %s\n", layer_prop->layerName, (char *)layer_prop->description,
                   spec_version, layer_version);
            AppDumpExtensions("\t", "Layer", inst.global_layers[i].extension_count, inst.global_layers[i].extension_properties,
                              out);
        }
        if (json_output) {
            if (i > 0) {
                printf(",");
            }
            printf("\n");
            printf("\t\t{\n");
            printf("\t\t\t\"layerName\": \"%s\",\n", layer_prop->layerName);
            printf("\t\t\t\"specVersion\": %u,\n", layer_prop->specVersion);
            printf("\t\t\t\"implementationVersion\": %u,\n", layer_prop->implementationVersion);
            printf("\t\t\t\"description\": \"%s\"\n", layer_prop->description);
            printf("\t\t}");
        }

        if (html_output) {
            fprintf(out, "\t\t\t\t\t<details><summary>Devices count = <span class='val'>%d</span></summary>\n", gpu_count);
        } else if (human_readable_output) {
            printf("\tDevices \tcount = %d\n", gpu_count);
        }

        char *layer_name = inst.global_layers[i].layer_properties.layerName;

        for (uint32_t j = 0; j < gpu_count; ++j) {
            if (html_output) {
                fprintf(out, "\t\t\t\t\t\t<details><summary>");
                fprintf(out, "GPU id: <span class='val'>%u</span> (%s)</summary></details>\n", j, gpus[j].props.deviceName);
            } else if (human_readable_output) {
                printf("\t\tGPU id       : %u (%s)\n", j, gpus[j].props.deviceName);
            }
            uint32_t count = 0;
            VkExtensionProperties *props;
            AppGetPhysicalDeviceLayerExtensions(&gpus[j], layer_name, &count, &props);
            if (html_output) {
                AppDumpExtensions("\t\t\t", "Layer-Device", count, props, out);
            } else if (human_readable_output) {
                AppDumpExtensions("\t\t", "Layer-Device", count, props, out);
            }
            free(props);
        }

        if (html_output) {
            fprintf(out, "\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            printf("\n");
        }
    }

    if (html_output) {
        fprintf(out, "\t\t\t</details>\n");
    }
    if (json_output && (inst.global_layer_count > 0)) {
        printf("\n\t]");
    }

    fflush(out);
    fflush(stdout);
    //-----------------------------

    if (html_output) {
        fprintf(out, "\t\t\t<details><summary>Presentable Surfaces</summary>");
        if (gpu_count > 0) {
            fprintf(out, "\n");
        } else {
            fprintf(out, "</details>\n");
        }
    } else if (human_readable_output) {
        printf("Presentable Surfaces:\n");
        printf("=====================\n");
    }
    inst.width = 256;
    inst.height = 256;
    int format_count = 0;
    int present_mode_count = 0;

#if defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
    bool has_display = true;
    const char *display_var = getenv("DISPLAY");
    if (display_var == NULL || strlen(display_var) == 0) {
        fprintf(stderr, "'DISPLAY' environment variable not set... skipping surface info\n");
        fflush(stderr);
        has_display = false;
    }
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    struct wl_display *wayland_display = wl_display_connect(NULL);
    bool has_wayland_display = false;
    if (wayland_display != NULL) {
        wl_display_disconnect(wayland_display);
        has_wayland_display = true;
    }
#endif

//--WIN32--
#ifdef VK_USE_PLATFORM_WIN32_KHR
    struct SurfaceExtensionNode surface_ext_win32;
    if (CheckExtensionEnabled(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, inst.inst_extensions, inst.inst_extensions_count)) {
        surface_ext_win32.name = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
        surface_ext_win32.create_window = AppCreateWin32Window;
        surface_ext_win32.create_surface = AppCreateWin32Surface;
        surface_ext_win32.destroy_window = AppDestroyWin32Window;

        surface_ext_win32.next = inst.surface_ext_infos_root;
        inst.surface_ext_infos_root = &surface_ext_win32;
    }
#endif
//--XCB--
#ifdef VK_USE_PLATFORM_XCB_KHR
    struct SurfaceExtensionNode surface_ext_xcb;
    if (CheckExtensionEnabled(VK_KHR_XCB_SURFACE_EXTENSION_NAME, inst.inst_extensions, inst.inst_extensions_count)) {
        surface_ext_xcb.name = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
        surface_ext_xcb.create_window = AppCreateXcbWindow;
        surface_ext_xcb.create_surface = AppCreateXcbSurface;
        surface_ext_xcb.destroy_window = AppDestroyXcbWindow;
        if (has_display) {
            surface_ext_xcb.next = inst.surface_ext_infos_root;
            inst.surface_ext_infos_root = &surface_ext_xcb;
        }
    }
#endif
//--XLIB--
#ifdef VK_USE_PLATFORM_XLIB_KHR
    struct SurfaceExtensionNode surface_ext_xlib;
    if (CheckExtensionEnabled(VK_KHR_XLIB_SURFACE_EXTENSION_NAME, inst.inst_extensions, inst.inst_extensions_count)) {
        surface_ext_xlib.name = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
        surface_ext_xlib.create_window = AppCreateXlibWindow;
        surface_ext_xlib.create_surface = AppCreateXlibSurface;
        surface_ext_xlib.destroy_window = AppDestroyXlibWindow;
        if (has_display) {
            surface_ext_xlib.next = inst.surface_ext_infos_root;
            inst.surface_ext_infos_root = &surface_ext_xlib;
        }
    }
#endif
//--MACOS--
#ifdef VK_USE_PLATFORM_MACOS_MVK
    struct SurfaceExtensionNode surface_ext_macos;
    if (CheckExtensionEnabled(VK_MVK_MACOS_SURFACE_EXTENSION_NAME, inst.inst_extensions, inst.inst_extensions_count)) {
        surface_ext_macos.name = VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
        surface_ext_macos.create_window = AppCreateMacOSWindow;
        surface_ext_macos.create_surface = AppCreateMacOSSurface;
        surface_ext_macos.destroy_window = AppDestroyMacOSWindow;

        surface_ext_macos.next = inst.surface_ext_infos_root;
        inst.surface_ext_infos_root = &surface_ext_macos;
    }
#endif
//--WAYLAND--
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    struct SurfaceExtensionNode surface_ext_wayland;
    if (CheckExtensionEnabled(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME, inst.inst_extensions, inst.inst_extensions_count)) {
        surface_ext_wayland.name = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
        surface_ext_wayland.create_window = AppCreateWaylandWindow;
        surface_ext_wayland.create_surface = AppCreateWaylandSurface;
        surface_ext_wayland.destroy_window = AppDestroyWaylandWindow;
        if (has_wayland_display) {
            surface_ext_wayland.next = inst.surface_ext_infos_root;
            inst.surface_ext_infos_root = &surface_ext_wayland;
        }
    }
#endif
    // TODO: Android

    for (struct SurfaceExtensionNode *sen = inst.surface_ext_infos_root; sen != NULL; sen = sen->next) {
        sen->create_window(&inst);
        sen->surface = sen->create_surface(&inst);
        AppDumpSurfaceExtension(&inst, gpus, gpu_count, sen, &format_count, &present_mode_count, out);
    }

    if (!inst.surface_ext_infos_root) {
        if (html_output) {
            fprintf(out, "\t\t\t\t<details><summary>None found</summary></details>\n");
        } else if (human_readable_output) {
            printf("None found\n\n");
        }
    }

    if (html_output) {
        fprintf(out, "\t\t\t</details>\n");
    }
    //---------

    if (CheckExtensionEnabled(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME, inst.inst_extensions, inst.inst_extensions_count)) {
        PFN_vkEnumeratePhysicalDeviceGroupsKHR vkEnumeratePhysicalDeviceGroupsKHR =
            (PFN_vkEnumeratePhysicalDeviceGroupsKHR)vkGetInstanceProcAddr(inst.instance, "vkEnumeratePhysicalDeviceGroupsKHR");

        uint32_t group_count;
        err = vkEnumeratePhysicalDeviceGroupsKHR(inst.instance, &group_count, NULL);
        if (err) {
            ERR_EXIT(err);
        }

        VkPhysicalDeviceGroupProperties *groups = malloc(sizeof(groups[0]) * group_count);
        if (!groups) {
            ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
        }

        err = vkEnumeratePhysicalDeviceGroupsKHR(inst.instance, &group_count, groups);
        if (err) {
            ERR_EXIT(err);
        }

        if (html_output) {
            fprintf(out, "\t\t\t<details><summary>Groups</summary>\n");
        } else if (human_readable_output) {
            printf("\nGroups :\n");
            printf("========\n");
        }

        for (uint32_t i = 0; i < group_count; ++i) {
            AppGroupDump(&groups[i], i, &inst, out);
            if (human_readable_output) {
                printf("\n\n");
            }
        }

        if (html_output) {
            fprintf(out, "\t\t\t</details>\n");
        }

        free(groups);
    }

    if (html_output) {
        fprintf(out, "\t\t\t<details><summary>Device Properties and Extensions</summary>\n");
    }

    for (uint32_t i = 0; i < gpu_count; ++i) {
        if (json_output && selected_gpu != i) {
            // Toggle json_output to allow html output without json output
            json_output = false;
            AppGpuDump(&gpus[i], out);
            json_output = true;
        } else {
            AppGpuDump(&gpus[i], out);
        }
        if (human_readable_output) {
            printf("\n\n");
        }
    }

    if (html_output) {
        fprintf(out, "\t\t\t</details>\n");
    }

    for (uint32_t i = 0; i < gpu_count; ++i) {
        AppGpuDestroy(&gpus[i]);
    }
    free(gpus);
    free(objs);

    for (struct SurfaceExtensionNode *sen = inst.surface_ext_infos_root; sen != NULL; sen = sen->next) {
        AppDestroySurface(&inst, sen->surface);
        sen->destroy_window(&inst);
    }

    AppDestroyInstance(&inst);

    if (html_output) {
        PrintHtmlFooter(out);
        fflush(out);
        fclose(out);
    }
    if (json_output) {
        printf("\n}\n");
    }

    fflush(stdout);

#ifdef _WIN32
    if (ConsoleIsExclusive() && human_readable_output) {
        Sleep(INFINITE);
    }
#endif

    return 0;
}
