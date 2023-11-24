

data_file=mysql.txt
tules='[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}'          # ipv4格式: xxx.xxx.xxx.xxx
tunl_ip=`grep -Eo $tules $data_file`                            # -E：正则表达式        -o：只打印匹配内容
line_IP=`grep -Eon $tules $data_file | cut -f1 -d:`             # -n：打印行号
echo $tunl_ip
echo $line_IP
arr_ip=(${tunl_ip})
arr_line=(${line_IP// / })
for(( i=0;i<${#arr_ip[@]};i+=2)) do
	echo ${arr_ip[i]} ${arr_ip[i+1]}
    # update_mysql_data ${arr_ip[i]} ${arr_ip[i+1]} ${arr_line[i]}
done;