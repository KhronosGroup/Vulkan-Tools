#!/usr/bin/python3 -i
#
# Copyright (c) 2015-2021 The Khronos Group Inc.
# Copyright (c) 2015-2021 Valve Corporation
# Copyright (c) 2015-2021 LunarG, Inc.
# Copyright (c) 2015-2021 Google Inc.
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
# Author: Mark Lobodzinski <mark@lunarg.com>
# Author: Tobin Ehlis <tobine@google.com>
# Author: John Zulauf <jzulauf@lunarg.com>

from base_generator import BaseGenerator

# HelperFileOutputGenerator - subclass of OutputGenerator. Outputs Vulkan helper files
class HelperFileOutputGenerator(BaseGenerator):
    def __init__(self):
        BaseGenerator.__init__(self)


    def generate(self):
        out = []

        # File Comment
        out.append('// *** THIS FILE IS GENERATED - DO NOT EDIT ***\n')
        out.append('// See vulkan_tools_helper_file_generator.py for modifications\n')

        # Copyright Notice
        out.append('''

/***************************************************************************
 *
 * Copyright (c) 2015-2017 The Khronos Group Inc.
 * Copyright (c) 2015-2017 Valve Corporation
 * Copyright (c) 2015-2017 LunarG, Inc.
 * Copyright (c) 2015-2017 Google Inc.
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
 * Author: Mark Lobodzinski <mark@lunarg.com>
 * Author: Courtney Goeltzenleuchter <courtneygo@google.com>
 * Author: Tobin Ehlis <tobine@google.com>
 * Author: Chris Forbes <chrisforbes@google.com>
 * Author: John Zulauf<jzulauf@lunarg.com>
 *
 ****************************************************************************/
''')

        # Generate header
        out.append('''
#pragma once
#include <vulkan/vulkan.h>

// These empty generic templates are specialized for each type with sType
// members and for each sType -- providing a two way map between structure
// types and sTypes

template <VkStructureType id> struct LvlSTypeMap {};
template <typename T> struct LvlTypeMap {};

''')

        # Generate the specializations for each type and stype
        for struct in self.vk.structs.values():
            if struct.sType is None:
                continue

            if struct.protect is not None:
                out.append(f'#ifdef {struct.protect}\n')

            out.append(f'// Map type {struct.name} to id {struct.sType}\n')
            out.append(f'template <> struct LvlTypeMap<{struct.name}> {{\n')
            out.append(f'    static const VkStructureType kSType = {struct.sType};\n')
            out.append('};\n\n')


            out.append(f'template <> struct LvlSTypeMap<{struct.sType}> {{\n')
            out.append(f'    typedef {struct.name} Type;\n')
            out.append('};\n\n')

            if struct.protect is not None:
                out.append(f'#endif // {struct.protect}\n')

 # Define the utilities (here so any renaming stays consistent), if this grows large, refactor to a fixed .h file

        out.append('''// Header "base class" for pNext chain traversal
struct LvlGenericHeader {
   VkStructureType sType;
   const LvlGenericHeader *pNext;
};
struct LvlGenericModHeader {
   VkStructureType sType;
   LvlGenericModHeader *pNext;
};

// Find an entry of the given type in the pNext chain
template <typename T> const T *lvl_find_in_chain(const void *next) {
    const LvlGenericHeader *current = reinterpret_cast<const LvlGenericHeader *>(next);
    const T *found = nullptr;
    while (current) {
        if (LvlTypeMap<T>::kSType == current->sType) {
            found = reinterpret_cast<const T*>(current);
            current = nullptr;
        } else {
            current = current->pNext;
        }
    }
    return found;
}
// Find an entry of the given type in the pNext chain
template <typename T> T *lvl_find_mod_in_chain(void *next) {
    LvlGenericModHeader *current = reinterpret_cast<LvlGenericModHeader *>(next);
    T *found = nullptr;
    while (current) {
        if (LvlTypeMap<T>::kSType == current->sType) {
            found = reinterpret_cast<T*>(current);
            current = nullptr;
        } else {
            current = current->pNext;
        }
    }
    return found;
}

// Init the header of an sType struct with pNext
template <typename T> T lvl_init_struct(void *p_next) {
    T out = {};
    out.sType = LvlTypeMap<T>::kSType;
    out.pNext = p_next;
    return out;
}

// Init the header of an sType struct
template <typename T> T lvl_init_struct() {
    T out = {};
    out.sType = LvlTypeMap<T>::kSType;
    return out;
}
''')

        self.write(''.join(out))

