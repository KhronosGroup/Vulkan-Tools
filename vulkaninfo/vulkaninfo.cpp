/*
 * Copyright (c) 2015-2021 The Khronos Group Inc.
 * Copyright (c) 2015-2021 Valve Corporation
 * Copyright (c) 2015-2021 LunarG, Inc.
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
 * Author: Richard Wright <richard@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 *
 */

#ifdef _WIN32
#include <crtdbg.h>
#endif
#include "vulkaninfo.hpp"

// =========== Dump Functions ========= //

void DumpExtensions(Printer &p, std::string section_name, std::vector<VkExtensionProperties> extensions, bool do_indent = false) {
    std::sort(extensions.begin(), extensions.end(), [](VkExtensionProperties &a, VkExtensionProperties &b) -> int {
        return std::string(a.extensionName) < std::string(b.extensionName);
    });

    size_t max_length = 0;
    for (const auto &ext : extensions) {
        max_length = std::max(max_length, std::strlen(ext.extensionName));
    }
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    const std::string portability_ext_name = VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME;
#endif  // defined(VK_ENABLE_BETA_EXTENSIONS)
    ObjectWrapper obj(p, section_name, extensions.size());
    if (do_indent) p.IndentDecrease();
    for (auto &ext : extensions) {
#if defined(VK_ENABLE_BETA_EXTENSIONS)
        if (p.Type() == OutputType::json && portability_ext_name == ext.extensionName) continue;
#endif  // defined(VK_ENABLE_BETA_EXTENSIONS)
        p.PrintExtension(ext.extensionName, ext.specVersion, max_length);
    }
    if (do_indent) p.IndentIncrease();
}

void DumpLayers(Printer &p, std::vector<LayerExtensionList> layers, const std::vector<std::unique_ptr<AppGpu>> &gpus) {
    std::sort(layers.begin(), layers.end(), [](LayerExtensionList &left, LayerExtensionList &right) -> int {
        return std::strncmp(left.layer_properties.layerName, right.layer_properties.layerName, VK_MAX_DESCRIPTION_SIZE) < 0;
    });
    switch (p.Type()) {
        case OutputType::text:
        case OutputType::html: {
            p.SetHeader();
            ArrayWrapper arr_layers(p, "Layers", layers.size());
            IndentWrapper indent(p);

            for (auto &layer : layers) {
                auto v_str = VkVersionString(layer.layer_properties.specVersion);
                auto props = layer.layer_properties;

                std::string header = p.DecorateAsType(props.layerName) + " (" + props.description + ") Vulkan version " +
                                     p.DecorateAsValue(v_str) + ", layer version " +
                                     p.DecorateAsValue(std::to_string(props.implementationVersion));
                ObjectWrapper obj(p, header);
                DumpExtensions(p, "Layer Extensions", layer.extension_properties);

                ArrayWrapper arr_devices(p, "Devices", gpus.size());
                for (auto &gpu : gpus) {
                    p.PrintKeyValue("GPU id", gpu->id, gpu->props.deviceName);
                    auto exts = gpu->inst.AppGetPhysicalDeviceLayerExtensions(gpu->phys_device, props.layerName);
                    DumpExtensions(p, "Layer-Device Extensions", exts);
                    p.AddNewline();
                }
            }
            break;
        }

        case OutputType::json: {
            assert(false && "unimplemented");
            break;
        }
        case OutputType::vkconfig_output: {
            ObjectWrapper obj(p, "Layer Properties");
            for (auto &layer : layers) {
                ObjectWrapper obj_name(p, layer.layer_properties.layerName);
                p.SetMinKeyWidth(21);
                p.PrintKeyString("layerName", layer.layer_properties.layerName);
                p.PrintKeyString("version", VkVersionString(layer.layer_properties.specVersion));
                p.PrintKeyValue("implementation version", layer.layer_properties.implementationVersion);
                p.PrintKeyString("description", layer.layer_properties.description);
                DumpExtensions(p, "Layer Extensions", layer.extension_properties);
                ObjectWrapper obj_devices(p, "Devices");
                for (auto &gpu : gpus) {
                    ObjectWrapper obj_gpu(p, gpu->props.deviceName);
                    p.PrintKeyValue("GPU id", gpu->id, gpu->props.deviceName);
                    auto exts = gpu->inst.AppGetPhysicalDeviceLayerExtensions(gpu->phys_device, layer.layer_properties.layerName);
                    DumpExtensions(p, "Layer-Device Extensions", exts);
                }
            }
            break;
        }
    }
}

void DumpSurfaceFormats(Printer &p, AppInstance &inst, AppSurface &surface) {
    std::vector<VkSurfaceFormatKHR> formats;
    if (inst.CheckExtensionEnabled(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME)) {
        for (auto &format : surface.surf_formats2) {
            formats.push_back(format.surfaceFormat);
        }
    } else {
        for (auto &format : surface.surf_formats) {
            formats.push_back(format);
        }
    }
    ObjectWrapper obj(p, "Formats", formats.size());
    int i = 0;
    for (auto &format : formats) {
        p.SetElementIndex(i++);
        DumpVkSurfaceFormatKHR(p, "SurfaceFormat", format);
    }
}

void DumpPresentModes(Printer &p, AppSurface &surface) {
    ArrayWrapper arr(p, "Present Modes", surface.surf_present_modes.size());
    for (auto &mode : surface.surf_present_modes) {
        p.SetAsType().PrintString(VkPresentModeKHRString(mode));
    }
}

void DumpSurfaceCapabilities(Printer &p, AppInstance &inst, AppGpu &gpu, AppSurface &surface) {
    auto &surf_cap = surface.surface_capabilities;
    p.SetSubHeader().SetIgnoreMinWidthInChild();
    DumpVkSurfaceCapabilitiesKHR(p, "VkSurfaceCapabilitiesKHR", surf_cap);

    if (inst.CheckExtensionEnabled(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME)) {
        p.SetSubHeader();
        ObjectWrapper obj(p, "VkSurfaceCapabilities2EXT");
        DumpVkSurfaceCounterFlagsEXT(p, "supportedSurfaceCounters", surface.surface_capabilities2_ext.supportedSurfaceCounters);
    }
    if (inst.CheckExtensionEnabled(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME)) {
        chain_iterator_surface_capabilities2(p, inst, gpu, surface.surface_capabilities2_khr.pNext);
    }
}

void DumpSurface(Printer &p, AppInstance &inst, AppGpu &gpu, AppSurface &surface, std::set<std::string> surface_types) {
    ObjectWrapper obj(p, std::string("GPU id : ") + p.DecorateAsValue(std::to_string(gpu.id)) + " (" + gpu.props.deviceName + ")");

    if (surface_types.size() == 0) {
        p.SetAsType().PrintKeyString("Surface type", "No type found");
    } else if (surface_types.size() == 1) {
        p.SetAsType().PrintKeyString("Surface type", surface.surface_extension.name);
    } else {
        ArrayWrapper arr(p, "Surface types", surface_types.size());
        for (auto &name : surface_types) {
            p.PrintString(name);
        }
    }

    DumpSurfaceFormats(p, inst, surface);
    DumpPresentModes(p, surface);
    DumpSurfaceCapabilities(p, inst, gpu, surface);

    p.AddNewline();
}

struct SurfaceTypeGroup {
    AppSurface *surface;
    AppGpu *gpu;
    std::set<std::string> surface_types;
};

bool operator==(AppSurface const &a, AppSurface const &b) {
    return a.phys_device == b.phys_device && a.surf_present_modes == b.surf_present_modes && a.surf_formats == b.surf_formats &&
           a.surf_formats2 == b.surf_formats2 && a.surface_capabilities == b.surface_capabilities &&
           a.surface_capabilities2_khr == b.surface_capabilities2_khr && a.surface_capabilities2_ext == b.surface_capabilities2_ext;
}

#if defined(VULKANINFO_WSI_ENABLED)
void DumpPresentableSurfaces(Printer &p, AppInstance &inst, const std::vector<std::unique_ptr<AppGpu>> &gpus,
                             const std::vector<std::unique_ptr<AppSurface>> &surfaces) {
    // Don't print anything if no surfaces are found
    if (surfaces.size() == 0) return;
    p.SetHeader();
    ObjectWrapper obj(p, "Presentable Surfaces");
    IndentWrapper indent(p);

    std::vector<SurfaceTypeGroup> surface_list;

    for (auto &surface : surfaces) {
        auto exists = surface_list.end();
        for (auto it = surface_list.begin(); it != surface_list.end(); it++) {
            // check for duplicate surfaces that differ only by the surface extension
            if (*(it->surface) == *(surface.get())) {
                exists = it;
                break;
            }
        }
        if (exists != surface_list.end()) {
            exists->surface_types.insert(surface.get()->surface_extension.name);
        } else {
            // find surface.phys_device's corresponding AppGpu
            AppGpu *corresponding_gpu = nullptr;
            for (auto &gpu : gpus) {
                if (gpu->phys_device == surface->phys_device) corresponding_gpu = gpu.get();
            }
            if (corresponding_gpu != nullptr)
                surface_list.push_back({surface.get(), corresponding_gpu, {surface.get()->surface_extension.name}});
        }
    }
    for (auto &group : surface_list) {
        DumpSurface(p, inst, *group.gpu, *group.surface, group.surface_types);
    }
    p.AddNewline();
}
#endif  // defined(VULKANINFO_WSI_ENABLED)

void DumpGroups(Printer &p, AppInstance &inst) {
    if (inst.CheckExtensionEnabled(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME)) {
        auto groups = GetGroups(inst);
        if (groups.size() == 0) {
            p.SetHeader();
            ObjectWrapper obj(p, "Groups");
            p.PrintString("No Device Groups Found");
            p.AddNewline();
            return;
        }

        p.SetHeader();
        ObjectWrapper obj_device_groups(p, "Device Groups");
        IndentWrapper indent(p);

        int group_id = 0;
        for (auto &group : groups) {
            ObjectWrapper obj_group(p, "Group " + std::to_string(group_id));
            auto group_props = GetGroupProps(inst, group);
            {
                ObjectWrapper obj_properties(p, "Properties");
                {
                    ArrayWrapper arr(p, "physicalDevices", group.physicalDeviceCount);
                    int id = 0;
                    for (auto &prop : group_props) {
                        p.PrintString(std::string(prop.deviceName) + " (ID: " + p.DecorateAsValue(std::to_string(id++)) + ")");
                    }
                }
                p.PrintKeyValue("subsetAllocation", group.subsetAllocation);
            }
            p.AddNewline();

            auto group_capabilities = GetGroupCapabilities(inst, group);
            if (!group_capabilities) {
                p.PrintKeyString("Present Capabilities",
                                 "Group does not support VK_KHR_device_group, skipping printing present capabilities");
            } else {
                ObjectWrapper obj_caps(p, "Present Capabilities");
                for (uint32_t i = 0; i < group.physicalDeviceCount; i++) {
                    ObjectWrapper obj_device(
                        p, std::string(group_props[i].deviceName) + " (ID: " + p.DecorateAsValue(std::to_string(i)) + ")");
                    ArrayWrapper arr(p, "Can present images from the following devices", group.physicalDeviceCount);

                    for (uint32_t j = 0; j < group.physicalDeviceCount; j++) {
                        uint32_t mask = 1 << j;
                        if (group_capabilities->presentMask[i] & mask) {
                            p.PrintString(std::string(group_props[j].deviceName) + " (ID: " + p.DecorateAsValue(std::to_string(j)) +
                                          ")");
                        }
                    }
                }
                DumpVkDeviceGroupPresentModeFlagsKHR(p, "Present modes", group_capabilities->modes);
            }
            p.AddNewline();
            group_id++;
        }
        p.AddNewline();
    }
}

void GpuDumpProps(Printer &p, AppGpu &gpu) {
    auto props = gpu.GetDeviceProperties();
    p.SetSubHeader();
    {
        ObjectWrapper obj(p, "VkPhysicalDeviceProperties");
        p.SetMinKeyWidth(17);
        p.PrintKeyValue("apiVersion", props.apiVersion, VkVersionString(props.apiVersion));
        p.PrintKeyValue("driverVersion", props.driverVersion, to_hex_str(props.driverVersion));
        p.PrintKeyString("vendorID", to_hex_str(props.vendorID));
        p.PrintKeyString("deviceID", to_hex_str(props.deviceID));
        p.PrintKeyString("deviceType", VkPhysicalDeviceTypeString(props.deviceType));
        p.PrintKeyString("deviceName", props.deviceName);
        p.PrintKeyString("pipelineCacheUUID", to_string_16(props.pipelineCacheUUID));
    }
    p.AddNewline();
    DumpVkPhysicalDeviceLimits(p, "VkPhysicalDeviceLimits", gpu.props.limits);
    p.AddNewline();
    DumpVkPhysicalDeviceSparseProperties(p, "VkPhysicalDeviceSparseProperties", gpu.props.sparseProperties);
    p.AddNewline();
    if (gpu.inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
        void *place = gpu.props2.pNext;
        chain_iterator_phys_device_props2(p, gpu.inst, gpu, place);
        p.AddNewline();
    }
}

void GpuDumpQueueProps(Printer &p, AppGpu &gpu, const AppQueueFamilyProperties &queue) {
    VkQueueFamilyProperties props = queue.props;
    p.SetSubHeader().SetElementIndex(static_cast<int>(queue.queue_index));
    ObjectWrapper obj_queue_props(p, "queueProperties");
    p.SetMinKeyWidth(27);
    if (p.Type() == OutputType::vkconfig_output) {
        DumpVkExtent3D(p, "minImageTransferGranularity", props.minImageTransferGranularity);
    } else {
        p.PrintKeyValue("minImageTransferGranularity", props.minImageTransferGranularity);
    }
    p.PrintKeyValue("queueCount", props.queueCount);
    p.PrintKeyString("queueFlags", VkQueueFlagsString(props.queueFlags));
    p.PrintKeyValue("timestampValidBits", props.timestampValidBits);

    if (queue.is_present_platform_agnostic) {
        p.PrintKeyString("present support", queue.platforms_support_present ? "true" : "false");
    } else {
        size_t width = 0;
        for (auto &surface : gpu.inst.surface_extensions) {
            if (surface.name.size() > width) width = surface.name.size();
        }
        ObjectWrapper obj_present_support(p, "present support");
        p.SetMinKeyWidth(width);
        for (auto &surface : gpu.inst.surface_extensions) {
            p.PrintKeyString(surface.name, surface.supports_present ? "true" : "false");
        }
    }
    chain_iterator_queue_properties2(p, gpu, queue.pNext);

    p.AddNewline();
}

// This prints a number of bytes in a human-readable format according to prefixes of the International System of Quantities (ISQ),
// defined in ISO/IEC 80000. The prefixes used here are not SI prefixes, but rather the binary prefixes based on powers of 1024
// (kibi-, mebi-, gibi- etc.).
#define kBufferSize 32

std::string NumToNiceStr(const size_t sz) {
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
    return std::string(buf);
}

std::string append_human_readable(VkDeviceSize memory) {
    return std::to_string(memory) + " (" + to_hex_str(memory) + ") (" + NumToNiceStr(static_cast<size_t>(memory)) + ")";
}

void GpuDumpMemoryProps(Printer &p, AppGpu &gpu) {
    p.SetHeader();
    ObjectWrapper obj_mem_props(p, "VkPhysicalDeviceMemoryProperties");
    IndentWrapper indent(p);
    {
        ObjectWrapper obj_mem_heaps(p, "memoryHeaps", gpu.memory_props.memoryHeapCount);

        for (uint32_t i = 0; i < gpu.memory_props.memoryHeapCount; ++i) {
            p.SetElementIndex(static_cast<int>(i));
            ObjectWrapper obj_mem_heap(p, "memoryHeaps");
            p.SetMinKeyWidth(6);
            p.PrintKeyString("size", append_human_readable(gpu.memory_props.memoryHeaps[i].size));
            if (gpu.CheckPhysicalDeviceExtensionIncluded(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME)) {
                p.PrintKeyString("budget", append_human_readable(gpu.heapBudget[i]));
                p.PrintKeyString("usage", append_human_readable(gpu.heapUsage[i]));
            }
            DumpVkMemoryHeapFlags(p, "flags", gpu.memory_props.memoryHeaps[i].flags);
        }
    }
    {
        ObjectWrapper obj_mem_types(p, "memoryTypes", gpu.memory_props.memoryTypeCount);
        for (uint32_t i = 0; i < gpu.memory_props.memoryTypeCount; ++i) {
            p.SetElementIndex(static_cast<int>(i));
            ObjectWrapper obj_mem_type(p, "memoryTypes");
            p.SetMinKeyWidth(13);
            p.PrintKeyValue("heapIndex", gpu.memory_props.memoryTypes[i].heapIndex);

            auto flags = gpu.memory_props.memoryTypes[i].propertyFlags;
            DumpVkMemoryPropertyFlags(p, "propertyFlags = " + to_hex_str(flags), flags);

            ObjectWrapper usable_for(p, "usable for");
            const uint32_t memtype_bit = 1U << i;

            // only linear and optimal tiling considered
            for (auto &image_tiling : gpu.memory_image_support_types) {
                p.SetOpenDetails();
                ArrayWrapper arr(p, VkImageTilingString(VkImageTiling(image_tiling.tiling)));
                bool has_any_support_types = false;
                bool regular = false;
                bool transient = false;
                bool sparse = false;
                for (auto &image_format : image_tiling.formats) {
                    if (image_format.type_support.size() > 0) {
                        bool has_a_support_type = false;
                        for (auto &img_type : image_format.type_support) {
                            if (img_type.Compatible(memtype_bit)) {
                                has_a_support_type = true;
                                has_any_support_types = true;
                                if (img_type.type == ImageTypeSupport::Type::regular) regular = true;
                                if (img_type.type == ImageTypeSupport::Type::transient) transient = true;
                                if (img_type.type == ImageTypeSupport::Type::sparse) sparse = true;
                            }
                        }
                        if (has_a_support_type) {
                            if (image_format.format == color_format) {
                                p.PrintString("color images");
                            } else {
                                p.PrintString(VkFormatString(image_format.format));
                            }
                        }
                    }
                }
                if (!has_any_support_types) {
                    p.PrintString("None");
                } else {
                    if (regular && !transient && sparse) p.PrintString("(non-transient)");
                    if (regular && transient && !sparse) p.PrintString("(non-sparse)");
                    if (regular && !transient && !sparse) p.PrintString("(non-sparse, non-transient)");
                    if (!regular && transient && sparse) p.PrintString("(sparse and transient only)");
                    if (!regular && !transient && sparse) p.PrintString("(sparse only)");
                    if (!regular && transient && !sparse) p.PrintString("(transient only)");
                }
            }
        }
    }
    p.AddNewline();
}

void GpuDumpFeatures(Printer &p, AppGpu &gpu) {
    p.SetHeader();
    DumpVkPhysicalDeviceFeatures(p, "VkPhysicalDeviceFeatures", gpu.features);
    p.AddNewline();
    if (gpu.inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
        void *place = gpu.features2.pNext;
        chain_iterator_phys_device_features2(p, gpu, place);
    }
}

void GpuDumpFormatProperty(Printer &p, VkFormat fmt, VkFormatProperties prop) {
    std::string name{};
    switch (p.Type()) {
        case OutputType::text: {
            name = "Properties";
            break;
        }
        case OutputType::html: {
            name = VkFormatString(fmt);
            break;
        }
        case OutputType::json: {
            name = "VkFormatProperties";
            break;
        }
        case OutputType::vkconfig_output: {
            name = VkFormatString(fmt);
            break;
        }
    }
    p.SetTitleAsType();
    ObjectWrapper obj(p, name);
    p.SetOpenDetails();
    DumpVkFormatFeatureFlags(p, "linearTilingFeatures", prop.linearTilingFeatures);
    p.SetOpenDetails();
    DumpVkFormatFeatureFlags(p, "optimalTilingFeatures", prop.optimalTilingFeatures);
    p.SetOpenDetails();
    DumpVkFormatFeatureFlags(p, "bufferFeatures", prop.bufferFeatures);
}

void GpuDumpToolingInfo(Printer &p, AppGpu &gpu) {
    auto tools = GetToolingInfo(gpu);
    if (tools.size() > 0) {
        p.SetSubHeader();
        ObjectWrapper obj(p, "Tooling Info");
        for (auto tool : tools) {
            DumpVkPhysicalDeviceToolProperties(p, tool.name, tool);
            p.AddNewline();
        }
    }
}

void GpuDevDump(Printer &p, AppGpu &gpu) {
    p.SetHeader();
    ObjectWrapper obj_format_props(p, "Format Properties");
    IndentWrapper indent_outer(p);

    if (p.Type() == OutputType::text) {
        auto fmtPropMap = FormatPropMap(gpu);

        int counter = 0;
        std::vector<VkFormat> unsupported_formats;
        for (auto &prop : fmtPropMap) {
            VkFormatProperties props;
            props.linearTilingFeatures = prop.first.linear;
            props.optimalTilingFeatures = prop.first.optimal;
            props.bufferFeatures = prop.first.buffer;
            if (props.linearTilingFeatures == 0 && props.optimalTilingFeatures == 0 && props.bufferFeatures == 0) {
                unsupported_formats = prop.second;
                continue;
            }

            p.SetElementIndex(counter++);
            ObjectWrapper obj_common_group(p, "Common Format Group");
            IndentWrapper indent_inner(p);
            {
                ArrayWrapper arr_formats(p, "Formats", prop.second.size());
                for (auto &fmt : prop.second) {
                    p.SetAsType().PrintString(VkFormatString(fmt));
                }
            }
            GpuDumpFormatProperty(p, VK_FORMAT_UNDEFINED, props);
            p.AddNewline();
        }

        ArrayWrapper arr_unsupported_formats(p, "Unsupported Formats", unsupported_formats.size());
        for (auto &fmt : unsupported_formats) {
            p.SetAsType().PrintString(VkFormatString(fmt));
        }
    } else {
        for (auto &format : gpu.supported_format_ranges) {
            if (gpu.FormatRangeSupported(format)) {
                for (int32_t fmt_counter = format.first_format; fmt_counter <= format.last_format; ++fmt_counter) {
                    VkFormat fmt = static_cast<VkFormat>(fmt_counter);

                    VkFormatProperties props;
                    gpu.inst.dll.fp_vkGetPhysicalDeviceFormatProperties(gpu.phys_device, fmt, &props);

                    GpuDumpFormatProperty(p, fmt, props);
                }
            }
        }
    }

    p.AddNewline();
}

// Print gpu info for text, html, & vkconfig_output
// Uses a separate function than schema-json for clarity
void DumpGpu(Printer &p, AppGpu &gpu, bool show_tooling_info, bool show_formats) {
    ObjectWrapper obj_gpu(p, "GPU" + std::to_string(gpu.id));
    IndentWrapper indent(p);

    GpuDumpProps(p, gpu);
    DumpExtensions(p, "Device Extensions", gpu.device_extensions);
    p.AddNewline();
    {
        p.SetHeader();
        ObjectWrapper obj_family_props(p, "VkQueueFamilyProperties");
        for (const auto &queue_prop : gpu.extended_queue_props) {
            GpuDumpQueueProps(p, gpu, queue_prop);
        }
    }
    GpuDumpMemoryProps(p, gpu);
    GpuDumpFeatures(p, gpu);
    if (show_tooling_info) {
        GpuDumpToolingInfo(p, gpu);
    }

    if (p.Type() != OutputType::text || show_formats) {
        GpuDevDump(p, gpu);
    }

    p.AddNewline();
}

// Print capabilities section of profiles schema
void DumpGpuProfileCapabilities(Printer &p, AppGpu &gpu) {
    ObjectWrapper capabilities(p, "capabilities");
    {
        ObjectWrapper temp_name_obj(p, "device");
        DumpExtensions(p, "extensions", gpu.device_extensions);
        {
            ObjectWrapper obj(p, "features");
            GpuDumpFeatures(p, gpu);
        }
        {
            ObjectWrapper obj(p, "properties");
            {
                ObjectWrapper props_obj(p, "VkPhysicalDeviceProperties");
                auto props = gpu.GetDeviceProperties();
                p.PrintKeyValue("apiVersion", props.apiVersion, VkVersionString(props.apiVersion));
                p.PrintKeyValue("deviceID", props.deviceID);
                p.PrintKeyString("deviceName", props.deviceName);
                p.PrintKeyString("deviceType", std::string("VK_") + VkPhysicalDeviceTypeString(props.deviceType));
                p.PrintKeyValue("driverVersion", props.driverVersion);
                DumpVkPhysicalDeviceLimits(p, "VkPhysicalDeviceLimits", gpu.props.limits);
                {
                    ArrayWrapper arr(p, "pipelineCacheUUID");
                    for (const auto &uuid : props.pipelineCacheUUID) p.PrintElement(static_cast<uint32_t>(uuid));
                }
                DumpVkPhysicalDeviceSparseProperties(p, "VkPhysicalDeviceSparseProperties", gpu.props.sparseProperties);
                p.PrintKeyValue("vendorID", props.vendorID);
            }
            if (gpu.inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
                void *place = gpu.props2.pNext;
                chain_iterator_phys_device_props2(p, gpu.inst, gpu, place);
            }
        }
        {
            ObjectWrapper obj(p, "formats");
            for (auto &format : gpu.supported_format_ranges) {
                if (gpu.FormatRangeSupported(format)) {
                    for (int32_t fmt_counter = format.first_format; fmt_counter <= format.last_format; ++fmt_counter) {
                        VkFormat fmt = static_cast<VkFormat>(fmt_counter);

                        VkFormatProperties props;
                        gpu.inst.dll.fp_vkGetPhysicalDeviceFormatProperties(gpu.phys_device, fmt, &props);

                        // don't print format properties that are unsupported
                        if ((props.linearTilingFeatures || props.optimalTilingFeatures || props.bufferFeatures) == 0) continue;

                        ObjectWrapper format_obj(p, std::string("VK_") + VkFormatString(fmt));
                        {
                            GpuDumpFormatProperty(p, fmt, props);

                            VkFormatProperties2 format_props2{};
                            format_props2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
                            format_props2.formatProperties = props;
                            std::unique_ptr<format_properties2_chain> chain_for_format_props2;
                            setup_format_properties2_chain(format_props2, chain_for_format_props2);
                            gpu.inst.ext_funcs.vkGetPhysicalDeviceFormatProperties2KHR(gpu.phys_device, fmt, &format_props2);
                            chain_iterator_format_properties2(p, gpu, format_props2.pNext);
                        }
                    }
                }
            }
        }
        {
            ArrayWrapper arr(p, "queueFamiliesProperties");
            for (const auto &extended_queue_prop : gpu.extended_queue_props) {
                ObjectWrapper queue_obj(p, "");
                {
                    ObjectWrapper obj_queue_props(p, "VkQueueFamilyProperties");
                    VkQueueFamilyProperties props = extended_queue_prop.props;
                    DumpVkExtent3D(p, "minImageTransferGranularity", props.minImageTransferGranularity);
                    p.PrintKeyValue("queueCount", props.queueCount);
                    DumpVkQueueFlags(p, "queueFlags", props.queueFlags);
                    p.PrintKeyValue("timestampValidBits", props.timestampValidBits);
                }
                chain_iterator_queue_properties2(p, gpu, extended_queue_prop.pNext);
            }
        }
    }
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    // Print portability subset extension, features, and properties if available
    if (gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) &&
        (gpu.inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) ||
         gpu.inst.instance_version >= VK_API_VERSION_1_1)) {
        ObjectWrapper macos_obj(p, "macos-specific");
        {
            ObjectWrapper ext_obj(p, "extensions");
            const std::string portability_ext_name = VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME;
            for (const auto &ext : gpu.device_extensions) {
                if (portability_ext_name == ext.extensionName) {
                    p.PrintExtension(ext.extensionName, ext.specVersion);
                }
            }
        }
        {
            ObjectWrapper features_obj(p, "features");
            void *feats_place = gpu.features2.pNext;
            while (feats_place) {
                VkBaseOutStructure *structure = static_cast<VkBaseOutStructure *>(feats_place);
                if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR) {
                    auto *features = reinterpret_cast<VkPhysicalDevicePortabilitySubsetFeaturesKHR *>(structure);
                    DumpVkPhysicalDevicePortabilitySubsetFeaturesKHR(p, "VkPhysicalDevicePortabilitySubsetFeaturesKHR", *features);
                    break;
                }
                feats_place = structure->pNext;
            }
        }
        {
            ObjectWrapper property_obj(p, "properties");
            void *props_place = gpu.props2.pNext;
            while (props_place) {
                VkBaseOutStructure *structure = static_cast<VkBaseOutStructure *>(props_place);
                if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR) {
                    auto *props = reinterpret_cast<VkPhysicalDevicePortabilitySubsetPropertiesKHR *>(structure);
                    DumpVkPhysicalDevicePortabilitySubsetPropertiesKHR(p, "VkPhysicalDevicePortabilitySubsetPropertiesKHR", *props);
                    break;
                }
                props_place = structure->pNext;
            }
        }
    }
#endif  // defined(VK_ENABLE_BETA_EXTENSIONS)
}
void PrintProfileBaseInfo(Printer &p, const std::string &device_name, uint32_t apiVersion, const std::string &device_label,
                          const std::vector<std::string> &capabilities) {
    ObjectWrapper vk_info(p, device_name);
    p.PrintKeyValue("version", 1);
    p.PrintKeyString("api-version", VkVersionString(apiVersion));
    p.PrintKeyString("label", device_label);
    p.PrintKeyString("description", "Exported from vulkaninfo");
    { ObjectWrapper contributors(p, "contributors"); }
    {
        ArrayWrapper contributors(p, "history");
        ObjectWrapper element(p, "");
        p.PrintKeyValue("revision", 1);
        std::time_t t = std::time(0);  // get time now
        std::tm *now = std::localtime(&t);
        std::string date =
            std::to_string(now->tm_year + 1900) + '-' + std::to_string(now->tm_mon + 1) + '-' + std::to_string(now->tm_mday);
        p.PrintKeyString("date", date);
        p.PrintKeyString("author", "Automated export from vulkaninfo");
        p.PrintKeyString("comment", "");
    }
    ArrayWrapper contributors(p, "capabilities");
    for (const auto &str : capabilities) p.PrintString(str);
}

// Prints profiles section of profiles schema
void DumpGpuProfileInfo(Printer &p, AppGpu &gpu) {
    ObjectWrapper profiles(p, "profiles");

    std::string device_label = std::string(gpu.props.deviceName) + " driver " + VkVersionString(gpu.props.driverVersion);
    std::string device_name =
        std::string("VP_VULKANINFO_") + std::string(gpu.props.deviceName) + "_" + VkVersionString(gpu.props.driverVersion);
    ;
    for (auto &c : device_name) {
        if (c == ' ' || c == '.') c = '_';
    }
    PrintProfileBaseInfo(p, device_name, gpu.props.apiVersion, device_label, {"device"});
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    if (gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) &&
        (gpu.inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) ||
         gpu.inst.instance_version >= VK_API_VERSION_1_1)) {
        PrintProfileBaseInfo(p, device_name + "_portability_subset", gpu.props.apiVersion, device_label + " subset",
                             {"device", "macos-specific"});
    }
#endif  // defined(VK_ENABLE_BETA_EXTENSIONS)
}

// Print summary of system
void DumpSummaryInstance(Printer &p, AppInstance &inst) {
    p.SetSubHeader();
    DumpExtensions(p, "Instance Extensions", inst.global_extensions, true);
    p.AddNewline();

    p.SetSubHeader();
    ArrayWrapper arr(p, "Instance Layers", inst.global_layers.size());
    IndentWrapper indent(p);
    std::sort(inst.global_layers.begin(), inst.global_layers.end(), [](LayerExtensionList &left, LayerExtensionList &right) -> int {
        return std::strncmp(left.layer_properties.layerName, right.layer_properties.layerName, VK_MAX_DESCRIPTION_SIZE) < 0;
    });
    size_t layer_name_max = 0;
    size_t layer_desc_max = 0;
    size_t layer_version_max = 0;

    // find max of each type to align everything in columns
    for (auto &layer : inst.global_layers) {
        auto props = layer.layer_properties;
        layer_name_max = std::max(layer_name_max, strlen(props.layerName));
        layer_desc_max = std::max(layer_desc_max, strlen(props.description));
        layer_version_max = std::max(layer_version_max, VkVersionString(layer.layer_properties.specVersion).size());
    }
    for (auto &layer : inst.global_layers) {
        auto v_str = VkVersionString(layer.layer_properties.specVersion);
        auto props = layer.layer_properties;

        auto name_padding = std::string(layer_name_max - strlen(props.layerName), ' ');
        auto desc_padding = std::string(layer_desc_max - strlen(props.description), ' ');
        auto version_padding = std::string(layer_version_max - v_str.size(), ' ');
        p.PrintString(std::string(props.layerName) + name_padding + " " + props.description + desc_padding + " " + v_str + " " +
                      version_padding + " version " + std::to_string(props.implementationVersion));
    }
    p.AddNewline();
}

void DumpSummaryGPU(Printer &p, AppGpu &gpu) {
    ObjectWrapper obj(p, "GPU" + std::to_string(gpu.id));
    p.SetMinKeyWidth(18);
    auto props = gpu.GetDeviceProperties();
    p.PrintKeyValue("apiVersion", props.apiVersion, VkVersionString(props.apiVersion));
    p.PrintKeyValue("driverVersion", props.driverVersion, to_hex_str(props.driverVersion));
    p.PrintKeyString("vendorID", to_hex_str(props.vendorID));
    p.PrintKeyString("deviceID", to_hex_str(props.deviceID));
    p.PrintKeyString("deviceType", VkPhysicalDeviceTypeString(props.deviceType));
    p.PrintKeyString("deviceName", props.deviceName);

    if (gpu.inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) &&
        (gpu.CheckPhysicalDeviceExtensionIncluded(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME) || gpu.api_version.minor >= 2)) {
        void *place = gpu.props2.pNext;
        while (place) {
            VkBaseOutStructure *structure = static_cast<VkBaseOutStructure *>(place);
            if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES) {
                VkPhysicalDeviceDriverProperties *driver_props = reinterpret_cast<VkPhysicalDeviceDriverProperties *>(structure);
                DumpVkDriverId(p, "driverID", driver_props->driverID);
                p.PrintKeyString("driverName", driver_props->driverName);
                p.PrintKeyString("driverInfo", driver_props->driverInfo);
                p.PrintKeyString("conformanceVersion", VkConformanceVersionString(driver_props->conformanceVersion));
            }
            if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES) {
                VkPhysicalDeviceIDProperties *device_id_props = reinterpret_cast<VkPhysicalDeviceIDProperties *>(structure);
                p.PrintKeyString("deviceUUID", to_string_16(device_id_props->deviceUUID));
                p.PrintKeyString("driverUUID", to_string_16(device_id_props->driverUUID));
            }
            place = structure->pNext;
        }
    }
}

// ============ Printing Logic ============= //

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
    SetConsoleTitle(TEXT(app_short_name));
}
#endif

// Global configuration
enum class OutputCategory { text, html, profile_json, vkconfig_output, summary };
const char *help_message_body =
    "OPTIONS:\n"
    "[-h, --help]        Print this help.\n"
    "[--summary]         Show a summary of the instance and GPU's on a system.\n"
    "[-o <filename>, --output <filename>]\n"
    "                    Print output to a new file whose name is specified by filename.\n"
    "                    File will be written to the current working directory.\n"
    "[--text]            Produce a text version of vulkaninfo output to stdout. This is\n"
    "                    the default output.\n"
    "[--html]            Produce an html version of vulkaninfo output, saved as\n"
    "                    \"vulkaninfo.html\" in the directory in which the command\n"
    "                    is run.\n"
    "[-j, --json]        Produce a json version of vulkaninfo output conforming to the Vulkan\n"
    "                    Profiles schema, saved as \n"
    "                     \"VP_VULKANINFO_[DEVICE_NAME]_[DRIVER_VERSION].json\"\n"
    "                     of the first gpu in the system.\n"
    "[-j=<gpu-number>, --json=<gpu-number>]\n"
    "                    For a multi-gpu system, a single gpu can be targetted by\n"
    "                    specifying the gpu-number associated with the gpu of \n"
    "                    interest. This number can be determined by running\n"
    "                    vulkaninfo without any options specified.\n"
    "[--show-tool-props] Show the active VkPhysicalDeviceToolPropertiesEXT that vulkaninfo finds.\n"
    "[--show-formats]    Display the format properties of each physical device.\n"
    "                    Note: This only affects text output.\n";

void print_usage(const std::string &executable_name) {
    std::cout << "\nvulkaninfo - Summarize Vulkan information in relation to the current environment.\n\n";
    std::cout << "USAGE: \n";
    std::cout << "    " << executable_name << " --summary\n";
    std::cout << "    " << executable_name << " -o <filename> | --output <filename>\n";
    std::cout << "    " << executable_name << " -j | -j=<gpu-number> | --json | --json=<gpu-number>\n";
    std::cout << "    " << executable_name << " --text\n";
    std::cout << "    " << executable_name << " --html\n";
    std::cout << "    " << executable_name << " --show-formats\n";
    std::cout << "    " << executable_name << " --show-tool-props\n";
    std::cout << "\n" << help_message_body << std::endl;
}

struct ParsedResults {
    OutputCategory output_category;
    uint32_t selected_gpu;
    bool has_selected_gpu;  // differentiate between selecting the 0th gpu and using the default 0th value
    bool show_tool_props;
    bool show_formats;
    bool print_to_file;
    std::string filename;  // set if explicitely given, or if vkconfig_output has a <path> argument
    std::string default_filename;
};

util::vulkaninfo_optional<ParsedResults> parse_arguments(int argc, char **argv, std::string executable_name) {
    ParsedResults results{};                         // default it to zero init everything
    results.output_category = OutputCategory::text;  // default output category
    results.default_filename = "vulkaninfo.txt";
    for (int i = 1; i < argc; ++i) {
        // A internal-use-only format for communication with the Vulkan Configurator tool
        // Usage "--vkconfig_output <path>"
        // -o can be used to specify the filename instead
        if (0 == strcmp("--vkconfig_output", argv[i])) {
            results.output_category = OutputCategory::vkconfig_output;
            results.print_to_file = true;
            results.default_filename = "vulkaninfo.json";
            if (argc > (i + 1) && argv[i + 1][0] != '-') {
#ifdef WIN32
                results.filename = (std::string(argv[i + 1]) + "\\vulkaninfo.json");
#else
                results.filename = (std::string(argv[i + 1]) + "/vulkaninfo.json");
#endif
                ++i;
            }
        } else if (strncmp("--json", argv[i], 6) == 0 || strcmp(argv[i], "-j") == 0) {
            if (strlen(argv[i]) > 7 && strncmp("--json=", argv[i], 7) == 0) {
                results.selected_gpu = static_cast<uint32_t>(strtol(argv[i] + 7, nullptr, 10));
                results.has_selected_gpu = true;
            }
            results.output_category = OutputCategory::profile_json;
            results.default_filename = "vulkaninfo.json";
            results.print_to_file = true;
        } else if (strcmp(argv[i], "--summary") == 0) {
            results.output_category = OutputCategory::summary;
        } else if (strcmp(argv[i], "--text") == 0) {
            results.output_category = OutputCategory::text;
            results.default_filename = "vulkaninfo.txt";
        } else if (strcmp(argv[i], "--html") == 0) {
            results.output_category = OutputCategory::html;
            results.print_to_file = true;
            results.default_filename = "vulkaninfo.html";
        } else if (strcmp(argv[i], "--show-tool-props") == 0) {
            results.show_tool_props = true;
        } else if (strcmp(argv[i], "--show-formats") == 0) {
            results.show_formats = true;
        } else if ((strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0) && argc > (i + 1)) {
            if (argv[i + 1][0] == '-') {
                std::cout << "-o or --output must be followed by a filename\n";
                return {};
            }
            results.print_to_file = true;
            results.filename = argv[i + 1];
            ++i;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(executable_name);
            return {};
        } else {
            print_usage(executable_name);
            return {};
        }
    }
    return results;
}

PrinterCreateDetails get_printer_create_details(ParsedResults &parse_data, AppInstance &inst, AppGpu &selected_gpu,
                                                std::string const &executable_name) {
    PrinterCreateDetails create{};
    create.print_to_file = parse_data.print_to_file;
    create.file_name = (!parse_data.filename.empty()) ? parse_data.filename : parse_data.default_filename;
    switch (parse_data.output_category) {
        default:
        case (OutputCategory::text):
            create.output_type = OutputType::text;
            break;
        case (OutputCategory::html):
            create.output_type = OutputType::html;
            break;
        case (OutputCategory::profile_json):
            create.output_type = OutputType::json;
            create.start_string =
                std::string("{\n\t\"$schema\": ") + "\"https://schema.khronos.org/vulkan/profiles-0.8-latest.json\"";
            if (parse_data.filename.empty()) {
                create.file_name = std::string("VP_VULKANINFO_") + std::string(selected_gpu.props.deviceName) + "_" +
                                   VkVersionString(selected_gpu.props.driverVersion);
                for (auto &c : create.file_name) {
                    if (c == ' ' || c == '.') c = '_';
                }
                create.file_name += ".json";
            }
            break;
        case (OutputCategory::vkconfig_output):
            create.output_type = OutputType::vkconfig_output;
            create.start_string = "{\n\t\"Vulkan Instance Version\": \"" + VkVersionString(inst.vk_version) + "\"";
            break;
    }
    return create;
}

void RunPrinter(Printer &p, ParsedResults parse_data, AppInstance &instance, std::vector<std::unique_ptr<AppGpu>> &gpus,
                std::vector<std::unique_ptr<AppSurface>> &surfaces) {
#ifdef VK_USE_PLATFORM_IOS_MVK
    p.SetAlwaysOpenDetails(true);
#endif
    if (parse_data.output_category == OutputCategory::summary) {
        DumpSummaryInstance(p, instance);
        p.SetHeader();
        ObjectWrapper obj(p, "Devices");
        IndentWrapper indent(p);
        for (auto &gpu : gpus) {
            DumpSummaryGPU(p, *(gpu.get()));
        }
    } else if (parse_data.output_category == OutputCategory::profile_json) {
        DumpGpuProfileCapabilities(p, *(gpus.at(parse_data.selected_gpu).get()));
        DumpGpuProfileInfo(p, *(gpus.at(parse_data.selected_gpu).get()));
    } else {
        // text, html, vkconfig_output
        p.SetHeader();
        DumpExtensions(p, "Instance Extensions", instance.global_extensions);
        p.AddNewline();

        DumpLayers(p, instance.global_layers, gpus);
#if defined(VULKANINFO_WSI_ENABLED)
        // Doesn't print anything if no surfaces are available
        DumpPresentableSurfaces(p, instance, gpus, surfaces);
#endif  // defined(VULKANINFO_WSI_ENABLED)
        DumpGroups(p, instance);

        p.SetHeader();
        ObjectWrapper obj(p, "Device Properties and Extensions");
        IndentWrapper indent(p);

        for (auto &gpu : gpus) {
            DumpGpu(p, *(gpu.get()), parse_data.show_tool_props, parse_data.show_formats);
        }
    }
}

#ifdef VK_USE_PLATFORM_IOS_MVK
// On iOS, we'll call this ourselves from a parent routine in the GUI
int vulkanInfoMain(int argc, char **argv) {
#else
int main(int argc, char **argv) {
#endif

    // Figure out the name of the executable, pull out the name if given a path
    // Default is `vulkaninfo`
    std::string executable_name = "vulkaninfo";
    if (argc >= 1) {
        const auto argv_0 = std::string(argv[0]);
        // don't include path separator
        // Look for forward slash first, only look for backslash if that found nothing
        auto last_occurrence = argv_0.rfind('/');
        if (last_occurrence == std::string::npos) {
            last_occurrence = argv_0.rfind('\\');
        }
        if (last_occurrence != std::string::npos && last_occurrence + 1 < argv_0.size()) {
            executable_name = argv_0.substr(last_occurrence + 1);
        }
    }

    auto parsing_return = parse_arguments(argc, argv, executable_name);
    if (!parsing_return) return 1;
    ParsedResults parse_data = parsing_return.value();

#if defined(_WIN32)
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

    if (ConsoleIsExclusive()) ConsoleEnlarge();
    User32Handles local_user32_handles;
    user32_handles = &local_user32_handles;
    if (!local_user32_handles.load()) {
        fprintf(stderr, "Failed to load user32.dll library!\n");
        if (parse_data.output_category == OutputCategory::text && !parse_data.print_to_file) wait_for_console_destroy();
        return 1;
    }
#endif

    int return_code = 0;  // set in case of error
    std::unique_ptr<Printer> printer;
    std::ostream std_out(std::cout.rdbuf());
    std::ofstream file_out;
    std::ostream *out = &std_out;

    // if any essential vulkan call fails, it throws an exception
    try {
        AppInstance instance = {};
        SetupWindowExtensions(instance);

        auto phys_devices = instance.FindPhysicalDevices();

        std::vector<std::unique_ptr<AppSurface>> surfaces;
#if defined(VULKANINFO_WSI_ENABLED)
        for (auto &surface_extension : instance.surface_extensions) {
            surface_extension.create_window(instance);
            surface_extension.surface = surface_extension.create_surface(instance);
            for (auto &phys_device : phys_devices) {
                try {
                    surfaces.push_back(std::unique_ptr<AppSurface>(new AppSurface(instance, phys_device, surface_extension)));
                } catch (std::exception &e) {
                    std::cerr << "ERROR while creating surface for extension " << surface_extension.name << " : " << e.what()
                              << "\n";
                }
            }
        }
#endif  // defined(VULKANINFO_WSI_ENABLED)

        std::vector<std::unique_ptr<AppGpu>> gpus;

        uint32_t gpu_counter = 0;
        for (auto &phys_device : phys_devices) {
            gpus.push_back(std::unique_ptr<AppGpu>(new AppGpu(instance, gpu_counter++, phys_device)));
        }

        if (parse_data.selected_gpu >= gpus.size()) {
            if (parse_data.has_selected_gpu) {
                std::cout << "The selected gpu (" << parse_data.selected_gpu << ") is not a valid GPU index. ";
                if (gpus.size() == 0) {
                    std::cout << "vulkaninfo could not find any GPU's.\n";
                    return 1;
                } else {
                    if (gpus.size() == 1) {
                        std::cout << "The only available GPU selection is 0.\n";
                    } else {
                        std::cout << "The available GPUs are in the range of 0 to " << gpus.size() - 1 << ".\n";
                    }
                    return 1;
                }
            } else if (parse_data.output_category == OutputCategory::profile_json) {
                std::cout << "vulkaninfo could not find any GPU's.\n";
            }
        }

        auto printer_data = get_printer_create_details(parse_data, instance, *gpus.at(parse_data.selected_gpu), executable_name);
        if (printer_data.print_to_file) {
            file_out = std::ofstream(printer_data.file_name);
            out = &file_out;
        }
        printer = std::unique_ptr<Printer>(new Printer(printer_data, *out, instance.vk_version));

        RunPrinter(*(printer.get()), parse_data, instance, gpus, surfaces);

#if defined(VULKANINFO_WSI_ENABLED)
        for (auto &surface_extension : instance.surface_extensions) {
            AppDestroySurface(instance, surface_extension.surface);
            surface_extension.destroy_window(instance);
        }
#endif  // defined(VULKANINFO_WSI_ENABLED)
    } catch (std::exception &e) {
        // Print the error to stderr and leave all outputs in a valid state (mainly for json)
        std::cerr << "ERROR at " << e.what() << "\n";
        if (printer) {
            printer->FinishOutput();
        }
        return_code = 1;
    }
    // Call the printer's destructor before the file handle gets closed
    printer.reset(nullptr);

#ifdef _WIN32
    if (parse_data.output_category == OutputCategory::text && !parse_data.print_to_file) wait_for_console_destroy();
#endif

    return return_code;
}
