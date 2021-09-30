# Stellaris Evalbot Firmware
Reviving the TI Stellaris Evalbot in 2021! 🤖

## Dependencies
The following are required to build and run this project.

### StellarisWare
Get StellarisWare: https://www.ti.com/tool/SW-DRL#downloads (yes, finally a link that is not dead!)

Extract to `../stellarisware` (or update the makefile with your installation path)

### Compiler gcc-arm-none-eabi
Get gcc-arm-none-eabi: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

Installation instructions for Ubuntu 20.04: https://askubuntu.com/a/1243405/1076308

Available for OS X with `brew`:
```
brew install --cask gcc-arm-embedded
```


### OpenOCD
Get OpenOCD: https://openocd.org/pages/getting-openocd.html

For supported Linux distributions, it can be installed with `apt`:
```
apt install openocd
```
or with `brew`:
```
brew install open-ocd
```

## Build and debug with VSCodium or VSCode
This project includes a `.vscode` folder with `tasks.json` for building and uploading. There is also a `launch.json` for debugging. Debugging requires on the Coretex-Debug addon:

VSCodium: https://open-vsx.org/extension/marus25/cortex-debug

VSCode: https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug


## Build and debug with terminal

### Building
1. Clone or download the code from this repository
2. Place this folder next to the `stellarisware` folder, or update `makefile`
3. Navigate to this folder and run `make`

### Flashing
Navigate to this folder and run:
```
openocd -f evalbot_flash.cfg
```

### Debugging
Navigate to this folder and run:
```
openocd -f evalbot.cfg
```

Then run GDB in a second terminal:
```
arm-none-eabi-gdb gcc/project.axf
```

Example of commands to run in GDB to load the program and set breakpoint in `main()`:
```
target extended-remote localhost:3333
monitor reset
load
break main
continue
detach
quit
```
