# 打包子目录下所有README到当前目录
source ./3.Tool/shell_script/zh_shell_function.sh
file_name="README.md"
file_dir=./

rm -f $file_name
#find ./ -name $file_name -type f -exec cat {} + >> template.md

for file in $(find $file_dir -type f -name "$file_name" | sort -V); do
    echo FILE: $file

    file_load=$(echo "$file" | sed "s#^\./##; s#/$file_name##g")
    echo "# [$file_load]($file_load)"  >> template.md
    cat $file >> template.md
    echo >> template.md
    echo "---------------------------"  >> template.md
done

mv template.md $file_name
zecho "GRE" "OUTFILE: $file_name"



