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
 * Author: Charles Giessen <charles@lunarg.com>
 *
 */

#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>

#ifdef __GNUC__
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#else
#define strndup(p, n) strdup(p)
#endif

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

#define ERR(err) std::cerr << __FILE__ << ":" << __LINE__ << ": failed with " << VkResultString(err) << "\n";

// global configuration
bool human_readable_output = true;
bool html_output = false;
bool json_output = false;
bool show_formats = false;

#ifdef _WIN32

#define strdup _strdup

// Returns nonzero if the console is used only for this process. Will return
// zero if another process (such as cmd.exe) is also attached.
static int ConsoleIsExclusive(void) {
    DWORD pids[2];
    DWORD num_pids = GetConsoleProcessList(pids, ARRAYSIZE(pids));
    return num_pids <= 1;
}

#define WAIT_FOR_CONSOLE_DESTROY                                            \
    do {                                                                    \
        if (ConsoleIsExclusive() && human_readable_output) Sleep(INFINITE); \
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

static const char *VkResultString(VkResult err);

const char *app_short_name = "vulkaninfo";

std::vector<const char *> get_c_str_array(std::vector<std::string> const &vec) {
    std::vector<const char *> ret;
    for (auto &str : vec) ret.push_back(str.c_str());
    return ret;
}

static const char *VkDebugReportFlagsEXTString(const VkDebugReportFlagsEXT flags) {
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
    std::cerr << VkDebugReportFlagsEXTString(msgFlags) << ": [" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg << "\n";

    // True is reserved for layer developers, and MAY mean calls are not distributed down the layer chain after validation error.
    // False SHOULD always be returned by apps:
    return VK_FALSE;
}

// ----------- Instance Setup ------- //

struct VkStructureHeader {
    VkStructureType sType;
    VkStructureHeader *pNext;
};

struct pNextChainBuildingBlockInfo {
    VkStructureType sType;
    uint32_t mem_size;
};

void buildpNextChain(VkStructureHeader *first, const std::vector<pNextChainBuildingBlockInfo> &chain_info) {
    VkStructureHeader *place = first;

    for (uint32_t i = 0; i < chain_info.size(); i++) {
        place->pNext = static_cast<VkStructureHeader *>(malloc(chain_info[i].mem_size));
        if (!place->pNext) {
            ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
        }
        std::memset(place->pNext, 0, chain_info[i].mem_size);
        place = place->pNext;
        place->sType = chain_info[i].sType;
    }

    place->pNext = nullptr;
}

void freepNextChain(VkStructureHeader *first) {
    VkStructureHeader *place = first;
    VkStructureHeader *next = nullptr;

    while (place) {
        next = place->pNext;
        free(place);
        place = next;
    }
}

struct LayerExtensionList {
    VkLayerProperties layer_properties;
    std::vector<VkExtensionProperties> extension_properties;
};

struct AppInstance;

struct SurfaceExtension {
    std::string name;
    void (*create_window)(AppInstance &);
    VkSurfaceKHR (*create_surface)(AppInstance &);
    void (*destroy_window)(AppInstance &);
    VkSurfaceKHR surface;
    VkBool32 supports_present = 0;

    bool operator==(const SurfaceExtension &other) {
        return name == other.name && surface == other.surface && supports_present == other.supports_present;
    }
};

struct VulkanVersion {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

struct AppInstance {
    VkInstance instance;
    uint32_t instance_version;
    VulkanVersion vk_version;

    std::vector<LayerExtensionList> global_layers;

    std::vector<VkExtensionProperties> global_extensions;  // Instance Extensions

    std::vector<std::string> inst_extensions;

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

    std::vector<SurfaceExtension> surface_extensions;

    int width = 256, height = 256;

    VkSurfaceCapabilitiesKHR surface_capabilities;

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
#ifdef VK_USE_PLATFORM_MACOS_MVK
    void *window;
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    wl_display *wayland_display;
    wl_surface *wayland_surface;
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR  // TODO
    ANativeWindow *window;
#endif
    AppInstance() {
        PFN_vkEnumerateInstanceVersion enumerate_instance_version =
            (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion");

        if (!enumerate_instance_version) {
            instance_version = VK_API_VERSION_1_0;
        } else {
            const VkResult err = enumerate_instance_version(&instance_version);
            if (err) ERR_EXIT(err);
        }

        vk_version = {VK_VERSION_MAJOR(instance_version), VK_VERSION_MINOR(instance_version), VK_VERSION_PATCH(VK_HEADER_VERSION)};

        AppGetInstanceExtensions();

        const VkDebugReportCallbackCreateInfoEXT dbg_info = {VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT, nullptr,
                                                             VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
                                                             DbgCallback};

        const VkApplicationInfo app_info = {
            VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, app_short_name, 1, nullptr, 0, VK_API_VERSION_1_0};

        AppCompileInstanceExtensionsToEnable();

        std::vector<const char *> inst_exts;
        for (auto &ext : inst_extensions) inst_exts.push_back(ext.c_str());

        const VkInstanceCreateInfo inst_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,  &dbg_info,       0, &app_info, 0, nullptr,
                                                static_cast<uint32_t>(inst_exts.size()), inst_exts.data()};

        VkResult err = vkCreateInstance(&inst_info, nullptr, &instance);
        if (err == VK_ERROR_INCOMPATIBLE_DRIVER) {
            std::cerr << "Cannot create Vulkan instance.\n";
            std::cerr << "This problem is often caused by a faulty installation of the Vulkan driver or attempting to use a GPU "
                         "that does not support Vulkan.\n";
            ERR_EXIT(err);
        } else if (err) {
            ERR_EXIT(err);
        }

        AppLoadInstanceCommands();
    }

    ~AppInstance() { vkDestroyInstance(instance, nullptr); }

    bool CheckExtensionEnabled(std::string extension_to_check) {
        for (auto &extension : inst_extensions) {
            if (extension_to_check == extension) {
                return true;
            }
        }
        return false;
    }

    /* Gets a list of layer and instance extensions */
    void AppGetInstanceExtensions() {
        /* Scan layers */
        std::vector<VkLayerProperties> global_layer_properties;

        VkResult err;
        uint32_t count = 0;
        do {
            err = vkEnumerateInstanceLayerProperties(&count, nullptr);
            if (err) ERR_EXIT(err);

            global_layer_properties.resize(count);

            err = vkEnumerateInstanceLayerProperties(&count, global_layer_properties.data());
        } while (err == VK_INCOMPLETE);
        if (err) ERR_EXIT(err);

        global_layers.resize(count);
        assert(global_layer_properties.size() == global_layers.size());

        for (size_t i = 0; i < global_layer_properties.size(); i++) {
            global_layers[i].layer_properties = global_layer_properties[i];

            global_layers[i].extension_properties = AppGetGlobalLayerExtensions(global_layer_properties[i].layerName);
        }

        // Collect global extensions
        // Gets instance extensions, if no layer was specified in the first paramteter
        global_extensions = AppGetGlobalLayerExtensions(nullptr);
    }
    void AppCompileInstanceExtensionsToEnable() {
        // Get all supported Instance extensions (excl. layer-provided ones)
        for (auto &ext : global_extensions) {
            inst_extensions.push_back(ext.extensionName);
        }
    }
    void AppLoadInstanceCommands() {
#define LOAD_INSTANCE_VK_CMD(cmd) cmd = (PFN_##cmd)vkGetInstanceProcAddr(instance, #cmd)

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

    void AddSurfaceExtension(SurfaceExtension ext) { surface_extensions.push_back(ext); }

    static std::vector<VkExtensionProperties> AppGetGlobalLayerExtensions(char *layer_name) {
        std::vector<VkExtensionProperties> ext_props;
        VkResult err;
        uint32_t ext_count = 0;
        do {
            // gets the extension count if the last parameter is nullptr
            err = vkEnumerateInstanceExtensionProperties(layer_name, &ext_count, nullptr);
            if (err) ERR_EXIT(err);

            ext_props.resize(ext_count);
            // gets the extension properties if the last parameter is not nullptr
            err = vkEnumerateInstanceExtensionProperties(layer_name, &ext_count, ext_props.data());
        } while (err == VK_INCOMPLETE);
        if (err) ERR_EXIT(err);
        return ext_props;
    }

    std::vector<VkPhysicalDevice> FindPhysicalDevices() {
        std::vector<VkPhysicalDevice> phys_devices;

        VkResult err;
        uint32_t gpu_count = 0;

        /* repeat get until VK_INCOMPLETE goes away */
        do {
            err = vkEnumeratePhysicalDevices(instance, &gpu_count, nullptr);
            if (err) ERR_EXIT(err);

            phys_devices.resize(gpu_count);
            err = vkEnumeratePhysicalDevices(instance, &gpu_count, phys_devices.data());
            if (err) ERR_EXIT(err);
            phys_devices.resize(gpu_count);
        } while (err == VK_INCOMPLETE);

        return phys_devices;
    }
};

// --------- Platform Specific Presentation Calls --------- //

//---------------------------Win32---------------------------
#ifdef VK_USE_PLATFORM_WIN32_KHR

// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return (DefWindowProc(hWnd, uMsg, wParam, lParam)); }

static void AppCreateWin32Window(AppInstance &inst) {
    inst.h_instance = GetModuleHandle(nullptr);

    WNDCLASSEX win_class;

    // Initialize the window class structure:
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = WndProc;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = inst.h_instance;
    win_class.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    win_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
    win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    win_class.lpszMenuName = nullptr;
    win_class.lpszClassName = app_short_name;
    win_class.hInstance = inst.h_instance;
    win_class.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);
    // Register window class:
    if (!RegisterClassEx(&win_class)) {
        // It didn't work, so try to give a useful error:
        fprintf(stderr, "Failed to register the window class!\n");
        exit(1);
    }
    // Create window with the registered class:
    RECT wr = {0, 0, inst.width, inst.height};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    inst.h_wnd = CreateWindowEx(0,
                                app_short_name,  // class name
                                app_short_name,  // app name
                                // WS_VISIBLE | WS_SYSMENU |
                                WS_OVERLAPPEDWINDOW,  // window style
                                100, 100,             // x/y coords
                                wr.right - wr.left,   // width
                                wr.bottom - wr.top,   // height
                                nullptr,              // handle to parent
                                nullptr,              // handle to menu
                                inst.h_instance,      // hInstance
                                nullptr);             // no extra parameters
    if (!inst.h_wnd) {
        // It didn't work, so try to give a useful error:
        fprintf(stderr, "Failed to create a window!\n");
        exit(1);
    }
}

static VkSurfaceKHR AppCreateWin32Surface(AppInstance &inst) {
    VkWin32SurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.hinstance = inst.h_instance;
    createInfo.hwnd = inst.h_wnd;

    VkSurfaceKHR surface;
    VkResult err = vkCreateWin32SurfaceKHR(inst.instance, &createInfo, nullptr, &surface);
    if (err) ERR_EXIT(err);
    return surface;
}

static void AppDestroyWin32Window(AppInstance &inst) { DestroyWindow(inst.h_wnd); }
#endif  // VK_USE_PLATFORM_WIN32_KHR
//-----------------------------------------------------------

#if defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WIN32_KHR) || \
    defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_WAYLAND_KHR) || defined(VK_USE_PLATFORM_ANDROID_KHR)
static void AppDestroySurface(AppInstance &inst, VkSurfaceKHR surface) {  // same for all platforms
    vkDestroySurfaceKHR(inst.instance, surface, nullptr);
}
#endif

//----------------------------XCB----------------------------

#ifdef VK_USE_PLATFORM_XCB_KHR
static void AppCreateXcbWindow(AppInstance &inst) {
    //--Init Connection--
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    // API guarantees non-null xcb_connection
    inst.xcb_connection = xcb_connect(nullptr, &scr);
    int conn_error = xcb_connection_has_error(inst.xcb_connection);
    if (conn_error) {
        fprintf(stderr, "XCB failed to connect to the X server due to error:%d.\n", conn_error);
        fflush(stderr);
        inst.xcb_connection = nullptr;
    }

    setup = xcb_get_setup(inst.xcb_connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0) {
        xcb_screen_next(&iter);
    }

    inst.xcb_screen = iter.data;
    //-------------------

    inst.xcb_window = xcb_generate_id(inst.xcb_connection);
    xcb_create_window(inst.xcb_connection, XCB_COPY_FROM_PARENT, inst.xcb_window, inst.xcb_screen->root, 0, 0, inst.width,
                      inst.height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, inst.xcb_screen->root_visual, 0, nullptr);

    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(inst.xcb_connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(inst.xcb_connection, cookie, 0);
    free(reply);
}

static VkSurfaceKHR AppCreateXcbSurface(AppInstance &inst) {
    if (!inst.xcb_connection) {
        ERR_EXIT(VK_ERROR_INITIALIZATION_FAILED);
    }

    VkXcbSurfaceCreateInfoKHR xcb_createInfo;
    xcb_createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    xcb_createInfo.pNext = nullptr;
    xcb_createInfo.flags = 0;
    xcb_createInfo.connection = inst.xcb_connection;
    xcb_createInfo.window = inst.xcb_window;

    VkSurfaceKHR surface;
    VkResult err = vkCreateXcbSurfaceKHR(inst.instance, &xcb_createInfo, nullptr, &surface);
    if (err) ERR_EXIT(err);
    return surface;
}

static void AppDestroyXcbWindow(AppInstance &inst) {
    if (!inst.xcb_connection) {
        return;  // Nothing to destroy
    }

    xcb_destroy_window(inst.xcb_connection, inst.xcb_window);
    xcb_disconnect(inst.xcb_connection);
}
#endif  // VK_USE_PLATFORM_XCB_KHR
//-----------------------------------------------------------

//----------------------------XLib---------------------------
#ifdef VK_USE_PLATFORM_XLIB_KHR
static void AppCreateXlibWindow(AppInstance &inst) {
    long visualMask = VisualScreenMask;
    int numberOfVisuals;

    inst.xlib_display = XOpenDisplay(nullptr);
    if (inst.xlib_display == nullptr) {
        fprintf(stderr, "XLib failed to connect to the X server.\nExiting ...\n");
        exit(1);
    }

    XVisualInfo vInfoTemplate = {};
    vInfoTemplate.screen = DefaultScreen(inst.xlib_display);
    XVisualInfo *visualInfo = XGetVisualInfo(inst.xlib_display, visualMask, &vInfoTemplate, &numberOfVisuals);
    inst.xlib_window = XCreateWindow(inst.xlib_display, RootWindow(inst.xlib_display, vInfoTemplate.screen), 0, 0, inst.width,
                                     inst.height, 0, visualInfo->depth, InputOutput, visualInfo->visual, 0, nullptr);

    XSync(inst.xlib_display, false);
    XFree(visualInfo);
}

static VkSurfaceKHR AppCreateXlibSurface(AppInstance &inst) {
    VkXlibSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.dpy = inst.xlib_display;
    createInfo.window = inst.xlib_window;

    VkSurfaceKHR surface;
    VkResult err = vkCreateXlibSurfaceKHR(inst.instance, &createInfo, nullptr, &surface);
    if (err) ERR_EXIT(err);
    return surface;
}

static void AppDestroyXlibWindow(AppInstance &inst) {
    XDestroyWindow(inst.xlib_display, inst.xlib_window);
    XCloseDisplay(inst.xlib_display);
}
#endif  // VK_USE_PLATFORM_XLIB_KHR
//-----------------------------------------------------------

//------------------------MACOS_MVK--------------------------
#ifdef VK_USE_PLATFORM_MACOS_MVK
static void AppCreateMacOSWindow(AppInstance &inst) {
    inst.window = CreateMetalView(inst.width, inst.height);
    if (inst.window == nullptr) {
        fprintf(stderr, "Could not create a native Metal view.\nExiting...\n");
        exit(1);
    }
}

static VkSurfaceKHR AppCreateMacOSSurface(AppInstance &inst) {
    VkMacOSSurfaceCreateInfoMVK createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.pView = inst.window;

    VkSurfaceKHR surface;
    VkResult err = vkCreateMacOSSurfaceMVK(inst.instance, &createInfo, nullptr, &surface);
    if (err) ERR_EXIT(err);
    return surface;
}

static void AppDestroyMacOSWindow(AppInstance &inst) { DestroyMetalView(inst.window); }
#endif  // VK_USE_PLATFORM_MACOS_MVK
//-----------------------------------------------------------

//-------------------------WAYLAND---------------------------
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
static void wayland_registry_global(void *data, struct wl_registry *registry, uint32_t id, const char *interface,
                                    uint32_t version) {
    AppInstance &inst = *static_cast<AppInstance *>(data);
    if (strcmp(interface, "wl_compositor") == 0) {
        struct wl_compositor *compositor = (struct wl_compositor *)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
        inst.wayland_surface = wl_compositor_create_surface(compositor);
    }
}
static void wayland_registry_global_remove(void *data, struct wl_registry *registry, uint32_t id) {}
static const struct wl_registry_listener wayland_registry_listener = {wayland_registry_global, wayland_registry_global_remove};

static void AppCreateWaylandWindow(AppInstance &inst) {
    inst.wayland_display = wl_display_connect(nullptr);
    struct wl_registry *registry = wl_display_get_registry(inst.wayland_display);
    wl_registry_add_listener(wl_display_get_registry(inst.wayland_display), &wayland_registry_listener, static_cast<void *>(&inst));
    wl_display_roundtrip(inst.wayland_display);
    wl_registry_destroy(registry);
}

static VkSurfaceKHR AppCreateWaylandSurface(AppInstance &inst) {
    VkWaylandSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.display = inst.wayland_display;
    createInfo.surface = inst.wayland_surface;

    VkSurfaceKHR surface;
    VkResult err = vkCreateWaylandSurfaceKHR(inst.instance, &createInfo, nullptr, &surface);
    if (err) ERR_EXIT(err);
    return surface;
}

static void AppDestroyWaylandWindow(AppInstance &inst) { wl_display_disconnect(inst.wayland_display); }
#endif  // VK_USE_PLATFORM_WAYLAND_KHR
//-----------------------------------------------------------

//-------------------------ANDROID---------------------------
#ifdef VK_USE_PLATFORM_ANDROID_KHR
static void AppCreateAndroidWindow(AppInstance &inst) {}
static VkSurfaceKHR AppCreateAndroidSurface(AppInstance &inst) {
    VkAndroidSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.window = (struct ANativeWindow *)(inst.window);

    err = vkCreateAndroidSurfaceKHR(inst.inst, &createInfo, NULL, &inst.surface);
    EXIT_ERR(err);
}
static VkSurfaceKHR AppDestroyAndroidSurface(AppInstance &inst) {}
#endif
//-----------------------------------------------------------

// ------------ Setup Windows ------------- //

void SetupWindowExtensions(AppInstance &inst) {
#if defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
    bool has_display = true;
    const char *display_var = getenv("DISPLAY");
    if (display_var == nullptr || strlen(display_var) == 0) {
        fprintf(stderr, "'DISPLAY' environment variable not set... skipping surface info\n");
        fflush(stderr);
        has_display = false;
    }
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    wl_display *wayland_display = wl_display_connect(nullptr);
    bool has_wayland_display = false;
    if (wayland_display != nullptr) {
        wl_display_disconnect(wayland_display);
        has_wayland_display = true;
    }
#endif

//--WIN32--
#ifdef VK_USE_PLATFORM_WIN32_KHR
    SurfaceExtension surface_ext_win32;
    if (inst.CheckExtensionEnabled(VK_KHR_WIN32_SURFACE_EXTENSION_NAME)) {
        surface_ext_win32.name = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
        surface_ext_win32.create_window = AppCreateWin32Window;
        surface_ext_win32.create_surface = AppCreateWin32Surface;
        surface_ext_win32.destroy_window = AppDestroyWin32Window;

        inst.AddSurfaceExtension(surface_ext_win32);
    }
#endif
//--XCB--
#ifdef VK_USE_PLATFORM_XCB_KHR
    SurfaceExtension surface_ext_xcb;
    if (inst.CheckExtensionEnabled(VK_KHR_XCB_SURFACE_EXTENSION_NAME)) {
        surface_ext_xcb.name = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
        surface_ext_xcb.create_window = AppCreateXcbWindow;
        surface_ext_xcb.create_surface = AppCreateXcbSurface;
        surface_ext_xcb.destroy_window = AppDestroyXcbWindow;
        if (has_display) {
            inst.AddSurfaceExtension(surface_ext_xcb);
        }
    }
#endif
//--XLIB--
#ifdef VK_USE_PLATFORM_XLIB_KHR
    SurfaceExtension surface_ext_xlib;
    if (inst.CheckExtensionEnabled(VK_KHR_XLIB_SURFACE_EXTENSION_NAME)) {
        surface_ext_xlib.name = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
        surface_ext_xlib.create_window = AppCreateXlibWindow;
        surface_ext_xlib.create_surface = AppCreateXlibSurface;
        surface_ext_xlib.destroy_window = AppDestroyXlibWindow;
        if (has_display) {
            inst.AddSurfaceExtension(surface_ext_xlib);
        }
    }
#endif
//--MACOS--
#ifdef VK_USE_PLATFORM_MACOS_MVK
    SurfaceExtension surface_ext_macos;
    if (inst.CheckExtensionEnabled(VK_MVK_MACOS_SURFACE_EXTENSION_NAME)) {
        surface_ext_macos.name = VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
        surface_ext_macos.create_window = AppCreateMacOSWindow;
        surface_ext_macos.create_surface = AppCreateMacOSSurface;
        surface_ext_macos.destroy_window = AppDestroyMacOSWindow;

        inst.AddSurfaceExtension(surface_ext_macos);
    }
#endif
//--WAYLAND--
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    SurfaceExtension surface_ext_wayland;
    if (inst.CheckExtensionEnabled(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME)) {
        surface_ext_wayland.name = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
        surface_ext_wayland.create_window = AppCreateWaylandWindow;
        surface_ext_wayland.create_surface = AppCreateWaylandSurface;
        surface_ext_wayland.destroy_window = AppDestroyWaylandWindow;
        if (has_wayland_display) {
            inst.AddSurfaceExtension(surface_ext_wayland);
        }
    }
#endif
//--ANDROID--
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    SurfaceExtension surface_ext_android;
    if (inst.CheckExtensionEnabled(VK_ANDROID_SURFACE_EXTENSION_NAME)) {
        surface_ext_android.name = VK_ANDROID_SURFACE_EXTENSION_NAME;
        surface_ext_android.create_window = AppCreateAndroidWindow;
        surface_ext_android.create_surface = AppCreateAndroidSurface;
        surface_ext_android.destroy_window = AppDestroyAndroidWindow;

        inst.AddSurfaceExtension(surface_ext_android);
    }
#endif
}

// ---------- Surfaces -------------- //

class AppSurface {
   public:
    AppInstance &inst;
    SurfaceExtension surface_extension;

    std::vector<VkPresentModeKHR> surf_present_modes;

    std::vector<VkSurfaceFormatKHR> surf_formats;
    std::vector<VkSurfaceFormat2KHR> surf_formats2;

    VkSurfaceCapabilitiesKHR surface_capabilities;
    VkSurfaceCapabilities2KHR surface_capabilities2_khr;
    VkSurfaceCapabilities2EXT surface_capabilities2_ext;

    AppSurface(AppInstance &inst, VkPhysicalDevice phys_device, SurfaceExtension surface_extension,
               std::vector<pNextChainBuildingBlockInfo> &sur_extension_pNextChain)
        : inst(inst), surface_extension(surface_extension) {
        uint32_t present_mode_count = 0;
        VkResult err =
            inst.vkGetPhysicalDeviceSurfacePresentModesKHR(phys_device, surface_extension.surface, &present_mode_count, nullptr);
        if (err) ERR_EXIT(err);

        surf_present_modes.resize(present_mode_count);
        err = inst.vkGetPhysicalDeviceSurfacePresentModesKHR(phys_device, surface_extension.surface, &present_mode_count,
                                                             surf_present_modes.data());
        if (err) ERR_EXIT(err);

        const VkPhysicalDeviceSurfaceInfo2KHR surface_info2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR, nullptr,
                                                               surface_extension.surface};

        uint32_t format_count = 0;
        if (inst.CheckExtensionEnabled(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME)) {
            VkResult err = inst.vkGetPhysicalDeviceSurfaceFormats2KHR(phys_device, &surface_info2, &format_count, nullptr);
            if (err) ERR_EXIT(err);
            surf_formats2.resize(format_count);
            for (uint32_t i = 0; i < format_count; ++i) {
                surf_formats2[i].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
                surf_formats2[i].pNext = nullptr;
            }
            err = inst.vkGetPhysicalDeviceSurfaceFormats2KHR(phys_device, &surface_info2, &format_count, surf_formats2.data());
            if (err) ERR_EXIT(err);
        } else {
            VkResult err =
                inst.vkGetPhysicalDeviceSurfaceFormatsKHR(phys_device, surface_extension.surface, &format_count, nullptr);
            if (err) ERR_EXIT(err);
            surf_formats.resize(format_count);
            err = inst.vkGetPhysicalDeviceSurfaceFormatsKHR(phys_device, surface_extension.surface, &format_count,
                                                            surf_formats.data());
            if (err) ERR_EXIT(err);
        }

        if (inst.CheckExtensionEnabled(VK_KHR_SURFACE_EXTENSION_NAME)) {
            VkResult err =
                inst.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phys_device, surface_extension.surface, &surface_capabilities);
            if (err) ERR_EXIT(err);
        }

        if (inst.CheckExtensionEnabled(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME)) {
            surface_capabilities2_khr.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;
            buildpNextChain((VkStructureHeader *)&surface_capabilities2_khr, sur_extension_pNextChain);

            VkPhysicalDeviceSurfaceInfo2KHR surface_info;
            surface_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
            surface_info.pNext = nullptr;
            surface_info.surface = surface_extension.surface;

            VkResult err = inst.vkGetPhysicalDeviceSurfaceCapabilities2KHR(phys_device, &surface_info, &surface_capabilities2_khr);
            if (err) ERR_EXIT(err);
        }

        if (inst.CheckExtensionEnabled(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME)) {
            surface_capabilities2_ext.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_EXT;
            surface_capabilities2_ext.pNext = nullptr;
            VkResult err =
                inst.vkGetPhysicalDeviceSurfaceCapabilities2EXT(phys_device, surface_extension.surface, &surface_capabilities2_ext);
            if (err) ERR_EXIT(err);
        }
    }

    ~AppSurface() {
        if (inst.CheckExtensionEnabled(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME)) {
            freepNextChain(static_cast<VkStructureHeader *>(surface_capabilities2_khr.pNext));
        }
    }
};

// -------------------- Device Groups ------------------------//

std::vector<VkPhysicalDeviceGroupProperties> GetGroups(AppInstance &inst) {
    if (inst.CheckExtensionEnabled(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME)) {
        PFN_vkEnumeratePhysicalDeviceGroupsKHR vkEnumeratePhysicalDeviceGroupsKHR =
            (PFN_vkEnumeratePhysicalDeviceGroupsKHR)vkGetInstanceProcAddr(inst.instance, "vkEnumeratePhysicalDeviceGroupsKHR");

        std::vector<VkPhysicalDeviceGroupProperties> groups;
        uint32_t group_count;
        VkResult err;
        do {
            err = vkEnumeratePhysicalDeviceGroupsKHR(inst.instance, &group_count, NULL);
            if (err != VK_SUCCESS && err != VK_INCOMPLETE) ERR_EXIT(err);
            groups.resize(group_count);

            err = vkEnumeratePhysicalDeviceGroupsKHR(inst.instance, &group_count, groups.data());
            if (err != VK_SUCCESS && err != VK_INCOMPLETE) ERR_EXIT(err);
        } while (err == VK_INCOMPLETE);
        return groups;
    }
    return {};
}

std::vector<VkPhysicalDeviceProperties> GetGroupProps(VkPhysicalDeviceGroupProperties group) {
    std::vector<VkPhysicalDeviceProperties> props(group.physicalDeviceCount);

    for (uint32_t i = 0; i < group.physicalDeviceCount; ++i) {
        vkGetPhysicalDeviceProperties(group.physicalDevices[i], &props[i]);
    }

    return props;
}

// The bool of the pair returns true if the extension VK_KHR_device_group is present
std::pair<bool, VkDeviceGroupPresentCapabilitiesKHR> GetGroupCapabilities(AppInstance &inst,
                                                                          VkPhysicalDeviceGroupProperties group) {
    // Build create info for logical device made from all physical devices in this group.
    std::vector<std::string> extensions_list = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DEVICE_GROUP_EXTENSION_NAME};
    VkDeviceGroupDeviceCreateInfoKHR dg_ci = {VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO_KHR, nullptr,
                                              group.physicalDeviceCount, group.physicalDevices};

    float queue_priority = 1.0f;

    auto ext_list = get_c_str_array(extensions_list);

    VkDeviceQueueCreateInfo q_ci = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, 0, 0, 1, &queue_priority};
    VkDeviceCreateInfo device_ci = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,   &dg_ci,         0, 1, &q_ci, 0, nullptr,
                                    static_cast<uint32_t>(ext_list.size()), ext_list.data()};

    VkDevice logical_device = VK_NULL_HANDLE;

    VkResult err = vkCreateDevice(group.physicalDevices[0], &device_ci, nullptr, &logical_device);
    if (err != VK_SUCCESS && err != VK_ERROR_EXTENSION_NOT_PRESENT) ERR_EXIT(err);

    if (err == VK_ERROR_EXTENSION_NOT_PRESENT) {
        VkDeviceGroupPresentCapabilitiesKHR group_capabilities = {VK_STRUCTURE_TYPE_DEVICE_GROUP_PRESENT_CAPABILITIES_KHR, nullptr};
        vkDestroyDevice(logical_device, nullptr);
        return std::pair<bool, VkDeviceGroupPresentCapabilitiesKHR>(false, group_capabilities);
    }

    VkDeviceGroupPresentCapabilitiesKHR group_capabilities = {VK_STRUCTURE_TYPE_DEVICE_GROUP_PRESENT_CAPABILITIES_KHR, nullptr};

    // If the KHR_device_group extension is present, write the capabilities of the logical device into a struct for later
    // output to user.
    PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR =
        (PFN_vkGetDeviceGroupPresentCapabilitiesKHR)vkGetInstanceProcAddr(inst.instance, "vkGetDeviceGroupPresentCapabilitiesKHR");
    err = vkGetDeviceGroupPresentCapabilitiesKHR(logical_device, &group_capabilities);
    if (err) ERR_EXIT(err);

    vkDestroyDevice(logical_device, nullptr);

    return std::pair<bool, VkDeviceGroupPresentCapabilitiesKHR>(true, group_capabilities);
}

// -------------------- Device Setup ------------------- //

struct MemImageSupport {
    bool regular_supported, sparse_supported, transient_supported;
    VkFormat format;
    uint32_t regular_memtypes, sparse_memtypes, transient_memtypes;
};

struct MemResSupport {
    std::array<std::array<MemImageSupport, 8>, 2> image;
    // TODO: buffers
};

struct pNextChainInfos {
    std::vector<pNextChainBuildingBlockInfo> phys_device_props2;
    std::vector<pNextChainBuildingBlockInfo> phys_device_mem_props2;
    std::vector<pNextChainBuildingBlockInfo> phys_device_features2;
    std::vector<pNextChainBuildingBlockInfo> surface_capabilities2;
    std::vector<pNextChainBuildingBlockInfo> format_properties2;
};

struct FormatRange {
    // the Vulkan standard version that supports this format range, or 0 if non-standard
    uint32_t minimum_instance_version;

    // The name of the extension that supports this format range, or NULL if the range
    // is only part of the standard
    const char *extension_name;

    // The first and last supported formats within this range.
    VkFormat first_format;
    VkFormat last_format;
};

struct AppGpu {
    AppInstance &inst;
    uint32_t id;
    VkPhysicalDevice phys_device;

    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceProperties2KHR props2;

    uint32_t queue_count;
    std::vector<VkQueueFamilyProperties> queue_props;
    std::vector<VkQueueFamilyProperties2KHR> queue_props2;

    VkPhysicalDeviceMemoryProperties memory_props;
    VkPhysicalDeviceMemoryProperties2KHR memory_props2;

    MemResSupport mem_type_res_support;

    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceFeatures2KHR features2;
    VkPhysicalDevice limits;

    std::vector<VkExtensionProperties> device_extensions;

    VkDevice dev;
    VkPhysicalDeviceFeatures enabled_features;

    std::array<VkDeviceSize, VK_MAX_MEMORY_HEAPS> heapBudget;
    std::array<VkDeviceSize, VK_MAX_MEMORY_HEAPS> heapUsage;

    std::vector<FormatRange> supported_format_ranges;

    AppGpu(AppInstance &inst, uint32_t id, VkPhysicalDevice phys_device, pNextChainInfos chainInfos)
        : inst(inst), id(id), phys_device(phys_device) {
        vkGetPhysicalDeviceProperties(phys_device, &props);

        if (inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
            props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
            buildpNextChain((VkStructureHeader *)&props2, chainInfos.phys_device_props2);

            inst.vkGetPhysicalDeviceProperties2KHR(phys_device, &props2);
        }

        /* get queue count */
        vkGetPhysicalDeviceQueueFamilyProperties(phys_device, &queue_count, nullptr);

        queue_props.resize(queue_count);

        vkGetPhysicalDeviceQueueFamilyProperties(phys_device, &queue_count, queue_props.data());

        if (inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
            queue_props2.resize(queue_count);

            for (size_t i = 0; i < queue_count; ++i) {
                queue_props2[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR;
                queue_props2[i].pNext = nullptr;
            }

            inst.vkGetPhysicalDeviceQueueFamilyProperties2KHR(phys_device, &queue_count, queue_props2.data());
        }

        vkGetPhysicalDeviceMemoryProperties(phys_device, &memory_props);

        vkGetPhysicalDeviceFeatures(phys_device, &features);

        if (inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
            memory_props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2_KHR;
            buildpNextChain((VkStructureHeader *)&memory_props2, chainInfos.phys_device_mem_props2);

            inst.vkGetPhysicalDeviceMemoryProperties2KHR(phys_device, &memory_props2);

            features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
            buildpNextChain((VkStructureHeader *)&features2, chainInfos.phys_device_features2);

            inst.vkGetPhysicalDeviceFeatures2KHR(phys_device, &features2);
        }

        device_extensions = AppGetPhysicalDeviceLayerExtensions(nullptr);

        const float queue_priority = 1.0f;
        const VkDeviceQueueCreateInfo q_ci = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                              nullptr,
                                              0,
                                              0,  // just pick the first one and hope for the best
                                              1,
                                              &queue_priority};
        VkPhysicalDeviceFeatures features = {0};
        // if (features.sparseBinding ) features.sparseBinding = VK_TRUE;
        enabled_features = features;
        const VkDeviceCreateInfo device_ci = {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0, 1, &q_ci, 0, nullptr, 0, nullptr, &enabled_features};

        VkResult err = vkCreateDevice(phys_device, &device_ci, nullptr, &dev);
        if (err) ERR_EXIT(err);

        const VkFormat color_format = VK_FORMAT_R8G8B8A8_UNORM;
        const std::vector<VkFormat> formats = {
            color_format,      VK_FORMAT_D16_UNORM,         VK_FORMAT_X8_D24_UNORM_PACK32, VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT,   VK_FORMAT_D32_SFLOAT_S8_UINT};
        assert(mem_type_res_support.image[0].size() == formats.size());
        const std::array<VkImageUsageFlags, 2> usages = {0, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT};
        const std::array<VkImageCreateFlags, 2> flagss = {0, VK_IMAGE_CREATE_SPARSE_BINDING_BIT};

        for (size_t fmt_i = 0; fmt_i < formats.size(); ++fmt_i) {
            // only iterate over VK_IMAGE_TILING_OPTIMAL and VK_IMAGE_TILING_LINEAR (0 and 1)
            for (int tiling = VK_IMAGE_TILING_OPTIMAL; tiling <= VK_IMAGE_TILING_LINEAR; ++tiling) {
                mem_type_res_support.image[tiling][fmt_i].format = formats[fmt_i];
                mem_type_res_support.image[tiling][fmt_i].regular_supported = true;
                mem_type_res_support.image[tiling][fmt_i].sparse_supported = true;
                mem_type_res_support.image[tiling][fmt_i].transient_supported = true;

                VkFormatProperties fmt_props;
                vkGetPhysicalDeviceFormatProperties(phys_device, formats[fmt_i], &fmt_props);
                if ((tiling == VK_IMAGE_TILING_OPTIMAL && fmt_props.optimalTilingFeatures == 0) ||
                    (tiling == VK_IMAGE_TILING_LINEAR && fmt_props.linearTilingFeatures == 0)) {
                    mem_type_res_support.image[tiling][fmt_i].regular_supported = false;
                    mem_type_res_support.image[tiling][fmt_i].sparse_supported = false;
                    mem_type_res_support.image[tiling][fmt_i].transient_supported = false;
                    continue;
                }

                for (size_t u_i = 0; u_i < usages.size(); ++u_i) {
                    for (size_t flg_i = 0; flg_i < flagss.size(); ++flg_i) {
                        VkImageCreateInfo image_ci = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                                      nullptr,
                                                      flagss[flg_i],
                                                      VK_IMAGE_TYPE_2D,
                                                      formats[fmt_i],
                                                      {8, 8, 1},
                                                      1,
                                                      1,
                                                      VK_SAMPLE_COUNT_1_BIT,
                                                      static_cast<VkImageTiling>(tiling),
                                                      usages[u_i],
                                                      VK_SHARING_MODE_EXCLUSIVE,
                                                      0,
                                                      nullptr,
                                                      VK_IMAGE_LAYOUT_UNDEFINED};

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

                        if (!enabled_features.sparseBinding && (image_ci.flags & VK_IMAGE_CREATE_SPARSE_BINDING_BIT)) {
                            mem_type_res_support.image[tiling][fmt_i].sparse_supported = false;
                            continue;
                        }

                        VkImageFormatProperties img_props;
                        err = vkGetPhysicalDeviceImageFormatProperties(phys_device, image_ci.format, image_ci.imageType,
                                                                       image_ci.tiling, image_ci.usage, image_ci.flags, &img_props);

                        uint32_t *memtypes;
                        bool *support;

                        if (image_ci.flags == 0 && !(image_ci.usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)) {
                            memtypes = &mem_type_res_support.image[tiling][fmt_i].regular_memtypes;
                            support = &mem_type_res_support.image[tiling][fmt_i].regular_supported;
                        } else if ((image_ci.flags & VK_IMAGE_CREATE_SPARSE_BINDING_BIT) &&
                                   !(image_ci.usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)) {
                            memtypes = &mem_type_res_support.image[tiling][fmt_i].sparse_memtypes;
                            support = &mem_type_res_support.image[tiling][fmt_i].sparse_supported;
                        } else if (image_ci.flags == 0 && (image_ci.usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)) {
                            memtypes = &mem_type_res_support.image[tiling][fmt_i].transient_memtypes;
                            support = &mem_type_res_support.image[tiling][fmt_i].transient_supported;
                        } else {
                            assert(false);
                            return;
                        }

                        if (err == VK_ERROR_FORMAT_NOT_SUPPORTED) {
                            *support = false;
                        } else {
                            if (err) ERR_EXIT(err);

                            VkImage dummy_img;
                            err = vkCreateImage(dev, &image_ci, nullptr, &dummy_img);
                            if (err) ERR_EXIT(err);

                            VkMemoryRequirements mem_req;
                            vkGetImageMemoryRequirements(dev, dummy_img, &mem_req);
                            *memtypes = mem_req.memoryTypeBits;

                            vkDestroyImage(dev, dummy_img, nullptr);
                        }
                    }
                }
            }
        }

        // Memory //

        struct VkStructureHeader *structure = NULL;
        if (inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
            structure = (struct VkStructureHeader *)memory_props2.pNext;

            while (structure) {
                if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT &&
                    CheckPhysicalDeviceExtensionIncluded(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME)) {
                    VkPhysicalDeviceMemoryBudgetPropertiesEXT *mem_budget_props =
                        (VkPhysicalDeviceMemoryBudgetPropertiesEXT *)structure;
                    for (int i = 0; i < VK_MAX_MEMORY_HEAPS; i++) {
                        heapBudget[i] = mem_budget_props->heapBudget[i];
                        heapUsage[i] = mem_budget_props->heapUsage[i];
                    }
                }

                structure = (struct VkStructureHeader *)structure->pNext;
            }
        }
        // TODO buffer - memory type compatibility

        supported_format_ranges = {
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
    }
    ~AppGpu() {
        vkDestroyDevice(dev, nullptr);

        if (inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
            freepNextChain(static_cast<VkStructureHeader *>(features2.pNext));
            freepNextChain(static_cast<VkStructureHeader *>(props2.pNext));
            freepNextChain(static_cast<VkStructureHeader *>(memory_props2.pNext));
        }
    }

    bool CheckPhysicalDeviceExtensionIncluded(std::string extension_to_check) {
        for (auto &extension : device_extensions) {
            if (extension_to_check == std::string(extension.extensionName)) {
                return true;
            }
        }
        return false;
    }

    std::vector<VkExtensionProperties> AppGetPhysicalDeviceLayerExtensions(char *layer_name) {
        std::vector<VkExtensionProperties> extension_properties;

        VkResult err;
        uint32_t ext_count = 0;

        /* repeat get until VK_INCOMPLETE goes away */
        do {
            err = vkEnumerateDeviceExtensionProperties(phys_device, layer_name, &ext_count, nullptr);
            if (err) ERR_EXIT(err);

            extension_properties.resize(ext_count);
            err = vkEnumerateDeviceExtensionProperties(phys_device, layer_name, &ext_count, extension_properties.data());
            if (err) ERR_EXIT(err);
            extension_properties.resize(ext_count);
        } while (err == VK_INCOMPLETE);

        return extension_properties;
    }

    // Helper function to determine whether a format range is currently supported.
    bool FormatRangeSupported(FormatRange &format_range) {
        // True if standard and supported by both this instance and this GPU
        if (format_range.minimum_instance_version > 0 && inst.instance_version >= format_range.minimum_instance_version &&
            props.apiVersion >= format_range.minimum_instance_version) {
            return true;
        }

        // True if this extension is present
        if (format_range.extension_name != nullptr) {
            return inst.CheckExtensionEnabled(format_range.extension_name);
        }

        // Otherwise, not supported.
        return false;
    }

    VkPhysicalDeviceProperties GetDeviceProperties() {
        if (inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
            return props2.properties;
        } else {
            return props;
        }
    }
};
struct AppQueueFamilyProperties {
    VkQueueFamilyProperties props;
    uint32_t queue_index;
    bool is_present_platform_agnostic = true;
    VkBool32 platforms_support_present = VK_FALSE;

    AppQueueFamilyProperties(AppGpu &gpu, uint32_t queue_index) : queue_index(queue_index) {
        if (gpu.inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
            props = gpu.queue_props2[queue_index].queueFamilyProperties;
        } else {
            props = gpu.queue_props[queue_index];
        }

        for (auto &surface_ext : gpu.inst.surface_extensions) {
            VkResult err = vkGetPhysicalDeviceSurfaceSupportKHR(gpu.phys_device, queue_index, surface_ext.surface,
                                                                &surface_ext.supports_present);
            if (err) ERR_EXIT(err);

            const bool first = (surface_ext == gpu.inst.surface_extensions.at(0));
            if (!first && platforms_support_present != surface_ext.supports_present) {
                is_present_platform_agnostic = false;

                platforms_support_present = surface_ext.supports_present;
            }
        }
    }
};

// --------- Format Properties ----------//

struct PropFlags {
    uint32_t linear;
    uint32_t optimal;
    uint32_t buffer;

    bool operator==(const PropFlags &other) const {
        return (linear == other.linear && optimal == other.optimal && buffer == other.buffer);
    }
};

namespace std {
template <>
struct hash<PropFlags> {
    std::size_t operator()(const PropFlags &k) const {
        return ((std::hash<uint32_t>()(k.linear) ^ (std::hash<uint32_t>()(k.optimal) << 1)) >> 1) ^
               (std::hash<uint32_t>()(k.buffer) << 1);
    }
};
}  // namespace std

// Used to sort the formats into buckets by their properties.
std::unordered_map<PropFlags, std::vector<VkFormat>> FormatPropMap(AppGpu &gpu) {
    std::unordered_map<PropFlags, std::vector<VkFormat>> map;
    for (auto fmtRange : gpu.supported_format_ranges) {
        for (uint32_t fmt = fmtRange.first_format; fmt <= fmtRange.last_format; ++fmt) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(gpu.phys_device, static_cast<VkFormat>(fmt), &props);

            PropFlags pf = {props.linearTilingFeatures, props.optimalTilingFeatures, props.bufferFeatures};

            map[pf].push_back(static_cast<VkFormat>(fmt));
        }
    }
    return map;
}

VkFormatProperties2 GetFormatProperties2(AppGpu &gpu, VkFormat format, pNextChainInfos &chainInfos) {
    VkFormatProperties2 props;
    props.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    buildpNextChain((VkStructureHeader *)&props, chainInfos.format_properties2);

    gpu.inst.vkGetPhysicalDeviceFormatProperties2KHR(gpu.phys_device, format, &props);
    return props;
}
