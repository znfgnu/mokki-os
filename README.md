# Mokki OS
Mokki OS is a part of Mokki Project, thereby is made with lowcostness in mind.

## CMSIS
Cortex Microcontroller Software Interface Standard. Microcontroller vendors put here device description that is necessary for Cortex core to run.

## "inc" and "src"
### \*.c and \*.h
Some of these files are not used - default project files.
#### main.c - entry file
Contains initialization of each module and run GUI function.
#### test.\* - some test routines
Everything you see on the console after turning it on while holding R1. Header file points to some sprites that are stored in loose \*.c files - they shouldn't be there.

### hw
Low-level drivers, mostly platform-dependent. Aims to be the only one directory to reimplement when porting to other platform.
#### btn
files responsible for initialization and handling keypresses.
#### clock
peripherals' clocks initialization.
#### debug
debugging serial port - contains functions for reading and writing things from/to computer while running. I don't know if it's working now.
#### dma
dma initialization for other modules (OLED and debug). I'm not sure if it should be stored in separate directory or these procedures should be stored in their modules' directories.
#### led
RGB led handling
#### oled
Display handling functions. There are no graphics functions, only communication with OLED (SSD1306). Contains two buffers, which are updated by gfx_update() function.
#### time
Systick handling - contains variable that is incremented each millisecond and time_delay() function (active delay)

### os
Core OS functions.
#### api
Contains map (table) that maps API function id to its real address. Used in program loader.
#### loader
Name can be misleading - functions here are used after program is loaded. It links OS functions based on corresponding identifiers and prepares launcher for program (pointer to function that can be run to run the program).
#### program
[TODO] Contains simple program, I don't even remember which one. These files used to be used for launcher testing purposes.
#### program_struct.h
Structures that describe structure of loaded program. You can read some useful values after making void* pointer to program and casting it to program_struct_t*.
#### raminfo
Some functions that calculate RAM usage (free/static/total/stack).
#### shell
[TODO] Very small shell used for loader testing. Not used now. I think it needs to be rewritten from scratch if needed.

### usb
More files responsible for USB communication can be found in /USB and /STM32-USBFS directories.
#### ramdisk
FAT12 handling, seems like a bit of magic, but works.

### gfx
All graphics stuff. Frame buffer is drawn here and then is sent to hw/oled module, to push it to the display.
#### character/dragon/dragon2/liero
Sprites used in demo. [TODO] need to be moved to better place.
#### font
Default font data and functions that can print char and string.
#### gfx_lut
Mysterious table that can be used for displaying screen in gfx_update(). Maybe speeds up a bit.
#### gfx
Set pixel, blit_sprite, clear, all this stuff.

### gui
The screen you see after turning on the console. Basically, it should be called an userspace program. Handles input using API, runnning with program priority. [TODO] Need to be rewritten as PendSV irq that runs programs by returning from irq.
