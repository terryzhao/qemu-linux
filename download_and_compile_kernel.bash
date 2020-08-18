#!/bin/bash


echo "Setup environment"

#sudo apt-get update && apt-get install -y qemu wget gcc-arm-linux-gnueabi bc

echo "Download linux 4.4.1"

#wget https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.4.1.tar.xz
#tar xvf linux-4.4.1.tar.xz

echo "Compile kernel"

cd linux-4.4.1
make ARCH=arm CROSS_COMPILE=arm-arm1176jzfssf-linux-gnueabi- vexpress_defconfig

echo "CONFIG_FHANDLE=y" >> .config
echo "CONFIG_LBDAF=y" >> .config
echo "CONFIG_XFS_FS=n" >> .config
echo "CONFIG_GFS2_FS=n" >> .config


make -j8 ARCH=arm CROSS_COMPILE=arm-arm1176jzfssf-linux-gnueabi- all
