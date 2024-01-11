#!/bin/bash

get_doname() {
    line=$[$1 + 0]
    str=$(sed -n "${line}p" $csv_file)
    # echo $str
    str=${str%%,*}

    # echo $str    
    csv_name=$str
}

get_doaddr() {
    line=$[$1 + 0]
    str=$(sed -n "${line}p" $csv_file)
    # echo $str
    str=${str#*,}
    str=${str%%,*}

    # echo $str    
    csv_addr=$str
}

get_nsname() {
    line=$[$1 + 0]
    str=$(sed -n "${line}p" $csv_file)
    # echo $str
    # str=${str#*\"meter_oi\":}
    str=${str%%,*}

    # echo $str    
    csv_name=$str
}

# 
get_nsaddr() {
    line=$[$1 + 0]
    str=$(sed -n "${line}p" $ns_file)
    # echo $str
    str=${str#*ress: }
    # str=${str%%,*}

    # echo $str
    ns_addr=$str
}

# 将数据拷贝到cmp_ip.csv文件
cpy_data() {
    name=$csv_name
    addr=$csv_addr

    echo $csv_name,$csv_addr >> $data_file

}


# 删除指定字符
del_char() {
    str=$1
    del=$2

    # echo str:$1 del:$2
    rtn=`echo $str | sed 's/'"${del}"'//g'`

    echo "$rtn"
}


# 验证xls与nslookup是否匹配
cmp_addr() {
    
    # nslookup ${csv_name} > $ns_file
    # cat $ns_file
    # cat $ns_file | grep Address > $ns_file

    nslookup ${csv_name} | grep Address > $ns_file
    # cat $ns_file

    
    ns_line=`cat $ns_file | wc -l`      # 获取文件行数，
    # echo ns_line: $ns_line
    for(( j=1;j<=${ns_line};j+=1 )) do

        get_nsaddr $j

        echo "  find: 域名:${csv_name}  表格地址:$csv_addr  设备地址:$ns_addr"

        if [ "$csv_addr" == "$ns_addr" ];then
            echo "  success:   域名:${csv_name}  表格地址:$csv_addr  设备地址:$ns_addr"
            # cmp_statu=true

            # 拷贝成功数据到xxx
            cpy_data
        fi

        # echo
    done;
}



# 表格数据结构
# csv_file=test_ip.txt
csv_file=$1
csv_name=
csv_addr=
csv_numb=2

# nslookup数据
ns_file=buf.json
ns_name=
ns_addr=
ns_numb=2

# sys
data_file=cmp_ip.csv
cmp_statu=false



# main  程序入口
echo "通过nslookup查看域名与IP 并验证"
echo


rm -f $data_file

csv_line=`cat $csv_file | wc -l`      # 获取文件行数，
echo ${csv_line}
for(( i=1;i<=${csv_line};i+=1 )) do
    get_doname ${i}
    get_doaddr ${i}
    echo 行:$i 域名:$csv_name 地址:$csv_addr

    # 调用函数del_char 并获取函数打印值
    csv_name=`del_char $csv_name \"`
    csv_addr=`del_char $csv_addr \"`

    # cmp_statu=false
    cmp_addr

done;






