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

### MacOS
#### C++ Compiler
##### XCode Command Line Tools using `brew`

[Brew](https://brew.sh/) is a command line package manager that makes it easy to install packages and software dependencies.

1. Open a new terminal and input the following command:
``` sh
bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```
2. If XCode Command Line Tools are not installed, the `brew` install process will ask if you want to install. Type `y` to install.
3. Wait for `brew` to install the XCode Command Line Tools. This make take some time.
4. Once `brew` is finished, confirm the XCode Command Line Tools have installed by opening a new terminal and inputting `clang --version`.

If stuck waiting for `brew` to install XCode Command Line Tools for over 15-20 minutes, you many need to cancel the installation (`CTRL-C` in the terminal)
and install XCode Command Line Tools though the installer.

##### XCode Command Line Tools using installer

1. Go to [developer.apple.com/downloads](https://developer.apple.com/download/all/).
2. Input your AppleID to access the developer downloads.
3. From the presented list, scroll until you find `Command Line Tools for Xcode <version>`.
4. Select `view more details` and then select `Additionals Tools for Xcode <version>.dmg`.
5. Once downloaded, double click the `.dmg` and follow the installer instructions.
6. Confirm XCode Command Line Tools have installed by opening a new terminal and inputting `clang --version`.

#### CMake
##### CMake using `brew`

1. Confirm you have `brew` installed:
``` sh
brew --version
```
2. Install CMake by running `brew install cmake`.
3. Once the install is finished, close the terminal and reopen it.
4. Confirm CMake is installed by running `cmake --version`.

##### CMake using official precompiled download

1. Go to [cmake.org/install](https://cmake.org/install/).
2. Follow the install instructions for MacOS on the website page.
3. Once CMake is installed and added to the path, confirm it's working by running `cmake --version`.

### Linux
#### C++ Compiler (Clang or GCC)

Many Linux operating systems have C++ compilers installed by default, so you might already `clang` or `gcc` preinstalled.
To test, try running the following in a new terminal:
``` sh
clang --version
```
``` sh
gcc --version
```

##### Install GCC or Clang on Ubuntu

1. Open a new terminal
2. (optional) Run `sudo apt-get update` to get latest package updates.
3. (optional) Run `sudo apt-get upgrade` to install latest package updates.
4. Run `sudo apt-get install build-essential` to install GCC or `sudo apt-get install clang` to install Clang.
5. Once the install is finished, close the terminal and reopen it.
6. Confirm GCC is installed by running `gcc --version` or Clang is installed by running `clang --version`.

##### Install GCC or Clang on Arch Linux

1. Open a new terminal.
2. Run `sudo pacman -S gcc` to install GCC or `sudo pacman -S clang` to install Clang.
3. Once the install is finished, close the terminal and reopen it.
4. Confirm Clang is installed by running `gcc --version`.

#### CMake

##### Install CMake on Ubuntu

1. Open the Ubuntu Software Center
2. In the search bar enter `cmake` and select `CMake - cross-platform build system` from the list
3. Press the `install` button
4. After CMake has installed open a new terminal
5. Type `cmake --version` to confirm CMake is installed

Or using the command line:

1. Open a new terminal
2. Run `sudo snap install cmake` to install CMake from the snap store
3. After CMake has installed, close the terminal and reopen it
4. Type `cmake --version` to confirm CMake is installed

##### Install CMake on Arch Linux

1. Open a new terminal.
2. Run `sudo pacman -S cmake` to install Cmake
3. After CMake has installed, close the terminal and reopen it
4. Type `cmake --version` to confirm CMake is installed.


## FAQ

1. CMake Error: CMAKE_C_COMPILER not set, after EnableLanguage
CMake Error: CMAKE_CXX_COMPILER not set, after EnableLanguage