#!/usr/bin/python3
#
# Copyright (c) 2019-2022 Valve Corporation
# Copyright (c) 2019-2022 LunarG, Inc.
# Copyright (c) 2019-2022 Google Inc.
# Copyright (c) 2023-2024 RasterGrid Kft.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Author: Charles Giessen <charles@lunarg.com>

from base_generator import BaseGenerator

from collections import OrderedDict

LICENSE_HEADER = '''
/*
 * Copyright (c) 2019-2022 The Khronos Group Inc.
 * Copyright (c) 2019-2022 Valve Corporation
 * Copyright (c) 2019-2022 LunarG, Inc.
 * Copyright (c) 2023-2024 RasterGrid Kft.
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
'''

CUSTOM_FORMATTERS = r'''
template <typename T>
std::string to_hex_str(const T i) {
    std::stringstream stream;
    stream << "0x" << std::setfill('0') << std::setw(sizeof(T)) << std::hex << i;
    return stream.str();
}

template <typename T>
std::string to_hex_str(Printer &p, const T i) {
    if (p.Type() == OutputType::json)
        return std::to_string(i);
    else if (p.Type() == OutputType::vkconfig_output)
        return std::string("\"") + to_hex_str(i) + std::string("\"");
    else
        return to_hex_str(i);
}

'''


# used in the .cpp code
STRUCTURES_TO_GEN = ['VkExtent3D', 'VkExtent2D', 'VkPhysicalDeviceLimits', 'VkPhysicalDeviceFeatures', 'VkPhysicalDeviceSparseProperties',
                     'VkSurfaceCapabilitiesKHR', 'VkSurfaceFormatKHR', 'VkLayerProperties', 'VkPhysicalDeviceToolProperties', 'VkFormatProperties',
                     'VkSurfacePresentScalingCapabilitiesKHR', 'VkSurfacePresentModeCompatibilityKHR', 'VkPhysicalDeviceHostImageCopyProperties',
                     'VkVideoProfileInfoKHR', 'VkVideoCapabilitiesKHR', 'VkVideoFormatPropertiesKHR']
ENUMS_TO_GEN = ['VkResult', 'VkFormat', 'VkPresentModeKHR',
                'VkPhysicalDeviceType', 'VkImageTiling']
FLAGS_TO_GEN = ['VkSurfaceTransformFlagsKHR', 'VkCompositeAlphaFlagsKHR', 'VkSurfaceCounterFlagsEXT', 'VkQueueFlags',
                'VkDeviceGroupPresentModeFlagsKHR', 'VkFormatFeatureFlags', 'VkFormatFeatureFlags2', 'VkMemoryPropertyFlags', 'VkMemoryHeapFlags']
FLAG_STRINGS_TO_GEN = ['VkQueueFlags']

STRUCT_SHORT_VERSIONS_TO_GEN = ['VkExtent3D']

STRUCT_COMPARISONS_TO_GEN = ['VkSurfaceFormatKHR', 'VkSurfaceFormat2KHR', 'VkSurfaceCapabilitiesKHR',
                             'VkSurfaceCapabilities2KHR', 'VkSurfaceCapabilities2EXT']
# don't generate these structures
STRUCT_BLACKLIST = ['VkVideoProfileListInfoKHR', 'VkDrmFormatModifierPropertiesListEXT', 'VkDrmFormatModifierPropertiesEXT', 'VkDrmFormatModifierPropertiesList2EXT']
# These structures are only used in version 1.1, otherwise they are included in the promoted structs
STRUCT_1_1_LIST = ['VkPhysicalDeviceProtectedMemoryFeatures', 'VkPhysicalDeviceShaderDrawParametersFeatures', 'VkPhysicalDeviceSubgroupProperties', 'VkPhysicalDeviceProtectedMemoryProperties']

# generate these structures such that they only print when not in json mode (as json wants them separate)
PORTABILITY_STRUCTS = ['VkPhysicalDevicePortabilitySubsetFeaturesKHR', 'VkPhysicalDevicePortabilitySubsetPropertiesKHR']

# iostream or custom outputter handles these types
PREDEFINED_TYPES = ['char', 'VkBool32', 'uint32_t', 'uint8_t', 'int32_t',
                    'float', 'uint64_t', 'size_t', 'VkDeviceSize', 'int64_t']

NAMES_TO_IGNORE = ['sType', 'pNext']

EXTENSION_TYPE_INSTANCE = 'instance'
EXTENSION_TYPE_DEVICE = 'device'
EXTENSION_TYPE_BOTH = 'both'

# Types that need pNext Chains built. 'extends' is the xml tag used in the structextends member. 'type' can be device, instance, or both
EXTENSION_CATEGORIES = OrderedDict((
    ('phys_device_props2',
        {'extends': 'VkPhysicalDeviceProperties2',
         'type': EXTENSION_TYPE_BOTH,
         'print_iterator': True,
         'can_show_promoted_structs': True,
         'ignore_vendor_exclusion': False}),
    ('phys_device_mem_props2',
        {'extends': 'VkPhysicalDeviceMemoryProperties2',
         'type': EXTENSION_TYPE_DEVICE,
         'print_iterator': False,
         'can_show_promoted_structs': False,
         'ignore_vendor_exclusion': False}),
    ('phys_device_features2',
        {'extends': 'VkPhysicalDeviceFeatures2',
         'type': EXTENSION_TYPE_DEVICE,
         'print_iterator': True,
         'can_show_promoted_structs': True,
         'ignore_vendor_exclusion': False}),
    ('surface_capabilities2',
        {'extends': 'VkSurfaceCapabilities2KHR',
         'type': EXTENSION_TYPE_BOTH,
         'print_iterator': True,
         'can_show_promoted_structs': False,
         'ignore_vendor_exclusion': False,
         'exclude': ['VkSurfacePresentScalingCapabilitiesKHR', 'VkSurfacePresentModeCompatibilityKHR']}),
    ('format_properties2',
        {'extends': 'VkFormatProperties2',
         'type': EXTENSION_TYPE_DEVICE,
         'print_iterator': True,
         'can_show_promoted_structs': False,
         'ignore_vendor_exclusion': False}),
    ('queue_properties2',
        {'extends': 'VkQueueFamilyProperties2',
         'type': EXTENSION_TYPE_DEVICE,
         'print_iterator': True,
         'can_show_promoted_structs': False,
         'ignore_vendor_exclusion': False}),
    ('video_profile_info',
        {'extends': 'VkVideoProfileInfoKHR',
         'type': EXTENSION_TYPE_DEVICE,
         'print_iterator': True,
         'can_show_promoted_structs': False,
         'ignore_vendor_exclusion': True}),
    ('video_capabilities',
        {'extends': 'VkVideoCapabilitiesKHR',
         'type': EXTENSION_TYPE_DEVICE,
         'print_iterator': True,
         'can_show_promoted_structs': False,
         'ignore_vendor_exclusion': True,}),
    ('video_format_properties',
        {'extends': 'VkVideoFormatPropertiesKHR',
         'type': EXTENSION_TYPE_DEVICE,
         'print_iterator': True,
         'can_show_promoted_structs': False,
         'ignore_vendor_exclusion': True})
                                   ))
class VulkanInfoGenerator(BaseGenerator):
    def __init__(self):
        BaseGenerator.__init__(self)
        self.format_ranges = []

    def generate(self):
        self.findFormatRanges()

        # gather the types that are needed to generate
        types_to_gen = set()
        types_to_gen.update(ENUMS_TO_GEN)
        types_to_gen.update(FLAGS_TO_GEN)
        types_to_gen.update(STRUCTURES_TO_GEN)

        extension_types = {}
        for key, ext_info in EXTENSION_CATEGORIES.items():
            extension_types[key] = []

            for extended_struct in self.vk.structs[ext_info.get('extends')].extendedBy:
                if ext_info.get('exclude') is not None and extended_struct in ext_info.get('exclude'):
                    continue
                elif ext_info.get('ignore_vendor_exclusion'):
                    extension_types[key].append(extended_struct)
                    continue
                vendor_tags = []
                for extension in self.vk.structs[extended_struct].extensions:
                    vendor_tags.append(extension.split('_')[1])
                if len(vendor_tags) == 0 or 'KHR' in vendor_tags or 'EXT' in vendor_tags:
                    extension_types[key].append(extended_struct)
            extension_types[key] = sorted(extension_types[key])
            types_to_gen.update(extension_types[key])

        # find all the types that need
        types_to_gen.update(self.findAllTypesToGen(types_to_gen))

        types_to_gen = sorted(types_to_gen)

        comparison_types_to_gen = set()
        comparison_types_to_gen.update(STRUCT_COMPARISONS_TO_GEN)
        comparison_types_to_gen.update(self.findAllTypesToGen(comparison_types_to_gen))
        comparison_types_to_gen = sorted(comparison_types_to_gen)


        # print the types gathered
        out = []
        out.append(LICENSE_HEADER + '\n')
        out.append('#include "vulkaninfo.h"\n')
        out.append('#include "outputprinter.h"\n')
        out.append(CUSTOM_FORMATTERS)

        out.extend(self.genVideoEnums())

        for enum in (e for e in types_to_gen if e in self.vk.enums):
            out.extend(self.PrintEnumToString(self.vk.enums[enum]))
            out.extend(self.PrintEnum(self.vk.enums[enum]))

        # Need to go through all flags to find if they or their associated bitmask needs printing
        # This is because both bitmask and flag types are generated in PrintBitMask
        for name in (x for x in sorted(self.vk.flags.keys()) if x in types_to_gen or self.vk.flags[x].bitmaskName in types_to_gen):
            bitmask = self.vk.bitmasks[self.vk.flags[name].bitmaskName]

            out.extend(self.PrintBitMask(bitmask, bitmask.flagName))

            if bitmask.flagName in FLAG_STRINGS_TO_GEN:
                out.extend(self.PrintBitMaskToString(bitmask, bitmask.flagName))

        for s in (x for x in types_to_gen if x in self.vk.structs and x not in STRUCT_BLACKLIST):
            out.extend(self.PrintStructure(self.vk.structs[s]))

        for key, value in EXTENSION_CATEGORIES.items():
            out.extend(self.PrintChainStruct(key, extension_types[key], value))

        for s in (x for x in comparison_types_to_gen if x in self.vk.structs):
            out.extend(self.PrintStructComparisonForwardDecl(self.vk.structs[s]))
        for s in (x for x in comparison_types_to_gen if x in self.vk.structs):
            out.extend(self.PrintStructComparison(self.vk.structs[s]))
        for s in (x for x in types_to_gen if x in self.vk.structs and x in STRUCT_SHORT_VERSIONS_TO_GEN):
            out.extend(self.PrintStructShort(self.vk.structs[s]))

        out.append('auto format_ranges = std::array{\n')
        for f in self.format_ranges:
            out.append(f'    FormatRange{{{f.minimum_instance_version}, {self.vk.extensions[f.extensions[0]].nameString if len(f.extensions) > 0 else "nullptr"}, ')
            out.append(f'static_cast<VkFormat>({f.first_format}), static_cast<VkFormat>({f.last_format})}},\n')
        out.append('};\n')

        out.extend(self.genVideoProfileUtils())

        self.write(''.join(out))


    def genVideoEnums(self):
        out = []
        for enum in self.vk.videoStd.enums.values():
            out.append(f'std::string {enum.name}String({enum.name} value) {{\n')
            out.append('    switch (value) {\n')
            for field in enum.fields:
                # Ignore aliases
                if field.value is not None:
                    out.append(f'        case {field.name}: return "{field.name}";\n')
            out.append(f'        default: return std::string("UNKNOWN_{enum.name}_value") + std::to_string(value);\n')
            out.append('    }\n}\n')
            out.append(f'void Dump{enum.name}(Printer &p, std::string name, {enum.name} value) {{\n')
            out.append(f'    p.PrintKeyString(name, {enum.name}String(value));\n}}\n')
        return out


    # Utility to get the extension / version precondition of a list of type names
    def GetTypesPrecondition(self, typelist, indent):
        indent = ' ' * indent
        out = []
        extEnables = []
        for typename in typelist:
            extEnables.extend(self.vk.structs[typename].extensions)

        version = None
        for typename in typelist:
            for v in self.vk.versions.values():
                if typename in v.name:
                    if version is not None and (v.major > version.major or (v.major == version.major and v.minor > version.minor)):
                        version = v


        has_version = version is not None
        has_extNameStr = len(extEnables) > 0
        if has_version or has_extNameStr:
            out.append(f'{indent}if (')
            has_printed_condition = False
            if has_extNameStr:
                for ext in extEnables:
                    if has_printed_condition:
                        out.append(f'\n{indent} || ')
                    else:
                        has_printed_condition = True
                        if has_version:
                            out.append('(')
                    if self.vk.extensions[ext].device:
                        out.append(f'gpu.CheckPhysicalDeviceExtensionIncluded({self.vk.extensions[ext].nameString})')
                    else:
                        assert False, 'Should never get here'
            if has_version:
                if has_printed_condition:
                    out.append(f'\n{indent} || (gpu.api_version >= {version.nameApi})')
                else:
                    out.append(f'gpu.api_version >= {version.nameApi}')
            out.append(') {\n')
        else:
            out = f'{indent}{{\n'
        return out

    # Utility to construct a capability prerequisite condition evaluation expression
    def GetRequiredCapsCondition(self, structName, memberName, memberRef, value):
        condition = ''
        requiredCapStructDef = self.vk.structs[structName]
        for member in requiredCapStructDef.members:
            if member.name == memberName:
                if member.type in self.vk.flags:
                    # Check that the flags contain all the required values
                    def genExpressionFromValue(value):
                        return value if value == "" else f"({memberRef} & {value}) != 0"

                    for char in condition:
                        if char in ['(', ')', '+', ',']:
                            condition += genExpressionFromValue(value)
                            value = ""
                            if char == '+':
                                # '+' means AND
                                condition += ' && '
                            elif char == ',':
                                # ',' means OR
                                condition += ' || '
                            else:
                                condition += char
                        else:
                            value += char
                    condition += genExpressionFromValue(value)
                else:
                    condition = f'{memberRef} == {value}'
        if condition == '':
            return 'true'
        else:
            return f'({condition})'

    def genVideoProfileUtils(self):
        out = []

        # Generate video format properties comparator
        out.append('''
bool is_video_format_same(const VkVideoFormatPropertiesKHR &format_a, const VkVideoFormatPropertiesKHR &format_b) {
    auto a = reinterpret_cast<const VkBaseInStructure*>(&format_a);
    auto b = reinterpret_cast<const VkBaseInStructure*>(&format_b);
    bool same = true;
    while (same && a != nullptr && b != nullptr) {
        if (a->sType != b->sType) {
            // Structure type mismatch (extension structures are expected to be chained in the same order)
            same = false;
        } else {
            switch (a->sType) {''')

        if 'VkVideoFormatPropertiesKHR' in self.registry.validextensionstructs:
            for extstruct in ['VkVideoFormatPropertiesKHR'] + self.registry.validextensionstructs['VkVideoFormatPropertiesKHR']:
                extstructDef = self.vk.structs[extstruct]
                out.append(f'''
                case {extstructDef.sType}:
                    same = same && memcmp(reinterpret_cast<const char*>(a) + sizeof(VkBaseInStructure),
                                          reinterpret_cast<const char*>(b) + sizeof(VkBaseInStructure),
                                          sizeof({extstruct}) - sizeof(VkBaseInStructure)) == 0;
                    break;''')

        out.append('''
                default:
                    // Unexpected structure type
                    same = false;
                    break;
            }
        }
        a = a->pNext;
        b = b->pNext;
    }
    return same;
}
''')

        # Generate video profile info capture utilities
        out.append('''
std::vector<std::unique_ptr<AppVideoProfile>> enumerate_supported_video_profiles(AppGpu &gpu) {
    std::vector<std::unique_ptr<AppVideoProfile>> result{};

    struct ChromaSubsamplingInfo {
        VkVideoChromaSubsamplingFlagsKHR value;
        const char* name;
    };
    const std::vector<ChromaSubsamplingInfo> chroma_subsampling_list = {
        {VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR, "4:2:0"},
        {VK_VIDEO_CHROMA_SUBSAMPLING_422_BIT_KHR, "4:2:2"},
        {VK_VIDEO_CHROMA_SUBSAMPLING_444_BIT_KHR, "4:4:4"},
        {VK_VIDEO_CHROMA_SUBSAMPLING_MONOCHROME_BIT_KHR, "monochrome"}
    };

    struct BitDepthInfo {
        VkVideoComponentBitDepthFlagsKHR value;
        const char* name;
    };
    const std::vector<BitDepthInfo> bit_depth_list = {
        {VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR, "8"},
        {VK_VIDEO_COMPONENT_BIT_DEPTH_10_BIT_KHR, "10"},
        {VK_VIDEO_COMPONENT_BIT_DEPTH_12_BIT_KHR, "12"}
    };

    auto find_caps_struct = [](const VkVideoCapabilitiesKHR &capabilities, VkStructureType stype) -> const VkBaseInStructure* {
        auto p = reinterpret_cast<const VkBaseInStructure*>(&capabilities);
        while (p != nullptr) {
            if (p->sType == stype) {
                return p;
            }
            p = p->pNext;
        }
        return nullptr;
    };

    auto base_format = []
        (const ChromaSubsamplingInfo &chroma_subsampling, const BitDepthInfo &luma_bit_depth, const BitDepthInfo &chroma_bit_depth) {
            std::string result{};
            result += " (";
            result += chroma_subsampling.name;
            result += " ";
            result += luma_bit_depth.name;
            if (luma_bit_depth.value != chroma_bit_depth.value) {
                result += ":";
                result += chroma_bit_depth.name;
            }
            result += "-bit)";
            return result;
        };

    auto add_profile = [&](
        const std::string &name,
        const VkVideoProfileInfoKHR &profile_info,
        AppVideoProfile::CreateProfileInfoChainCb create_profile_info_chain,
        AppVideoProfile::CreateCapabilitiesChainCb create_capabilities_chain,
        const AppVideoProfile::CreateFormatPropertiesChainCbList &create_format_properties_chain_list,
        AppVideoProfile::InitProfileCb init_profile) {
            auto profile = std::make_unique<AppVideoProfile>(gpu, gpu.phys_device,
                                                             name, profile_info,
                                                             create_profile_info_chain,
                                                             create_capabilities_chain,
                                                             create_format_properties_chain_list,
                                                             init_profile);
            if (profile->supported) {
                result.push_back(std::move(profile));
            }
        };
''')

        # Generate individual video profiles from the video codec metadata
        for videoCodec in self.vk.videoCodecs.values():
            # Ignore video codec categories
            if videoCodec.value is None:
                continue

            out.append('\n')
            out.extend(self.GetTypesPrecondition(videoCodec.profiles.keys(), 4))
            out.append(f'{" " * 8}const std::string codec_name = "{videoCodec.name}";\n')

            out.append('''
        for (auto chroma_subsampling : chroma_subsampling_list) {
            for (auto luma_bit_depth : bit_depth_list) {
                for (auto chroma_bit_depth : bit_depth_list) {
                    if (chroma_subsampling.value == VK_VIDEO_CHROMA_SUBSAMPLING_MONOCHROME_BIT_KHR && luma_bit_depth.value != chroma_bit_depth.value) {
                        // Ignore the chroma bit depth dimension for monochrome
                        continue;
                    }

                    std::string profile_base_name = codec_name + base_format(chroma_subsampling, luma_bit_depth, chroma_bit_depth);
''')

            # Setup video profile info
            out.append(f'{" " * 20}VkVideoProfileInfoKHR profile_info{{\n')
            out.append(f'{" " * 20}    VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR,\n')
            out.append(f'{" " * 20}    nullptr,\n')
            out.append(f'{" " * 20}    {videoCodec.value},\n')
            out.append(f'{" " * 20}    chroma_subsampling.value,\n')
            out.append(f'{" " * 20}    luma_bit_depth.value,\n')
            out.append(f'{" " * 20}    chroma_bit_depth.value\n')
            out.append(f'{" " * 20}}};\n\n')

            # Setup video profile info chain creation callback
            out.append(f'{" " * 20}auto create_profile_info_chain = [&](const void **ppnext) -> std::unique_ptr<video_profile_info_chain> {{\n')
            out.append(f'{" " * 20}    auto profile_info_chain = std::make_unique<video_profile_info_chain>();\n')
            for profileStruct in videoCodec.profiles:
                structDef = self.vk.structs[profileStruct]
                out.append(self.AddGuardHeader(structDef))
                out.append(f'{" " * 24}if (profile_info_chain != nullptr) {{\n')
                out.append(f'{" " * 28}profile_info_chain->{profileStruct[2:]}.sType = {structDef.sType};\n')
                out.append(f'{" " * 28}profile_info_chain->{profileStruct[2:]}.pNext = nullptr;\n')
                out.append(f'{" " * 28}*ppnext = &profile_info_chain->{profileStruct[2:]};\n')
                out.append(f'{" " * 28}ppnext = &profile_info_chain->{profileStruct[2:]}.pNext;\n')
                out.append(f'{" " * 24}}}\n')
                if structDef.protect:
                    out.append(f'#else\n{" " * 20}profile_info_chain = nullptr;\n')
                out.append(self.AddGuardFooter(structDef))
            out.append(f'{" " * 20}    return profile_info_chain;\n')
            out.append(f'{" " * 20}}};\n\n')

            # Setup video capabilities chain creation callback
            out.append(f'{" " * 20}auto create_capabilities_chain = [&](void **ppnext) -> std::unique_ptr<video_capabilities_chain> {{\n')
            out.append(f'{" " * 20}    auto capabilities_chain = std::make_unique<video_capabilities_chain>();\n')
            for capabilities in videoCodec.capabilities:
                structDef = self.vk.structs[capabilities]
                out.append(self.AddGuardHeader(structDef))
                out.append(f'{" " * 24}if (capabilities_chain != nullptr) {{\n')
                out.extend(self.GetTypesPrecondition([capabilities], 28))
                out.append(f'{" " * 32}capabilities_chain->{capabilities[2:]}.sType = {structDef.sType};\n')
                out.append(f'{" " * 32}capabilities_chain->{capabilities[2:]}.pNext = nullptr;\n')
                out.append(f'{" " * 32}*ppnext = &capabilities_chain->{capabilities[2:]};\n')
                out.append(f'{" " * 32}ppnext = &capabilities_chain->{capabilities[2:]}.pNext;\n')
                out.append(f'{" " * 28}}}\n')
                out.append(f'{" " * 24}}}\n')
                out.append(self.AddGuardFooter(structDef))
            out.append(f'{" " * 20}    return capabilities_chain;\n')
            out.append(f'{" " * 20}}};\n\n')

            # Setup video format properties chain creation callbacks
            out.append(f'{" " * 20}const AppVideoProfile::CreateFormatPropertiesChainCbList create_format_properties_chain_list = {{\n')
            for format in videoCodec.formats.values():
                out.append(f'{" " * 24}AppVideoProfile::CreateFormatPropertiesChainCb {{\n')
                out.append(f'{" " * 28}"{format.name}",\n')
                out.append(f'{" " * 28}{format.usage.replace("+", " | ")},\n')

                # Callback to check required capabilities
                out.append(f'{" " * 28}[&](const VkVideoCapabilitiesKHR &capabilities) -> bool {{\n')
                out.append(f'{" " * 28}    bool supported = true;\n')
                for requiredCap in format.requiredCaps:
                    structDef = self.vk.structs[requiredCap.struct]
                    out.append(self.AddGuardHeader(structDef))
                    out.extend(self.GetTypesPrecondition([requiredCap.struct], 32))
                    out.append(f'{" " * 32}    auto caps = reinterpret_cast<const {requiredCap.struct}*>(find_caps_struct(capabilities, {structDef.sType}));\n')
                    out.append(f'{" " * 32}    if (caps != nullptr) {{\n')
                    out.append(f'{" " * 32}        supported = supported && {self.GetRequiredCapsCondition(requiredCap.struct, requiredCap.member, f"caps->{requiredCap.member}", requiredCap.value)};\n')
                    out.append(f'{" " * 32}    }} else {{\n')
                    out.append(f'{" " * 32}        supported = false;\n')
                    out.append(f'{" " * 32}    }}\n')
                    out.append(f'{" " * 32}}} else {{\n')
                    out.append(f'{" " * 32}    supported = false;\n')
                    out.append(f'{" " * 32}}}\n')
                    if structDef.protect:
                        out.append(f'#else\n{" " * 32}supported = false;\n')
                    out.append(self.AddGuardFooter(structDef))
                out.append(f'{" " * 28}    return supported;\n')
                out.append(f'{" " * 28}}},\n')

                # Callback to create video format properties chain
                out.append(f'{" " * 28}[&](void **ppnext) -> std::unique_ptr<video_format_properties_chain> {{\n')
                out.append(f'{" " * 28}    auto format_properties_chain = std::make_unique<video_format_properties_chain>();\n')
                for formatProps in format.properties:
                    structDef = self.vk.structs[formatProps]
                    out.append(self.AddGuardHeader(structDef))
                    out.append(f'{" " * 32}if (format_properties_chain != nullptr) {{\n')
                    out.extend(self.GetTypesPrecondition([formatProps], 36))
                    out.append(f'{" " * 40}format_properties_chain->{formatProps[2:]}.sType = {structDef.sType};\n')
                    out.append(f'{" " * 40}format_properties_chain->{formatProps[2:]}.pNext = nullptr;\n')
                    out.append(f'{" " * 40}*ppnext = &format_properties_chain->{formatProps[2:]};\n')
                    out.append(f'{" " * 40}ppnext = &format_properties_chain->{formatProps[2:]}.pNext;\n')
                    out.append(f'{" " * 36}}}\n')
                    out.append(f'{" " * 32}}}\n')
                    out.append(self.AddGuardFooter(structDef))
                out.append(f'{" " * 28}    return format_properties_chain;\n')
                out.append(f'{" " * 28}}},\n')

                out.append(f'{" " * 24}}},\n')
            out.append(f'{" " * 20}}};\n\n')

            # Permute profiles for each profile struct member value
            profiles = {'': []}
            for profileStruct in videoCodec.profiles.values():
                for profileStructMember in profileStruct.members.values():
                    newProfiles = {}
                    for profileStructMemberValue, profileStructMemberName in profileStructMember.values.items():
                        for profileName, profile in profiles.items():
                            # Only add video profile name suffix to the full descriptive name if not empty to avoid excess whitespace
                            newProfileName = profileName if profileStructMemberName == '' else f'{profileName} {profileStructMemberName}'
                            newProfiles[newProfileName] = profile + [{
                                "struct": profileStruct.name,
                                "member": profileStructMember.name,
                                "value": profileStructMemberValue
                            }]
                    profiles = newProfiles

            for profileName, profile in profiles.items():
                out.append(f'{" " * 20}add_profile(profile_base_name + "{profileName}", profile_info,\n')
                out.append(f'{" " * 20}            create_profile_info_chain, create_capabilities_chain,\n')
                out.append(f'{" " * 20}            create_format_properties_chain_list,\n')
                out.append(f'{" " * 20}            [](AppVideoProfile& profile) {{\n')
                for profileStruct in videoCodec.profiles:
                    structDef = self.vk.structs[profileStruct]
                    out.append(self.AddGuardHeader(structDef))
                    for elem in profile:
                        if elem['struct'] == profileStruct:
                            out.append(f'{" " * 24}profile.profile_info_chain->{elem["struct"][2:]}.{elem["member"]} = {elem["value"]};\n')
                    out.append(self.AddGuardFooter(structDef))
                out.append(f'{" " * 20}}});\n')

            out.append(f'{" " * 16}}}\n')
            out.append(f'{" " * 12}}}\n')
            out.append(f'{" " * 8}}}\n')
            out.append(f'{" " * 4}}}\n')

        out.append('    return result;\n')
        out.append('}\n\n')

        return out


    # finds all the ranges of formats from core (1.0), core versions (1.1+), and extensions
    def findFormatRanges(self):
        min_val = 2**32
        prev_field = None
        max_val = 0
        for f in self.vk.enums['VkFormat'].fields:
            if f.value is None:
                continue
            if prev_field is not None and f.value != prev_field.value + 1:
                for ext in prev_field.extensions:
                    if self.vk.extensions[ext].promotedTo is not None:
                        self.format_ranges.append(VulkanFormatRange(self.vk.extensions[ext].promotedTo.replace("VK_", "VK_API_"), [], min_val, max_val))
                        break
                # only bother with the first extension
                self.format_ranges.append(VulkanFormatRange(0, prev_field.extensions, min_val, max_val))
                min_val = 2**32
                max_val = 0
            min_val = min(min_val, f.value)
            max_val = max(max_val, f.value)

            prev_field = f

        for ext in prev_field.extensions:
            if self.vk.extensions[ext].promotedTo is not None:
                self.format_ranges.append(VulkanFormatRange(self.vk.extensions[ext].promotedTo.replace("VK_", "VK_API_"), [], min_val, max_val))
                break

        self.format_ranges.append(VulkanFormatRange(0, prev_field.extensions, min_val, max_val))

    def findAllTypesToGen(self, initial_type_set):
        out_set = set()
        current_set = initial_type_set
        while len(current_set) > 0:
            out_set.update(current_set)
            next_set = set()

            for current_item in current_set:
                if current_item in self.vk.structs:
                    for member in self.vk.structs[current_item].members:
                        if member.type not in out_set and member.name not in NAMES_TO_IGNORE:
                            next_set.add(member.type)

            current_set = next_set
        return out_set

    def AddGuardHeader(self,obj):
        if obj is not None and obj.protect is not None:
            return f'#ifdef {obj.protect}\n'
        else:
            return ''


    def AddGuardFooter(self,obj):
        if obj is not None and obj.protect is not None:
            return f'#endif  // {obj.protect}\n'
        else:
            return ''

    def PrintEnumToString(self,enum):
        out = []
        out.append(self.AddGuardHeader(enum))
        out.append(f'std::string {enum.name}String({enum.name} value) {{\n')
        out.append('    switch (value) {\n')
        for v in enum.fields:
            out.append(f'        case ({v.name}): return "{v.name[3:]}";\n')
        out.append(f'        default: return std::string("UNKNOWN_{enum.name}_value") + std::to_string(value);\n')
        out.append('    }\n}\n')
        out.append(self.AddGuardFooter(enum))
        return out


    def PrintEnum(self,enum):
        out = []
        out.append(self.AddGuardHeader(enum))
        out.append(f'''void Dump{enum.name}(Printer &p, std::string name, {enum.name} value) {{
    if (p.Type() == OutputType::json)
        p.PrintKeyString(name, std::string("VK_") + {enum.name}String(value));
    else
        p.PrintKeyString(name, {enum.name}String(value));
}}
''')
        out.append(self.AddGuardFooter(enum))
        return out


    def PrintGetFlagStrings(self,name, bitmask):
        out = []
        out.append(f'std::vector<const char *> {name}GetStrings({name} value) {{\n')
        out.append('    std::vector<const char *> strings;\n')
        # If a bitmask contains a field whose value is zero, we want to support printing the correct bitflag
        # Otherwise, use "None" for when there are not bits set in the bitmask
        if bitmask.flags[0].value != 0:
            out.append('    if (value == 0) { strings.push_back("None"); return strings; }\n')
        else:
            out.append(f'    if (value == 0) {{ strings.push_back("{bitmask.flags[0].name[3:]}"); return strings; }}\n')
        for v in bitmask.flags:
            # only check single-bit flags
            if v.value != 0 and (v.value & (v.value - 1)) == 0:
                out.append(f'    if ({v.name} & value) strings.push_back("{v.name[3:]}");\n')
        out.append('    return strings;\n}\n')
        return out


    def PrintFlags(self, bitmask, name):
        out = []
        out.append(f'void Dump{name}(Printer &p, std::string name, {name} value) {{\n')
        out.append(f'''    if (static_cast<{bitmask.name}>(value) == 0) {{
        ArrayWrapper arr(p, name, 0);
        if (p.Type() != OutputType::json && p.Type() != OutputType::vkconfig_output)
            p.SetAsType().PrintString("None");
        return;
    }}
    auto strings = {bitmask.name}GetStrings(static_cast<{bitmask.name}>(value));
    ArrayWrapper arr(p, name, strings.size());
    for(auto& str : strings){{
        if (p.Type() == OutputType::json)
            p.SetAsType().PrintString(std::string("VK_") + str);
        else
            p.SetAsType().PrintString(str);
    }}
}}
''')
        return out


    def PrintFlagBits(self, bitmask):
        return [f'''void Dump{bitmask.name}(Printer &p, std::string name, {bitmask.name} value) {{
    auto strings = {bitmask.name}GetStrings(value);
    if (strings.size() > 0) {{
        if (p.Type() == OutputType::json)
            p.PrintKeyString(name, std::string("VK_") + strings.at(0));
        else
            p.PrintKeyString(name, strings.at(0));
    }}
}}
''']


    def PrintBitMask(self,bitmask, name):
        out = []
        out.extend(self.PrintGetFlagStrings(bitmask.name, bitmask))
        out.append(self.AddGuardHeader(bitmask))
        out.extend(self.PrintFlags(bitmask, name))
        out.extend(self.PrintFlagBits(bitmask))
        out.append(self.AddGuardFooter(bitmask))
        out.append('\n')
        return out


    def PrintBitMaskToString(self, bitmask, name):
        out = []
        out.append(self.AddGuardHeader(bitmask))
        out.append(f'std::string {name}String({name} value) {{\n')
        out.append('    std::string out;\n')
        out.append('    bool is_first = true;\n')
        for v in bitmask.flags:
            out.append(f'    if ({v.name} & value) {{\n')
            out.append('        if (is_first) { is_first = false; } else { out += " | "; }\n')
            out.append(f'        out += "{str(v.name)[3:]}";\n')
            out.append('    }\n')
        out.append('    return out;\n')
        out.append('}\n')
        out.append(self.AddGuardFooter(bitmask))
        return out


    def PrintStructure(self,struct):
        if len(struct.members) == 0:
            return []
        out = []
        out.append(self.AddGuardHeader(struct))
        max_key_len = 0
        for v in struct.members:
            if (v.type in PREDEFINED_TYPES or v.type in STRUCT_BLACKLIST) and (v.length is None or v.type in ['char'] or v.fixedSizeArray[0] in ['VK_UUID_SIZE', 'VK_LUID_SIZE']):
                max_key_len = max(max_key_len, len(v.name))

        out.append(f'void Dump{struct.name}(Printer &p, std::string name, const {struct.name} &obj) {{\n')
        if struct.name == 'VkPhysicalDeviceLimits':
            out.append('    if (p.Type() == OutputType::json)\n')
            out.append('        p.ObjectStart("limits");\n')
            out.append('    else\n')
            out.append('        p.SetSubHeader().ObjectStart(name);\n')
        elif struct.name == 'VkPhysicalDeviceSparseProperties':
            out.append('    if (p.Type() == OutputType::json)\n')
            out.append('        p.ObjectStart("sparseProperties");\n')
            out.append('    else\n')
            out.append('        p.SetSubHeader().ObjectStart(name);\n')
        else:
            out.append('    ObjectWrapper object{p, name};\n')
        if max_key_len > 0:
            out.append(f'    p.SetMinKeyWidth({max_key_len});\n')
        for v in struct.members:
            # arrays
            if v.length is not None:
                # strings
                if v.type == 'char':
                    out.append(f'    p.PrintKeyString("{v.name}", obj.{v.name});\n')
                # uuid's
                elif v.type == 'uint8_t' and (v.fixedSizeArray[0] == 'VK_LUID_SIZE' or v.fixedSizeArray[0] == 'VK_UUID_SIZE'):  # VK_UUID_SIZE
                    if v.fixedSizeArray[0] == 'VK_LUID_SIZE':
                        out.append('    if (obj.deviceLUIDValid) { // special case\n')
                    out.append(f'    p.PrintKeyValue("{v.name}", obj.{v.name});\n')
                    if v.fixedSizeArray[0] == 'VK_LUID_SIZE':
                        out.append('    }\n')
                elif struct.name == 'VkQueueFamilyGlobalPriorityProperties' and v.name == 'priorities':
                    out.append(f'    ArrayWrapper arr(p,"{v.name}", obj.priorityCount);\n')
                    out.append('    for (uint32_t i = 0; i < obj.priorityCount; i++) {\n')
                    out.append('       if (p.Type() == OutputType::json)\n')
                    out.append('           p.PrintString(std::string("VK_") + VkQueueGlobalPriorityString(obj.priorities[i]));\n')
                    out.append('       else\n')
                    out.append('           p.PrintString(VkQueueGlobalPriorityString(obj.priorities[i]));\n')
                    out.append('    }\n')
                elif len(v.fixedSizeArray) == 2:
                    out.append(f'    {{\n        ArrayWrapper arr(p,"{v.name}", ' + v.fixedSizeArray[0] + ');\n')
                    out.append(f'        for (uint32_t i = 0; i < {v.fixedSizeArray[0]}; i++) {{\n')
                    out.append(f'           for (uint32_t j = 0; j < {v.fixedSizeArray[1]}; j++) {{\n')
                    out.append(f'                p.PrintElement(obj.{v.name}[i][j]); }} }}\n')
                    out.append('    }\n')
                elif len(v.fixedSizeArray) == 1:
                    out.append(f'    {{\n        ArrayWrapper arr(p,"{v.name}", ' + v.fixedSizeArray[0] + ');\n')
                    out.append(f'        for (uint32_t i = 0; i < {v.fixedSizeArray[0]}; i++) {{ p.PrintElement(obj.{v.name}[i]); }}\n')
                    out.append('    }\n')
                else:  # dynamic array length based on other member
                    out.append(f'    if (obj.{v.length} == 0 || obj.{v.name} == nullptr) {{\n')
                    out.append(f'        p.PrintKeyString("{v.name}", "NULL");\n')
                    out.append('    } else {\n')
                    out.append(f'        ArrayWrapper arr(p,"{v.name}", obj.{v.length});\n')
                    out.append(f'        for (uint32_t i = 0; i < obj.{v.length}; i++) {{\n')
                    out.append(f'            Dump{v.type}(p, std::to_string(i), obj.{v.name}[i]);\n')
                    out.append('        }\n')
                    out.append('    }\n')
            elif v.type == 'VkBool32':
                out.append(f'    p.PrintKeyBool("{v.name}", static_cast<bool>(obj.{v.name}));\n')
            elif v.type == 'uint8_t':
                out.append(f'    p.PrintKeyValue("{v.name}", static_cast<uint32_t>(obj.{v.name}));\n')
            elif v.type == 'VkDeviceSize' or (v.type == 'uint32_t' and v.name in ['vendorID', 'deviceID']):
                out.append(f'    p.PrintKeyValue("{v.name}", to_hex_str(p, obj.{v.name}));\n')
            elif v.type in PREDEFINED_TYPES:
                out.append(f'    p.PrintKeyValue("{v.name}", obj.{v.name});\n')
            elif v.name not in NAMES_TO_IGNORE:
                # if it is an enum/flag/bitmask
                if v.type in ['VkFormatFeatureFlags', 'VkFormatFeatureFlags2']:
                    out.append('    p.SetOpenDetails();\n') # special case so that feature flags are open in html output
                out.append(f'    Dump{v.type}(p, "{v.name}", obj.{v.name});\n')

        if struct.name in ['VkPhysicalDeviceLimits', 'VkPhysicalDeviceSparseProperties']:
            out.append('    p.ObjectEnd();\n')
        out.append('}\n')

        out.append(self.AddGuardFooter(struct))
        return out


    def PrintStructShort(self,struct):
        out = []
        out.append(self.AddGuardHeader(struct))
        out.append(f'std::ostream &operator<<(std::ostream &o, {struct.name} &obj) {{\n')
        out.append('    return o << "(" << ')

        first = True
        for v in struct.members:
            if first:
                first = False
                out.append(f'obj.{v.name} << ')
            else:
                out.append(f'\',\' << obj.{v.name} << ')
        out.append('")";\n')
        out.append('}\n')
        out.append(self.AddGuardFooter(struct))
        return out

    def PrintChainStruct(self, listName, structs_to_print, chain_details):
        version_desc = ''
        if chain_details.get('type') in [EXTENSION_TYPE_DEVICE, EXTENSION_TYPE_BOTH]:
            version_desc = 'gpu.api_version'
        else:
            version_desc = 'inst.instance_version'

        out = []

        # use default constructor and delete copy & move operators
        out.append(f'''struct {listName}_chain {{
    {listName}_chain() = default;
    {listName}_chain(const {listName}_chain &) = delete;
    {listName}_chain& operator=(const {listName}_chain &) = delete;
    {listName}_chain({listName}_chain &&) = delete;
    {listName}_chain& operator=({listName}_chain &&) = delete;
''')

        out.append('    void* start_of_chain = nullptr;\n')
        for s in structs_to_print:
            if s in STRUCT_BLACKLIST:
                continue
            struct = self.vk.structs[s]
            out.append(self.AddGuardHeader(struct))
            if struct.sType is not None:
                out.append(f'    {struct.name} {struct.name[2:]}{{}};\n')
                # Specific versions of drivers have an incorrect definition of the size of these structs.
                # We need to artificially pad the structure it just so the driver doesn't write out of bounds and
                # into other structures that are adjacent. This bug comes from the in-development version of
                # the extension having a larger size than the final version, so older drivers try to write to
                # members which don't exist.
                if struct.name in ['VkPhysicalDeviceShaderIntegerDotProductFeatures', 'VkPhysicalDeviceHostImageCopyFeaturesEXT']:
                    out.append(f'    char {struct.name}_padding[64];\n')
                for member in struct.members:
                    if member.length is not None and len(member.fixedSizeArray) == 0:
                        out.append(f'    std::vector<{member.type}> {struct.name}_{member.name};\n')
            out.append(self.AddGuardFooter(struct))
        out.append('    void initialize_chain(')
        args = []
        if chain_details.get('type') in [EXTENSION_TYPE_INSTANCE, EXTENSION_TYPE_BOTH]:
            args.append('AppInstance &inst')
        if chain_details.get('type') in [EXTENSION_TYPE_DEVICE, EXTENSION_TYPE_BOTH]:
            args.append('AppGpu &gpu')
        if chain_details.get('can_show_promoted_structs'):
            args.append('bool show_promoted_structs')
        out.append(f'{", ".join(args)}) noexcept {{\n')
        for s in structs_to_print:
            if s in STRUCT_BLACKLIST:
                continue
            struct = self.vk.structs[s]

            out.append(self.AddGuardHeader(struct))
            out.append(f'        {struct.name[2:]}.sType = {struct.sType};\n')
            out.append(self.AddGuardFooter(struct))

        out.append('        std::vector<VkBaseOutStructure*> chain_members{};\n')
        for s in structs_to_print:
            if s in STRUCT_BLACKLIST:
                continue
            struct = self.vk.structs[s]
            out.append(self.AddGuardHeader(struct))

            has_version = struct.version is not None
            has_extNameStr = len(struct.extensions) > 0 or len(struct.aliases) > 0
            if has_version or has_extNameStr:
                out.append('        if (')
                has_printed_condition = False
                if has_extNameStr:
                    for ext in struct.extensions:
                        if has_printed_condition:
                            out.append('\n         || ')
                        else:
                            has_printed_condition = True
                            if has_version:
                                out.append('(')
                        if self.vk.extensions[ext].device:
                            out.append(f'gpu.CheckPhysicalDeviceExtensionIncluded({self.vk.extensions[ext].nameString})')
                        elif self.vk.extensions[ext].instance:
                            out.append(f'inst.CheckExtensionEnabled({self.vk.extensions[ext].nameString})')
                        else:
                            assert False, 'Should never get here'
                if has_version:
                    str_show_promoted_structs = '|| show_promoted_structs' if chain_details.get('can_show_promoted_structs') else ''
                    if struct.name in STRUCT_1_1_LIST:
                        out.append(f'{version_desc} == {struct.version.nameApi} {str_show_promoted_structs}')
                    elif has_printed_condition:
                        out.append(f')\n            && ({version_desc} < {struct.version.nameApi} {str_show_promoted_structs})')
                    else:
                        out.append(f'({version_desc} >= {struct.version.nameApi})')
                out.append(')\n            ')
            else:
                out.append('        ')
            out.append(f'chain_members.push_back(reinterpret_cast<VkBaseOutStructure*>(&{struct.name[2:]}));\n')
            out.append(self.AddGuardFooter(struct))
        chain_param_list = []
        chain_arg_list = []
        if chain_details.get('type') in [EXTENSION_TYPE_INSTANCE, EXTENSION_TYPE_BOTH]:
            chain_param_list.append('AppInstance &inst')
            chain_arg_list.append('inst')
        if chain_details.get('type') in [EXTENSION_TYPE_DEVICE, EXTENSION_TYPE_BOTH]:
            chain_param_list.append('AppGpu &gpu')
            chain_arg_list.append('gpu')
        if chain_details.get('can_show_promoted_structs'):
            chain_param_list.append('bool show_promoted_structs')
            chain_arg_list.append('show_promoted_structs')

        out.append(f'''
        if (!chain_members.empty()) {{
            for(size_t i = 0; i < chain_members.size() - 1; i++){{
                chain_members[i]->pNext = chain_members[i + 1];
            }}
            start_of_chain = chain_members[0];
        }}
    }}
}};
void setup_{listName}_chain({chain_details['extends']}& start, std::unique_ptr<{listName}_chain>& chain, {','.join(chain_param_list)}){{
    chain = std::unique_ptr<{listName}_chain>(new {listName}_chain());
    chain->initialize_chain({','.join(chain_arg_list)});
    start.pNext = chain->start_of_chain;
}};
''')
        if chain_details.get('print_iterator'):
            out.append('\n')
            out.append(f'void chain_iterator_{listName}(')
            args = ['Printer &p']
            if chain_details.get('type') in [EXTENSION_TYPE_INSTANCE, EXTENSION_TYPE_BOTH]:
                args.append('AppInstance &inst')
            if chain_details.get('type') in [EXTENSION_TYPE_DEVICE, EXTENSION_TYPE_BOTH]:
                args.append('AppGpu &gpu')
            if chain_details.get('can_show_promoted_structs'):
                args.append('bool show_promoted_structs')
            args.append('const void * place')
            out.append(f'{", ".join(args)}) {{\n')
            out.append('    while (place) {\n')
            out.append('        const VkBaseOutStructure *structure = (const VkBaseOutStructure *)place;\n')
            out.append('        p.SetSubHeader();\n')

            for s in structs_to_print:
                if s in STRUCT_BLACKLIST:
                    continue
                struct = self.vk.structs[s]

                out.append(self.AddGuardHeader(struct))
                out.append(f'        if (structure->sType == {struct.sType}')
                if struct.name in PORTABILITY_STRUCTS:
                    out.append(' && p.Type() != OutputType::json')
                out.append(') {\n')
                out.append(f'            const {struct.name}* props = (const {struct.name}*)structure;\n')
                out.extend(self.PrintStructNameDecisionLogic(struct, version_desc, chain_details.get('can_show_promoted_structs')))
                out.append('            p.AddNewline();\n')
                out.append('        }\n')
                out.append(self.AddGuardFooter(struct))
            out.append('        place = structure->pNext;\n')
            out.append('    }\n')
            out.append('}\n')

        out.append('\n')
        out.append(f'bool prepare_{listName}_twocall_chain_vectors(std::unique_ptr<{listName}_chain>& chain) {{\n')
        out.append('    (void)chain;\n')
        is_twocall = False
        for s in structs_to_print:
            if s in STRUCT_BLACKLIST:
                continue
            struct = self.vk.structs[s]
            has_length = False
            for member in struct.members:
                if member.length is not None and len(member.fixedSizeArray) == 0:
                    has_length = True
            if not has_length:
                continue
            out.append(self.AddGuardHeader(struct))
            for member in struct.members:
                if member.length is not None and len(member.fixedSizeArray) == 0:
                    out.append(f'    chain->{struct.name}_{member.name}.resize(chain->{struct.name[2:]}.{member.length});\n')
                    out.append(f'    chain->{struct.name[2:]}.{member.name} = chain->{struct.name}_{member.name}.data();\n')
            out.append(self.AddGuardFooter(struct))
            is_twocall = True
        out.append(f'    return {"true" if is_twocall else "false"};\n')
        out.append('}\n')

        return out

    def GetStructCheckStringForMatchingExtension(self, struct, structName):
        for ext_name in struct.extensions:
            ext = self.vk.extensions[ext_name]
            vendor = ext.name.split('_')[1]
            if structName.endswith(vendor):
                if ext.device:
                    return f'gpu.CheckPhysicalDeviceExtensionIncluded({ext.nameString})'
                elif ext.instance:
                    return f'inst.CheckExtensionEnabled({ext.nameString})'
        return None

    # Function is complex because it has to do the following:
    # Always print the struct with the most appropriate name given the gpu api version & enabled instance/device extensions
    # Print struct aliases when --show-promoted-structs is set
    # Not let alias printing duplicate the most appropriate name
    def PrintStructNameDecisionLogic(self, struct, version_desc, can_show_promoted_structs):
        out = []
        out.append(f'{" " * 12}const char* name = ')
        # Get a list of all the conditions to check and the type name to use
        check_list = []
        if struct.version is not None:
            check_list.append([f'{version_desc} >= {struct.version.nameApi}', struct.name])
        else:
            check_list.append([f'{self.GetStructCheckStringForMatchingExtension(struct, struct.name)}', struct.name])

        for alias in struct.aliases:
            ext_str = self.GetStructCheckStringForMatchingExtension(struct, alias)
            if ext_str is not None:
                check_list.append([f'{self.GetStructCheckStringForMatchingExtension(struct, alias)}', alias])
        end_parens = ''
        # Turn the conditions into a nested ternary condition -
        for check in check_list:
            if check == check_list[-1]:
                out.append( f'"{check[1]}"')
            else:
                out.append( f'{check[0]} ? "{check[1]}" : (')
                end_parens += ')'
        out.append(f'{end_parens};\n')
        out.append(f'{" " * 12}Dump{struct.name}(p, name, *props);\n')
        if not can_show_promoted_structs:
            return out
        for alias in struct.aliases:
            ext_str = self.GetStructCheckStringForMatchingExtension(struct, alias)
            if ext_str is not None:
                out.append(f'{" " * 12}if (show_promoted_structs && strcmp(name, "{alias}") != 0 && {ext_str}) {{\n')
                out.append(f'{" " * 16}p.AddNewline();\n')
                out.append(f'{" " * 16}p.SetSubHeader();\n')
                out.append(f'{" " * 16}Dump{struct.name}(p, "{alias}", *props);\n')
                out.append(f'{" " * 12}}}\n')
        return out

    def PrintStructComparisonForwardDecl(self,structure):
        out = []
        out.append(f'bool operator==(const {structure.name} & a, const {structure.name} b);\n')
        return out


    def PrintStructComparison(self,structure):
        out = []
        out.append(f'bool operator==(const {structure.name} & a, const {structure.name} b) {{\n')
        out.append('    return ')
        is_first = True
        for m in structure.members:
            if m.name not in NAMES_TO_IGNORE:
                if not is_first:
                    out.append('\n        && ')
                else:
                    is_first = False
                out.append(f'a.{m.name} == b.{m.name}')
        out.append(';\n')
        out.append('}\n')
        return out

class VulkanFormatRange:
    def __init__(self, min_inst_version, extensions, first, last):
        self.minimum_instance_version = min_inst_version
        self.extensions = extensions
        self.first_format = first
        self.last_format = last
