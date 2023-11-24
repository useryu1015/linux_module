#这个文件名叫作“iconv_tmp.sh”，批处理脚本会调用这个文件
#将一个文件转码，使用了一个中转文件
 
echo $1
iconv -f GB2312 -t UTF-8 -o iconv.........tmp $1
cp iconv.........tmp $1
rm iconv.........tmp


