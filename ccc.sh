#!/bin/bash
#cp -v arch/arm64/configs/linux_h618_defconfig .config
export CCACHE_BASEDIR=/home/ub22ss/aw-image-build/source/linux/
#make -j15 ARCH=arm64 'CROSS_COMPILE=ccache /home/ub22ss/aw-image-build/toolchains/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-' Image modules dtbs
make -j15 ARCH=arm64 'CROSS_COMPILE=ccache /home/ub22ss/aw-image-build/source/linux/toolchain/arm-gnu-toolchain-15.2.rel1-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-' $@