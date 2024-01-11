#!/bin/bash

update_tunnel_rate() {
	echo $1 > /xlian/proc/ipsec/tunnel_rate
	echo $2 >> /xlian/proc/ipsec/tunnel_rate
}

# 字节转换
change_unit() {
    # rate_unit=K
    # rate_buf=$1
    # rate_int=0
    # rate_float=00
    # 转字符串 bytes->K/M
    # tips: 计算数值不能大于1073741824122112211； 早溢出了
    # https://blog.csdn.net/houzhizhen/article/details/80653347
    totalsize=$1
    if [[ "$totalsize" =~ ^[0-9]+$ ]];then      # 正则匹配0-9
        if [ 1024 -gt $totalsize ];
        then 
            size=`echo  "scale=2; a = $totalsize / 1024 ; if (length(a)==scale(a)) print 0;print a"  | bc `
            # size=`echo "scale=2; 1.2323293128 / 1.1" | bc -l`
            size="$size"K
        elif [ 1048576 -gt $totalsize ];then
            size=`echo  "scale=2; a = $totalsize / 1024 ; if (length(a)==scale(a)) print 0;print a"  | bc `
            size="$size"K
        elif [ 1073741824 -gt $totalsize ];then
            size=`echo  "scale=2; a = $totalsize / 1048576 ; if (length(a)==scale(a)) print 0;print a"  | bc `
            size="$size"M
        elif [ 1073741824 -le $totalsize ];then
            size=`echo  "scale=2; a = $totalsize / 1073741824 ; if (length(a)==scale(a)) print 0;print a"  | bc `
            size="$size"G
        else
            size="0"
        fi
    else
        size="NULL"
        fi

    cmp=`echo ${size:0:2}`          # 验证， 删除整数 00
    if [ $cmp == "00" ];then
        size=`echo ${size:1}`
        # echo $size
    fi

    tunl_current_str=$size
    # return $size
}

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
    tunl_diff=$diff

    hard=${strr#*hard: }
    hard=${hard%%(s)*} # get hard

    val=`expr $hard - $diff` # get key_life
    key_life=$val
    # return $val
}

# 从setkey查找xx实时数据，并更新到数据库
update_mysql_data() {
    line_ip=$3
    # line_ip=$(sed -n "/$1 $2/=" $data_file)
    if [ -z "$3" ] || [ -z "$1" ] || [ -z "$2" ];then
        echo "param is null" $1 $2 $3
        return 1
    fi
    
    get_key_life $line_ip
    get_current $line_ip

    # 转字符串 bytes->K/M/G
    change_unit $tunl_current           # ques: 单个key_life周期数据，  setkey会初始化。
 

    tunl_io=`show_mysql $1 $2`                      # 2022年6月28日， 如果没配置数据库，都为in
    if [ -z "$tunl_io" ];then
        echo "in"
        update_mysql_recv $2 $1 $tunl_current_str

        # echo $tunl_current
        # echo $tunl_diff
		if [ $rate_buf_bak -eq $tunl_current ];then
			recv_rate=0
		else
			rate_buf=`expr $tunl_current / $tunl_diff`      # 总字节/总时间
			recv_rate=`expr $recv_rate + $rate_buf`         # 取所有隧道数据总和。
		fi
		
		rate_buf_bak=$tunl_current
        # echo $recv_rate
    else
        echo "out"
        update_mysql_send $1 $2 1 $key_life $tunl_current_str

        # echo $tunl_current
        # echo $tunl_diff
        # echo $send_rate
        send_buf=`expr $tunl_current / $tunl_diff`      # 总字节/总时间
		send_rate=`expr $send_rate + $send_buf`         # unit: bytes/s
        # echo "NOW:" $send_buf
        # echo $send_rate
    fi

    echo ""
    echo "调试数据" recv:$recv_rate  send:$send_rate unit:$tunl_current_str

}

source /xlian/script/update_mysql_api.sh          # 使能外部链接
mkdir -p /xlian/proc/ipsec

#           全局变量           #
# src_ip=0
# dst_ip=0
# sec_proto="keep"
# establish=0
key_life=0
# recv_bytes=0
# send_bytes=0
data_file="/xlian/proc/ipsec/setkey_data"      #"mysql.txt"   
tunl_current=0
tunl_diff=0
recv_rate=0
send_rate=0
tunl_current_str=0K
tunl_current_bak=0
rate_buf_bak=0
spi_buf_bak=0


for ((;;))
do
setkey -D > $data_file

recv_rate=0
send_rate=0                     # 2022年6月28日 平均速率未初始化
tunl_current_bak=tunl_current   # 2022年6月29日 周期性判断收发速率

# grep -Eo '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' mysql.txt
tules='[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}'          # ipv4格式: xxx.xxx.xxx.xxx
tunl_ip=`grep -Eo $tules $data_file`                            # -E：正则表达式        -o：只打印匹配内容
line_IP=`grep -Eon $tules $data_file | cut -f1 -d:`             # -n：打印行号
# echo $tunl_ip
# echo $line_IP
arr_ip=(${tunl_ip// / })
arr_line=(${line_IP// / })
for(( i=0;i<${#arr_ip[@]};i+=2)) do
	# echo ${arr_ip[i]} ${arr_ip[i+1]}
    update_mysql_data ${arr_ip[i]} ${arr_ip[i+1]} ${arr_line[i]}
done;

update_tunnel_rate $recv_rate $send_rate


sleep 5
done


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
