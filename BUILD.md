# Build Instructions

Instructions for building this repository on Linux, Windows, Android, and MacOS.

## Index

1. [Contributing](#contributing-to-the-repository)
2. [Repository Set-Up](#repository-set-up)
3. [Windows Build](#building-on-windows)
4. [Linux Build](#building-on-linux)
5. [Android Build](#building-on-android)
6. [MacOS build](#building-on-macos)

## Contributing to the Repository

If you intend to contribute, the preferred work flow is for you to develop
your contribution in a fork of this repository in your GitHub account and
then submit a pull request.
Please see the [CONTRIBUTING.md](CONTRIBUTING.md) file in this repository for more details.

## Repository Set-Up

### Display Drivers

This repository does not contain a Vulkan-capable driver.
Before proceeding, it is strongly recommended that you obtain a Vulkan driver from your
graphics hardware vendor and install it properly.

### Download the Repository

To create your local git repository:

    git clone https://github.com/KhronosGroup/Vulkan-Tools

## Building On Windows

### Windows Build Requirements

Windows 7+ with the following software packages:

- Microsoft Visual Studio 2013 Update 4 Professional, VS2015 (any version), or VS2017 (any version).
- [CMake](http://www.cmake.org/download/)
  - Tell the installer to "Add CMake to the system PATH" environment variable.
- [Python 3](https://www.python.org/downloads)
  - Select to install the optional sub-package to add Python to the system PATH
    environment variable.
  - Ensure the `pip` module is installed (it should be by default)
  - Python3.3 or later is necessary for the Windows py.exe launcher that is used to select python3
  rather than python2 if both are installed
- [Git](http://git-scm.com/download/win)
  - Tell the installer to allow it to be used for "Developer Prompt" as well as "Git Bash".
  - Tell the installer to treat line endings "as is" (i.e. both DOS and Unix-style line endings).
  - Install both the 32-bit and 64-bit versions, as the 64-bit installer does not install the
    32-bit libraries and tools.
- Vulkan Loader Library
  - Building the cube and vulkaninfo applications require linking to the Vulkan Loader Library (vulkan-1.dll).
    Locating the library for this repo can be done in two different ways:
      -  The Vulkan SDK can be installed. In this case, cmake should be able to locate the loader repo through the VulkanSDK
         environment variable.
      -  The library can be built from the [Vulkan-Loader](https://github.com/KhronosGroup/Vulkan-Loader.git) repository.
         In this case, the following option should be used on the cmake command line:
             LOADER_REPO_ROOT=c:\absolute_path_to\Vulkan-Loader
         and use absolute (not relative) paths, like so:
             cmake -DLOADER_REPO_ROOT=c:\absolute_path_to\Vulkan-Loader ....
    Currently, the build directory *must* be named either 'build' or 'build32'.
- [glslang](https://github.com/KhronosGroup/glslang)
  - By default, the build scripts will attempt to download the necessary components from the glslang repo.
    However, if a specific version of this file is required, please see the [Custom glslang Version](#custom-glslang-version) section below.

### Windows Build - Microsoft Visual Studio

1. Open a Developer Command Prompt for VS201x
2. Change directory to `Vulkan-Tools` -- the root of the cloned git repository
3. Run 'git submodule update --init --recursive' -- this will download in-tree external dependencies
4. Create a `build` directory, change into that directory, and run cmake

For example, assuming an SDK is installed, for VS2017 (generators for other versions are [specified here](#cmake-visual-studio-generators)):

    cmake "Visual Studio 15 2017 Win64" ..

If a specific version of the Loader is requred, specify the root of the loader repository, like so:

    cmake -DLOADER_REPO_ROOT=c:/absolute_path_to/Vulkan-Loader -G "Visual Studio 15 2017 Win64" ..

This will create a Windows solution file named `Vulkan-Tools.sln` in the build directory.

Launch Visual Studio and open the "Vulkan-Tools.sln" solution file in the build folder.
You may select "Debug" or "Release" from the Solution Configurations drop-down list.
Start a build by selecting the Build->Build Solution menu item.
This solution copies the loader it built to each program's build directory
to ensure that the program uses the loader built from this solution.

### Windows Notes

#### CMake Visual Studio Generators

The above example used Visual Studio 2017, and specified its generator as "Visual Studio 15 2017 Win64".
The chosen generator should match your Visual Studio version. Appropriate Visual Studio generators include:

| Build Platform               | 64-bit Generator              | 32-bit Generator        |
|------------------------------|-------------------------------|-------------------------|
| Microsoft Visual Studio 2013 | "Visual Studio 12 2013 Win64" | "Visual Studio 12 2013" |
| Microsoft Visual Studio 2015 | "Visual Studio 14 2015 Win64" | "Visual Studio 14 2015" |
| Microsoft Visual Studio 2017 | "Visual Studio 15 2017 Win64" | "Visual Studio 15 2017" |

## Building On Linux

### Linux Build Requirements

This repository has been built and tested on the two most recent Ubuntu LTS versions.
Currently, the oldest supported version is Ubuntu 14.04, meaning that the minimum supported compiler versions are GCC 4.8.2 and Clang 3.4, although earlier versions may work.
It should be straightforward to adapt this repository to other Linux distributions.

**Required Package List:**

    sudo apt-get install git cmake build-essential libx11-xcb-dev libxkbcommon-dev libmirclient-dev libwayland-dev libxrandr-dev

- [glslang](https://github.com/KhronosGroup/glslang)
  - By default, the build scripts will attempt to download the necessary components from the glslang repo.
    However, if a specific version of this file is required, please see the [Custom glslang Version](#custom-glslang-version) section below.

Vulkan Loader Library
  - Building the cube and vulkaninfo applications require linking to the Vulkan Loader Library (libvulkan.so.1).
      - The following option should be used on the cmake command line to specify a vulkan loader library:
             LOADER_REPO_ROOT=/absolute_path_to/Vulkan-Loader
         making sure to specify an absoute path, like so:
             cmake -DLOADER_REPO_ROOT=/absolute_path_to/Vulkan-Loader ....
    Currently, the build directory *must* be named either 'build' or 'build32'.

### Linux Build

Example debug build

See **Loader and Validation Layer Dependencies** for more information and other options:

1. In a Linux terminal, `cd Vulkan-Tools` -- the root of the cloned git repository
2. Execute 'git submodule update --init --recursive' -- this will download in-tree external components
3. Create a `build` directory, change into that directory, and run cmake:

        mkdir build
        cd build
        # If an SDK is installed and the setup-env.sh script has been run,
        cmake -DCMAKE_BUILD_TYPE=Debug ..
        # Else if a specific version of the loader is desired, indicate the root of the loader repository like so:
        cmake -DLOADER_REPO_ROOT=/absolute_path_to/Vulkan-Loader -DCMAKE_BUILD_TYPE=Debug ..

4. Run `make -j8` to begin the build

If your build system supports ccache, you can enable that via CMake option `-DUSE_CCACHE=On`

### WSI Support Build Options

By default, the Vulkan Tools cube and cubepp are built with support for all 4 Vulkan-defined WSI display servers: Xcb, Xlib, Wayland, and Mir.
It is recommended to build the repository components with support for these display servers to maximize their usability across Linux platforms.
If it is necessary to build these modules without support for one of the display servers, the appropriate CMake option of the form `BUILD_WSI_xxx_SUPPORT` can be set to `OFF`.
See the CMakeLists.txt file in `Vulkan-Tools/cube` for more info.

Note vulkaninfo currently only supports Xcb and Xlib WSI display servers.  See the CMakeLists.txt file in `Vulkan-Tools/vulkaninfo` for more info.

### Linux Install to System Directories

Installing the files resulting from your build to the systems directories is optional since environment variables can usually be used instead to locate the binaries.
There are also risks with interfering with binaries installed by packages.
If you are certain that you would like to install your binaries to system directories, you can proceed with these instructions.

Assuming that you have built the code as described above and the current directory is still `build`, you can execute:

    sudo make install

This command installs files to:

- `/usr/local/lib`:  Vulkan Tools shared objects  (e.g., Mock ICD shared library)
- `/usr/local/bin`:  vulkaninfo application
- `/usr/local/share/vulkan/icd.d`:  Mock ICD JSON file

You may need to run `ldconfig` in order to refresh the system loader search cache on some Linux systems.

Note:  The Mock ICD is not installed by default since it is a "null" driver that does not render anything
and is used for testing purposes.
Installing it to system directories may cause some applications to discover and use
this driver instead of other full drivers installed on the system.
If you really want to install this null driver, use:

    -DINSTALL_ICD=ON

You can further customize the installation location by setting additional CMake variables to override their defaults.
For example, if you would like to install to `/tmp/build` instead of `/usr/local`, on your CMake command line specify:

    -DCMAKE_INSTALL_PREFIX=/tmp/build
    -DDEST_DIR=/tmp/build

Then run `make install` as before. The install step places the files in `/tmp/build`.

You can further customize the installation directories by using the CMake variables
`CMAKE_INSTALL_SYSCONFDIR` to rename the `etc` directory and `CMAKE_INSTALL_DATADIR`
to rename the `share` directory.

See the CMake documentation for more details on using these variables
to further customize your installation.

### Linux Uninstall

To uninstall the files from the system directories, you can execute:

    sudo make uninstall-Vulkan-Tools

### Linux Tests

After making any changes to the repository, you should perform some quick sanity tests, such as
running the cube demo with validation enabled.

To run the **Cube application** with validation, in a terminal change to the `build/cube`
directory and run:

    VK_LAYER_PATH=../path/to/validation/layers ./cube --validate

You can select which WSI subsystem is used to build the cube applications using a CMake option
called DEMOS_WSI_SELECTION.
Supported options are XCB (default), XLIB, WAYLAND, and MIR.
Note that you must build using the corresponding BUILD_WSI_*_SUPPORT enabled at the
base repository level (all SUPPORT options are ON by default).
For instance, creating a build that will use Xlib to build the demos,
your CMake command line might look like:

    cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug -DDEMOS_WSI_SELECTION=XLIB

### Linux Notes

#### Linux 32-bit support

Usage of the contents of this repository in 32-bit Linux environments is not officially supported.
However, since this repository is supported on 32-bit Windows,
these modules should generally work on 32-bit Linux.

Here are some notes for building 32-bit targets on a 64-bit Ubuntu "reference" platform:

If not already installed, install the following 32-bit development libraries:

`gcc-multilib g++-multilib libx11-dev:i386`

This list may vary depending on your distribution and which windowing systems you are building for.

Set up your environment for building 32-bit targets:

    export ASFLAGS=--32
    export CFLAGS=-m32
    export CXXFLAGS=-m32
    export PKG_CONFIG_LIBDIR=/usr/lib/i386-linux-gnu

Again, your PKG_CONFIG configuration may be different, depending on your distribution.

Finally, rebuild the repository using `cmake` and `make`, as explained above.

## Building On Android

Install the required tools for Linux and Windows covered above, then add the following.

### Android Build Requirements

- Install [Android Studio 2.3](https://developer.android.com/studio/index.html) or later.
- From the "Welcome to Android Studio" splash screen, add the following components using
  Configure > SDK Manager:
  - SDK Platforms > Android 6.0 and newer
  - SDK Tools > Android SDK Build-Tools
  - SDK Tools > Android SDK Platform-Tools
  - SDK Tools > Android SDK Tools
  - SDK Tools > NDK

#### Add Android specifics to environment

For each of the below, you may need to specify a different build-tools version, as Android Studio will roll it forward fairly regularly.

On Linux:

    export ANDROID_SDK_HOME=$HOME/Android/sdk
    export ANDROID_NDK_HOME=$HOME/Android/sdk/ndk-bundle
    export PATH=$ANDROID_SDK_HOME:$PATH
    export PATH=$ANDROID_NDK_HOME:$PATH
    export PATH=$ANDROID_SDK_HOME/build-tools/23.0.3:$PATH

On Windows:

    set ANDROID_SDK_HOME=%LOCALAPPDATA%\Android\sdk
    set ANDROID_NDK_HOME=%LOCALAPPDATA%\Android\sdk\ndk-bundle
    set PATH=%LOCALAPPDATA%\Android\sdk\ndk-bundle;%PATH%

On OSX:

    export ANDROID_SDK_HOME=$HOME/Library/Android/sdk
    export ANDROID_NDK_HOME=$HOME/Library/Android/sdk/ndk-bundle
    export PATH=$ANDROID_NDK_PATH:$PATH
    export PATH=$ANDROID_SDK_HOME/build-tools/23.0.3:$PATH

Note: If `jarsigner` is missing from your platform, you can find it in the
Android Studio install or in your Java installation.
If you do not have Java, you can get it with something like the following:

  sudo apt-get install openjdk-8-jdk

#### Additional OSX System Requirements

Tested on OSX version 10.13.3

Setup Homebrew and components

- Follow instructions on [brew.sh](http://brew.sh) to get Homebrew installed.

      /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

- Ensure Homebrew is at the beginning of your PATH:

      export PATH=/usr/local/bin:$PATH

- Add packages with the following:

      brew install cmake python

### Android Build

There are two options for building the Android tools.
Either using the SPIRV tools provided as part of the Android NDK, or using upstream sources.
To build with SPIRV tools from the NDK, remove the build-android/third_party directory created by
running update_external_sources_android.sh, (or avoid running update_external_sources_android.sh).
Use the following script to build everything in the repository for Android, including validation
layers, tests, demos, and APK packaging: This script does retrieve and use the upstream SPRIV tools.

    cd build-android
    ./build_all.sh

Test and application APKs can be installed on production devices with:

    ./install_all.sh [-s <serial number>]

Note that there are no equivalent scripts on Windows yet, that work needs to be completed.
The following per platform commands can be used for layer only builds:

#### Linux and OSX

Follow the setup steps for Linux or OSX above, then from your terminal:

    cd build-android
    ./update_external_sources_android.sh --no-build
    ./android-generate.sh
    ndk-build -j4

#### Windows

Follow the setup steps for Windows above, then from Developer Command Prompt for VS2013:

    cd build-android
    update_external_sources_android.bat
    android-generate.bat
    ndk-build

### Android Tests and Demos

After making any changes to the repository you should perform some quick sanity tests,
including the layer validation tests and the cube and smoke demos with validation enabled.

#### Run Layer Validation Tests

Use the following steps to build, install, and run the layer validation tests for Android:

    cd build-android
    ./build_all.sh
    adb install -r bin/VulkanLayerValidationTests.apk
    adb shell am start com.example.VulkanLayerValidationTests/android.app.NativeActivity

Alternatively, you can use the test_APK script to install and run the layer validation tests:

    test_APK.sh -s <serial number> -p <plaform name> -f <gtest_filter>

#### Run Cube with Validation

TODO: This must be reworked to pull in layers from the ValidationLayers repo

Use the following steps to build, install, and run Cube for Android:

    cd build-android
    ./build_all.sh
    adb install -r ../demos/android/cube/bin/cube.apk
    adb shell am start com.example.Cube/android.app.NativeActivity

To build, install, and run Cube with validation layers,
first build layers using steps above, then run:

    cd build-android
    ./build_all.sh
    adb install -r ../demos/android/cube-with-layers/bin/cube-with-layers.apk

##### Run without validation enabled

    adb shell am start com.example.CubeWithLayers/android.app.NativeActivity

##### Run with validation enabled

    adb shell am start -a android.intent.action.MAIN -c android-intent.category.LAUNCH -n com.example.CubeWithLayers/android.app.NativeActivity --es args "--validate"

## Building on MacOS

### MacOS Build Requirements

Tested on OSX version 10.12.6

Setup Homebrew and components

- Follow instructions on [brew.sh](http://brew.sh) to get Homebrew installed.

      /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

- Ensure Homebrew is at the beginning of your PATH:

      export PATH=/usr/local/bin:$PATH

- Add packages with the following (may need refinement)

      brew install cmake python python3 git

- [glslang](https://github.com/KhronosGroup/glslang)
  - By default, the build scripts will attempt to download the necessary components from the glslang repo.
    However, if a specific version of this file is required, please see the [Custom glslang Version](#custom-glslang-version) section below.

### Clone the Repository

Clone the Vulkan-LoaderAndValidationLayers repository:

    git clone https://github.com/KhronosGroup/Vulkan-LoaderAndValidationLayers.git

### Get the External Libraries

[MoltenVK](https://github.com/KhronosGroup/MoltenVK) Library

- Building the cube and vulkaninfo applications require linking to the MoltenVK Library (libMoltenVK.dylib)
  - The following option should be used on the cmake command line to specify a vulkan loader library:
    MOLTENVK_REPO_ROOT=/absolute_path_to/MoltenVK making sure to specify an absolute path, like so:
    cmake -DMOLTENVK_REPO_ROOT=/absolute_path_to/MoltenVK ....

Vulkan Loader Library

- Building the cube and vulkaninfo applications require linking to the Vulkan Loader Library (libvulkan.1.dylib)
  - The following option should be used on the cmake command line to specify a vulkan loader library:
    LOADER_REPO_ROOT=/absolute_path_to/Vulkan-Loader making sure to specify an absolute path, like so:
    cmake -DLOADER_REPO_ROOT=/absolute_path_to/Vulkan-Loader ....

### MacOS build

#### CMake Generators

This repository uses CMake to generate build or project files that are
then used to build the repository.
The CMake generators explicitly supported in this repository are:

- Unix Makefiles
- Xcode

#### Building with the Unix Makefiles Generator

This generator is the default generator, so all that is needed for a debug
build is:

        mkdir build
        cd build
        cmake -DCMAKE_BUILD_TYPE=Debug -DLOADER_REPO_ROOT=/absolute_path_to/Vulkan-Loader -DMOLTENVK_REPO_ROOT=/absolute_path_to/MoltenVK ..
        make

To speed up the build on a multi-core machine, use the `-j` option for `make`
to specify the number of cores to use for the build.
For example:

    make -j4

You can now run the demo applications from the command line:

    open cube/cube.app
    open cube/cubepp.app
    open vulkaninfo/vulkaninfo.app

Or you can locate them from `Finder` and launch them from there.

##### The Install Target and RPATH

The applications you just built are "bundled applications", but the executables
are using the `RPATH` mechanism to locate runtime dependencies that are still
in your build tree.

To see this, run this command from your `build` directory:

    otool -l cube/cube.app/Contents/MacOS/cube

and note that the `cube` executable contains loader commands:

- `LC_LOAD_DYLIB` to load `libvulkan.1.dylib` via an `@rpath`
- `LC_RPATH` that contains an absolute path to the build location of the Vulkan loader

This makes the bundled application "non-transportable", meaning that it won't run
unless the Vulkan loader is on that specific absolute path.
This is useful for debugging the loader or other components built in this repository,
but not if you want to move the application to another machine or remove your build tree.

To address this problem, run:

    make install

This step "cleans up" the `RPATH` to remove any external references
and performs other bundle fix-ups.
After running `make install`, re-run the `otool` command again and note:

- `LC_LOAD_DYLIB` is now `@executable_path/../MacOS/libvulkan.1.dylib`
- `LC_RPATH` is no longer present

The "bundle fix-up" operation also puts a copy of the Vulkan loader into the bundle,
making the bundle completely self-contained and self-referencing.

Note that the "install" target has a very different meaning compared to the Linux
"make install" target.
The Linux "install" copies the targets to system directories.
In MacOS, "install" means fixing up application bundles.
In both cases, the "install" target operations clean up the `RPATH`.

##### The Non-bundled vulkaninfo Application

There is also a non-bundled version of the `vulkaninfo` application that you can
run from the command line:

    vulkaninfo/vulkaninfo

If you run this before you run "make install", vulkaninfo's RPATH is already set
to point to the Vulkan loader in the build tree, so it has no trouble finding it.
But the loader will not find the MoltenVK driver and you'll see a message about an
incompatible driver.  To remedy this:

    VK_ICD_FILENAMES=<path-to>/MoltenVK/Package/Latest/MoltenVK/macOS/MoltenVK_icd.json demos/vulkaninfo

If you run `vulkaninfo` after doing a "make install", the `RPATH` in the `vulkaninfo` application
got removed and the OS needs extra help to locate the Vulkan loader:

    DYLD_LIBRARY_PATH=<path-to>/Vulkan-Loader/loader VK_ICD_FILENAMES=<path-to>/MoltenVK/Package/Latest/MoltenVK/macOS/MoltenVK_icd.json demos/vulkaninfo

#### Building with the Xcode Generator

To create and open an Xcode project:

        mkdir build-xcode
        cd build-xcode
        cmake -DLOADER_REPO_ROOT=/absolute_path_to/Vulkan-Loader -DMOLTENVK_REPO_ROOT=/absolute_path_to/MoltenVK -GXcode ..
        open VULKAN.xcodeproj

Within Xcode, you can select Debug or Release builds in the project's Build Settings.
You can also select individual schemes for working with specific applications like `cube`.

## Ninja Builds - All Platforms

The [Qt Creator IDE](https://qt.io/download-open-source/#section-2) can open a root CMakeList.txt
as a project directly, and it provides tools within Creator to configure and generate Vulkan SDK
build files for one to many targets concurrently.
Alternatively, when invoking CMake, use the `-G "Codeblocks - Ninja"` option to generate Ninja build
files to be used as project files for QtCreator

- Open, configure, and build the Vulkan-Tools CMakeList.txt file
- In order to debug with QtCreator, a
  [Microsoft WDK: eg WDK 10](http://go.microsoft.com/fwlink/p/?LinkId=526733) is required.

Note that installing the WDK breaks the MSVC vcvarsall.bat build scripts provided by MSVC,
requiring that the LIB, INCLUDE, and PATHenv variables be set to the WDK paths by some other means

## Custom glslang version

The Glslang repository is not a git sub-module of Vulkan-Tools, but glslang components are required to build
the cube and cubepp applications. By default, the cmake scripts will download the required
components into the repo 'glslang' directory.

If a *specific* version of the glslang components is desired, perform the following steps:

1) clone the glslang repository:

    `git clone https://github.com/KhronosGroup/glslang.git`

2) Configure the glslang source tree with CMake and build it with your IDE of choice, following the instructions
   in the glslang BUILD.md document including using the INSTALL_PREFIX and 'make install'. Note the install directory.

3) Pass the location of the glslang install directory using an absolute path via your cmake command like so:

    cmake -DGLSLANG_INSTALL_DIR=c:\absolute_path_to\glslang\build\install

## Optional software packages

- [Cygwin for windows](https://www.cygwin.com/)
  - Cygwin provides some Linux-like tools, which can be valuable for working with the repository,
    such as the BASH shell and git packages
  - With appropriate adjustments, it is possible to use other shells and environments as well

- [Ninja on all platforms](https://github.com/ninja-build/ninja/releases)
- [The Ninja-build project](https://ninja-build.org)
- [Ninja Users Manual](https://ninja-build.org/manual.html)

- [QtCreator as IDE for CMake builds on all platforms](https://qt.io/download-open-source/#section-2)
