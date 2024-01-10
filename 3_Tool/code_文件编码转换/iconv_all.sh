# 转换目标目录下的所有指定文件的编码
# tips： 建议拷贝文件到当前目录转换，避免异常或不符合预期

# 转码函数
function convert_encoding {
    echo $1
    iconv -f GB2312 -t UTF-8 -o iconv_template_ "$1"
    cp iconv_template_ "$1"
    rm iconv_template_
}

EX_LIST=".c .h .md"
FILE_DIR="./"

echo "target ${EX_LIST}"
# 遍历EX_LIST
for ex in $EX_LIST ; do 
    echo -------------------------------process "${ex}"
    # find $FILE_DIR -name '*'${ex} -exec iconv_tmp.sh {}  \;

    # 遍历 find file
    for file in $(find $FILE_DIR -type f -name '*'${ex}); do
        
        echo FILE: $file
        convert_encoding "$file" # 调用转码函数
    done
done
