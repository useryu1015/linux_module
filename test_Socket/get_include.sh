#!/bin/bash
# OLDIFS=$IFS	# 

#			-脚本说明-			#
# 功能：输出当前目录下 所有.h文件 的路径
# 作用：makefile自动添加头文件路径
# Tips：1、



incs_all=$(find ./ -name "*.h")	# 获取当前目录下，所有.h文件
#incs_file=$(find ./ -name "*.h" |sed 's#.*/##')	#
#incs_path=$(echo ${incs_all##*/})

touch include_file incs_var_f
echo $incs_all > incs_var_f

num=0
i=1
for line in $incs_all[@]	# line str
do
#echo $incs_all | awk -v t="${i}" '{print $t}' 		# awk、shell 中间变量交互
#num=$($incs_all | awk -v t="${i}" '{print $t}' )
num=$(awk -v t="${i}" '{print $t}' incs_var_f)
num=$(echo ${num%/*})
echo $num >> include_file	# 以文件方式保存	
echo $num                	# 输出过滤后的路径
i=$(($i+1))
done

rm -f incs_var_f include_file

#echo "test"
#echo $incs_all | awk '{ i = 1; while ( i <= NF ) {print $i; i++}}'  # 变量的初始值为1，若i小于可等于NF(记录中域的个数),则执行打印语句，且i增加1。直到i的值大于NF.
#echo "end"
