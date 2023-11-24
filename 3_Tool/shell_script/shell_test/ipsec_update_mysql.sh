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
    line_ip=$3
    # line_ip=$(sed -n "/$1 $2/=" $data_file)
    if [ -z "$3" ] || [ -z "$1" ] || [ -z "$2" ];then
        echo "param is null" $1 $2 $3
        return 1
    fi
    
    # echo $line_ip
    # echo $1
    # echo $2
    # return

    get_spi $line_ip
    get_key_life $line_ip
    get_current $line_ip
    # echo $tunl_spi
    # echo $key_life
    # echo $tunl_current
    
    let "tunl_io=$tunl_spi & 7"     # 位操作
    if [ $tunl_io == 3 ];then
        echo "in"
        update_mysql_recv $2 $1 $tunl_current
    elif [ $tunl_io == 2 ];then
        echo "out"
        update_mysql_send $1 $2 1 $key_life $tunl_current
        show_mysql $1 $2
    else
        echo "error: 异常spi值"
    fi

}

source ./update_mysql_api.sh          # 使能外部链接

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

# setkey -D > $data_file

# grep -Eo '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' mysql.txt
tules='[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}'          # ipv4格式: xxx.xxx.xxx.xxx
tunl_ip=`grep -Eo $tules $data_file`                            # -E：正则表达式        -o：只打印匹配内容
line_IP=`grep -Eon $tules $data_file | cut -f1 -d:`             # -n：打印行号
echo $tunl_ip
echo $line_IP
arr_ip=(${tunl_ip// / })
arr_line=(${line_IP// / })
for(( i=0;i<${#arr_ip[@]};i+=2)) do
	# echo ${arr_ip[i]} ${arr_ip[i+1]}
    update_mysql_data ${arr_ip[i]} ${arr_ip[i+1]} ${arr_line[i]}
done;



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
