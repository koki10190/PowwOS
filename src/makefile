#####################################################################
#  Copyright (c) 2019, AJXS.
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by the
#  Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  Authors:
#     Anthony <ajxs [at] panoptic.online>
#####################################################################

.POSIX:
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules

KERNEL_DIR        := kernel
KERNEL_BINARY     := ${KERNEL_DIR}/build/kernel.elf

BOOTLOADER_DIR    := bootloader
BOOTLOADER_BINARY := ${BOOTLOADER_DIR}/build/bootx64.efi

BUILD_DIR         := ../build
DISK_IMG          := ${BUILD_DIR}/kernel.img
DISK_IMG_SIZE     := 2880

ASSET_DIR = ../assets

QEMU_FLAGS :=                                                \
	-enable-kvm \
	-machine q35 \
	-drive file=${DISK_IMG},format=raw    \
	-drive file=../iso/blank.img,format=raw    \
	-cpu host \
	-m 2G \
	-bios OVMF.fd                                              \
	-serial stdio                                              \
	-usb                                                       \
	-net none                                                  \
	-vga std
	# -device usb-storage,drive=uas-disk1                        \

.PHONY: all clean emu

all: ${DISK_IMG}

bootloader: ${BOOTLOADER_BINARY}

debug: ${DISK_IMG}
	qemu-system-x86_64    \
		${QEMU_FLAGS}       \
		-S                  \
		-gdb tcp::1234

emu: ${DISK_IMG}
	qemu-system-x86_64    \
		${QEMU_FLAGS}

kernel: ${KERNEL_BINARY}

${DISK_IMG}: ${BUILD_DIR} ${BOOTLOADER_BINARY} ${KERNEL_BINARY}
	# Create UEFI boot disk image in DOS format.
	dd if=/dev/zero of=${DISK_IMG} bs=1M count=40
	mformat -i ${DISK_IMG} -F -h 64 -s 32 -t \$\(\(40000-1\)\) ::
	mmd -i ${DISK_IMG} ::/EFI
	mmd -i ${DISK_IMG} ::/EFI/BOOT
	# Copy the bootloader to the boot partition.
	mcopy -i ${DISK_IMG} ${BOOTLOADER_BINARY} ::/efi/boot/bootx64.efi
	mcopy -i ${DISK_IMG} ${KERNEL_BINARY} ::/kernel.elf
	mcopy -i ${DISK_IMG} ${ASSET_DIR}/tamsyn5x9.psf ::/tamsyn5x9.psf
	mcopy -i ${DISK_IMG} ${ASSET_DIR}/zap-vga09.psf ::/zap-vga09.psf
	mcopy -i ${DISK_IMG} ${ASSET_DIR}/zap-ext-vga16.psf ::/zap-ext-vga16.psf


${BOOTLOADER_BINARY}:
	make -C ${BOOTLOADER_DIR}

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

${KERNEL_BINARY}:
	make -C ${KERNEL_DIR}

clean:
	make clean -C ${BOOTLOADER_DIR}
	make clean -C ${KERNEL_DIR}
	rm -f ${DISK_IMG}
	rm -rf ${BUILD_DIR}
