#!/bin/bash

if [ "clean" == $1 ]; then
# if [ "$1" -n ]; then
    echo "clean"
    rm -f shm_master shm_slave
    exit
fi



echo "slave"
gcc -o shm_slave shm_slave.c

echo "master"
gcc -o shm_master shm_master.c



