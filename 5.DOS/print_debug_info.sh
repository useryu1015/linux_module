#!/bin/sh

write_file=$1
if [ -z "$write_file" ]; then
    write_file="/dev/tty"
fi

current_path=$(dirname "$0")
# echo "当前文件的执行路径是: $current_path"

${current_path}/echo_proc $write_file
${current_path}/echo_unix $write_file
${current_path}/echo_ppid $write_file
