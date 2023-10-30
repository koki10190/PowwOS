dd if=/dev/zero of=blank.img bs=1M count=40
mformat -i blank.img -F -h 64 -s 32 -t \$\(\(40000-1\)\) ::
mv blank.img iso