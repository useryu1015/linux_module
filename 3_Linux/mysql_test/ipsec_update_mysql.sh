#!/bin/bash

# 获取流量： 即setkey -D中的current值
get_current() {
    line=$[$1 + 8]
    str=$(sed -n "${line}p" $data_file)  # get指定行
    str=${str#*current: }
    str=${str%%(bytes)*}
    tunl_current=$str
}
# 获取 xx时间
get_key_life() {
    line=$[$1 + 6]
    strr=$(sed -n "${line}p" $data_file) # get指定行
    diff=${strr#*diff: }
    diff=${diff%%(s)*} # get diff

    hard=${strr#*hard: }
    hard=${hard%%(s)*} # get hard

    val=`expr $hard - $diff` # get key_life
    key_life=$val
    # return $val
}
# set tunl_spi
get_spi() {
    line=$[$1 + 1]
    str=$(sed -n "${line}p" $data_file)  # get指定行
    str=${str#*spi=}
    str=${str%%(*)*}
    tunl_spi=$str
    # return $str
}

# 从setkey查找xx实时数据，并更新到数据库
update_mysql_data() {
    line_ip=$(sed -n "/$1 $2/=" $data_file)
    if [ -z "$line_ip" ];then
        echo "str is null"
        return 1
    fi

    get_spi $line_ip
    get_key_life $line_ip
    get_current $line_ip
    echo $tunl_spi
    echo $key_life
    echo $tunl_current
    
    let "tunl_io=$tunl_spi & 7"
    # echo $tunl_io
    if [ $tunl_io == 3 ];then
        echo "in"
        update_mysql_recv $2 $1 $tunl_current
    elif [ $tunl_io == 2 ];then
        echo "out"
        update_mysql_send $1 $2 1 $key_life $tunl_current
    else
        echo "error: 异常spi值"
    fi


    # get_current $line_ip
    # send_bytes=$?
    # echo "send_bytes" $send_bytes

    # get_key_life $line_ip
    # # key_life=$?                       # 2022年6月21日： ques、$?的值需要重新使能？
    # echo "key_life" $key_life

    # line_ip=$(sed -n "/$2 $1/=" $data_file)
    # if [ -z "$line_ip" ];then 
    #     echo "str is null"
    #     return 1
    # fi
    # get_current $line_ip
    # recv_bytes=$?
    # echo "recv_bytes" $recv_bytes
    # echo 
}

source ./update_mysql.sh          # 使能外部链接
# update_mysql_send '192.168.100.253' '192.168.100.252' 0 0 'send'

# 全局变量
src_ip=
dst_ip=
sec_proto="keep"
establish=
key_life=
recv_bytes=
send_bytes=
data_file="mysql.txt"
tunl_spi=
tunl_current=

#setkey -D > mysql.txt
# 2、正则表达式 get ip from setkey
# tun_ip规则(setkey -D)：   1、必须为双数（成对出现）     2、格式遵循：x.x.x.x x.x.x.x
# tun_ip=$(grep -E '^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}[[:space:]][0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' $data_file)
# if [ -z "$tun_ip" ];then 
#     echo "str is null"
#     exit 1
# fi


tules='^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}[[:space:]][0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}'
tu_ip=`grep -E $tules $data_file`
array=(${tu_ip// / })
for(( i=0;i<${#array[@]};i+=2)) do
	# echo_data ${array[i]} ${array[i+1]};
	# echo ${array[i]} ${array[i+1]}
    update_mysql_data ${array[i]} ${array[i+1]}
done;



# stat="false"
# for ip in $tun_ip
# do
#     if [ $stat == "false" ];then
#         # echo "false" "$ip"
#         stat="true"
#         src_ip=$ip
#     else
#         # echo "true" " $ip"
#         stat="false"
#         dst_ip=$ip
#         # update tunnel
#         update_mysql $src_ip $dst_ip
#     fi
# done





# line_ip=$(sed -n '/192.168.100.251 192.168.100.252/=' mysql.txt)
# # if !null
# echo $line_ip
# if [ -n "$line_ip" ];then
#     echo "str1 is not null"
# else
# 	echo "str1 is null"
#     exit 1
# fi
# echo 
# echo 
# echo $line_ip
# get_current $line_ip
# send_bytes=$?
# echo $send_bytes

# get_key_life $line_ip
# key_life=$?

# echo $line
# echo $str

# # 输入
# echo $src_ip
# echo $dst_ip
# echo $sec_proto

# # 输出
# echo $establish
# echo $key_life
# echo $recv_bytes
# echo $send_bytes



# ### 创建 ipsec_tunnel_stat 数据表
# CREATE TABLE `ipsec_tunnel_stat` (
#   `id` int unsigned NOT NULL AUTO_INCREMENT,
#   `src_ip` varchar(255) DEFAULT NULL,
#   `dst_ip` varchar(255) DEFAULT NULL,
#   `sec_proto` varchar(255) DEFAULT NULL,
#   `establish` tinyint DEFAULT NULL,
#   `key_life` smallint DEFAULT NULL,
#   `recv_bytes` varchar(255) DEFAULT NULL,
#   `send_bytes` varchar(255) DEFAULT NULL,
#   PRIMARY KEY (`id`)
# ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

# (src_ip, dst_ip, sec_proto, establish, key_life, recv_bytes, send_bytes)
