#!/bin/bash

data_file="mysql.txt"

echo_data(){
	echo $1
	echo $2
	ip=$1" "$2
	echo $ip
}

tules='^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}[[:space:]][0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}'
#tu_ip=`setkey -D|grep -E $tules`
tu_ip=`grep -E $tules $data_file`
array=(${tu_ip// / })
for(( i=0;i<${#array[@]};i+=2)) do
	# echo_data ${array[i]} ${array[i+1]};
	echo ${array[i]} ${array[i+1]}
done;

# ret=select src_ip,dst_ip from IPSec_STA           # get ip from mysql