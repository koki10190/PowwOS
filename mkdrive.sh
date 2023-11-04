dd if=/dev/zero of=blank.img bs=1M count=40
mformat -i blank.img -F -h 64 -s 32 -t \$\(\(40000-1\)\) ::
mcopy -i blank.img ./src/kernel/build/kernel.elf ::/kernel.elf
mv blank.img iso