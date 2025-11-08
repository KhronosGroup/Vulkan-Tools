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
#if defined(_WIN32)
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
#if defined(VK_USE_PLATFORM_DIRECTFB_EXT)
PFN_vkCreateDirectFBSurfaceEXT vkCreateDirectFBSurfaceEXT;
#endif
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
PFN_vkCreateScreenSurfaceQNX vkCreateScreenSurfaceQNX;
#endif

// Instance functions
PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
PFN_vkCreateDevice vkCreateDevice;
PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
PFN_vkDestroyInstance vkDestroyInstance;
PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
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
PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR;

// While technically a device function, it needs to be loaded with get instance proc addr
PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;

// Device functions
PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
PFN_vkAllocateMemory vkAllocateMemory;
PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
PFN_vkBindBufferMemory vkBindBufferMemory;
PFN_vkBindImageMemory vkBindImageMemory;
PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
PFN_vkCmdBindPipeline vkCmdBindPipeline;
PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
PFN_vkCmdDraw vkCmdDraw;
PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
PFN_vkCmdSetScissor vkCmdSetScissor;
PFN_vkCmdSetViewport vkCmdSetViewport;
PFN_vkCreateBuffer vkCreateBuffer;
PFN_vkCreateCommandPool vkCreateCommandPool;
PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
PFN_vkCreateFence vkCreateFence;
PFN_vkCreateFramebuffer vkCreateFramebuffer;
PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
PFN_vkCreateImage vkCreateImage;
PFN_vkCreateImageView vkCreateImageView;
PFN_vkCreatePipelineCache vkCreatePipelineCache;
PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
PFN_vkCreateRenderPass vkCreateRenderPass;
PFN_vkCreateSampler vkCreateSampler;
PFN_vkCreateSemaphore vkCreateSemaphore;
PFN_vkCreateShaderModule vkCreateShaderModule;
PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
PFN_vkDestroyBuffer vkDestroyBuffer;
PFN_vkDestroyCommandPool vkDestroyCommandPool;
PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
PFN_vkDestroyDevice vkDestroyDevice;
PFN_vkDestroyFence vkDestroyFence;
PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
PFN_vkDestroyImage vkDestroyImage;
PFN_vkDestroyImageView vkDestroyImageView;
PFN_vkDestroyPipeline vkDestroyPipeline;
PFN_vkDestroyPipelineCache vkDestroyPipelineCache;
PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
PFN_vkDestroyRenderPass vkDestroyRenderPass;
PFN_vkDestroySampler vkDestroySampler;
PFN_vkDestroySemaphore vkDestroySemaphore;
PFN_vkDestroyShaderModule vkDestroyShaderModule;
PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
PFN_vkEndCommandBuffer vkEndCommandBuffer;
PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
PFN_vkFreeMemory vkFreeMemory;
PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
PFN_vkGetDeviceQueue vkGetDeviceQueue;
PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout;
PFN_vkGetPastPresentationTimingGOOGLE vkGetPastPresentationTimingGOOGLE;
PFN_vkGetRefreshCycleDurationGOOGLE vkGetRefreshCycleDurationGOOGLE;
PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
PFN_vkMapMemory vkMapMemory;
PFN_vkQueuePresentKHR vkQueuePresentKHR;
PFN_vkQueueSubmit vkQueueSubmit;
PFN_vkResetCommandBuffer vkResetCommandBuffer;
PFN_vkResetFences vkResetFences;
PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
PFN_vkUnmapMemory vkUnmapMemory;
PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
PFN_vkWaitForFences vkWaitForFences;

#define LOAD_INSTANCE_FUNCTION(instance, function) function = (PFN_##function)vkGetInstanceProcAddr(instance, #function)
#define LOAD_DEVICE_FUNCTION(device, function) function = (PFN_##function)vkGetDeviceProcAddr(device, #function)

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
#if defined(VK_USE_PLATFORM_DIRECTFB_EXT)
    LOAD_INSTANCE_FUNCTION(instance, vkCreateDirectFBSurfaceEXT);
#endif
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
    LOAD_INSTANCE_FUNCTION(instance, vkCreateScreenSurfaceQNX);
#endif
    LOAD_INSTANCE_FUNCTION(instance, vkCreateDebugUtilsMessengerEXT);
    LOAD_INSTANCE_FUNCTION(instance, vkCreateDevice);
    LOAD_INSTANCE_FUNCTION(instance, vkDestroyDebugUtilsMessengerEXT);
    LOAD_INSTANCE_FUNCTION(instance, vkDestroyInstance);
    LOAD_INSTANCE_FUNCTION(instance, vkDestroySurfaceKHR);
    LOAD_INSTANCE_FUNCTION(instance, vkEnumerateDeviceExtensionProperties);
    LOAD_INSTANCE_FUNCTION(instance, vkEnumeratePhysicalDevices);
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
    LOAD_INSTANCE_FUNCTION(instance, vkGetDeviceProcAddr);
    LOAD_INSTANCE_FUNCTION(instance, vkCreateDisplayPlaneSurfaceKHR);
}
static void load_vulkan_device_functions(VkDevice device) {
    LOAD_DEVICE_FUNCTION(device, vkAcquireNextImageKHR);
    LOAD_DEVICE_FUNCTION(device, vkAllocateCommandBuffers);
    LOAD_DEVICE_FUNCTION(device, vkAllocateDescriptorSets);
    LOAD_DEVICE_FUNCTION(device, vkAllocateMemory);
    LOAD_DEVICE_FUNCTION(device, vkBeginCommandBuffer);
    LOAD_DEVICE_FUNCTION(device, vkBindBufferMemory);
    LOAD_DEVICE_FUNCTION(device, vkBindImageMemory);
    LOAD_DEVICE_FUNCTION(device, vkCmdBeginDebugUtilsLabelEXT);
    LOAD_DEVICE_FUNCTION(device, vkCmdBeginRenderPass);
    LOAD_DEVICE_FUNCTION(device, vkCmdBindDescriptorSets);
    LOAD_DEVICE_FUNCTION(device, vkCmdBindPipeline);
    LOAD_DEVICE_FUNCTION(device, vkCmdCopyBufferToImage);
    LOAD_DEVICE_FUNCTION(device, vkCmdDraw);
    LOAD_DEVICE_FUNCTION(device, vkCmdEndDebugUtilsLabelEXT);
    LOAD_DEVICE_FUNCTION(device, vkCmdEndRenderPass);
    LOAD_DEVICE_FUNCTION(device, vkCmdPipelineBarrier);
    LOAD_DEVICE_FUNCTION(device, vkCmdSetScissor);
    LOAD_DEVICE_FUNCTION(device, vkCmdSetViewport);
    LOAD_DEVICE_FUNCTION(device, vkCreateBuffer);
    LOAD_DEVICE_FUNCTION(device, vkCreateCommandPool);
    LOAD_DEVICE_FUNCTION(device, vkCreateDescriptorPool);
    LOAD_DEVICE_FUNCTION(device, vkCreateDescriptorSetLayout);
    LOAD_DEVICE_FUNCTION(device, vkCreateFence);
    LOAD_DEVICE_FUNCTION(device, vkCreateFramebuffer);
    LOAD_DEVICE_FUNCTION(device, vkCreateGraphicsPipelines);
    LOAD_DEVICE_FUNCTION(device, vkCreateImage);
    LOAD_DEVICE_FUNCTION(device, vkCreateImageView);
    LOAD_DEVICE_FUNCTION(device, vkCreatePipelineCache);
    LOAD_DEVICE_FUNCTION(device, vkCreatePipelineLayout);
    LOAD_DEVICE_FUNCTION(device, vkCreateRenderPass);
    LOAD_DEVICE_FUNCTION(device, vkCreateSampler);
    LOAD_DEVICE_FUNCTION(device, vkCreateSemaphore);
    LOAD_DEVICE_FUNCTION(device, vkCreateShaderModule);
    LOAD_DEVICE_FUNCTION(device, vkCreateSwapchainKHR);
    LOAD_DEVICE_FUNCTION(device, vkDestroyBuffer);
    LOAD_DEVICE_FUNCTION(device, vkDestroyCommandPool);
    LOAD_DEVICE_FUNCTION(device, vkDestroyDescriptorPool);
    LOAD_DEVICE_FUNCTION(device, vkDestroyDescriptorSetLayout);
    LOAD_DEVICE_FUNCTION(device, vkDestroyDevice);
    LOAD_DEVICE_FUNCTION(device, vkDestroyFence);
    LOAD_DEVICE_FUNCTION(device, vkDestroyFramebuffer);
    LOAD_DEVICE_FUNCTION(device, vkDestroyImage);
    LOAD_DEVICE_FUNCTION(device, vkDestroyImageView);
    LOAD_DEVICE_FUNCTION(device, vkDestroyPipeline);
    LOAD_DEVICE_FUNCTION(device, vkDestroyPipelineCache);
    LOAD_DEVICE_FUNCTION(device, vkDestroyPipelineLayout);
    LOAD_DEVICE_FUNCTION(device, vkDestroyRenderPass);
    LOAD_DEVICE_FUNCTION(device, vkDestroySampler);
    LOAD_DEVICE_FUNCTION(device, vkDestroySemaphore);
    LOAD_DEVICE_FUNCTION(device, vkDestroyShaderModule);
    LOAD_DEVICE_FUNCTION(device, vkDestroySwapchainKHR);
    LOAD_DEVICE_FUNCTION(device, vkDeviceWaitIdle);
    LOAD_DEVICE_FUNCTION(device, vkEndCommandBuffer);
    LOAD_DEVICE_FUNCTION(device, vkFreeCommandBuffers);
    LOAD_DEVICE_FUNCTION(device, vkFreeMemory);
    LOAD_DEVICE_FUNCTION(device, vkGetBufferMemoryRequirements);
    LOAD_DEVICE_FUNCTION(device, vkGetDeviceProcAddr);
    LOAD_DEVICE_FUNCTION(device, vkGetDeviceQueue);
    LOAD_DEVICE_FUNCTION(device, vkGetImageMemoryRequirements);
    LOAD_DEVICE_FUNCTION(device, vkGetImageSubresourceLayout);
    LOAD_DEVICE_FUNCTION(device, vkGetPastPresentationTimingGOOGLE);
    LOAD_DEVICE_FUNCTION(device, vkGetRefreshCycleDurationGOOGLE);
    LOAD_DEVICE_FUNCTION(device, vkGetSwapchainImagesKHR);
    LOAD_DEVICE_FUNCTION(device, vkMapMemory);
    LOAD_DEVICE_FUNCTION(device, vkQueuePresentKHR);
    LOAD_DEVICE_FUNCTION(device, vkQueueSubmit);
    LOAD_DEVICE_FUNCTION(device, vkResetCommandBuffer);
    LOAD_DEVICE_FUNCTION(device, vkResetFences);
    LOAD_DEVICE_FUNCTION(device, vkSetDebugUtilsObjectNameEXT);
    LOAD_DEVICE_FUNCTION(device, vkUnmapMemory);
    LOAD_DEVICE_FUNCTION(device, vkUpdateDescriptorSets);
    LOAD_DEVICE_FUNCTION(device, vkWaitForFences);
}

#undef LOAD_INSTANCE_FUNCTION
#undef LOAD_DEVICE_FUNCTION
