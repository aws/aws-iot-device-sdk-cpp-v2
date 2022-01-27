# PREREQUISITES

## C++ 11 or higher
Checkout [C++ Compiler](#C++-Compiler)

## CMake 3.1+
1. Download CMake3.1+ for your platform: https://cmake.org/download/

2. Run the Cmake Installer. Make sure you add CMake into **PATH**.

3. Restart the command prompt / terminal.

## C++ Compiler (Clang 3.9+ or GCC 4.8+ or MSVC 2015+)
1. Check if Clang or GCC is installed
``` sh
g++ --version
```
```
clang --version 
```

### Windows
#### MSVC
Install Visual Studio with MSVC
1. Download **Visual Studio Installer**
https://visualstudio.microsoft.com/downloads/


2. Run the installer, check the **Desktop development with C++** workload and select Install.

3. Verify your MSVC installation
   * In Windows Start up Menu, try open "Developer Command Prompt for VS" 

Continue with the Developer Command Prompt to install the SDK and samples. 


## FAQ

1. CMake Error: CMAKE_C_COMPILER not set, after EnableLanguage
CMake Error: CMAKE_CXX_COMPILER not set, after EnableLanguage