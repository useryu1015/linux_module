#!/bin/bash

echo "设置sink_main.json文件中devices字段的内容  copy40份"


table_name=sink_main
copy_sum=60


meter_oi=200
meter_sum=5



meter_oi=SF6数字化密度表计
new_oi=

for(( i=1; 1; i+=1)) do
    
    oi_line=$(echo `grep ${meter_oi} ${table_name}.json -no | cut -f1 -d:` | cut -f1 -d" ")
    echo $oi_line

    # obj=$(grep ${meter_oi} ${table_name}.json)
    if [ -z "$oi_line" ];then
        echo "string is empty"
        break;
    fi

    # new_oi=`expr ${meter_oi} + ${i}`
    new_oi="表计"-${i}
    sed -i "${oi_line}s/${meter_oi}/${new_oi}/g" ${table_name}.json

    # sed -i "/$}s/2001/${new_oi}/g" ${table_name}.json

    # sed  -i "s/${meter_oi}/${new_oi}/g"  ${new_file}.json



done;