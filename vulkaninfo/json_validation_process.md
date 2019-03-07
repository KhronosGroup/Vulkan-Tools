# Validating [vulkaninfo](https://github.com/KhronosGroup/Vulkan-Tools/tree/master/vulkaninfo) JSON output

The format of vulkaninfo's JSON output is designed to be used as input to the LunarG
[Device Simulation](https://github.com/LunarG/VulkanTools/blob/master/layersvt/device_simulation.md)
(DevSim) layer.
When changes are made to vulkaninfo's JSON output, the formatting should be
validated against DevSim's JSON schema, to ensure the text is correctly
formatted.

The DevSim JSON schema specifies exactly how its JSON input data must
be structured.
The schema may be found at
https://schema.khronos.org/vulkan/devsim_1_0_0.json

## Steps to validate JSON data against the DevSim schema

1. Generate the text to be tested using `vulkaninfo --json` and save to a file.
1. Download the [DevSim schema](https://schema.khronos.org/vulkan/devsim_1_0_0.json) to another file.
1. For each of the on-line JSON validator tools listed below:
   1. Paste the schema and and sample text into the `schema` and `data` fields.
   1. Depending on the tool, it may validate automatically, or require clicking a `validate` button.
   1. Ensure the tool reports no errors.

## List of recommended JSON validator tools

Each of these tools seem to have their own quirks and/or holes in coverage.
I recommend using all of them, ensuring they all report that the data successfully
validates against the schema.
* https://www.jsonschemavalidator.net/
* https://jsonschemalint.com/#/version/draft-04/markup/json
* https://json-schema-validator.herokuapp.com/index.jsp

If you have suggestions for better tools
(e.g.: an official reference validator,
an automatable tool to integrate with continuous integration)
please create an
[issue](https://github.com/KhronosGroup/Vulkan-Tools/issues)
to recommend it.
