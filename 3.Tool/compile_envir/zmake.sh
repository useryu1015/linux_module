#!/bin/sh
##                          main                            ##
RUNDIR=`cd $(dirname $0); pwd -P`
core=$1
if [[ -z "$core" ]]; then
    core=x86                    # 默认编译选项
    # core=x300                    # 默认编译选项
fi

if [ "$core" == "x100" ];then
    COMPILER_PREFIX=arm-openwrt-linux-gnueabi-
elif [ "$core" == "x300" ];then
    COMPILER_PREFIX=arm-openwrt-linux-muslgnueabi-
elif [ "$core" == "x86" ];then
    COMPILER_PREFIX=
elif [ "$core" == "clean" ];then
    make clean
    exit  
elif [ "$core" == "-h" ];then
    echo "Usage: $0 {x100|x300|x86|clean}"
    exit
else
    echo "Usage: $0 {x100|x300|x86|clean|distclean}"
    exit
fi
COMPILER_LIB_PTAH=./lib/lib_$core

if [ "$2" == "debug" ];then
    make debug COMPILER_LIB_PTAH=$COMPILER_LIB_PTAH COMPILER_PREFIX=$COMPILER_PREFIX
    exit
fi

##                          编译                            ##
make COMPILER_LIB_PTAH=$COMPILER_LIB_PTAH COMPILER_PREFIX=$COMPILER_PREFIX

##                          cp mnt                          ##
# cp ditoo /home/share_mnt/
# cp ditoo_main.json /home/share_mnt/
# cp modbus_dp.json /home/share_mnt/
# cp virtual_dp.json /home/share_mnt/

##                          cp x86 self                     ##
if [ "$core" == "x86" ];then
    # \cp ditoo_main.json /mnt/nandflash/DT4/conf
    rm -f /mnt/nandflash/DT4/ditoo
    \cp ditoo /mnt/nandflash/DT4/
fi

echo "pack done"











# 远端同步： 
# while true; do date ; sleep 2; \cp ../mount_dir/ditoo .; done
# while true; do date ; sleep 2; then \mv /mnt/nandflash/mount_dir/ditoo /mnt/nandflash/DT4/ \ ; done