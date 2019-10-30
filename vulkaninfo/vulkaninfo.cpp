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

#include "vulkaninfo.hpp"

// =========== Dump Functions ========= //

void DumpExtensions(Printer &p, std::string layer_name, std::vector<VkExtensionProperties> extensions) {
    std::sort(extensions.begin(), extensions.end(), [](VkExtensionProperties &a, VkExtensionProperties &b) -> int {
        return std::string(a.extensionName) < std::string(b.extensionName);
    });

    if (p.Type() == OutputType::json) return;

    int max_length = 0;
    if (extensions.size() > 0) {
        max_length = strlen(extensions.at(0).extensionName);
        for (auto &ext : extensions) {
            int len = strlen(ext.extensionName);
            if (len > max_length) max_length = len;
        }
    }

    p.ArrayStart(layer_name + " Extensions", extensions.size());
    for (auto &ext : extensions) {
        p.PrintExtension(ext.extensionName, ext.specVersion, max_length);
    }
    p.ArrayEnd();
}

void DumpLayers(Printer &p, std::vector<LayerExtensionList> layers, const std::vector<std::unique_ptr<AppGpu>> &gpus) {
    std::sort(layers.begin(), layers.end(), [](LayerExtensionList &left, LayerExtensionList &right) -> int {
        const char *a = left.layer_properties.layerName;
        const char *b = right.layer_properties.layerName;
        return a && (!b || std::strcmp(a, b) < 0);
    });

    if (p.Type() == OutputType::text || p.Type() == OutputType::html) {
        p.SetHeader().ArrayStart("Layers", layers.size());
        p.IndentDecrease();
        for (auto &layer : layers) {
            auto v_str = VkVersionString(layer.layer_properties.specVersion);
            auto props = layer.layer_properties;

            std::string header;
            if (p.Type() == OutputType::text)
                header = std::string(props.layerName) + " (" + props.description + ") Vulkan version " + v_str +
                         ", layer version " + std::to_string(props.implementationVersion);
            else if (p.Type() == OutputType::html)
                header = std::string("<span class='type'>") + props.layerName + "</span> (" + props.description +
                         ") Vulkan version <span class='val'>" + v_str + "</span>, layer version <span class='val'>" +
                         std::to_string(props.implementationVersion) + "</span>";

            p.ObjectStart(header);
            DumpExtensions(p, "Layer", layer.extension_properties);

            p.ArrayStart("Devices", gpus.size());
            for (auto &gpu : gpus) {
                p.PrintElement(std::string("GPU id \t: ") + std::to_string(gpu->id), gpu->props.deviceName);
                auto exts = gpu->AppGetPhysicalDeviceLayerExtensions(props.layerName);
                DumpExtensions(p, "Layer-Device", exts);
                p.AddNewline();
            }
            p.ArrayEnd();
            p.ObjectEnd();
        }
        p.IndentIncrease();
        p.ArrayEnd();
    } else if (p.Type() == OutputType::json) {
        p.ArrayStart("ArrayOfVkLayerProperties", layers.size());
        int i = 0;
        for (auto &layer : layers) {
            p.SetElementIndex(i++);
            DumpVkLayerProperties(p, "layerProperty", layer.layer_properties);
        }
        p.ArrayEnd();
    }
}

void DumpSurfaceFormats(Printer &p, AppInstance &inst, AppSurface &surface) {
    p.ArrayStart("Formats", surface.surf_formats2.size());
    int i = 0;
    if (inst.CheckExtensionEnabled(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME)) {
        for (auto &format : surface.surf_formats2) {
            p.SetElementIndex(i++);
            DumpVkSurfaceFormatKHR(p, "SurfaceFormat", format.surfaceFormat);
        }
    } else {
        for (auto &format : surface.surf_formats) {
            p.SetElementIndex(i++);
            DumpVkSurfaceFormatKHR(p, "SurfaceFormat", format);
        }
    }
    p.ArrayEnd();
}

void DumpPresentModes(Printer &p, AppInstance &inst, AppSurface &surface) {
    p.ArrayStart("Present Modes", surface.surf_present_modes.size());
    for (auto &mode : surface.surf_present_modes) {
        p.SetAsType().PrintElement(VkPresentModeKHRString(mode));
    }
    p.ArrayEnd();
}

void DumpSurfaceCapabilities(Printer &p, AppInstance &inst, AppGpu &gpu, AppSurface &surface) {
    auto &surf_cap = surface.surface_capabilities;
    p.SetSubHeader();
    DumpVkSurfaceCapabilitiesKHR(p, "VkSurfaceCapabilitiesKHR", surf_cap);

    p.SetSubHeader().ObjectStart("VkSurfaceCapabilities2EXT");
    {
        p.ObjectStart("supportedSurfaceCounters");
        if (surface.surface_capabilities2_ext.supportedSurfaceCounters == 0) p.PrintElement("None");
        if (surface.surface_capabilities2_ext.supportedSurfaceCounters & VK_SURFACE_COUNTER_VBLANK_EXT) {
            p.SetAsType().PrintElement("VK_SURFACE_COUNTER_VBLANK_EXT");
        }
        p.ObjectEnd();
    }
    p.ObjectEnd();  // VkSurfaceCapabilities2EXT

    chain_iterator_surface_capabilities2(p, inst, gpu, surface.surface_capabilities2_khr.pNext);
}

void DumpSurface(Printer &p, AppInstance &inst, AppGpu &gpu, AppSurface &surface, std::vector<std::string> surface_types) {
    std::string header;
    if (p.Type() == OutputType::text)
        header = std::string("GPU id : ") + std::to_string(gpu.id) + " (" + gpu.props.deviceName + ")";
    else if (p.Type() == OutputType::html)
        header = std::string("GPU id : <span class='val'>") + std::to_string(gpu.id) + "</span> (" + gpu.props.deviceName + ")";
    p.ObjectStart(header);

    if (surface_types.size() == 0) {
        p.SetAsType().PrintKeyValue("Surface type", surface.surface_extension.name);
    } else {
        p.ArrayStart("Surface types", surface_types.size());
        for (auto &name : surface_types) {
            p.PrintElement(name);
        }
        p.ArrayEnd();
    }

    DumpSurfaceFormats(p, inst, surface);

    DumpPresentModes(p, inst, surface);

    DumpSurfaceCapabilities(p, inst, gpu, surface);

    p.ObjectEnd();
    p.AddNewline();
}

struct SurfaceTypeGroup {
    AppSurface *surface;
    std::vector<std::string> surface_types;
    AppGpu *gpu;
};

bool operator==(AppSurface const &a, AppSurface const &b) {
    return a.surf_present_modes == b.surf_present_modes && a.surf_formats == b.surf_formats && a.surf_formats2 == b.surf_formats2 &&
           a.surface_capabilities == b.surface_capabilities && a.surface_capabilities2_khr == b.surface_capabilities2_khr &&
           a.surface_capabilities2_ext == b.surface_capabilities2_ext;
}

void DumpPresentableSurfaces(Printer &p, AppInstance &inst, const std::vector<std::unique_ptr<AppGpu>> &gpus,
                             const std::vector<std::unique_ptr<AppSurface>> &surfaces) {
    p.SetHeader().ObjectStart("Presentable Surfaces");
    p.IndentDecrease();
    std::vector<SurfaceTypeGroup> surface_list;

    for (auto &surface : surfaces) {
        for (auto &gpu : gpus) {
            auto exists = surface_list.end();
            for (auto it = surface_list.begin(); it != surface_list.end(); it++) {
                // use custom comparator to check if the surface has the same values
                if (it->gpu == gpu.get() && *it->surface == *surface.get()) {
                    exists = it;
                }
            }
            if (exists != surface_list.end()) {
                exists->surface_types.push_back(surface.get()->surface_extension.name);
            } else {
                surface_list.push_back({surface.get(), {surface.get()->surface_extension.name}, gpu.get()});
            }
        }
    }
    for (auto &group : surface_list) {
        DumpSurface(p, inst, *group.gpu, *group.surface, group.surface_types);
    }
    p.IndentIncrease();
    p.ObjectEnd();
    p.AddNewline();
}

void DumpGroups(Printer &p, AppInstance &inst) {
    if (inst.CheckExtensionEnabled(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME)) {
        p.SetHeader().ObjectStart("Groups");
        auto groups = GetGroups(inst);
        int group_id = 0;
        for (auto &group : groups) {
            p.ObjectStart("Device Group Properties (Group " + std::to_string(group_id) + ")");
            auto group_props = GetGroupProps(group);
            p.ArrayStart("physicalDeviceCount", group.physicalDeviceCount);
            int id = 0;
            for (auto &prop : group_props) {
                std::string device_out = prop.deviceName;
                if (p.Type() == OutputType::text) {
                    device_out += " (ID: " + std::to_string(id++) + ")";
                } else if (p.Type() == OutputType::html) {
                    device_out += " (ID: <span class='val'>" + std::to_string(id++) + "</span>)";
                }
                p.PrintElement(device_out);
            }
            p.ArrayEnd();
            p.PrintKeyValue("subsetAllocation", group.subsetAllocation);
            p.ObjectEnd();
            p.AddNewline();

            p.ObjectStart("Device Group Present Capabilities (Group " + std::to_string(group_id) + ")");
            auto group_capabilities = GetGroupCapabilities(inst, group);
            if (group_capabilities.first == false) {
                p.PrintElement("Group does not support VK_KHR_device_group, skipping printing capabilities");
            } else {
                for (uint32_t i = 0; i < group.physicalDeviceCount; i++) {
                    std::string device_out;
                    if (p.Type() == OutputType::text) {
                        device_out = std::string(group_props[i].deviceName) + " (ID: " + std::to_string(i) + ")";
                    } else if (p.Type() == OutputType::html) {
                        device_out =
                            std::string(group_props[i].deviceName) + " (ID: <span class='val'>" + std::to_string(i) + "</span>)";
                    }
                    p.PrintElement(device_out);
                    p.ObjectStart("Can present images from the following devices");
                    for (uint32_t j = 0; j < group.physicalDeviceCount; j++) {
                        uint32_t mask = 1 << j;
                        if (group_capabilities.second.presentMask[i] & mask) {
                            if (p.Type() == OutputType::text)
                                p.PrintElement(std::string(group_props[j].deviceName) + " (ID: " + std::to_string(j) + ")");
                            if (p.Type() == OutputType::html)
                                p.PrintElement(std::string(group_props[j].deviceName) + " (ID: <span class='val'>" +
                                               std::to_string(j) + "</span>)");
                        }
                    }
                    p.ObjectEnd();
                }
                DumpVkDeviceGroupPresentModeFlagsKHR(p, "Present modes", group_capabilities.second.modes);
            }
            p.ObjectEnd();
            p.AddNewline();
            group_id++;
        }
        p.ObjectEnd();
        p.AddNewline();
    }
}

void GpuDumpProps(Printer &p, AppGpu &gpu) {
    auto props = gpu.GetDeviceProperties();
    p.SetSubHeader().ObjectStart("VkPhysicalDeviceProperties");
    p.PrintKeyValue("apiVersion", props.apiVersion, 14, VkVersionString(props.apiVersion));
    p.PrintKeyValue("driverVersion", props.driverVersion, 14, to_hex_str(props.driverVersion));
    if (p.Type() == OutputType::json) {
        p.PrintKeyValue("vendorID", props.vendorID, 14);
        p.PrintKeyValue("deviceID", props.deviceID, 14);
        p.PrintKeyValue("deviceType", props.deviceType, 14);
    } else {
        p.PrintKeyValue("vendorID", to_hex_str(props.vendorID), 14);
        p.PrintKeyValue("deviceID", to_hex_str(props.deviceID), 14);
        p.PrintKeyString("deviceType", VkPhysicalDeviceTypeString(props.deviceType), 14);
    }
    p.PrintKeyString("deviceName", props.deviceName, 14);
    if (p.Type() == OutputType::json) {
        p.ArrayStart("pipelineCacheUUID");
        for (uint32_t i = 0; i < VK_UUID_SIZE; ++i) {
            p.PrintElement(static_cast<uint32_t>(props.pipelineCacheUUID[i]));
        }
        p.ArrayEnd();
    }
    p.AddNewline();
    if (p.Type() != OutputType::json) {
        p.ObjectEnd();  // limits and sparse props are not sub objects in the text and html output
    }

    if (gpu.inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
        DumpVkPhysicalDeviceLimits(p, "VkPhysicalDeviceLimits", gpu.props2.properties.limits);
    } else {
        DumpVkPhysicalDeviceLimits(p, "VkPhysicalDeviceLimits", gpu.props.limits);
    }
    p.AddNewline();
    if (gpu.inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
        DumpVkPhysicalDeviceSparseProperties(p, "VkPhysicalDeviceSparseProperties", gpu.props2.properties.sparseProperties);
    } else {
        DumpVkPhysicalDeviceSparseProperties(p, "VkPhysicalDeviceSparseProperties", gpu.props.sparseProperties);
    }
    p.AddNewline();
    if (p.Type() == OutputType::json) {
        p.ObjectEnd();  // limits and sparse props are sub objects in the json output
    }

    if (p.Type() != OutputType::json) {
        if (gpu.inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
            void *place = gpu.props2.pNext;
            chain_iterator_phys_device_props2(p, gpu, place);
        }
    }
    p.AddNewline();
}
void GpuDumpQueueProps(Printer &p, std::vector<SurfaceExtension> &surfaces, AppQueueFamilyProperties &queue) {
    p.SetHeader().SetElementIndex(queue.queue_index).ObjectStart("VkQueueFamilyProperties");
    if (p.Type() == OutputType::json) {
        DumpVkExtent3D(p, "minImageTransferGranularity", queue.props.minImageTransferGranularity);
    } else {
        p.PrintKeyString("minImageTransferGranularity", VkExtent3DString(queue.props.minImageTransferGranularity), 27);
    }
    p.PrintKeyValue("queueCount", queue.props.queueCount, 27);
    if (p.Type() == OutputType::json) {
        p.PrintKeyValue("queueFlags", queue.props.queueFlags, 27);
    } else {
        p.PrintKeyValue("queueFlags", VkQueueFlagsString(queue.props.queueFlags), 27);
    }

    p.PrintKeyValue("timestampValidBits", queue.props.timestampValidBits, 27);

    if (p.Type() != OutputType::json) {
        if (queue.is_present_platform_agnostic) {
            p.PrintKeyString("present support", queue.platforms_support_present ? "true" : "false");
        } else {
            p.ObjectStart("present support");
            for (auto &surface : surfaces) {
                p.PrintKeyString(surface.name, surface.supports_present ? "true" : "false", 19);
            }
            p.ObjectEnd();
        }
    }
    p.ObjectEnd();
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

void GpuDumpMemoryProps(Printer &p, AppGpu &gpu) {
    p.SetHeader().ObjectStart("VkPhysicalDeviceMemoryProperties");
    p.IndentDecrease();
    p.ArrayStart("memoryHeaps", gpu.memory_props.memoryHeapCount);
    for (uint32_t i = 0; i < gpu.memory_props.memoryHeapCount; ++i) {
        const VkDeviceSize memSize = gpu.memory_props.memoryHeaps[i].size;
        std::string mem_size_human_readable = NumToNiceStr(static_cast<size_t>(memSize));

        std::string mem_size_str = std::to_string(memSize) + " (" + to_hex_str(memSize) + ") (" + mem_size_human_readable + ")";

        p.SetElementIndex(i).ObjectStart("memoryHeaps");
        if (p.Type() != OutputType::json) {
            p.PrintKeyValue("size", mem_size_str, 6);
            p.PrintKeyValue("budget", gpu.heapBudget[i], 6);
            p.PrintKeyValue("usage", gpu.heapUsage[i], 6);
            DumpVkMemoryHeapFlags(p, "flags", gpu.memory_props.memoryHeaps[i].flags, 6);
        } else {
            p.PrintKeyValue("flags", gpu.memory_props.memoryHeaps[i].flags);
            p.PrintKeyValue("size", memSize);
        }
        p.ObjectEnd();
    }
    p.ArrayEnd();

    p.ArrayStart("memoryTypes", gpu.memory_props.memoryTypeCount);
    for (uint32_t i = 0; i < gpu.memory_props.memoryTypeCount; ++i) {
        p.SetElementIndex(i).ObjectStart("memoryTypes");
        p.PrintKeyValue("heapIndex", gpu.memory_props.memoryTypes[i].heapIndex, 13);
        if (p.Type() == OutputType::json) {
            p.PrintKeyValue("propertyFlags", gpu.memory_props.memoryTypes[i].propertyFlags, 13);
        } else {
            auto flags = gpu.memory_props.memoryTypes[i].propertyFlags;
            DumpVkMemoryPropertyFlags(p, "propertyFlags = " + to_hex_str(flags), flags);

            p.ObjectStart("usable for");
            const uint32_t memtype_bit = 1 << i;

            // only linear and optimal tiling considered
            for (uint32_t tiling = VK_IMAGE_TILING_OPTIMAL; tiling < gpu.mem_type_res_support.image.size(); ++tiling) {
                std::string usable;
                usable += std::string(VkImageTilingString(VkImageTiling(tiling))) + ": ";
                size_t orig_usable_str_size = usable.size();
                bool first = true;
                for (size_t fmt_i = 0; fmt_i < gpu.mem_type_res_support.image[tiling].size(); ++fmt_i) {
                    const MemImageSupport *image_support = &gpu.mem_type_res_support.image[tiling][fmt_i];
                    const bool regular_compatible =
                        image_support->regular_supported && (image_support->regular_memtypes & memtype_bit);
                    const bool sparse_compatible =
                        image_support->sparse_supported && (image_support->sparse_memtypes & memtype_bit);
                    const bool transient_compatible =
                        image_support->transient_supported && (image_support->transient_memtypes & memtype_bit);

                    if (regular_compatible || sparse_compatible || transient_compatible) {
                        if (!first) usable += ", ";
                        first = false;

                        if (fmt_i == 0) {
                            usable += "color images";
                        } else {
                            usable += VkFormatString(gpu.mem_type_res_support.image[tiling][fmt_i].format);
                        }

                        if (regular_compatible && !sparse_compatible && !transient_compatible && image_support->sparse_supported &&
                            image_support->transient_supported) {
                            usable += "(non-sparse, non-transient)";
                        } else if (regular_compatible && !sparse_compatible && image_support->sparse_supported) {
                            if (image_support->sparse_supported) usable += "(non-sparse)";
                        } else if (regular_compatible && !transient_compatible && image_support->transient_supported) {
                            if (image_support->transient_supported) usable += "(non-transient)";
                        } else if (!regular_compatible && sparse_compatible && !transient_compatible &&
                                   image_support->sparse_supported) {
                            if (image_support->sparse_supported) usable += "(sparse only)";
                        } else if (!regular_compatible && !sparse_compatible && transient_compatible &&
                                   image_support->transient_supported) {
                            if (image_support->transient_supported) usable += "(transient only)";
                        } else if (!regular_compatible && sparse_compatible && transient_compatible &&
                                   image_support->sparse_supported && image_support->transient_supported) {
                            usable += "(sparse and transient only)";
                        }
                    }
                }
                if (usable.size() == orig_usable_str_size)  // not usable for anything
                {
                    usable += "None";
                }
                p.PrintElement(usable);
            }
            p.ObjectEnd();
        }

        p.ObjectEnd();
    }
    p.ArrayEnd();
    p.IndentIncrease();
    p.ObjectEnd();
    p.AddNewline();
}

void GpuDumpFeatures(Printer &p, AppGpu &gpu) {
    p.SetHeader();
    DumpVkPhysicalDeviceFeatures(p, "VkPhysicalDeviceFeatures", gpu.features);
    p.AddNewline();
    if (p.Type() != OutputType::json) {
        if (gpu.inst.CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
            void *place = gpu.features2.pNext;
            chain_iterator_phys_device_features2(p, gpu, place);
        }
    }
}

void GpuDumpFormatProperty(Printer &p, VkFormat fmt, VkFormatProperties prop) {
    if (p.Type() == OutputType::text) {
        p.ObjectStart("Properties");
    } else if (p.Type() == OutputType::html) {
        p.SetTitleAsType().ObjectStart(VkFormatString(fmt));
    } else if (p.Type() == OutputType::json) {
        p.ObjectStart("");
    }
    if (p.Type() == OutputType::html || p.Type() == OutputType::text) {
        p.SetOpenDetails();
        DumpVkFormatFeatureFlags(p, "linearTiling", prop.linearTilingFeatures);
        p.SetOpenDetails();
        DumpVkFormatFeatureFlags(p, "optimalTiling", prop.optimalTilingFeatures);
        p.SetOpenDetails();
        DumpVkFormatFeatureFlags(p, "bufferFeatures", prop.bufferFeatures);
    } else if (p.Type() == OutputType::json) {
        p.PrintKeyValue("formatID", fmt);
        p.PrintKeyValue("linearTilingFeatures", prop.linearTilingFeatures);
        p.PrintKeyValue("optimalTilingFeatures", prop.optimalTilingFeatures);
        p.PrintKeyValue("bufferFeatures", prop.bufferFeatures);
    }
    p.ObjectEnd();
}

void GpuDevDump(Printer &p, AppGpu &gpu, pNextChainInfos &chainInfos) {
    if (p.Type() == OutputType::json) {
        p.ArrayStart("ArrayOfVkFormatProperties");
    } else {
        p.SetHeader().ObjectStart("Format Properties");
        p.IndentDecrease();
    }

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

            p.SetElementIndex(counter++).ObjectStart("Common Format Group");
            p.IndentDecrease();
            p.ObjectStart("Formats");
            for (auto &fmt : prop.second) {
                p.SetAsType().PrintElement(VkFormatString(fmt));
            }
            p.ObjectEnd();

            GpuDumpFormatProperty(p, VK_FORMAT_UNDEFINED, props);

            p.IndentIncrease();
            p.ObjectEnd();
            p.AddNewline();
        }

        p.ObjectStart("Unsupported Formats");
        for (auto &fmt : unsupported_formats) {
            p.SetAsType().PrintElement(VkFormatString(fmt));
        }
        p.ObjectEnd();

    } else {
        for (auto &format : gpu.supported_format_ranges) {
            if (gpu.FormatRangeSupported(format)) {
                for (uint32_t fmt_counter = format.first_format; fmt_counter <= format.last_format; ++fmt_counter) {
                    VkFormat fmt = static_cast<VkFormat>(fmt_counter);

                    VkFormatProperties props;
                    vkGetPhysicalDeviceFormatProperties(gpu.phys_device, fmt, &props);

                    // if json, don't print format properties that are unsupported
                    if (p.Type() == OutputType::json &&
                        (props.linearTilingFeatures || props.optimalTilingFeatures || props.bufferFeatures) == 0)
                        continue;

                    GpuDumpFormatProperty(p, fmt, props);
                }
            }
        }
    }

    if (p.Type() == OutputType::json) {
        p.ArrayEnd();
    } else {
        p.IndentIncrease();
        p.ObjectEnd();
    }

    p.AddNewline();
}

void DumpGpu(Printer &p, AppGpu &gpu, bool show_formats, pNextChainInfos &chainInfos) {
    if (p.Type() != OutputType::json) {
        p.ObjectStart("GPU" + std::to_string(gpu.id));
        p.IndentDecrease();
    }
    GpuDumpProps(p, gpu);

    if (p.Type() != OutputType::json) {
        DumpExtensions(p, "Device", gpu.device_extensions);
        p.AddNewline();
    }

    if (p.Type() == OutputType::json) p.ArrayStart("ArrayOfVkQueueFamilyProperties");
    for (uint32_t i = 0; i < gpu.queue_count; i++) {
        AppQueueFamilyProperties queue_props = AppQueueFamilyProperties(gpu, i);
        GpuDumpQueueProps(p, gpu.inst.surface_extensions, queue_props);
    }
    if (p.Type() == OutputType::json) p.ArrayEnd();

    GpuDumpMemoryProps(p, gpu);
    GpuDumpFeatures(p, gpu);
    if (p.Type() != OutputType::text || show_formats) {
        GpuDevDump(p, gpu, chainInfos);
    }

    if (p.Type() != OutputType::json) {
        p.IndentIncrease();
        p.ObjectEnd();
    }
    p.AddNewline();
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

void print_usage(const char *argv0) {
    std::cout << "\nvulkaninfo - Summarize Vulkan information in relation to the current environment.\n\n";
    std::cout << "USAGE: " << argv0 << " [options]\n\n";
    std::cout << "OPTIONS:\n";
    std::cout << "-h, --help            Print this help.\n";
    std::cout << "--html                Produce an html version of vulkaninfo output, saved as\n";
    std::cout << "                      \"vulkaninfo.html\" in the directory in which the command is\n";
    std::cout << "                      run.\n";
    std::cout << "-j, --json            Produce a json version of vulkaninfo to standard output of the\n";
    std::cout << "                      first gpu in the system conforming to the DevSim schema.\n";
    std::cout << "--json=<gpu-number>   For a multi-gpu system, a single gpu can be targetted by\n";
    std::cout << "                      specifying the gpu-number associated with the gpu of \n";
    std::cout << "                      interest. This number can be determined by running\n";
    std::cout << "                      vulkaninfo without any options specified.\n";
    std::cout << "--show-formats        Display the format properties of each physical device.\n\n";
}

int main(int argc, char **argv) {
#ifdef _WIN32
    if (ConsoleIsExclusive()) ConsoleEnlarge();
#endif

    uint32_t selected_gpu = 0;

    // Combinations of output: html only, html AND json, json only, human readable only
    for (int i = 1; i < argc; ++i) {
        if (strncmp("--json", argv[i], 6) == 0 || strcmp(argv[i], "-j") == 0) {
            if (strlen(argv[i]) > 7 && strncmp("--json=", argv[i], 7) == 0) {
                selected_gpu = strtol(argv[i] + 7, nullptr, 10);
            }
            human_readable_output = false;
            json_output = true;
        } else if (strcmp(argv[i], "--html") == 0) {
            human_readable_output = false;
            html_output = true;
        } else if (strcmp(argv[i], "--show-formats") == 0) {
            show_formats = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 1;
        } else {
            print_usage(argv[0]);
            return 1;
        }
    }

    AppInstance instance = {};
    SetupWindowExtensions(instance);

    auto pNext_chains = get_chain_infos();

    auto phys_devices = instance.FindPhysicalDevices();

    std::vector<std::unique_ptr<AppSurface>> surfaces;
#if defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WIN32_KHR) || \
    defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_WAYLAND_KHR)
    for (auto &surface_extension : instance.surface_extensions) {
        surface_extension.create_window(instance);
        surface_extension.surface = surface_extension.create_surface(instance);
        for (auto &phys_device : phys_devices) {
            surfaces.push_back(std::unique_ptr<AppSurface>(
                new AppSurface(instance, phys_device, surface_extension, pNext_chains.surface_capabilities2)));
        }
    }
#endif

    std::vector<std::unique_ptr<AppGpu>> gpus;

    uint32_t gpu_counter = 0;
    for (auto &phys_device : phys_devices) {
        gpus.push_back(std::unique_ptr<AppGpu>(new AppGpu(instance, gpu_counter++, phys_device, pNext_chains)));
    }

    if (selected_gpu >= gpus.size()) {
        selected_gpu = 0;
    }

    std::vector<std::unique_ptr<Printer>> printers;

    std::streambuf *buf;
    buf = std::cout.rdbuf();
    std::ostream out(buf);
    std::ofstream html_out;

    if (human_readable_output) {
        printers.push_back(std::unique_ptr<Printer>(new Printer(OutputType::text, out, selected_gpu, instance.vk_version)));
    }
    if (html_output) {
        html_out = std::ofstream("vulkaninfo.html");
        printers.push_back(std::unique_ptr<Printer>(new Printer(OutputType::html, html_out, selected_gpu, instance.vk_version)));
    }
    if (json_output) {
        printers.push_back(std::unique_ptr<Printer>(new Printer(OutputType::json, out, selected_gpu, instance.vk_version)));
    }

    for (auto &p : printers) {
        p->SetHeader();
        DumpExtensions(*p.get(), "Instance", instance.global_extensions);
        p->AddNewline();

        DumpLayers(*p.get(), instance.global_layers, gpus);

        if (p->Type() != OutputType::json) {
#if defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WIN32_KHR) || \
    defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_WAYLAND_KHR)
            DumpPresentableSurfaces(*p.get(), instance, gpus, surfaces);
#endif
            DumpGroups(*p.get(), instance);

            p->SetHeader().ObjectStart("Device Properties and Extensions");
            p->IndentDecrease();
        }
        for (auto &gpu : gpus) {
            if ((p->Type() == OutputType::json && gpu->id == selected_gpu) || p->Type() == OutputType::text ||
                p->Type() == OutputType::html) {
                DumpGpu(*p.get(), *gpu.get(), show_formats, pNext_chains);
            }
        }
        if (p->Type() != OutputType::json) {
            p->IndentIncrease();
            p->ObjectEnd();
        }
    }

#if defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WIN32_KHR) || \
    defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_WAYLAND_KHR)

    for (auto &surface_extension : instance.surface_extensions) {
        AppDestroySurface(instance, surface_extension.surface);
        surface_extension.destroy_window(instance);
    }
#endif

    WAIT_FOR_CONSOLE_DESTROY;

    return 0;
}
