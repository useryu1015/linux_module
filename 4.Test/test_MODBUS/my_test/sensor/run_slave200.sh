#!/bin/bash
proc_name="modbus_slave"

# 定义运行次数
N=100

# 循环运行 N 次
killall ${proc_name}
for ((i=1; i<=$N; i++))
do
    echo "hello $i"
    
    port=$((502 + $i))
    slave=1
    ./${proc_name} $port $slave > /dev/null &
done

sleep 1
netstat -natp | grep modbus

while true
do
    for ((i=1; i<=$N; i++))
    do
        port=$((502 + $i))
        slave=1

        slave_net=$(netstat -natp | grep :$port | grep modbus | grep LISTEN)
        if [ -z "$slave_net" ]; then
            echo "restart slave port:$port"
            ./${proc_name} $port $slave > /dev/null &
        fi
    done
    sleep 1  # 可以适当调整延迟时间
done
netstat -natp | grep :598 | grep modbus | grep LISTEN