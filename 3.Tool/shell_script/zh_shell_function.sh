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

show_cmd_usage() {
    echo "Usage: $0 opt"
    echo "opt=<clean|-h|--help>"
    exit 0
}

execute_command() {
    case "$1" in
        -h | --help)
            show_cmd_usage
            ;;
        clean)
            echo "clean ${code_file}"
            init_code_file
            rm -f test_app
            rm -f $buff_file
            exit 0
            ;;
        *)
            ;;
    esac
}

show_usage()
{
    echo "Usage: $0 [-h] [-c core] [-o opt]"
    echo "Options:"
    echo "  -h, --help      Display help message"
    echo "  -d, --demo      Display help message"
    echo "  -c core         core: target platform architecture"
    echo "  -o opt          opt: [debug | clean | pack]"

    exit 0
}


process_options()
{
        # 解析命令行参数
        while getopts ":dhc:o:-:" opt; do
        case $opt in
            h)
                show_usage                
                ;;
            d)
                print_func_usage           
                ;;                
            c)
                # 处理 -c 选项的参数
                CORE_APP="$OPTARG"
                ;;
            o)
                # 处理 -o 选项的参数
                OTHER_OPER="$OPTARG"
                ;;
            -)
                case "${OPTARG}" in
                    help)
                        # 处理 --help 选项
                        show_usage
                    ;;
                    demo)
                        # 处理 --help 选项
                        print_func_usage                        
                    ;;
                    *)
                        # 其他选项
                    ;;
                esac
                ;;
            \?)
                # 处理无效选项
                echo "Invalid option: -$OPTARG" >&2
                exit 1
                ;;
            :)
                # 处理缺少参数的选项
                echo "Option -$OPTARG requires an argument." >&2
                exit 1
                ;;
        esac
    done

}

zecho() {
    local color_code
    case "$1" in
        "RED") color_code="31";;    # 红色
        "BLU") color_code="34";;    # 蓝色
        "GRE") color_code="32";;    # 绿色
        "YEL") color_code="33";;    # 黄色
        "CYA") color_code="36";;    # 青色
        "PUR") color_code="35";;    # 紫色
        "WHI") color_code="37";;    # 白色
        *) color_code="0";;  # 默认为 0，即无颜色
    esac
    echo -e "\033[0;${color_code}m$2\033[0m"
}

zecho_red(){
    zecho "RED" "$1" 
}

zecho_blue() {
    zecho "BLU" "$1"
}

zecho_green() {
    zecho "GRE" "$1"
}


#           使用介绍            #
print_func_usage() {
    # get_last_word()   特殊字符截取
    last_word=$(get_last_word ',' 'apple,banana,orange') 
    echo get_last_word: $last_word

    # zecho()   使用函数输出不同颜色的文本
    zecho "RED" "This is red text"
    zecho "BLU" "This is blue text"
    zecho "GRE" "This is green text"
    zecho "YEL" "This is yellow text"
    zecho "CYA" "This is cyan text"
    zecho "PUR" "This is purple text"
    zecho "WHI" "This is white text"
}

process_options "$@"zh_shell_function.sh