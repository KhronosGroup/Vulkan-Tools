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

#pragma once

#include <iomanip>
#include <iostream>
#include <ostream>
#include <stack>
#include <sstream>
#include <string>

#include <assert.h>

std::string insert_quotes(std::string s) { return "\"" + s + "\""; }

std::string to_string_16(uint8_t uid[16]) {
    std::stringstream stream;
    stream << std::setw(2) << std::hex;
    stream << (int)uid[0] << (int)uid[1] << (int)uid[2] << (int)uid[3] << "-";
    stream << (int)uid[4] << (int)uid[5] << "-";
    stream << (int)uid[6] << (int)uid[7] << "-";
    stream << (int)uid[8] << (int)uid[9] << "-";
    stream << (int)uid[10] << (int)uid[11] << (int)uid[12] << (int)uid[13] << (int)uid[14] << (int)uid[15];

    return stream.str();
}

std::string to_string_8(uint8_t uid[8]) {
    std::stringstream stream;
    stream << std::setw(2) << std::hex;
    stream << (int)uid[0] << (int)uid[1] << (int)uid[2] << (int)uid[3] << "-";
    stream << (int)uid[4] << (int)uid[5] << (int)uid[6] << (int)uid[7];

    return stream.str();
}

std::string VkVersionString(uint32_t version) {
    uint32_t major = version >> 22;
    uint32_t minor = (version >> 12) & 0x3ff;
    uint32_t patch = version & 0xfff;
    return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}

std::string VkVersionString(VulkanVersion v) {
    return std::to_string(v.major) + "." + std::to_string(v.minor) + "." + std::to_string(v.patch);
}

enum class OutputType { text, html, json };

class Printer {
   public:
    Printer(OutputType output_type, std::ostream &out, const uint32_t selected_gpu, const VulkanVersion vulkan_version)
        : output_type(output_type), out(out) {
        switch (output_type) {
            case (OutputType::text):
                out << "==========\n";
                out << "VULKANINFO\n";
                out << "==========\n\n";
                out << "Vulkan Instance Version: " << VkVersionString(vulkan_version) << "\n\n\n";

                break;
            case (OutputType::html):
                out << "<!doctype html>\n";
                out << "<html lang='en'>\n";
                out << "\t<head>\n";
                out << "\t\t<title>vulkaninfo</title>\n";
                out << "\t\t<style>\n";
                out << "\t\thtml {\n";
                out << "\t\t\tbackground-color: #0b1e48;\n";
                out << "\t\t\tbackground-image: url(\"https://vulkan.lunarg.com/img/bg-starfield.jpg\");\n";
                out << "\t\t\tbackground-position: center;\n";
                out << "\t\t\t-webkit-background-size: cover;\n";
                out << "\t\t\t-moz-background-size: cover;\n";
                out << "\t\t\t-o-background-size: cover;\n";
                out << "\t\t\tbackground-size: cover;\n";
                out << "\t\t\tbackground-attachment: fixed;\n";
                out << "\t\t\tbackground-repeat: no-repeat;\n";
                out << "\t\t\theight: 100%;\n";
                out << "\t\t}\n";
                out << "\t\t#header {\n";
                out << "\t\t\tz-index: -1;\n";
                out << "\t\t}\n";
                out << "\t\t#header>img {\n";
                out << "\t\t\tposition: absolute;\n";
                out << "\t\t\twidth: 160px;\n";
                out << "\t\t\tmargin-left: -280px;\n";
                out << "\t\t\ttop: -10px;\n";
                out << "\t\t\tleft: 50%;\n";
                out << "\t\t}\n";
                out << "\t\t#header>h1 {\n";
                out << "\t\t\tfont-family: Arial, \"Helvetica Neue\", Helvetica, sans-serif;\n";
                out << "\t\t\tfont-size: 44px;\n";
                out << "\t\t\tfont-weight: 200;\n";
                out << "\t\t\ttext-shadow: 4px 4px 5px #000;\n";
                out << "\t\t\tcolor: #eee;\n";
                out << "\t\t\tposition: absolute;\n";
                out << "\t\t\twidth: 400px;\n";
                out << "\t\t\tmargin-left: -80px;\n";
                out << "\t\t\ttop: 8px;\n";
                out << "\t\t\tleft: 50%;\n";
                out << "\t\t}\n";
                out << "\t\tbody {\n";
                out << "\t\t\tfont-family: Consolas, monaco, monospace;\n";
                out << "\t\t\tfont-size: 14px;\n";
                out << "\t\t\tline-height: 20px;\n";
                out << "\t\t\tcolor: #eee;\n";
                out << "\t\t\theight: 100%;\n";
                out << "\t\t\tmargin: 0;\n";
                out << "\t\t\toverflow: hidden;\n";
                out << "\t\t}\n";
                out << "\t\t#wrapper {\n";
                out << "\t\t\tbackground-color: rgba(0, 0, 0, 0.7);\n";
                out << "\t\t\tborder: 1px solid #446;\n";
                out << "\t\t\tbox-shadow: 0px 0px 10px #000;\n";
                out << "\t\t\tpadding: 8px 12px;\n\n";
                out << "\t\t\tdisplay: inline-block;\n";
                out << "\t\t\tposition: absolute;\n";
                out << "\t\t\ttop: 80px;\n";
                out << "\t\t\tbottom: 25px;\n";
                out << "\t\t\tleft: 50px;\n";
                out << "\t\t\tright: 50px;\n";
                out << "\t\t\toverflow: auto;\n";
                out << "\t\t}\n";
                out << "\t\tdetails>details {\n";
                out << "\t\t\tmargin-left: 22px;\n";
                out << "\t\t}\n";
                out << "\t\tdetails>summary:only-child::-webkit-details-marker {\n";
                out << "\t\t\tdisplay: none;\n";
                out << "\t\t}\n";
                out << "\t\t.var, .type, .val {\n";
                out << "\t\t\tdisplay: inline;\n";
                out << "\t\t}\n";
                out << "\t\t.var {\n";
                out << "\t\t}\n";
                out << "\t\t.type {\n";
                out << "\t\t\tcolor: #acf;\n";
                out << "\t\t\tmargin: 0 12px;\n";
                out << "\t\t}\n";
                out << "\t\t.val {\n";
                out << "\t\t\tcolor: #afa;\n";
                out << "\t\t\tbackground: #222;\n";
                out << "\t\t\ttext-align: right;\n";
                out << "\t\t}\n";
                out << "\t\t</style>\n";
                out << "\t</head>\n";
                out << "\t<body>\n";
                out << "\t\t<div id='header'>\n";
                out << "\t\t\t<h1>vulkaninfo</h1>\n";
                out << "\t\t</div>\n";
                out << "\t\t<div id='wrapper'>\n";

                out << "\t\t\t<details><summary>Vulkan Instance Version: <span class='val'>" << VkVersionString(vulkan_version)
                    << "</span></summary></details>\n\t\t\t<br />\n";
                indents += 3;
                break;
            case (OutputType::json):
                out << "{\n";
                out << "\t\"$schema\": \"https://schema.khronos.org/vulkan/devsim_1_0_0.json#\",\n";
                out << "\t\"comments\": {\n";
                out << "\t\t\"desc\": \"JSON configuration file describing GPU " << selected_gpu
                    << ". Generated using the vulkaninfo program.\",\n";
                out << "\t\t\"vulkanApiVersion\": \"" << VkVersionString(vulkan_version) << "\"\n";
                out << "\t}";
                indents++;
                is_first_item.push(false);
                break;
            default:
                break;
        }
    }
    ~Printer() {
        switch (output_type) {
            case (OutputType::text):

                break;
            case (OutputType::html):
                out << "\t\t</div>\n";
                out << "\t</body>\n";
                out << "</html>\n";
                indents -= 3;
                break;
            case (OutputType::json):
                out << "\n}\n";
                indents--;
                is_first_item.pop();
                assert(is_first_item.empty() && "mismatched number of ObjectStart/ObjectEnd or ArrayStart/ArrayEnd's");
                break;
        }
        assert(indents == 0 && "indents must be zero at program end");
    };

    OutputType Type() { return output_type; }

    // Custom Formatting
    // use by prepending with p.SetXXX().ObjectStart/ArrayStart

    Printer &SetHeader() {
        set_next_header = true;
        return *this;
    }

    Printer &SetSubHeader() {
        set_next_subheader = true;
        return *this;
    }

    Printer &SetOpenDetails() {
        set_details_open = true;
        return *this;
    }

    Printer &SetTitleAsType() {
        set_object_name_as_type = true;
        return *this;
    }

    Printer &SetAsType() {
        set_as_type = true;
        return *this;
    }

    Printer &SetElementIndex(int index) {
        assert(index >= 0 && "cannot set element index to a negative value");
        element_index = index;
        return *this;
    }

    void ObjectStart(std::string object_name) {
        switch (output_type) {
            case (OutputType::text): {
                out << std::string(indents, '\t') << object_name;
                if (element_index != -1) {
                    out << "[" << element_index << "]";
                }
                out << ":\n";
                int headersize = object_name.size() + 1;
                if (element_index != -1) {
                    headersize += 1 + std::to_string(element_index).size();
                    element_index = -1;
                }
                PrintHeaderUnderlines(headersize);
                break;
            }
            case (OutputType::html):
                out << std::string(indents, '\t');
                if (set_details_open) {
                    out << "<details open>";
                    set_details_open = false;
                } else {
                    out << "<details>";
                }
                out << "<summary>";
                if (set_object_name_as_type) {
                    out << "<span class='type'>" << object_name << "</span>";
                    set_object_name_as_type = false;
                } else {
                    out << object_name;
                }
                if (element_index != -1) {
                    out << "[<span class='val'>" << element_index << "</span>]";
                    element_index = -1;
                }
                out << "</summary>\n";
                break;
            case (OutputType::json):
                if (!is_first_item.top()) {
                    out << ",\n";
                } else {
                    is_first_item.top() = false;
                }
                out << std::string(indents, '\t');
                // Objects with no name are elements in an array of objects
                if (object_name == "" || element_index != -1) {
                    out << "{\n";
                    element_index = -1;
                } else {
                    out << "\"" << object_name << "\": {\n";
                }

                is_first_item.push(true);
                break;
            default:
                break;
        }
        indents++;
    }
    void ObjectEnd() {
        indents--;
        assert(indents >= 0 && "indents cannot go below zero");
        switch (output_type) {
            case (OutputType::text):

                break;
            case (OutputType::html):
                out << std::string(indents, '\t') << "</details>\n";
                break;
            case (OutputType::json):
                out << "\n" << std::string(indents, '\t') << "}";
                is_first_item.pop();
                break;
            default:
                break;
        }
    }
    void ArrayStart(std::string array_name, int element_count = 0) {
        switch (output_type) {
            case (OutputType::text):
                out << std::string(indents, '\t') << array_name << ": "
                    << "count = " << element_count << "\n";
                PrintHeaderUnderlines(array_name.size() + 1);
                break;
            case (OutputType::html):
                out << std::string(indents, '\t');
                if (set_details_open) {
                    out << "<details open>";
                    set_details_open = false;
                } else {
                    out << "<details>";
                }
                out << "<summary>" << array_name << ": count = <span class='val'>" << element_count << "</span></summary>\n";
                break;
            case (OutputType::json):
                if (!is_first_item.top()) {
                    out << ",\n";
                } else {
                    is_first_item.top() = false;
                }
                out << std::string(indents, '\t') << "\"" << array_name << "\": "
                    << "[\n";
                is_first_item.push(true);
                break;
            default:
                break;
        }
        indents++;
    }
    void ArrayEnd() {
        indents--;
        assert(indents >= 0 && "indents cannot go below zero");
        switch (output_type) {
            case (OutputType::text):

                break;
            case (OutputType::html):
                out << std::string(indents, '\t') << "</details>\n";
                break;
            case (OutputType::json):
                out << "\n" << std::string(indents, '\t') << "]";
                is_first_item.pop();
                break;
            default:
                break;
        }
    }

    // For printing key-value pairs.
    // min_key_width lines up the values listed
    // value_description is for reference information and is displayed inside parenthesis after the value
    template <typename T>
    void PrintKeyValue(std::string key, T value, size_t min_key_width = 0, std::string value_description = "") {
        switch (output_type) {
            case (OutputType::text):
                if (min_key_width > key.size()) {
                    out << std::string(indents, '\t') << key << std::string(min_key_width - key.size(), ' ');
                } else {
                    out << std::string(indents, '\t') << key;
                }
                out << " = " << value;
                if (value_description != "") {
                    out << " (" << value_description << ")";
                }
                out << "\n";
                break;
            case (OutputType::html):
                out << std::string(indents, '\t') << "<details><summary>" << key;
                if (min_key_width > key.size()) {
                    out << std::string(min_key_width - key.size(), ' ');
                }
                if (set_as_type) {
                    set_as_type = false;
                    out << " = <span class='type'>" << value << "</span>";
                } else {
                    out << " = <span class='val'>" << value << "</span>";
                }
                if (value_description != "") {
                    out << " (<span class='val'>" << value_description << "</span>)";
                }
                out << "</summary></details>\n";
                break;
            case (OutputType::json):
                if (!is_first_item.top()) {
                    out << ",\n";
                } else {
                    is_first_item.top() = false;
                }
                out << std::string(indents, '\t') << "\"" << key << "\": " << value;
            default:
                break;
        }
    }

    // For printing key - string pairs (necessary because of json)
    void PrintKeyString(std::string key, std::string value, size_t min_key_width = 0, std::string value_description = "") {
        switch (output_type) {
            case (OutputType::text):
            case (OutputType::html):
                PrintKeyValue(key, value, min_key_width, value_description);
                break;
            case (OutputType::json):
                PrintKeyValue(key, std::string("\"") + value + "\"", min_key_width, value_description);
                break;
            default:
                break;
        }
    }

    // For printing key - string pairs (necessary because of json)
    void PrintKeyBool(std::string key, bool value, size_t min_key_width = 0, std::string value_description = "") {
        switch (output_type) {
            case (OutputType::text):
            case (OutputType::html):
                PrintKeyValue(key, value ? "true" : "false", min_key_width, value_description);
                break;
            case (OutputType::json):
                PrintKeyValue(key, value, min_key_width, value_description);
                break;
            default:
                break;
        }
    }

    // print inside array
    template <typename T>
    void PrintElement(T element, std::string value_description = "") {
        switch (output_type) {
            case (OutputType::text):
                out << std::string(indents, '\t') << element;
                if (value_description != "") {
                    out << " (" << value_description << ")";
                }
                out << "\n";
                break;
            case (OutputType::html):
                out << std::string(indents, '\t') << "<details><summary>";
                if (set_as_type) {
                    set_as_type = false;
                    out << "<span class='type'>" << element << "</span>";
                } else {
                    out << "<span class='val'>" << element << "</span>";
                }
                if (value_description != "") {
                    out << " (<span class='val'>" << value_description << "</span>)";
                }
                out << "</summary></details>\n";
                break;
            case (OutputType::json):
                if (!is_first_item.top()) {
                    out << ",\n";
                } else {
                    is_first_item.top() = false;
                }
                out << std::string(indents, '\t') << element;
                break;
            default:
                break;
        }
    }
    void PrintExtension(std::string ext_name, int revision, size_t min_width = 0) {
        switch (output_type) {
            case (OutputType::text):
                out << std::string(indents, '\t') << ext_name << std::string(min_width - ext_name.size(), ' ')
                    << " : extension revision " << revision << "\n";
                break;
            case (OutputType::html):
                out << std::string(indents, '\t') << "<details><summary><span class='type'>" << ext_name << "</span>"
                    << std::string(min_width - ext_name.size(), ' ') << " : extension revision <span class='val'>" << revision
                    << "</span></summary></details>\n";
                break;
            case (OutputType::json):

                break;
            default:
                break;
        }
    }
    void AddNewline() {
        switch (output_type) {
            case (OutputType::text):
                out << "\n";
                break;
            default:
                break;
        }
    }
    void IndentIncrease() {
        switch (output_type) {
            case (OutputType::text):
                indents++;
                break;
            default:
                break;
        }
    }
    void IndentDecrease() {
        switch (output_type) {
            case (OutputType::text):
                indents--;
                assert(indents >= 0 && "indents cannot go below zero");
                break;
            default:
                break;
        }
    }

   protected:
    OutputType output_type;
    std::ostream &out;
    int indents = 0;

    // header, subheader
    bool set_next_header = false;
    bool set_next_subheader = false;

    // html coloring
    bool set_as_type = false;

    // open <details>
    bool set_details_open = false;

    // make object titles the color of types
    bool set_object_name_as_type = false;

    // objects which are in an array
    int element_index = -1;  // negative one is the sentinel value

    // json
    std::stack<bool> is_first_item;

    // utility
    void PrintHeaderUnderlines(int length) {
        assert(indents >= 0 && "indents must not be negative");
        assert(length >= 0 && "length must not be negative");
        if (set_next_header) {
            out << std::string(indents, '\t') << std::string(length, '=') << "\n";
            set_next_header = false;
        } else if (set_next_subheader) {
            out << std::string(indents, '\t') << std::string(length, '-') << "\n";
            set_next_subheader = false;
        }
    }
};
