#!/bin/bash
get_enum_values() {
    enum_str=`sed -n '/enum /,/}/p' code_content.c`
    # enum_str=`sed -n '/ENUM_STA/,/ENUM_END/p' code_content.c | sed '1d;$d' `
    enum_str=`sed -n '/Insert user code/,/Insert end/p' code_content.c | sed '1d;$d' `
    # echo $enum_str

    # 获取 枚举名称
    enum_name=$(echo "$enum_str" | sed -n 's/.*}\(.*\);.*/\1/p')
    enum_name=$(echo "$enum_name" | tr -d '[:space:]')

    # 获取枚举值
    enum_values=$(echo "$enum_str" | sed -n '/{/,/}/p' | sed '1d;$d')      # 截取括号内的字符串
    enum_values=$(echo "$enum_values" | sed -e '1s/.*{//; $s/}.*//; s/,//g')   # 移除 enum 定义行并提取括号内字符串

    # return
    echo enum_name:     $enum_name
    echo enum_values:   $enum_values
}

# 获取字符串中，最后一个单词
# $1 分隔符  $2 要处理的字符串
get_last_word() {
    delimiter=$1
    string=$2

    if [ -z "$delimiter" ] || [ -z "$string" ]; then
        echo "Usage: get_last_word <delimiter> <string>"
        return 1
    fi

    IFS="$delimiter"
    read -ra words <<< "$string"
    
    last_word=${words[0]}
    for fruit in "${words[@]}"; do
        last_word="$fruit"
    done

    echo "$last_word"
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

# 
print_func_id() {
   filename=$1

    if [ -z "$filename" ]; then
        echo "Usage: write_c_function_to_file <filename>"
        return 1
    fi

    cat << EOF > "$filename"
${enum_name} get_${enum_name}_id(const char *name)
{
    int i = 0, cnt = sizeof(${enum_name}_values) / sizeof(${enum_name}_values[0]);

    for (i = 0; i < cnt; ++i) {
        if (0 == strcasecmp(name, ${enum_name}_values[i].name)) {
            return (${enum_name})${enum_name}_values[i].id;
        }
    }
    return 0;
}

EOF
}

print_func_name() {
   filename=$1

    if [ -z "$filename" ]; then
        echo "Usage: write_c_function_to_file <filename>"
        return 1
    fi

    cat << EOF > "$filename"
const char *get_${enum_name}_name(${enum_name} type)
{
    int i = 0, cnt = sizeof(${enum_name}_values) / sizeof(${enum_name}_values[0]);

    for (i = 0; i < cnt; ++i) {
        if (type == ${enum_name}_values[i].id) {
            return ${enum_name}_values[i].name;
        }
    }
    return "";
}

EOF
}

code_file=code_content.c
buff_file=code_buffer.c
enum_name=""
enum_values=""


# 寄存枚举信息
echo "start generate code..."
get_enum_values


# Init: 格式化文件内容
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

# 生成: 枚举值的字符串映射表
echo -n > $buff_file
echo enum_map_t ${enum_name}_values[] = { >> $buff_file

arr_values=(${enum_values// /});   # echo arr:${arr_values[@]}
for((i=0; i<${#arr_values[@]}; i+=1)) do
    echo ${arr_values[i]}
    last_word=$(get_last_word "_" ${arr_values[i]})
    echo -e "\t{${arr_values[i]}, \t\"${last_word}\"}," >> $buff_file           # 插入对应字符串
done;
echo "};" >> $buff_file
insert_into_code $buff_file $code_file

# 生成: 
echo -n > $buff_file
print_func_id $buff_file
insert_into_code $buff_file $code_file

# 生成: 
echo -n > $buff_file
print_func_name $buff_file
insert_into_code $buff_file $code_file

