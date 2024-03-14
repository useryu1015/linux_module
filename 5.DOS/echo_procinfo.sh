#!/bin/sh

write_file=$1
if [ -z "$write_file" ]; then
    write_file="/dev/tty"
fi


echo 当前时间： > ${write_file}; date >> ${write_file}; echo ------ >> ${write_file};

echo 内存使用情况： >> ${write_file}; free -h >> ${write_file}; echo ------ >> ${write_file};

echo 磁盘使用情况： >> ${write_file}; df -h >> ${write_file}; echo ------ >> ${write_file};

echo 在线进程信息： >> ${write_file}; ps | grep ditoo >> ${write_file}; echo ------ >> ${write_file};
# echo 在线进程信息： >> ${write_file}; ps -ef | grep ditoo >> ${write_file}; echo ------ >> ${write_file};

echo 网络连接状态： >> ${write_file}; netstat -natp | grep ditoo >> ${write_file}; echo ------ >> ${write_file};
