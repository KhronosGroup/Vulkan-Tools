#!/usr/bin/python3
#
# Copyright (c) 2019-2022 Valve Corporation
# Copyright (c) 2019-2022 LunarG, Inc.
# Copyright (c) 2019-2022 Google Inc.
# Copyright (c) 2023-2023 RasterGrid Kft.
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

import re
import sys
import operator
from collections import OrderedDict
import generator as gen
from common_codegen import GetFeatureProtect
from generator import GeneratorOptions, OutputGenerator

LICENSE_HEADER = '''
/*
 * Copyright (c) 2019-2022 The Khronos Group Inc.
 * Copyright (c) 2019-2022 Valve Corporation
 * Copyright (c) 2019-2022 LunarG, Inc.
 * Copyright (c) 2023-2023 RasterGrid Kft.
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
                     'VkSurfacePresentScalingCapabilitiesEXT', 'VkSurfacePresentModeCompatibilityEXT', 'VkPhysicalDeviceHostImageCopyPropertiesEXT']
ENUMS_TO_GEN = ['VkResult', 'VkFormat', 'VkPresentModeKHR',
                'VkPhysicalDeviceType', 'VkImageTiling']
FLAGS_TO_GEN = ['VkSurfaceTransformFlagsKHR', 'VkCompositeAlphaFlagsKHR', 'VkSurfaceCounterFlagsEXT', 'VkQueueFlags',
                'VkDeviceGroupPresentModeFlagsKHR', 'VkFormatFeatureFlags', 'VkFormatFeatureFlags2', 'VkMemoryPropertyFlags', 'VkMemoryHeapFlags']
FLAG_STRINGS_TO_GEN = ['VkQueueFlags']

STRUCT_SHORT_VERSIONS_TO_GEN = ['VkExtent3D']

STRUCT_COMPARISONS_TO_GEN = ['VkSurfaceFormatKHR', 'VkSurfaceFormat2KHR', 'VkSurfaceCapabilitiesKHR',
                             'VkSurfaceCapabilities2KHR', 'VkSurfaceCapabilities2EXT']
# don't generate these structures
STRUCT_BLACKLIST = ['VkVideoProfileListInfoKHR', 'VkVideoProfileInfoKHR', 'VkDrmFormatModifierPropertiesListEXT', 'VkDrmFormatModifierPropertiesEXT', 'VkDrmFormatModifierPropertiesList2EXT']
# These structures are only used in version 1.1, otherwise they are included in the promoted structs
STRUCT_1_1_LIST = ['VkPhysicalDeviceProtectedMemoryFeatures', 'VkPhysicalDeviceShaderDrawParametersFeatures', 'VkPhysicalDeviceSubgroupProperties', 'VkPhysicalDeviceProtectedMemoryProperties']

# generate these structures such that they only print when not in json mode (as json wants them separate)
PORTABILITY_STRUCTS = ['VkPhysicalDevicePortabilitySubsetFeaturesKHR', 'VkPhysicalDevicePortabilitySubsetPropertiesKHR']

# iostream or custom outputter handles these types
PREDEFINED_TYPES = ['char', 'VkBool32', 'uint32_t', 'uint8_t', 'int32_t',
                    'float', 'uint64_t', 'size_t', 'VkDeviceSize', 'int64_t']

NAMES_TO_IGNORE = ['sType', 'pNext', 'stdProfileIdc']

EXTENSION_TYPE_INSTANCE = 'instance'
EXTENSION_TYPE_DEVICE = 'device'
EXTENSION_TYPE_BOTH = 'both'

# Types that need pNext Chains built. 'extends' is the xml tag used in the structextends member. 'type' can be device, instance, or both
EXTENSION_CATEGORIES = OrderedDict((
    ('phys_device_props2',
        {'extends': 'VkPhysicalDeviceProperties2',
         'type': EXTENSION_TYPE_BOTH,
         'holder_type': 'VkPhysicalDeviceProperties2',
         'print_iterator': True,
        'exclude': ['VkPhysicalDeviceHostImageCopyPropertiesEXT']}),
    ('phys_device_mem_props2',
        {'extends': 'VkPhysicalDeviceMemoryProperties2',
        'type': EXTENSION_TYPE_DEVICE,
        'holder_type':'VkPhysicalDeviceMemoryProperties2',
        'print_iterator': False}),
    ('phys_device_features2',
        {'extends': 'VkPhysicalDeviceFeatures2,VkDeviceCreateInfo',
        'type': EXTENSION_TYPE_DEVICE,
        'holder_type': 'VkPhysicalDeviceFeatures2',
        'print_iterator': True}),
    ('surface_capabilities2',
        {'extends': 'VkSurfaceCapabilities2KHR',
        'type': EXTENSION_TYPE_BOTH,
        'holder_type': 'VkSurfaceCapabilities2KHR',
        'print_iterator': True,
        'exclude': ['VkSurfacePresentScalingCapabilitiesEXT', 'VkSurfacePresentModeCompatibilityEXT']}),
    ('format_properties2',
        {'extends': 'VkFormatProperties2',
        'type': EXTENSION_TYPE_DEVICE,
        'holder_type':'VkFormatProperties2',
        'print_iterator': True}),
    ('queue_properties2',
        {'extends': 'VkQueueFamilyProperties2',
        'type': EXTENSION_TYPE_DEVICE,
        'holder_type': 'VkQueueFamilyProperties2',
        'print_iterator': True})
                                   ))
class VulkanInfoGeneratorOptions(GeneratorOptions):
    def __init__(self,
                 conventions=None,
                 input=None,
                 filename=None,
                 directory='.',
                 genpath = None,
                 apiname=None,
                 profile=None,
                 versions='.*',
                 emitversions='.*',
                 defaultExtensions=None,
                 addExtensions=None,
                 removeExtensions=None,
                 emitExtensions=None,
                 sortProcedure=None,
                 prefixText='',
                 genFuncPointers=True,
                 protectFile=True,
                 protectFeature=True,
                 protectProto=None,
                 protectProtoStr=None,
                 apicall='',
                 apientry='',
                 apientryp='',
                 indentFuncProto=True,
                 indentFuncPointer=False,
                 alignFuncParam=0,
                 expandEnumerants=True,
                 ):
        GeneratorOptions.__init__(self,
                 conventions = conventions,
                 filename = filename,
                 directory = directory,
                 genpath = genpath,
                 apiname = apiname,
                 profile = profile,
                 versions = versions,
                 emitversions = emitversions,
                 defaultExtensions = defaultExtensions,
                 addExtensions = addExtensions,
                 removeExtensions = removeExtensions,
                 emitExtensions = emitExtensions,
                 sortProcedure = sortProcedure)
        self.input = input
        self.prefixText = prefixText
        self.genFuncPointers = genFuncPointers
        self.protectFile = protectFile
        self.protectFeature = protectFeature
        self.protectProto = protectProto
        self.protectProtoStr = protectProtoStr
        self.apicall = apicall
        self.apientry = apientry
        self.apientryp = apientryp
        self.indentFuncProto = indentFuncProto
        self.indentFuncPointer = indentFuncPointer
        self.alignFuncParam = alignFuncParam

# VulkanInfoGenerator - subclass of OutputGenerator.
# Generates a vulkan info output helper function


class VulkanInfoGenerator(OutputGenerator):

    def __init__(self,
                 errFile=sys.stderr,
                 warnFile=sys.stderr,
                 diagFile=sys.stdout):
        OutputGenerator.__init__(self, errFile, warnFile, diagFile)

        self.constants = OrderedDict()

        self.types_to_gen = set()

        self.extension_sets = OrderedDict()
        for ext_cat in EXTENSION_CATEGORIES.keys():
            self.extension_sets[ext_cat] = set()

        self.enums = []
        self.flags = []
        self.bitmasks = []
        self.format_ranges = []
        self.all_structures = []
        self.aliases = OrderedDict()

        self.extFuncs = OrderedDict()
        self.extTypes = OrderedDict()

        self.vendor_abbreviations = []
        self.vulkan_versions = []

    def beginFile(self, genOpts):
        gen.OutputGenerator.beginFile(self, genOpts)

        for node in self.registry.reg.findall('enums'):
            if node.get('name') == 'API Constants':
                for item in node.findall('enum'):
                    self.constants[item.get('name')] = item.get('value')

        for node in self.registry.reg.find('extensions').findall('extension'):
            ext = VulkanExtension(node)
            for item in ext.vktypes:
                if item not in self.extTypes:
                    self.extTypes[item] = []
                self.extTypes[item].append(ext)
            for item in ext.vkfuncs:
                self.extFuncs[item] = ext

        # need list of venders to blacklist vendor extensions
        for tag in self.registry.reg.find('tags'):
            if tag.get('name') not in ['KHR', 'EXT']:
                self.vendor_abbreviations.append('_' + tag.get('name'))

        for ver in self.registry.reg.findall('feature'):
            self.vulkan_versions.append(VulkanVersion(ver))

    def endFile(self):
        self.findFormatRanges()

        # gather the types that are needed to generate
        types_to_gen = set()
        for s in ENUMS_TO_GEN:
            types_to_gen.add(s)

        for f in FLAGS_TO_GEN:
            types_to_gen.add(f)

        types_to_gen.update(
            GatherTypesToGen(self.all_structures, STRUCTURES_TO_GEN))
        for key, info in EXTENSION_CATEGORIES.items():
            types_to_gen.update(
                GatherTypesToGen(self.all_structures, self.extension_sets[key], info.get('exclude')))
        types_to_gen = sorted(types_to_gen)

        names_of_structures_to_gen = set()
        for s in self.all_structures:
            if s.name in types_to_gen:
                names_of_structures_to_gen.add(s.name)
        names_of_structures_to_gen = sorted(names_of_structures_to_gen)

        structs_to_comp = set()
        for s in STRUCT_COMPARISONS_TO_GEN:
            structs_to_comp.add(s)
        structs_to_comp.update(
            GatherTypesToGen(self.all_structures, STRUCT_COMPARISONS_TO_GEN))

        for key, value in self.extension_sets.items():
            self.extension_sets[key] = sorted(value)

        self.enums = sorted(self.enums, key=operator.attrgetter('name'))
        self.flags = sorted(self.flags, key=operator.attrgetter('name'))
        self.bitmasks = sorted(self.bitmasks, key=operator.attrgetter('name'))
        self.all_structures = sorted(self.all_structures, key=operator.attrgetter('name'))

        # print the types gathered
        out = ''
        out += LICENSE_HEADER + '\n'
        out += '#include "vulkaninfo.h"\n'
        out += '#include "outputprinter.h"\n'
        out += CUSTOM_FORMATTERS

        for enum in (e for e in self.enums if e.name in types_to_gen):
            out += PrintEnumToString(enum, self)
            out += PrintEnum(enum, self)

        for flag in self.flags:
            if flag.name in types_to_gen:
                for bitmask in (b for b in self.bitmasks if b.name == flag.enum):
                    out += PrintBitMask(bitmask, flag.name, self)

            if flag.name in FLAG_STRINGS_TO_GEN:
                for bitmask in (b for b in self.bitmasks if b.name == flag.enum):
                    out += PrintBitMaskToString(bitmask, flag.name, self)

        for s in (x for x in self.all_structures if x.name in types_to_gen and x.name not in STRUCT_BLACKLIST):
            out += PrintStructure(s, types_to_gen)

        for key, value in EXTENSION_CATEGORIES.items():
            out += PrintChainStruct(key, self.extension_sets[key], self.all_structures, value, self.extTypes, self.aliases, self.vulkan_versions)

        for s in (x for x in self.all_structures if x.name in structs_to_comp):
            out += PrintStructComparisonForwardDecl(s)
        for s in (x for x in self.all_structures if x.name in structs_to_comp):
            out += PrintStructComparison(s)
        for s in (x for x in self.all_structures if x.name in STRUCT_SHORT_VERSIONS_TO_GEN):
            out += PrintStructShort(s)

        out += 'auto format_ranges = std::array{\n'
        for f in self.format_ranges:
            out += f'    FormatRange{{{f.minimum_instance_version}, {f.extension_name if f.extension_name is not None else "nullptr"}, '
            out += f'static_cast<VkFormat>({f.first_format}), static_cast<VkFormat>({f.last_format})}},\n'
        out += '};\n'


        gen.write(out, file=self.outFile)

        gen.OutputGenerator.endFile(self)

    def genCmd(self, cmd, name, alias):
        gen.OutputGenerator.genCmd(self, cmd, name, alias)

    # These are actually constants
    def genEnum(self, enuminfo, name, alias):
        gen.OutputGenerator.genEnum(self, enuminfo, name, alias)

    # These are actually enums
    def genGroup(self, groupinfo, groupName, alias):
        gen.OutputGenerator.genGroup(self, groupinfo, groupName, alias)

        if alias is not None:
            if alias in self.aliases.keys():
                self.aliases[alias].append(groupName)
            else:
                self.aliases[alias] = [groupName, ]
            return

        if groupinfo.elem.get('type') == 'bitmask':
            self.bitmasks.append(VulkanBitmask(groupinfo.elem))
        elif groupinfo.elem.get('type') == 'enum':
            self.enums.append(VulkanEnum(groupinfo.elem))

    def genType(self, typeinfo, name, alias):
        gen.OutputGenerator.genType(self, typeinfo, name, alias)

        if alias is not None:
            if alias in self.aliases.keys():
                self.aliases[alias].append(name)
            else:
                self.aliases[alias] = [name, ]
            return

        if typeinfo.elem.get('category') == 'bitmask':
            self.flags.append(VulkanFlags(typeinfo.elem))

        if typeinfo.elem.get('category') == 'struct':
            self.all_structures.append(VulkanStructure(
                name, typeinfo.elem, self.constants, self.extTypes))

        for vendor in self.vendor_abbreviations:
            for node in typeinfo.elem.findall('member'):
                if node.get('values') is not None:
                    if node.get('values').find(vendor) != -1:
                        return

        for key, value in EXTENSION_CATEGORIES.items():
            if str(typeinfo.elem.get('structextends')).find(value.get('extends')) != -1:
                if value.get('exclude') is None or name not in value.get('exclude'):
                    self.extension_sets[key].add(name)

    # finds all the ranges of formats from core (1.0), core versions (1.1+), and extensions
    def findFormatRanges(self):
        for enums in self.registry.reg.findall('enums'):
            if enums.get('name') == 'VkFormat':
                min_val = 2**32
                max_val = 0
                for enum in enums.findall('enum'):
                    if enum.get('value') is None:
                        continue
                    value = int(enum.get('value'))
                    min_val = min(min_val, value)
                    max_val = max(max_val, value)
                if min_val < 2**32 and max_val > 0:
                    self.format_ranges.append(VulkanFormatRange(0, None, min_val, max_val))

        for feature in self.registry.reg.findall('feature'):
            for require in feature.findall('require'):
                comment = require.get('comment')
                original_ext = None
                if comment is not None and comment.find('Promoted from') >= 0:
                    # may need tweaking in the future - some ext names aren't just the upper case version
                    original_ext = comment.split(' ')[2].upper() + '_EXTENSION_NAME'
                min_val = 2**32
                max_val = 0
                for enum in require.findall('enum'):
                    if enum.get('extends') == 'VkFormat':
                        value = CalcEnumValue(int(enum.get('extnumber')), int(enum.get('offset')))
                        min_val = min(min_val, value)
                        max_val = max(max_val, value)
                if min_val < 2**32 and max_val > 0:
                    self.format_ranges.append(VulkanFormatRange(feature.get('name').replace('_VERSION_', '_API_VERSION_'), None, min_val, max_val))
                    # If the formats came from an extension, add a format range for that extension so it'll be printed if the ext is supported but not the core version
                    if original_ext is not None:
                        self.format_ranges.append(VulkanFormatRange(0, original_ext, min_val, max_val))

        for extension in self.registry.reg.find('extensions').findall('extension'):
            if not self.genOpts.apiname in extension.get('supported').split(','):
                continue

            min_val = 2**32
            max_val = 0
            enum_name_string = ''
            for require in extension.findall('require'):
                for enum in require.findall('enum'):
                    if enum.get('value') is not None and enum.get('value').find(extension.get('name')):
                        enum_name_string = enum.get('name')
                    if enum.get('extends') == 'VkFormat':
                        if enum.get('offset') is None:
                            continue
                        value = CalcEnumValue(int(extension.get('number')), int(enum.get('offset')))
                        min_val = min(min_val, value)
                        max_val = max(max_val, value)
            if min_val < 2**32 and max_val > 0:
                self.format_ranges.append(VulkanFormatRange(0, enum_name_string, min_val, max_val))



def GatherTypesToGen(structure_list, structures, exclude = None):
    if exclude is None:
        exclude = []
    types = set()
    for s in structures:
        types.add(s)
    added_stuff = True  # repeat until no new types are added
    while added_stuff is True:
        added_stuff = False
        for s in structure_list:
            if s.name in types:
                for m in s.members:
                    if m.typeID not in PREDEFINED_TYPES and m.name not in NAMES_TO_IGNORE:
                        if m.typeID not in types:
                            if s.name not in exclude:
                                types.add(m.typeID)
                                added_stuff = True
    return types


def GetExtension(name, generator):
    if name in generator.extFuncs:
        return generator.extFuncs[name]
    elif name in generator.extTypes:
        return generator.extTypes[name][0]
    else:
        return None


def AddGuardHeader(obj):
    if obj is not None and obj.guard is not None:
        return f'#ifdef {obj.guard}\n'
    else:
        return ''


def AddGuardFooter(obj):
    if obj is not None and obj.guard is not None:
        return f'#endif  // {obj.guard}\n'
    else:
        return ''

def CalcEnumValue(num, offset):
    base = 1000000000
    block_size = 1000
    return base + (num - 1) * block_size + offset

def PrintEnumToString(enum, generator):
    out = ''
    out += AddGuardHeader(GetExtension(enum.name, generator))
    out += f'std::string {enum.name}String({enum.name} value) {{\n'
    out += '    switch (value) {\n'
    for v in enum.options:
        out += f'        case ({v.name}): return "{v.name[3:]}";\n'
    out += f'        default: return std::string("UNKNOWN_{enum.name}_value") + std::to_string(value);\n'
    out += '    }\n}\n'
    out += AddGuardFooter(GetExtension(enum.name, generator))
    return out


def PrintEnum(enum, generator):
    out = ''
    out += AddGuardHeader(GetExtension(enum.name, generator))
    out += f'''void Dump{enum.name}(Printer &p, std::string name, {enum.name} value) {{
    if (p.Type() == OutputType::json)
        p.PrintKeyString(name, std::string("VK_") + {enum.name}String(value));
    else
        p.PrintKeyString(name, {enum.name}String(value));
}}
'''
    out += AddGuardFooter(GetExtension(enum.name, generator))
    return out


def PrintGetFlagStrings(name, bitmask):
    out = ''
    out += f'std::vector<const char *> {name}GetStrings({name} value) {{\n'
    out += '    std::vector<const char *> strings;\n'
    # If a bitmask contains a field whose value is zero, we want to support printing the correct bitflag
    # Otherwise, use "None" for when there are not bits set in the bitmask
    if bitmask.options[0].value != 0:
        out += '    if (value == 0) { strings.push_back("None"); return strings; }\n'
    for v in bitmask.options:
        # only check single-bit flags
        if (v.value & (v.value - 1)) == 0:
            out += f'    if ({v.name} & value) strings.push_back("{v.name[3:]}");\n'
    out += '    return strings;\n}\n'
    return out


def PrintFlags(bitmask, name):
    out = f'void Dump{name}(Printer &p, std::string name, {name} value) {{\n'
    out += f'''    if (static_cast<{bitmask.name}>(value) == 0) {{
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
'''
    return out


def PrintFlagBits(bitmask):
    return f'''void Dump{bitmask.name}(Printer &p, std::string name, {bitmask.name} value) {{
    auto strings = {bitmask.name}GetStrings(value);
    if (strings.size() > 0) {{
        if (p.Type() == OutputType::json)
            p.PrintKeyString(name, std::string("VK_") + strings.at(0));
        else
            p.PrintKeyString(name, strings.at(0));
    }}
}}
'''



def PrintBitMask(bitmask, name, generator):
    out = PrintGetFlagStrings(bitmask.name, bitmask)
    out += AddGuardHeader(GetExtension(bitmask.name, generator))
    out += PrintFlags(bitmask, name)
    out += PrintFlagBits(bitmask)
    out += AddGuardFooter(GetExtension(bitmask.name, generator))
    out += '\n'
    return out


def PrintBitMaskToString(bitmask, name, generator):
    out = AddGuardHeader(GetExtension(bitmask.name, generator))
    out += f'std::string {name}String({name} value) {{\n'
    out += '    std::string out;\n'
    out += '    bool is_first = true;\n'
    for v in bitmask.options:
        out += f'    if ({v.name} & value) {{\n'
        out += '        if (is_first) { is_first = false; } else { out += " | "; }\n'
        out += f'        out += "{str(v.name)[3:]}";\n'
        out += '    }\n'
    out += '    return out;\n'
    out += '}\n'
    out += AddGuardFooter(GetExtension(bitmask.name, generator))
    return out


def PrintStructure(struct, types_to_gen):
    if len(struct.members) == 0:
        return ''
    out = ''
    out += AddGuardHeader(struct)
    max_key_len = 0
    for v in struct.members:
        if v.arrayLength is not None:
            if len(v.name) + len(v.arrayLength) + 2 > max_key_len:
                max_key_len = len(v.name) + len(v.arrayLength) + 2
        elif v.typeID in PREDEFINED_TYPES or v.typeID in STRUCT_BLACKLIST:
            if len(v.name) > max_key_len:
                max_key_len = len(v.name)

    out += f'void Dump{struct.name}(Printer &p, std::string name, const {struct.name} &obj) {{\n'
    if struct.name == 'VkPhysicalDeviceLimits':
        out += '    if (p.Type() == OutputType::json)\n'
        out += '        p.ObjectStart("limits");\n'
        out += '    else\n'
        out += '        p.SetSubHeader().ObjectStart(name);\n'
    elif struct.name == 'VkPhysicalDeviceSparseProperties':
        out += '    if (p.Type() == OutputType::json)\n'
        out += '        p.ObjectStart("sparseProperties");\n'
        out += '    else\n'
        out += '        p.SetSubHeader().ObjectStart(name);\n'
    else:
        out += '    ObjectWrapper object{p, name};\n'
    if max_key_len > 0:
        out += f'    p.SetMinKeyWidth({max_key_len});\n'
    for v in struct.members:
        # arrays
        if v.arrayLength is not None:
            # strings
            if v.typeID == 'char':
                out += f'    p.PrintKeyString("{v.name}", obj.{v.name});\n'
            # uuid's
            elif v.typeID == 'uint8_t' and (v.arrayLength == '8' or v.arrayLength == '16'):  # VK_UUID_SIZE
                if v.arrayLength == '8':
                    out += '    if (obj.deviceLUIDValid) { // special case\n'
                out += f'    p.PrintKeyValue("{v.name}", obj.{v.name});\n'
                if v.arrayLength == '8':
                    out += '    }\n'
            elif struct.name == 'VkQueueFamilyGlobalPriorityPropertiesKHR' and v.name == 'priorities':
                out += f'    ArrayWrapper arr(p,"{v.name}", obj.priorityCount);\n'
                out += '    for (uint32_t i = 0; i < obj.priorityCount; i++) {\n'
                out += '       if (p.Type() == OutputType::json)\n'
                out += '           p.PrintString(std::string("VK_") + VkQueueGlobalPriorityKHRString(obj.priorities[i]));\n'
                out += '       else\n'
                out += '           p.PrintString(VkQueueGlobalPriorityKHRString(obj.priorities[i]));\n'
                out += '    }\n'
            elif v.arrayLength.isdigit():
                out += f'    {{\n        ArrayWrapper arr(p,"{v.name}", ' + v.arrayLength + ');\n'
                out += f'        for (uint32_t i = 0; i < {v.arrayLength}; i++) {{ p.PrintElement(obj.{v.name}[i]); }}\n'
                out += '    }\n'
            else:  # dynamic array length based on other member
                out += '    {\n'
                out += f'        ArrayWrapper arr(p,"{v.name}", obj.' + v.arrayLength + ');\n'
                out += f'        for (uint32_t i = 0; i < obj.{v.arrayLength}; i++) {{\n'
                if v.typeID in types_to_gen:
                    out += f'            if (obj.{v.name} != nullptr) {{\n'
                    out += '                p.SetElementIndex(i);\n'
                    out += '                if (p.Type() == OutputType::json)\n'
                    out += f'                    p.PrintString(std::string("VK_") + {v.typeID}String(obj.{v.name}[i]));\n'
                    out += '                else\n'
                    out += f'                    p.PrintString({v.typeID}String(obj.{v.name}[i]));\n'
                    out += '            }\n'
                else:
                    out += f'            p.PrintElement(obj.{v.name}[i]);\n'
                out += '        }\n'
                out += '    }\n'
        elif v.typeID == 'VkBool32':
            out += f'    p.PrintKeyBool("{v.name}", static_cast<bool>(obj.{v.name}));\n'
        elif v.typeID == 'uint8_t':
            out += f'    p.PrintKeyValue("{v.name}", static_cast<uint32_t>(obj.{v.name}));\n'
        elif v.typeID == 'VkDeviceSize':
            out += f'    p.PrintKeyValue("{v.name}", to_hex_str(p, obj.{v.name}));\n'
        elif v.typeID in PREDEFINED_TYPES:
            out += f'    p.PrintKeyValue("{v.name}", obj.{v.name});\n'
        elif v.name not in NAMES_TO_IGNORE:
            # if it is an enum/flag/bitmask
            if v.typeID in ['VkFormatFeatureFlags', 'VkFormatFeatureFlags2']:
                out += '    p.SetOpenDetails();\n' # special case so that feature flags are open in html output
            out += f'    Dump{v.typeID}(p, "{v.name}", obj.{v.name});\n'

    if struct.name in ['VkPhysicalDeviceLimits', 'VkPhysicalDeviceSparseProperties']:
        out += '    p.ObjectEnd();\n'
    out += '}\n'

    out += AddGuardFooter(struct)
    return out


def PrintStructShort(struct):
    out = ''
    out += AddGuardHeader(struct)
    out += f'std::ostream &operator<<(std::ostream &o, {struct.name} &obj) {{\n'
    out += '    return o << "(" << '

    first = True
    for v in struct.members:
        if first:
            first = False
            out += f'obj.{v.name} << '
        else:
            out += f'\',\' << obj.{v.name} << '
    out += '")";\n'
    out += '}\n'
    out += AddGuardFooter(struct)
    return out

def PrintChainStruct(listName, structures, all_structures, chain_details, extTypes, aliases, vulkan_versions):
    sorted_structures = sorted(
        all_structures, key=operator.attrgetter('name'))

    version_desc = ''
    if chain_details.get('type') in [EXTENSION_TYPE_DEVICE, EXTENSION_TYPE_BOTH]:
        version_desc = 'gpu.api_version'
    else:
        version_desc = 'inst.instance_version'

    out = ''
    structs_to_print = []
    for s in sorted_structures:
        if s.name in structures:
            structs_to_print.append(s)
    # use default constructor and delete copy & move operators
    out += f'''struct {listName}_chain {{
    {listName}_chain() = default;
    {listName}_chain(const {listName}_chain &) = delete;
    {listName}_chain& operator=(const {listName}_chain &) = delete;
    {listName}_chain({listName}_chain &&) = delete;
    {listName}_chain& operator=({listName}_chain &&) = delete;
'''

    out += '    void* start_of_chain = nullptr;\n'
    for s in structs_to_print:
        if s.name in STRUCT_BLACKLIST:
            continue
        out += AddGuardHeader(s)
        if s.sTypeName is not None:
            out += f'    {s.name} {s.name[2:]}{{}};\n'
            # Specific versions of drivers have an incorrect definition of the size of these structs.
            # We need to artificially pad the structure it just so the driver doesn't write out of bounds and
            # into other structures that are adjacent. This bug comes from the in-development version of
            # the extension having a larger size than the final version, so older drivers try to write to
            # members which don't exist.
            if s.name in ['VkPhysicalDeviceShaderIntegerDotProductFeatures', 'VkPhysicalDeviceHostImageCopyFeaturesEXT']:
                out += f'    char {s.name}_padding[64];\n'
        out += AddGuardFooter(s)
    out += '    void initialize_chain('
    if chain_details.get('type') in [EXTENSION_TYPE_INSTANCE, EXTENSION_TYPE_BOTH]:
        out += 'AppInstance &inst, '
    if chain_details.get('type') in [EXTENSION_TYPE_DEVICE, EXTENSION_TYPE_BOTH]:
        out += 'AppGpu &gpu '
    out += ') noexcept {\n'
    for s in structs_to_print:
        if s.name in STRUCT_BLACKLIST:
            continue
        out += AddGuardHeader(s)
        out += f'        {s.name[2:]}.sType = {s.sTypeName};\n'
        out += AddGuardFooter(s)


    out += '        std::vector<VkBaseOutStructure*> chain_members{};\n'
    for s in structs_to_print:
        if s.name in STRUCT_BLACKLIST:
            continue
        out += AddGuardHeader(s)
        extEnables = {}
        for k, elem in extTypes.items():
            if k == s.name or (s.name in aliases.keys() and k in aliases[s.name]):
                for e in elem:
                    extEnables[e.extNameStr] = e.type

        version = None
        oldVersionName = None
        for v in vulkan_versions:
            if s.name in v.names:
                version = v
        if s.name in aliases.keys():
            for alias in aliases[s.name]:
                oldVersionName = alias

        has_version = version is not None
        has_extNameStr = len(extEnables) > 0 or s.name in aliases.keys()
        if has_version or has_extNameStr:
            out += '        if ('
            has_printed_condition = False
            if has_extNameStr:
                for key, value in extEnables.items():
                    if has_printed_condition:
                        out += '\n         || '
                    else:
                        has_printed_condition = True
                        if has_version:
                            out += '('
                    if value == EXTENSION_TYPE_DEVICE:
                        out += f'gpu.CheckPhysicalDeviceExtensionIncluded({key})'
                    elif value == EXTENSION_TYPE_INSTANCE:
                        out += f'inst.CheckExtensionEnabled({key})'
                    else:
                        assert False, 'Should never get here'
            if has_version:
                if s.name in STRUCT_1_1_LIST:
                    out += f'{version_desc} == {version.constant}'
                elif has_printed_condition:
                    out += f')\n            && {version_desc} < {version.constant}'
                else:
                    out += f'{version_desc} >= {version.constant}'
            out += ')\n            '
        else:
            out += '        '
        out += f'chain_members.push_back(reinterpret_cast<VkBaseOutStructure*>(&{s.name[2:]}));\n'
        out += AddGuardFooter(s)
    chain_param_list = []
    chain_arg_list = []
    if chain_details.get('type') in [EXTENSION_TYPE_INSTANCE, EXTENSION_TYPE_BOTH]:
        chain_param_list.append('AppInstance &inst')
        chain_arg_list.append('inst')
    if chain_details.get('type') in [EXTENSION_TYPE_DEVICE, EXTENSION_TYPE_BOTH]:
        chain_param_list.append('AppGpu &gpu')
        chain_arg_list.append('gpu')

    out += f'''
        if (!chain_members.empty()) {{
            for(size_t i = 0; i < chain_members.size() - 1; i++){{
                chain_members[i]->pNext = chain_members[i + 1];
            }}
            start_of_chain = chain_members[0];
        }}
    }}
}};
void setup_{listName}_chain({chain_details['holder_type']}& start, std::unique_ptr<{listName}_chain>& chain, {','.join(chain_param_list)}){{
    chain = std::unique_ptr<{listName}_chain>(new {listName}_chain());
    chain->initialize_chain({','.join(chain_arg_list)});
    start.pNext = chain->start_of_chain;
}};
'''
    if chain_details.get('print_iterator'):
        out += '\n'
        out += f'void chain_iterator_{listName}(Printer &p, '
        if chain_details.get('type') in [EXTENSION_TYPE_INSTANCE, EXTENSION_TYPE_BOTH]:
            out += 'AppInstance &inst, '
        if chain_details.get('type') in [EXTENSION_TYPE_DEVICE, EXTENSION_TYPE_BOTH]:
            out += 'AppGpu &gpu, '
        out += 'void * place) {\n'
        out += '    while (place) {\n'
        out += '        struct VkBaseOutStructure *structure = (struct VkBaseOutStructure *)place;\n'
        out += '        p.SetSubHeader();\n'

        for s in sorted_structures:
            if s.sTypeName is None or s.name in STRUCT_BLACKLIST:
                continue

            extEnables = {}
            for k, elem in extTypes.items():
                if k == s.name or (s.name in aliases.keys() and k in aliases[s.name]):
                    for e in elem:
                        extEnables[e.extNameStr] = e.type

            version = None
            oldVersionName = None
            for v in vulkan_versions:
                if s.name in v.names:
                    version = v
            if s.name in aliases.keys():
                for alias in aliases[s.name]:
                    oldVersionName = alias

            if s.name in structures:
                out += AddGuardHeader(s)
                out += f'        if (structure->sType == {s.sTypeName}'
                if s.name in PORTABILITY_STRUCTS:
                    out += ' && p.Type() != OutputType::json'
                has_version = version is not None
                has_extNameStr = len(extEnables) > 0 or s.name in aliases.keys()

                if has_version or has_extNameStr:
                    out += ' &&\n           ('
                    has_printed_condition = False
                    if has_extNameStr:
                        for key, value in extEnables.items():
                            if has_printed_condition:
                                out += ' || '
                            else:
                                has_printed_condition = True
                                if has_version:
                                    out += '('
                            if value == EXTENSION_TYPE_DEVICE:
                                out += f'gpu.CheckPhysicalDeviceExtensionIncluded({key})'
                            elif value == EXTENSION_TYPE_INSTANCE:
                                out += f'inst.CheckExtensionEnabled({key})'
                            else:
                                assert False, 'Should never get here'
                    if has_version:
                        if s.name in STRUCT_1_1_LIST:
                            out += f'{version_desc} == {version.constant}'
                        elif has_printed_condition:
                            out += f') &&\n            {version_desc} < {version.constant}'
                        else:
                            out += f'{version_desc} >= {version.constant}'
                    out += ')'
                out += ') {\n'
                out += f'            {s.name}* props = ({s.name}*)structure;\n'
                out += f'            Dump{s.name}(p, '
                if s.name in aliases.keys() and version is not None:
                    out += f'{version_desc} >= {version.constant} ?"{s.name}":"{oldVersionName}"'
                else:
                    out += f'"{s.name}"'
                out += ', *props);\n'
                out += '            p.AddNewline();\n'
                out += '        }\n'
                out += AddGuardFooter(s)
        out += '        place = structure->pNext;\n'
        out += '    }\n'
        out += '}\n'

    return out


def PrintStructComparisonForwardDecl(structure):
    out = ''
    out += f'bool operator==(const {structure.name} & a, const {structure.name} b);\n'
    return out


def PrintStructComparison(structure):
    out = ''
    out += f'bool operator==(const {structure.name} & a, const {structure.name} b) {{\n'
    out += '    return '
    is_first = True
    for m in structure.members:
        if m.name not in NAMES_TO_IGNORE:
            if not is_first:
                out += '\n        && '
            else:
                is_first = False
            out += f'a.{m.name} == b.{m.name}'
    out += ';\n'
    out += '}\n'
    return out


class VulkanEnum:
    class Option:

        def __init__(self, name, value, bitpos, comment):
            self.name = name
            self.comment = comment

            if bitpos is not None:
                value = 1 << int(bitpos)
            elif isinstance(value, str):
                if value.lower().startswith('0x'):
                    value = int(value, 16)
                else:
                    value = int(value)

            self.value = value

        def values(self):
            return {
                'optName': self.name,
                'optValue': self.value,
                'optComment': self.comment,
            }

    def __init__(self, rootNode):
        self.name = rootNode.get('name')
        self.type = rootNode.get('type')
        self.options = []

        for child in rootNode:
            childName = child.get('name')
            childValue = child.get('value')
            childBitpos = child.get('bitpos')
            childComment = child.get('comment')
            childExtends = child.get('extends')
            childOffset = child.get('offset')
            childExtNum = child.get('extnumber')
            support = child.get('supported')
            if support == 'disabled':
                continue

            if childName is None:
                continue
            if childValue is None and childBitpos is None and childOffset is None:
                continue

            if childExtends is not None and childExtNum is not None and childOffset is not None:
                childValue = CalcEnumValue(int(childExtNum), int(childOffset))
                if 'dir' in child.keys():
                    childValue = -childValue
            duplicate = False
            for o in self.options:
                if o.values()['optName'] == childName:
                    duplicate = True
            if duplicate:
                continue

            self.options.append(VulkanEnum.Option(
                childName, childValue, childBitpos, childComment))


class VulkanBitmask:

    def __init__(self, rootNode):
        self.name = rootNode.get('name')
        self.type = rootNode.get('type')

        # Read each value that the enum contains
        self.options = []
        for child in rootNode:
            childName = child.get('name')
            childValue = child.get('value')
            childBitpos = child.get('bitpos')
            childComment = child.get('comment')
            support = child.get('supported')
            if childName is None or (childValue is None and childBitpos is None):
                continue
            if support == 'disabled':
                continue

            duplicate = False
            for option in self.options:
                if option.name == childName:
                    duplicate = True
            if duplicate:
                continue

            self.options.append(VulkanEnum.Option(
                childName, childValue, childBitpos, childComment))


class VulkanFlags:

    def __init__(self, rootNode):
        self.name = rootNode.get('name')
        self.type = rootNode.get('type')
        self.enum = rootNode.get('requires')
        # 64 bit flags use bitvalues, not requires
        if self.enum is None:
            self.enum = rootNode.get('bitvalues')


class VulkanVariable:
    def __init__(self, rootNode, constants):
        self.name = rootNode.find('name').text
        # Typename, dereferenced and converted to a useable C++ token
        self.typeID = rootNode.find('type').text
        self.baseType = self.typeID
        self.childType = None
        self.arrayLength = None
        self.text = ''
        for node in rootNode.itertext():
            comment = rootNode.find('comment')
            if comment is not None and comment.text == node:
                continue
            self.text += node

        typeMatch = re.search('.+?(?=' + self.name + ')', self.text)
        self.type = typeMatch.string[typeMatch.start():typeMatch.end()]
        self.type = ' '.join(self.type.split())
        bracketMatch = re.search('(?<=\\[)[a-zA-Z0-9_]+(?=\\])', self.text)
        if bracketMatch is not None:
            matchText = bracketMatch.string[bracketMatch.start(
            ):bracketMatch.end()]
            self.childType = self.type
            self.type += '[' + matchText + ']'
            if matchText in constants:
                self.arrayLength = constants[matchText]
            else:
                self.arrayLength = matchText

        self.lengthMember = False
        lengthString = rootNode.get('len')
        lengths = []
        if lengthString is not None:
            lengths = re.split(',', lengthString)
            lengths = list(filter(('null-terminated').__ne__, lengths))
        if self.arrayLength is None and len(lengths) > 0:
            self.childType = '*'.join(self.type.split('*')[0:-1])
            self.arrayLength = lengths[0]
            self.lengthMember = True
        if self.arrayLength is not None and self.arrayLength.startswith('latexmath'):
            code = self.arrayLength[10:len(self.arrayLength)]
            code = re.sub('\\[', '', code)
            code = re.sub('\\]', '', code)
            code = re.sub('\\\\(lceil|rceil)', '', code)
            code = re.sub('{|}', '', code)
            code = re.sub('\\\\mathit', '', code)
            code = re.sub('\\\\over', '/', code)
            code = re.sub('\\\\textrm', '', code)
            self.arrayLength = code

        # Dereference if necessary and handle members of variables
        if self.arrayLength is not None:
            self.arrayLength = re.sub('::', '->', self.arrayLength)
            sections = self.arrayLength.split('->')
            if sections[-1][0] == 'p' and sections[0][1].isupper():
                self.arrayLength = '*' + self.arrayLength


class VulkanStructure:
    def __init__(self, name, rootNode, constants, extTypes):
        self.name = name
        self.members = []
        self.guard = None
        self.sTypeName = None
        self.extendsStruct = rootNode.get('structextends')

        for node in rootNode.findall('member'):
            if node.get('values') is not None:
                self.sTypeName = node.get('values')
            self.members.append(VulkanVariable(node, constants))

        for k, elem in extTypes.items():
            if k == self.name:
                for e in elem:
                    if e.guard is not None:
                        self.guard = e.guard


class VulkanExtension:
    def __init__(self, rootNode):
        self.name = rootNode.get('name')
        self.number = int(rootNode.get('number'))
        self.type = rootNode.get('type')
        self.dependency = rootNode.get('requires')
        self.guard = GetFeatureProtect(rootNode)
        self.supported = rootNode.get('supported')
        self.extNameStr = None
        self.vktypes = []
        self.vkfuncs = []
        self.constants = OrderedDict()
        self.enumValues = OrderedDict()
        self.node = rootNode

        for req in rootNode.findall('require'):
            for ty in req.findall('type'):
                self.vktypes.append(ty.get('name'))

            for func in req.findall('command'):
                self.vkfuncs.append(func.get('name'))

            for enum in req.findall('enum'):
                base = enum.get('extends')
                name = enum.get('name')
                value = enum.get('value')
                bitpos = enum.get('bitpos')
                offset = enum.get('offset')
                # gets the VK_XXX_EXTENSION_NAME string
                if value == f'"{self.name}"':
                    self.extNameStr = name

                if value is None and bitpos is not None:
                    value = 1 << int(bitpos)

                if offset is not None:
                    offset = int(offset)
                if base is not None and offset is not None:
                    enumValue = 1000000000 + 1000*(self.number - 1) + offset
                    if enum.get('dir') == '-':
                        enumValue = -enumValue
                    self.enumValues[base] = (name, enumValue)
                else:
                    self.constants[name] = value


class VulkanVersion:
    def __init__(self, rootNode):
        self.name = rootNode.get('name')
        self.constant = self.name.replace('_VERSION_', '_API_VERSION_')
        self.names = set()

        for req in rootNode.findall('require'):
            for ty in req.findall('type'):
                self.names.add(ty.get('name'))
            for func in req.findall('command'):
                self.names.add(func.get('name'))
            for enum in req.findall('enum'):
                self.names.add(enum.get('name'))
        self.names = sorted(self.names)

class VulkanFormatRange:
    def __init__(self, min_inst_version, ext_name, first, last):
        self.minimum_instance_version = min_inst_version
        self.extension_name = ext_name
        self.first_format = first
        self.last_format = last
