dd if=/dev/zero of=fat.img bs=1M count=40
mformat -i fat.img -F -h 64 -s 32 -t \$\(\(40000-1\)\) ::
mmd -i fat.img ::/EFI
mmd -i fat.img ::/EFI/BOOT
mcopy -i fat.img src/bootloader/build/bootx64.efi ::/EFI/BOOT
mcopy -i fat.img src/kernel/build/kernel.elf ::/
cp fat.img iso
xorriso -as mkisofs -R -f -e fat.img -no-emul-boot -o cdimage.iso iso
mv cdimage.iso iso