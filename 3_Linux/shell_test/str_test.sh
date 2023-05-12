#!/bin/bash

data_file=mysql.txt


# grep 正则表达式
grep_E() {
	
	# 
	tules='[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}'          # ipv4格式: xxx.xxx.xxx.xxx
	tunl_ip=`grep -Eo $tules $data_file`                            # -E：正则表达式        -o：只打印匹配内容
	line_IP=`grep -Eon $tules $data_file | cut -f1 -d:`             # -n：打印行号
	echo $tunl_ip
	echo $line_IP
}

sed_n() {

	# 字符串截取
	str="esp mode=tunnel spi=29571(0x00007383) reqid=0(0x00000000)";echo "old "$str;str=${str#*spi=};str=${str%%(*)*};echo "new "$str


	echo "# get指定行的内容"
	sed -n "12p" mysql.txt  
	echo "# 查找字符串所在行号"
	sed -n "/spi/=" mysql.txt 

}

awk_test() {

	echo "# 查找字符串所在行号"
	awk '/192.168.100.252 192.168.100.253/ {print FNR}' mysql.txt

}

cut_test() {

	# -b ：以字节为单位进行分割。这些字节位置将忽略多字节字符边界，除非也指定了 -n 标志。
	# -c ：以字符为单位进行分割。
	# -d ：自定义分隔符，默认为制表符。
	# -f ：与-d一起使用，指定显示哪个区域。
	# -n ：取消分割多字节字符。仅和 -b 标志一起使用。如果字符的最后一个字节落在由 -b 标志的 List 参数指示的范围之内，该字符将被写出；否则，该字符将被排除
	grep -Eon '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' mysql.txt
	
	grep -Eon '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' mysql.txt | cut -f1 -d:
	grep -Eon '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' mysql.txt | cut -f2 -d:
	
	grep -Eon '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' mysql.txt | cut -f1 -d.
	grep -Eon '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' mysql.txt | cut -f3 -d.

}

function usr_main() {
	echo "    shell脚本字符串操作   "

	cut_test
	# awk_test
	# sed_n
	# grep_E

	exit 0
}


usr_main
exit 0






if_test() {
	read -p "请输入今天的日期，格式为”xxxx-xx-xx：" time
	##判断参数
	if [ -z ${time} ];then
		echo "请您重新运行脚本，并且输入时间，格式为xxxx-xx-xx"
		exit 1
	fi

	if [ -n ${time} ];then
		echo ${time} | egrep -q [[:alpha:]]
		if [ $? -eq 0 ];then
			echo "您输入的信息中包含英文字母，请您重新运行脚本，并且输入纯数字格式的日期"
			exit 2
		else
			echo ${time} | egrep -q [[:digit:]]{4}-[[:digit:]]{2}-[[:digit:]]{2}$
			if [ $? -eq 0 ];then
				echo "输入日期格式CORRECT"
				echo $time
			else
				echo "输入日期格式FAIL"
			fi
		fi
	fi
}

