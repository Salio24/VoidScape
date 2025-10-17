# VoidScape

This is my game I'm building using my CoriEngine, it's very much in work in progress state. 

Now its more of a demo and a showcase of my game engine.

## Getting Started

Cori Engine uses CMake as a build system. Currently, it supports only Windows.

<ins>**1. Necessary tools**</ins>
- CMake 3.28.x - 3.31.x
- Ninja build system
- Python 3.9 or newer
- Jinja2 python module
- Supported compilers:
  - Clang 20+
  - GCC 15+
  - Should work with earlier versions of compilers that also support C++23, but untested.
  - No support for MSVC and Clang-CL.

<ins>**2. Building:**</ins>

### Windows

First install Jinja2 by running: `pip install Jinja2`

Now you have several options: Visual Studio 2022 CMake integration, plain CMake, CLion

#### Visual Studio 2022 CMake integration
1. Clone the repository with `git clone -b Stable https://github.com/Salio24/VoidScape.git --recursive`
2. Download LLVM toolkit from here: `https://github.com/llvm/llvm-project/releases` 
    - Note: Don't change the default installation folder, and if you did so you need to change paths to the executables in: `clang_toolchain.cmake`
3. Install CMake tools and the clang-cl toolset for Visual Studio in the Visual Studio installer.
4. Copy `CMakePresets.json` and `clang_toolchain.cmake` from `platform/Windows/VS2022` to the project root folder
5. Open project as a folder in Visual Studio 2022, wait for the CMake generation and choose `VoidScape` target and hit build

- Note: Syntax highlighting might be working weirdly in VS2022 versions 17.13.0 and higher because as always microsoft broke somthing, and IntelliSense doesn't work great with clang.
- MSVC and Clang-CL are not supported! As well as visual studio solution!

#### Plain CMake

1. Clone the repository with `git clone -b Stable https://github.com/Salio24/VoidScape.git --recursive`
2. Download LLVM toolkit from here: `https://github.com/llvm/llvm-project/releases`
    - Note: Don't change the default installation folder, and if you did so you need to change paths to the executables in: `Build_Debug_Windows.bat` `Build_Release_Windows.bat` `Build_RelWithDebInfo_Windows.bat`
3. Run one of build scrips for windows: `Build_Debug_Windows.bat` `Build_Release_Windows.bat` `Build_RelWithDebInfo_Windows.bat`

#### CLion

1. Create new project from version control, paste this `https://github.com/Salio24/VoidScape.git` into URL field.
2. By default, CLion will create a `Debug` CMake profile, you can add `Release` and `RelWithDebInfo` if you want.
3. Wait for CLion to generate CMake config and hit build on `VoidScape` target.

### Linux

First install Jinja2:
- For Arch, you need to run: `sudo pacman -S python-jinja`
- I don't know about other distros, try `pip install Jinja2`

Now you have several options: CLion, plain CMake

#### CLion

1. Create new project from version control, paste this `https://github.com/Salio24/VoidScape.git` into URL field.
2. By default, CLion will create a `Debug` CMake profile, you can add `Release` and `RelWithDebInfo` if you want.
3. Wait for CLion to generate CMake config and hit build on `VoidScape` target.

#### Plain CMake

1. Make sure you have gcc package installed
2. Run one of build scripts for linux: `Build_Debug_Linux.sh` `Build_Release_Linux.sh` `Build_RelWithDebInfo_Linux.sh`