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

show_usage() {
    echo "Usage: $0 opt"
    echo "opt=<clean|-h|--help>"
    exit 1
}

execute_command() {
    case "$1" in
        -h | --help)
            show_usage
            ;;
        clean)
            echo "clean ${code_file}"
            init_code_file
            rm -f test_app
            rm -f $buff_file
            exit 1
            ;;
        *)
            ;;
    esac
}


# 关联数组用于存储颜色值和对应的 ANSI 转义序列
declare -A colors=(
    [RED]='\033[0;31m'
    [GREEN]='\033[0;32m'
    [YELLOW]='\033[0;33m'
    [BLUE]='\033[0;34m'
    [MAGENTA]='\033[0;35m'
    [CYAN]='\033[0;36m'
    [WHITE]='\033[0;37m'
)

# 封装函数，接收颜色和消息参数，输出带有指定颜色的文本
# 参数: $1 颜色，$2 输出内容
color_echo() {
    local color="$1"
    local message="$2"
    NC='\033[0m'            # 重置颜色代码

    if [ -z "${colors[$color]}" ]; then
        echo "$message"     # 如果颜色不存在，默认输出文本内容
    else
        echo -e "${colors[$color]}${message}${NC}"  # 输出指定颜色的文本内容
    fi
}


#           使用介绍            #
print_func_usage() {
    # get_last_word()
    last_word=$(get_last_word ',' 'apple,banana,orange') 
    echo get_last_word: $last_word

    # 使用函数输出不同颜色的文本
    color_echo "RED" "Error message"
    color_echo "GREEN" "Success message"
    color_echo "YELLOW" "Warning message"
    color_echo "BLUE" "Info message"
    color_echo "MAGENTA" "Magenta message"
    color_echo "CYAN" "Cyan message"
    color_echo "WHITE" "White message"

}

# print_func_usage