/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 Valve Corporation
 * Copyright (c) 2025 LunarG, Inc.
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
#pragma once

// Need to include relevant system headers to open the dynamic library
#if defined(WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <stdlib.h>
#include <dlfcn.h>
#else
#include <dlfcn.h>
#endif

// We are providing storage for all functions, make sure the vulkan headers doesn't conflate with ours.
#if !defined(VK_NO_PROTOTYPES)
#define VK_NO_PROTOTYPES
#endif

#include <vulkan/vulkan.h>

void* vulkan_library;

// Global Functions
PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
PFN_vkCreateInstance vkCreateInstance;

// Per-platform Instance functions
#if defined(VK_USE_PLATFORM_WIN32_KHR)
PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
#endif
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR;
#endif
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
#endif
#if defined(VK_USE_PLATFORM_XLIB_KHR)
PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;
#endif
#if defined(VK_USE_PLATFORM_XCB_KHR)
PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR;
#endif
#if defined(VK_USE_PLATFORM_METAL_EXT)
PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT;
#endif
#if defined(VK_USE_PLATFORM_MACOS_MVK)
PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK;
#endif
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
PFN_vkCreateScreenSurfaceQNX vkCreateScreenSurfaceQNX;
#endif
#if defined(VK_USE_PLATFORM_DIRECTFB_EXT)
PFN_vkCreateDirectFBSurfaceEXT vkCreateDirectFBSurfaceEXT;
#endif
#if defined(VK_USE_PLATFORM_GGP)
PFN_vkCreateStreamDescriptorSurfaceGGP vkCreateStreamDescriptorSurfaceGGP;
#endif
// Instance functions
PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
PFN_vkCreateDevice vkCreateDevice;
PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;
PFN_vkDestroyInstance vkDestroyInstance;
PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR;
PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR;
PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR;
PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR;
PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR vkGetPhysicalDeviceVideoCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormats2KHR vkGetPhysicalDeviceSurfaceFormats2KHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR;
PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR vkGetPhysicalDeviceVideoFormatPropertiesKHR;
PFN_vkEnumeratePhysicalDeviceGroupsKHR vkEnumeratePhysicalDeviceGroupsKHR;
PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR;
PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties;
PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR;
PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR;
PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR;
PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR vkGetPhysicalDeviceQueueFamilyProperties2KHR;
PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vkGetPhysicalDeviceSurfaceCapabilities2EXT;
PFN_vkGetPhysicalDeviceToolPropertiesEXT vkGetPhysicalDeviceToolPropertiesEXT;
PFN_vkGetPhysicalDeviceFormatProperties2KHR vkGetPhysicalDeviceFormatProperties2KHR;

// Device functions
PFN_vkCreateImage vkCreateImage;
PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
PFN_vkDestroyImage vkDestroyImage;
PFN_vkDestroyDevice vkDestroyDevice;

#define LOAD_INSTANCE_FUNCTION(instance, function) function = (PFN_##function)vkGetInstanceProcAddr(instance, #function)

static VkResult load_vulkan_library() {
    vulkan_library = NULL;
#if defined(WIN32)
    HMODULE library = LoadLibraryA("vulkan-1.dll");
    if (library == NULL) return VK_ERROR_INITIALIZATION_FAILED;

    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)(void (*)(void))GetProcAddress(library, "vkGetInstanceProcAddr");
#elif defined(__APPLE__)
    void* library = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
    if (library == NULL) {
        library = dlopen("libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL);
    }
    if (library == NULL && getenv("DYLD_FALLBACK_LIBRARY_PATH") == NULL) {
        library = dlopen("/usr/local/lib/libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
        if (library == NULL) {
            library = dlopen("/usr/local/lib/libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL);
        }
    }
    if (library == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(library, "vkGetInstanceProcAddr");
#else  // Assume everything else supports dlopen
    void* library = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
    if (library == NULL) library = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (library == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(library, "vkGetInstanceProcAddr");
#endif
    if (vkGetInstanceProcAddr == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    vulkan_library = library;
    LOAD_INSTANCE_FUNCTION(NULL, vkEnumerateInstanceExtensionProperties);
    LOAD_INSTANCE_FUNCTION(NULL, vkEnumerateInstanceLayerProperties);
    LOAD_INSTANCE_FUNCTION(NULL, vkEnumerateInstanceVersion);
    LOAD_INSTANCE_FUNCTION(NULL, vkCreateInstance);

    return VK_SUCCESS;
}

static void unload_vulkan_library() {
    if (vulkan_library != NULL) {
#if defined(WIN32)
        FreeLibrary((HMODULE)vulkan_library);
#else
        dlclose(vulkan_library);
#endif
        vulkan_library = NULL;
    }
}

static void load_vulkan_instance_functions(VkInstance instance) {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    LOAD_INSTANCE_FUNCTION(instance, vkCreateWin32SurfaceKHR);
#endif
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
    LOAD_INSTANCE_FUNCTION(instance, vkCreateWaylandSurfaceKHR);
#endif
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    LOAD_INSTANCE_FUNCTION(instance, vkCreateAndroidSurfaceKHR);
#endif
#if defined(VK_USE_PLATFORM_XLIB_KHR)
    LOAD_INSTANCE_FUNCTION(instance, vkCreateXlibSurfaceKHR);
#endif
#if defined(VK_USE_PLATFORM_XCB_KHR)
    LOAD_INSTANCE_FUNCTION(instance, vkCreateXcbSurfaceKHR);
#endif
#if defined(VK_USE_PLATFORM_METAL_EXT)
    LOAD_INSTANCE_FUNCTION(instance, vkCreateMetalSurfaceEXT);
#endif
#if defined(VK_USE_PLATFORM_MACOS_MVK)
    LOAD_INSTANCE_FUNCTION(instance, vkCreateMacOSSurfaceMVK);
#endif
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
    LOAD_INSTANCE_FUNCTION(instance, vkCreateScreenSurfaceQNX);
#endif
#if defined(VK_USE_PLATFORM_DIRECTFB_EXT)
    LOAD_INSTANCE_FUNCTION(instance, vkCreateDirectFBSurfaceEXT);
#endif
#if defined(VK_USE_PLATFORM_GGP)
    LOAD_INSTANCE_FUNCTION(instance, vkCreateStreamDescriptorSurfaceGGP);
#endif

    LOAD_INSTANCE_FUNCTION(instance, vkCreateDebugReportCallbackEXT);
    LOAD_INSTANCE_FUNCTION(instance, vkCreateDevice);
    LOAD_INSTANCE_FUNCTION(instance, vkDestroyDebugReportCallbackEXT);
    LOAD_INSTANCE_FUNCTION(instance, vkDestroyInstance);
    LOAD_INSTANCE_FUNCTION(instance, vkDestroySurfaceKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkEnumerateDeviceExtensionProperties);
    LOAD_INSTANCE_FUNCTION(instance, vkGetDisplayModePropertiesKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetDisplayPlaneCapabilitiesKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetDisplayPlaneSupportedDisplaysKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceDisplayPlanePropertiesKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceDisplayPropertiesKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceFeatures);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceFormatProperties);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceMemoryProperties);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceProperties);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceQueueFamilyProperties);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceSurfaceFormatsKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceSurfacePresentModesKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceSurfaceSupportKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceVideoCapabilitiesKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceSurfaceFormats2KHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceSurfaceCapabilities2KHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceVideoFormatPropertiesKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkEnumeratePhysicalDeviceGroupsKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetDeviceGroupPresentCapabilitiesKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceImageFormatProperties);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceProperties2KHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceMemoryProperties2KHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceFeatures2KHR);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceQueueFamilyProperties2KHR);
    LOAD_INSTANCE_FUNCTION(instance, vkEnumeratePhysicalDevices);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceSurfaceCapabilities2EXT);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceToolPropertiesEXT);
    LOAD_INSTANCE_FUNCTION(instance, vkGetPhysicalDeviceFormatProperties2KHR);

    // Load device functions using vkGetInstanceProcAddr, vulkaninfo doesn't care about the extra indirection it causes
    LOAD_INSTANCE_FUNCTION(instance, vkCreateImage);
    LOAD_INSTANCE_FUNCTION(instance, vkGetImageMemoryRequirements);
    LOAD_INSTANCE_FUNCTION(instance, vkDestroyImage);
    LOAD_INSTANCE_FUNCTION(instance, vkDestroyDevice);
}

#undef LOAD_INSTANCE_FUNCTION
