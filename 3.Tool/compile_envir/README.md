## 自动化编译与打包工具
> 工具介绍：参考CMake、AutoMake和RPM制作zmake工具，用于快速搭建C/C++项目的交叉编译环境&程序安装包。
> 
> CSDN博文：[集成开发环境IDE设计](https://blog.csdn.net/ling0604/article/details/129144156)

**Usage**
1. 将 zmake 和 Makefile 拷贝到工程目录
2. make 默认编译当前目录所有c/cpp源文件，可自定义源文件路径、链接库、交叉编译、ignore等
3. 查看交叉编译选项执行命令：./zmake -h
```
[root@localhost compile_envir]# ./zmake -h
Usage: ./zmake [-h] [-c core] [-o opt]
Options:
  -h, --help      Display help message
  -c core         Set target platform architecture(e320, x100, x300, x86)
  -o opt          Set operation mode:
                    debug: build in debug mode
                    clean: clean up build artifacts
                    pack: pack the built artifacts
```