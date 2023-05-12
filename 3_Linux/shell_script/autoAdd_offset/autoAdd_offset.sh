#!/bin/bash


# 获取数据长度字段值
get_datalen() {
    line=$1
    str=$(sed -n "${line}p" $fileName)  # get指定行
    str=${str#*\"data_length\":}
    str=${str%%,*}
    datalen=$str
}

get_meteroi() {
    line=$[$1 +2]
    str=$(sed -n "${line}p" $fileName)  # get line
    str=${str#*\"meter_oi\":}
    str=${str%%,*}
    meteroi=$str
}

# 修改offset值
set_offset() {
    line=$1

    str=$(sed -n "${line}p" $fileName)  # get line
    str=${str#*\"offset\":}
    str=${str%%,*}
    #str=${str%%:*}
    # str_offset="        ${str}: ${offset},"
    if [ -z "$str" ];then
        echo "string is empty $str"
        break;
    fi

    sed -i "${line}s/${str}/${offset}/g" ${fileName}
}

# 校验offset值 是否正确
cmp_offset() {
    line=$1

    str=$(sed -n "${line}p" $fileName)  # get line
    str=${str#*\"offset\":}
    str=${str%%,*}
    #str=${str%%:*}
    # str_offset="        ${str}: ${offset},"
    if [ -z "$str" ];then
        echo "string is empty $str"
        exit 1;
    fi
    
    if [ $str -eq $offset ];then
        sta=true
        echo " $sta $str -eq $offset"
    else
        sta=false
        echo " $sta $str -eq $offset"
        exit 1;
    fi


}


offset=0		# 偏移地址
datalen=0		# 数据长度
meteroi=""

fileName=$1
#fileName=md66_communicate.json

line_offset=`grep -Eon "offset" ${fileName} | cut -f1 -d:`
line_datalen=`grep -Eon "data_length" ${fileName} | cut -f1 -d:`
arr_Oline=(${line_offset// / })
arr_Dline=(${line_datalen// / })

for(( i=0;i<${#arr_Oline[@]};i+=1)) do
	# echo point line is ${arr_Oline[i]} ${arr_Dline[i]}

    get_meteroi ${arr_Dline[i]}
    echo "meter_oi:${meteroi}" 
    get_datalen ${arr_Dline[i]}
    echo " datalen:${datalen}"
    echo " offset:$offset"

    set_offset ${arr_Oline[i]}
    cmp_offset ${arr_Oline[i]}

    offset=`expr $offset + $datalen`
    echo
done;



# sed -i 's/\r$//' autoAdd_offset.sh; ./autoAdd_offset.sh


