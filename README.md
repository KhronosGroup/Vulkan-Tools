# Vulkan Ecosystem Components

This project provides Khronos official Vulkan Tools and Utilities for Windows, Linux, Android, and MacOS.

## Default branch changed to 'main' 2023-01-16

As discussed in #729, the default branch of this repository is now 'main'. This change should be largely transparent to repository users, since github rewrites many references to the old 'master' branch to 'main'. However, if you have a checked-out local clone, you may wish to take the following steps as recommended by github:

```sh
git branch -m master main
git fetch origin
git branch -u origin/main main
git remote set-head origin -a
```

## CI Build Status
[![Build Status](https://github.com/KhronosGroup/Vulkan-Tools/workflows/CI%20Build/badge.svg?branch=main)](https://github.com/KhronosGroup/Vulkan-Tools/actions)

## Introduction

This project provides Vulkan tools and utilities that can assist development by enabling developers to
verify their applications correct use of the Vulkan API.

The following components are available in this repository:
- [*Mock ICD*](icd/)
- [*Vkcube and Vkcube++ Demo*](cube/)
- [*VulkanInfo*](vulkaninfo/)
- [*Windows Runtime*](windows-runtime-installer/)

## Contact Information
* [Tobin Ehlis](mailto:tobine@google.com)
* [Mark Lobodzinski](mailto:mark@lunarg.com)

## Information for Developing or Contributing:

Please see the [CONTRIBUTING.md](CONTRIBUTING.md) file in this repository for more details.
Please see the [GOVERNANCE.md](GOVERNANCE.md) file in this repository for repository management details.

## How to Build and Run

[BUILD.md](BUILD.md)
Includes directions for building all components as well as running the vkcube demo applications.

## Version Tagging Scheme

Updates to the `Vulkan-Tools` repository which correspond to a new Vulkan specification release are tagged using the following format: `v<`_`version`_`>` (e.g., `v1.1.96`).

**Note**: Marked version releases have undergone thorough testing but do not imply the same quality level as SDK tags. SDK tags follow the `sdk-<`_`version`_`>.<`_`patch`_`>` format (e.g., `sdk-1.1.92.0`).

This scheme was adopted following the 1.1.96 Vulkan specification release.

## License
This work is released as open source under a Apache-style license from Khronos including a Khronos copyright.

See LICENSE.txt for a full list of licenses used in this repository.

## Acknowledgements
While this project has been developed primarily by LunarG, Inc., there are many other
companies and individuals making this possible: Valve Corporation, funding
project development; Google providing significant contributions to the validation layers;
Khronos providing oversight and hosting of the project.
