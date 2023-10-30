# PowwOS

# Fork of: https://github.com/ajxs/uefi-elf-bootloader because my bootloader was SHIT!

## For learning purposes

# Features

- Simple Window Manager (Still working on it)
- Graphics Output Protocol for rendering with triple buffering
- GDT, IDT & IRQ
- Panic State
- Paging (Still working on it)
- Applications (Currently they're hard coded)
- Terminal
- Mouse Driver
- Keyboard Driver
- RTC Time

# Features that are going to be added

- Filesystem
- PCI
- More to be decided

## How to build

### Dependencies:

- GNU Make
- GNU-EFI
- x86_64 ELF Cross Compiler Toolchain in PATH
- QEMU To Emulate the OS

```
git clone https://github.com/koki10190/PowwOS
cd PowwOS
./run
```

## Learning Resources

- https://forum.osdev.org/index.php
- https://github.com/lucianoforks/tetris-os
- https://github.com/pdoane/osdev
- https://github.com/Andrispowq/HackOS
- https://github.com/torvalds/linux

# THIS IS A HOBBY OS FOR LEARNING AND FUN PURPOSES!

### WARNING! This repo is full of garbage code!

---

# TODO

- Desktop Enviroment where there are applications you can launch - SEMI-DONE STILL WORK IN PROGRESS!
- Window Collision Function (window pointer and rect to calculate the positions instantly without manually adding)
- RAM Filesystem for testing and fun (basically volatile filesystem) then try to implement an actual filesystem!
- Someway for users to program custom applications (a simple programming language basically) that is runnable

---

# HOW TO USE CERTAIN APPS

## File Editor

- END To write to file
- ESC To close without writing to file

## Paint

- Press 1 to change color to BLUE
- Press 1 to change color to BLUE
- Press 2 to change color to LIGHTBLUE
- Press 3 to change color to GREEN
- Press 4 to change color to LIGHTGREEN
- Press 5 to change color to RED
- Press 6 to change color to LIGHTRED
- Press 7 to change color to MAGENTA
- Press 8 to change color to PINK
- Press 9 to change color to LIGHTGRAY
- Press 0 to change color to DARKGRAY
- Press MINUS to change color to WHITE
- Press EQUAL to change color to BLACK
- Press DELETE to clear

This is a remake of an UEFI Application test I made!
