#!/bin/bash

# 定义日志文件路径
write_file=$1
if [ -z "$write_file" ]; then
    write_file="/dev/tty"
fi

# 获取当前时间
time=$(date "+%Y-%m-%d %H:%M:%S")

# 监控CPU使用率
cpu_usage=$(top -b -n 1 | grep "Cpu(s)" | awk '{print $2}' | awk -F "." '{print $1}')
cpu_info="CPU使用率：$cpu_usage%"

# 监控内存使用率
mem_usage=$(free | grep Mem | awk '{print $3/$2 * 100.0}' | awk -F "." '{print $1}')
mem_info="内存使用率：$mem_usage%"

# 监控磁盘使用率
disk_usage=$(df -h | awk '$NF=="/"{printf "%s", $5}')
disk_info="磁盘使用率：$disk_usage"

# 监控网络连接数
# net_connections=$(netstat -an | grep 1883 | grep ESTABLISHED | wc -l)
net_connections=$(netstat -an | grep ESTABLISHED | wc -l)
net_info="网络连接数：$net_connections"

# 将监控结果输出到日志文件中
echo "$time $cpu_info $mem_info $disk_info $net_info" >> "$write_file"