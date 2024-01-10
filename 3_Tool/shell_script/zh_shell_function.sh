# 获取字符串中的最后一个单词
#   参数: $1 分隔符，$2 要处理的字符串
#   用法: get_last_word ',' 'apple,banana,orange'
get_last_word() {
    delimiter=$1
    string=$2

    if [ -z "$delimiter" ] || [ -z "$string" ]; then
        echo "Usage: get_last_word <delimiter> <string>"
        return 1
    fi

    # 根据IFS 分割&寄存 字符串数组
    IFS="$delimiter"
    read -ra words <<< "$string"
    
    last_word=${words[0]}
    for fruit in "${words[@]}"; do
        last_word="$fruit"
    done

    echo "$last_word"
}

# 获取指定行中的 'current' 值
# 参数: $1 行号，$data_file 要处理的文件
get_current() {
    line=$(($1 + 8))  # 计算行号偏移量
    str=$(sed -n "${line}p" "$data_file")  # 获取指定行的内容
    str=${str#*current: }       # 去除文本中 'current: ' 之前的部分
    str=${str%%(bytes)*}        # 去除文本中 '(bytes)' 之后的部分
    tunl_current=$str
}

# 获取 xx时间
get_key_life() {
    line=$[$1 + 6]
    strr=$(sed -n "${line}p" $data_file) # get指定行
    diff=${strr#*diff: }
    diff=${diff%%(s)*} # get diff
    # tunl_diff=$diff

    hard=${strr#*hard: }
    hard=${hard%%(s)*} # get hard

    val=`expr $hard - $diff` # get key_life
    key_life=$val
    # return $val
}






#           使用介绍            #
print_func_usage() {
    # get_last_word()
    last_word=$(get_last_word ',' 'apple,banana,orange') 
    echo get_last_word: $last_word
    
    # 

}
# print_func_usage()