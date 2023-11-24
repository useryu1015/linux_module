#转换目标目录下的所有指定文件的编码
#本文件和iconv_tmp.sh以及目标目录必须放在一起
#不建议使用参数传递目标目录，以免误操作
 
 
EX_LIST=".c .cpp .h .sh"
FILE_DIR="./Autumn_1124"

echo "target ${EX_LIST}"
# 遍历EX_LIST
for ex in $EX_LIST ; do 
    echo -------------------------------process "${ex}"
    # find $FILE_DIR -name '*'${ex} -exec iconv_tmp.sh {}  \;

    # 遍历 find file
    for file in $(find $FILE_DIR -type f -name '*'${ex}); do
        
        echo FILE: $file
        ./iconv_tmp.sh $file
    done


done
	

# for f in $(find ./Autumn_1124 -type f -name "*.${s#.}"); do
