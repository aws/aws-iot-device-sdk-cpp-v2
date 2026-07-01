# Prerequisites

## Overview

To build the AWS IoT Device SDK for C++ v2, you need:

**Minimum Requirements:**
- C++ 11 compiler (Clang 6+, GCC 4.8+, or MSVC 15+)
- CMake 3.9+

**Quick Install (Generic):**
1. Download CMake 3.9+ from https://cmake.org/download/
2. Run installer and add CMake to PATH
3. Restart terminal

**Platform-Specific Instructions:**
- [Windows](#windows) - MSVC or MinGW-w64 setup
- [macOS](#macos) - Xcode Command Line Tools
- [Linux](#linux) - GCC/Clang via package manager

# Windows

## C++ Compiler Options

Choose one of the following compilers:

### Option 1: MSVC (Microsoft Visual C++)

**Installation Steps:**
1. Download [Visual Studio Installer](https://visualstudio.microsoft.com/downloads/)
2. Run installer and check **Desktop development with C++** workload
3. Verify installation:
   - Open "Developer Command Prompt for VS" from Start Menu
   - Run `cl.exe` to see compiler version

**Important:** Use Developer Command Prompt (not regular terminal) when building with MSVC.

### Option 2: MinGW-w64

MinGW-w64 provides GCC compiler support on Windows and works well with Visual Studio Code.

**Installation Steps:**
1. Download and run the MSYS2 installer from [msys2.org](https://www.msys2.org/)
2. Follow the MSYS2 website instructions to update database and base packages
3. Install MinGW-w64 toolchain:
   ```sh
   pacman -S --needed base-devel mingw-w64-x86_64-toolchain
   ```
4. Add MinGW to Windows PATH:
   - Open Windows Settings → Search "Edit environment variables for your account"
   - Select `Path` in User variables → Click `Edit`
   - Click `New` → Add `C:\msys64\mingw64\bin` (or your install path)
   - Click `OK` to save
5. Restart terminal and verify: `g++ --version`

## CMake Installation

### Method 1: Manual Installation
1. Download [CMake 3.9+](https://cmake.org/download/) for Windows
2. Run installer and check "Add CMake to system PATH" option
3. If you didn't check the PATH option:
   - Open Windows Settings → "Edit environment variables for your account"
   - Add CMake bin folder (e.g., `C:\Program Files (x86)\CMake.x.x\bin`) to PATH
4. Restart terminal and verify: `cmake --version`

### Method 2: Via MSYS2 (if using MinGW-w64)
```sh
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-ninja  # or mingw-w64-x86_64-make
cmake --version
```

# macOS

## C++ Compiler (Xcode Command Line Tools)

**Manual Installation:**
1. Go to [developer.apple.com/downloads](https://developer.apple.com/download/all/)
2. Sign in with Apple ID
3. Download "Command Line Tools for Xcode"
4. Install and verify: `clang --version`

## CMake Installation

**Using Homebrew:**
```sh
brew install cmake
cmake --version
```

**Manual Installation:**
1. Download from [cmake.org](https://cmake.org/install/)
2. Drag CMake.app to Applications folder
3. Add to PATH:
   ```sh
   sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
   ```
4. Restart terminal and verify: `cmake --version`

# Linux

## C++ Compilers

Many Linux distributions have C++ compilers installed by default. Check if already installed:
```sh
clang --version
gcc --version
```

If both these commands fail, install a compiler using your package manager:

**Ubuntu/Debian:**
```sh
sudo apt update
sudo apt install build-essential  # for GCC
# OR
sudo apt install clang           # for Clang
```

**Arch Linux:**
```sh
sudo pacman -S gcc    # for GCC
# OR  
sudo pacman -S clang  # for Clang
```

**RHEL/CentOS/Fedora:**
```sh
sudo dnf install gcc-c++
# OR
sudo dnf install clang
```

## CMake Installation

**Ubuntu/Debian:**
```sh
sudo apt update
sudo apt install cmake
```

**Arch Linux:**
```sh
sudo pacman -S cmake
```

**RHEL/CentOS/Fedora:**
```sh
sudo dnf install cmake
# OR
sudo yum install cmake
```

After that, verify that `cmake` is available:

```sh
cmake --version
```

---

## Troubleshooting

**Common Issues:**
- **Compiler not found**: Ensure your compiler is installed and in PATH
- **CMake version too old**: Download latest CMake from official website
- **Permission errors**: Use `sudo` on Linux/macOS or run as Administrator on Windows
- **PATH issues**: Restart terminal after installing tools

**Getting Help:**
- Check our [FAQ](./FAQ.md)
- Search [existing issues](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues)
- Create a [new issue](https://github.com/aws/aws-iot-device-sdk-cpp-v2/issues/new/choose)
