#/bin/bash

qemu-system-arm -M vexpress-a9 -m 512M -s -kernel ./linux-4.4.1/arch/arm/boot/zImage -dtb  ./linux-4.4.1/arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic -append "root=/dev/mmcblk0 rw roottype=ext4 console=ttyAMA0" -sd a9rootfs.ext3

