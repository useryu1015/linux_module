#!/bin/bash
get_insert_values() {
    insert_values=`sed -n '/Insert user code/,/Insert end/p' $code_file | sed '1d;$d' `
    if [ -z "$insert_values" ] ; then
        echo "Insert format error"
        exit 1
    fi

    # return
    echo insert_values:   $insert_values
}


# 函数：insert_into_code
# 参数: $1 源文件名, $2 目标文件名
insert_into_code() {
    source_file="$1"
    target_file="$2"

    if [ -z "$source_file" ] || [ -z "$target_file" ]; then
        echo "Usage: insert_into_code <source_file> <target_file>"
        return 1
    fi

    if [ ! -f "$source_file" ]; then
        echo "Source file does not exist."
        return 1
    fi

    if [ ! -f "$target_file" ]; then
        echo "Target file does not exist."
        return 1
    fi

    # 查找目标文件中的注释块位置
    start_line=$(grep -n 'Output code' "$target_file" | cut -d ':' -f 1)
    end_line=$(grep -n 'Output end' "$target_file" | cut -d ':' -f 1)
    sed_line=`expr $end_line - 1`

    # 如果找到注释块位置
    if [ -n "$start_line" ] && [ -n "$end_line" ]; then
        # # 提取目标文件中注释块之前的内容
        # head_content=$(head -n "$((start_line - 1))" "$target_file")

        # # 提取目标文件中注释块之后的内容
        # tail_content=$(tail -n "+$((end_line + 1))" "$target_file")

        # # 将源文件的内容插入到目标文件的注释块之间
        # {
        #     echo "$head_content"
        #     cat "$source_file"
        #     echo "$tail_content"
        # } > "$target_file.tmp" && mv "$target_file.tmp" "$target_file"

        sed -i "${sed_line}r $source_file" "$target_file"

        # echo "Content from '$source_file' inserted into '$target_file' between comment blocks."
    else
        echo "Error: Comment blocks not found in '$target_file'."
        return 1
    fi
}

init_code_file() {
    line_sta=$(grep -n 'Output code' "$code_file" | cut -d ':' -f 1)    # get first line
    line_end=$(grep -n 'Output end'  "$code_file" | cut -d ':' -f 1)
    line_sta=`expr $line_sta + 1`
    line_end=`expr $line_end - 1`
    # line_sta=$(awk '/Output code/{print NR+1; exit}' "$code_file")
    # line_end=$(awk '/Output end/{print NR-1; exit}' "$code_file")

    if [ -n "$line_sta" ] && [ -n "$line_end" ] && [ "$line_end" -gt "$line_sta" ]; then
        sed -i "${line_sta},${line_end}d" "$code_file"      # delete line xx to xx
    elif [ "$line_end" -lt "$line_sta" ]; then
        echo "Dont delete"
    else
        echo "Error find '$code_file'."
    fi
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
            echo "start generate code..."
            ;;
    esac
}


parse_json_jq() {
    insert_step="first"
    json_string="$1"

    cat << EOF > "$buff_file"
int parse_some_info(const cJSON *json)
{
    struct cJSON *next = NULL;

    if (!json) {
        return -1;
    }

    for (next = json->child; NULL != next; next = next->next) {
EOF

    keys=$(echo "$json_string" | jq -r 'keys[]')
    for key in $keys; do
        value=$(echo "$json_string" | jq ".$key")
        vtype=$(echo "$value" | jq -r 'type')
        if [[ "$value" == *"."* ]] && [[ "$vtype" == "number" ]]; then
            vtype="double"
        elif [[ "$vtype" == "number" ]];then
            vtype="int"
        fi
        printf "Key: %-20s Value: %-20s Type: %-20s\n" "$key" "$value" "$vtype"

        if [ $insert_step == "first" ];then
            echo -e "\t\tif (0 == cJSON_strcasecmp(\"${key}\", next->string)) {" >> "$buff_file"
            insert_step="else"
            continue;
        fi

        echo -e "\t\t} else if (0 == cJSON_strcasecmp(\"${key}\", next->string)) {" >> "$buff_file"
        case "$vtype" in
            string)
                echo -e "\t\t\tstrncpy(vstring, next->valuestring, sizeof(vstring)-1);" >> "$buff_file"
                ;;
            number)
                echo -e "\033[0;31mError type\033[0m"
                ;;
            double)
                    echo -e "\t\t\tvdouble = next->valuedouble;" >> "$buff_file"
                ;;
            int)
                    echo -e "\t\t\tvint = next->valueint;" >> "$buff_file"
                ;;
            boolean)
                echo -e "\t\t\tvbool = (next->valueint == cJSON_True) ? true : false;" >> "$buff_file"
                ;;
        esac

    done

    cat << EOF >> "$buff_file"
        }
    }

    return 0;
}
EOF
}


inset_data="true"
code_file=json_content.c
buff_file=code_buffer.c
enum_name=""
insert_values=""

execute_command "$1"
init_code_file          # 格式化
get_insert_values       # 寄存枚举信息

parse_json_jq "$insert_values"
insert_into_code $buff_file $code_file
