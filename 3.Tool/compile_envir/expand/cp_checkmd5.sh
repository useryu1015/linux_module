#!/bin/sh
###############################################################
# 说明：
#       校验（md5、date）文件是否被修改，如果被修改则替换
# 作用：
#       文件自动同步（开发环境搭建、）
# 优化：
#       1. 通过免密scp远程拷贝文件，远程同步
#       2. - 创建工作区，同步工作区内容
#       3. tbd
###############################################################
file_path="/mnt/nandflash/mount_dir/ditoo"
previous_hash=$(md5sum "$file_path" | awk '{print $1}')
echo "start..."

while true; do
    if [ -n "$previous_hash" ]; then
        current_hash=$(md5sum "$file_path" | awk '{print $1}')

        # 比较两个哈希值是否相等
        if [ "$previous_hash" != "$current_hash" ]; then
            
            # use file
            \cp $file_path /mnt/nandflash/DT4/
            echo "File has been modified. Performing action..."
            previous_hash="$current_hash"
        else
            # echo "File has not been modified."
            echo >> /dev/null
        fi
    else
        previous_hash=$(md5sum "$file_path" | awk '{print $1}')
        echo "Initial hash value saved."
    fi

    usleep 100000
done
