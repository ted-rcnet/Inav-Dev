# Building in Windows with MSYS2
- This environment does not require installing WSL which may not be available or would get in the way of other virtualization and/or anti-cheat softwares. 
- It is also much faster to install and get set up because of its small size(~2 GB download and ~3.12 GB total after building hex file).

## Setting up the environment

1. Download and install the latest MSYS2 release from https://repo.msys2.org/distrib/x86_64/
    - Scroll all the way down for an executable
    - Scroll halfway down for a self-extracting archive

1. Open an MSYS2 terminal by running C:\msys64\msys2_shell.cmd

1. In the newly opened shell, set up your work path
```
mkdir /c/Workspace
cd /c/Workspace
```

## Downloading the INAV repository

- For master:
```
git clone https://github.com/iNavFlight/inav
```
- For [a branch](https://github.com/iNavFlight/inav/branches) or [a tag](https://github.com/iNavFlight/inav/tags): 
```
# "5.1.0" here can be the name of a branch or a tag 
git clone --branch 5.1.0 https://github.com/iNavFlight/inav
```
- If you are internet speed or space restrained, you can also use `--depth 1` which won't download the whole history and `--single-branch` which won't download other branches:
```
git clone --depth 1 --single-branch --branch 5.1.0 https://github.com/iNavFlight/inav
```
This results in ~315 MB instead of ~476 MB download/install size

## Installing dependencies

### Installing other dependencies:
```
pacman -S git ruby make cmake gcc mingw-w64-x86_64-libwinpthread-git unzip wget
```

### Installing xPack 
1. Create xPack directory:
```
mkdir /c/Workspace/xpack
cd /c/Workspace/xpack
```
2. Get the xPack version you need for your INAV version:
```
cat /c/Workspace/inav/cmake/arm-none-eabi-checks.cmake | grep "set(arm_none_eabi_gcc_version" | cut -d\" -f2
```
3. Find the version you need from the [releases page](https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/), then either:
- Download the "...-win32-x64.zip" and copy the folder inside, or
- Right click, choose "Copy link address" and paste it into the following commands:
```
cd /c/Workspace/xpack
# paste the link after "wget"
wget https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v10.2.1-1.1/xpack-arm-none-eabi-gcc-10.2.1-1.1-win32-x64.zip
# paste the file name after "unzip"
unzip xpack-arm-none-eabi-gcc-10.2.1-1.1-win32-x64.zip
# you can delete the zip file after as it is no longer needed
rm xpack-arm-none-eabi-gcc-10.2.1-1.1-win32-x64.zip
```
3. This is important, put the toolkit first before your path so that it is picked up ahead of any other versions that may be present on your system:
```
export PATH=/c/Workspace/xpack/xpack-arm-none-eabi-gcc-10.2.1-1.1/bin:$PATH
cd /c/Workspace/inav/build
```

## Building the INAV firmware
1. Create the build directory:
```
cd /c/Workspace/inav
mkdir build
```
2. Go into the build directory:
```
cd build
```
3. Run cmake
```
cmake ..
```
4. Compile the firmware for your flight controller.
```
make MATEKF405
```
- The list of available targets in INAV can be found here: https://github.com/inavflight/inav/tree/master/src/main/target

- The generated hex file will be in the /c/Workspace/inav/build folder

- For the following runs, repeat steps 2 to 4

## Troubleshooting

### *** multiple target patterns.  Stop. | Error 2
#### Delete everything in the build directory that contains previous runs
You can either use file explorer and delete everything inside C:\Workspace\inav\build
or run:
```
cd /c/Workspace/inav/build
rm -rf *
```
### -- could not find arm-none-eabi-gcc
#### Redo export PATH:
```
export PATH=/c/Workspace/xpack/xpack-arm-none-eabi-gcc-10.2.1-1.1/bin:$PATH
```
