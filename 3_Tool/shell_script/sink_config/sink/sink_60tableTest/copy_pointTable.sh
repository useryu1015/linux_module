#!/bin/bash


table_name=md66_communicate
copy_sum=60


meter_oi=200       # 
meter_sum=5


for(( i=1;i<=${copy_sum};i+=1)) do

    ## copy table
    new_file=${table_name}${i}
    echo copied file is $new_file  
    \cp ${table_name}.json ${new_file}.json

    ## set param 
    sed  -i "s/${table_name}/${new_file}/g"  ${new_file}.json

    for(( j=0;j<${meter_sum};j+=1)) do
        oi=${meter_oi}${j}
        oi_head=`expr ${i} + 30`
        new_oi=${oi_head}0${j}

        echo oi:${oi}  new_oi:${new_oi}

        sed  -i "s/${oi}/${new_oi}/g"  ${new_file}.json
    done;



done;