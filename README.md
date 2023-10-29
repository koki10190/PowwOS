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

This is a remake of an UEFI Application test I made!
