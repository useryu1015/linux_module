

# 如何 交叉编译sqlite   并移植到嵌入式系统


# 网址：https://blog.csdn.net/weixin_43829959/article/details/103146643



# 命令		# 如果报错，绝对路径
	./configure CC=/opt/hisi-linux/x86-arm/aarch64-himix100-linux/bin/aarch64-himix100-linux-gcc --host=aarch64-himix100-linux --prefix=./libs_aarch
	make; make install






1、交叉编译sqllite3可以先从官网下载最新最新的源码进行编译。sqlite3下载sqlite3有两种版本的源代码，sqlite-amalgamation-3300100.zip这种是将所有的操作放到sqlite3中进行使用的。虽然官方推荐使用这种方法。但是对于嵌入式移植还是使用sqlite-autoconf-3300100.tar.gz自动配置编译成so库来使用比较方便。
2、首先使用tar xzvf sqlite-autoconf-3300100.tar.gz解压源码。
3、进入解压后的目录。
4、./configure CC=/opt/poky/1.6.1/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc --host=arm-linux --prefix= /usr/tmp/
说明：CC赋值为嵌入式开发环境所使用的交叉编译工具，–host指定软件运行环境为arm-linux， --prefix指定源码交叉编译后生成文件的路径。
5、使用make和make install安装编译好的sqlite3的动态库。
6、将在 /usr/tmp/lib 目录下生成的 libsqlite3.so libsqlite3.so.0 libsqlite3.so.0.8.6三个so文件拷贝的，嵌入式开发环境的/lib/目录 或者程序运行依赖的lib目录下即可。
将/usr/tmp/bin 目录下的 sqlite3 应用程序拷贝到嵌入式开发环境下就可以运行并创建数据库了。
在使用虚拟机进行编译完成之后，当拷贝这几个文件到windows下是，VM虚拟机不让拷贝并报错。这种方式可以将这几个库压缩成一个压缩包，直接全部拷贝出来即可。

遇到的问题
1、在将库放到嵌入式之后，运行sqlite3 test.db命令的时候，会出现“SQLite header and source version mismatch”的问题。这个问题是在你的嵌入式环境中已经有一个sqlite3，需要使用你新编译的sqlite3来替换原来的。如果不知道老版本的sqlite3放置的位置，可以使用find命令全局搜索“libsqlite3*”的库，找到之后，使用新 编译的库代替原来的库。然后全局搜索“sqlite3*”找到老版本的sqlite执行程序，用新编译的代替，然后直接运行“sqlite3 test.db”即可执行成功。
2、如果在创建表的时候，想让主键有自动增加的功能，可以将主键的类型设置成“INTEGER” ,并在后面增加“AUTOINCREMENT”,在使用INSERT INTO的时候，将主键的值设置成null，就会在库中自动增加一个值。
具体可以参考以下链接https://www.cnblogs.com/zhw511006/archive/2010/09/08/1821596.html
————————————————
版权声明：本文为CSDN博主「刘利强」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/weixin_43829959/article/details/103146643








