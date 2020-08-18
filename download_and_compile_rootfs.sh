#!/bin/bash

TOPDIR=`pwd`

##download busybox
#echo "downloading busybox..."
#wget https://busybox.net/downloads/busybox-1.31.0.tar.bz2
#
#tar -jxvf busybox-1.31.0.tar.bz2

echo "compiling busybox"
cd $TOPDIR/busybox-1.31.0
make -j8 distclean
make -j8 defconfig
make -j8 CROSS_COMPILE=arm-arm1176jzfssf-linux-gnueabi-
make -j8 install CROSS_COMPILE=arm-arm1176jzfssf-linux-gnueabi-

cd $TOPDIR
rm rootfs* -fr
rm a9rootfs.ext3 -f

echo "building ramfs.cpio"
mkdir $TOPDIR/rootfs
cd    $TOPDIR/rootfs
cp    $TOPDIR/busybox-1.31.0/_install/* -rf ./
mkdir -p $TOPDIR/rootfs/{dev,etc/init.d,lib,proc,sys}
cp -P /opt/arm-linux/arm-arm1176jzfssf-linux-gnueabi/gcc-4.6.4/arm-arm1176jzfssf-linux-gnueabi/lib/* ./lib/

sudo mknod ./dev/tty1 c 4 1
sudo mknod ./dev/tty2 c 4 2
sudo mknod ./dev/tty3 c 4 3
sudo mknod ./dev/tty4 c 4 4
sudo mknod ./dev/console c 5 1
sudo mknod ./dev/ttyAMA0 c 204 64
sudo mknod ./dev/null c 1 3


cp $TOPDIR/init ./
chmod +x ./init

cp $TOPDIR/rcS ./etc/init.d/
chmod +x ./etc/init.d/rcS

find . -print0 | cpio -o --format=newc | gzip > $TOPDIR/rootfs.cpio.gz


echo "building rootfs.ext3"
cd $TOPDIR
dd if=/dev/zero of=a9rootfs.ext3 bs=1M count=64
mkfs.ext3 a9rootfs.ext3
mkdir -p tmpfs
sudo mount -t ext3 a9rootfs.ext3 tmpfs/ -o loop
sudo cp -rf rootfs/* tmpfs/
sudo umount tmpfs
rmdir tmpfs

