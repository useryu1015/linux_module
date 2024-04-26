#!/bin/sh

write_file=$1
if [ -z "$write_file" ]; then
    write_file="/dev/tty"
fi

proc_name="ditoo"

echo 当前时间： > ${write_file}; date >> ${write_file}; echo ------ >> ${write_file};

echo 内存使用情况： >> ${write_file}; free -h >> ${write_file}; echo ------ >> ${write_file};

echo 磁盘使用情况： >> ${write_file}; df -h >> ${write_file}; echo ------ >> ${write_file};

echo 在线进程信息： >> ${write_file}; ps | grep ${proc_name} >> ${write_file}; echo ------ >> ${write_file};
# echo 在线进程信息： >> ${write_file}; ps -ef | grep ${proc_name} >> ${write_fise}; echo ------ >> ${write_file};

echo 网络连接状态： >> ${write_file}; netstat -natp | grep ${proc_name} >> ${write_file}; echo ------ >> ${write_file};
