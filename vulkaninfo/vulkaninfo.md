<!-- markdownlint-disable MD041 -->
<!-- Copyright 2015-2019 LunarG, Inc. -->

[![Khronos Vulkan][1]][2]

[1]: https://vulkan.lunarg.com/img/Vulkan_100px_Dec16.png "https://www.khronos.org/vulkan/"
[2]: https://www.khronos.org/vulkan/

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/

# Vulkan Information

Vulkan Info is a program provided in the SDK which outputs various types of Vulkan information such as:

- device properties of identified GPUs
- Vulkan extensions supported by each GPU
- recognized layers
- supported image formats and format properties.

## Running Vulkan Info

After downloading and installing the SDK and setting up the runtime environment (see the [Getting Started](./getting_started.md#user-content-download-the-sdk) documentation) you will be able to run the Vulkan Info program from a command prompt.

```
vulkaninfo
```

Executing `vulkaninfo` without specifying the type of output will default to human-readable output to the console.

Note: The Vulkan Info binary found in the Windows Vulkan SDK is named `vulkaninfoSDK.exe`. This is so it doesn't get confused with the Vulkan Info distributed by an IVH, as it is installed with the Vulkan Runtime Package. Since the SDK does not install Vulkan Info to the path, the Windows command line version used will be from the IHV distribution. To run the SDK version, either use the start menu shortcuts or navigate to the `VulkanSDK/Bin` directory and run `vulkaninfoSDK.exe` directly.

```
vulkaninfo --html
```

To organize output in a convenient HTML format use the `--html` option. Executing `vulkaninfo` with the `--html` option produces a file called `vulkaninfo.html` and can be found in your build directory.

```
vulkaninfo --json
```

 Use the `--json` option to produce [DevSim-schema](https://schema.khronos.org/vulkan/devsim_1_0_0.json)-compatible JSON output for your device. Additionally, JSON output can be specified with the `-j` option and for multi-GPU systems, a single GPU can be targeted using the `--json=`*`GPU-number`* option where the *`GPU-number`* indicates the GPU of interest (e.g., `--json=0`). To determine the GPU number corresponding to a particular GPU, execute `vulkaninfo` with the `--html` option (or none at all) first; doing so will summarize all GPUs in the system.
 The generated configuration information can be used as input for the [`VK_LAYER_LUNARG_device_simulation`](./device_simulation_layer.html) layer.

 Use the `--help` or `-h` option to produce a list of all available Vulkan Info options.

```
vulkaninfo - Summarize Vulkan information in relation to the current environment.

USAGE: ./vulkaninfo [options]

OPTIONS:
-h, --help          Print this help.
--html              Produce an html version of vulkaninfo output, saved as
                    "vulkaninfo.html" in the directory in which the command is
                    run.
-j, --json          Produce a json version of vulkaninfo output to standard
                    output.
--json=<gpu-number> For a multi-gpu system, a single gpu can be targetted by
                    specifying the gpu-number associated with the gpu of
                    interest. This number can be determined by running
                    vulkaninfo without any options specified.
--portability       Produce a json version of vulkaninfo to standard output of the first
                    gpu in the system conforming to the DevSim Portability Subset schema.
--portability=<N>   Produce the json output conforming to the DevSim Portability
                    Subset Schema for the GPU specified to standard output
                    where N is the GPU desired.
--show-formats      Display the format properties of each physical device.
                    Note: This option does not affect html or json output;
                    they will always print format properties.
-o <filename>, --output<filename>
                    Print output to a new file whose name is specified by filename.
                    File will be written to the current working directory.
--summary           Show a summary of the instance and GPU's on a system.
```

### Windows

Vulkan Info can also be found as a shortcut under the Start Menu.

- `Start Menu -> Vulkan SDK`*`version`*`-> vulkaninfo`

Note: In order to review and/or save the output produced when using Visual Studio execute `vulkaninfo` with the JSON option, you will have to redirect output to a file by modifying the command line arguments in the debug options.

Note: The Vulkan Info binary found in the Vulkan SDK is renamed to `vulkaninfoSDK.exe`. This was done to demark any Vulkan Info version installed by the Vulkan Runtime Package, which is from an IHV distribution. When using command line on Windows, the Vulkan info version used will be from the IHV distribution, not the SDK. To run the SDK version, either use the start menu shortcuts or navigate to the `VulkanSDK/Bin` directory and run `vulkaninfoSDK.exe` directly.

### iOS Devices

Vulkan Info is now avaialble for iOS devices, but must be built manually.

The Vulkan Info project contains a folder at [vulkaninfo/ios](https://github.com/KhronosGroup/Vulkan-Tools/tree/master/vulkaninfo/iOS) that contains an XCode project that will build Vulkan Info for iOS devices. In order to deploy to an iOS device, the developer must be a member of the [Apple Developer Program](https://developer.apple.com/programs/). In addition, Vulkan Info makes use of the [MoltenVK](https://github.com/KhronosGroup/MoltenVK) libraries, which is a seperate project from Vulkan-Tools and you will need to add the MoltenVK libraries to your XCode project manually. See the [LunarG Getting Started with the macOS SDK](https://vulkan.lunarg.com/doc/sdk/latest/mac/getting_started.html) guide for more information about using MoltenVK in the Vulkan SDK.

When Vulkan Info is run on an iOS device, it will create the standard vulkaninfo.json file and also a portability.json file. An HTML file is also created, and is displayed in a webview on the iOS device when the application is run. These files are located on the iOS device, and are available via the [iOS application file sharing mechanism](https://support.apple.com/en-us/HT201301) via iTunes for Windows or macOS. You can also access these files using the macOS Finder or Windows Explorer when the device is attached to the host computer via a USB cable. Select the device, and navigate to the "vulkaninfo" folder to access these files. There is currently no supported means to access these files on an iOS device from a Linux host computer.




