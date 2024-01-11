#			-readme-			#
# 功能：自动捕获指定路径下的工程文件，再进行编译&链接
# 变量：
#	include：头文件路径
#	source： 源文件路径
TARGET	:= proc_test
CC	:= gcc
CFLAGS  = -g -Wall $(INCS_P)	#1 设置编译选项：		# 在编译.c文件时	#-I:指定头文件路径 L:链接lib库 -Wl,R:动态链接lib库
LDFLAGS	= -lpthread -lm     	#2 指定链接器要链接那些库文件	# 在链接.o文件时

# 			指定项目头文件和源文件路径			#
include := ./ ../lib_modbus_src
source  := ./ ../lib_modbus_src
#			指定不编译的文件				#
src_out := ./test/* #./realtime/source/rtdb_gdw376.c ./commons/test/*.c ./config/test/*.c ./mqtt/test/*.c ./realtime/test/*.c ./modbus/test/*.c

## 添加链接器-I文件路径
incs := $(shell find ${include} -name "*.h")    # 1、获取当前目录下，所有.h文件
incs := $(dir ${incs} hacks)                    # 2、过滤：只取文件路径，丢弃
incs := $(sort $(incs))                         # 3、过滤相同的单词， 并排序
INCS_P := $(foreach n,$(incs),-I$(n))           # 4、循环追加-I

## 汇编目录下.c源文件
srcs := $(shell find ${source} -name "*.c")     # 预汇编：捕获目录下所有源文件
src_out := $(shell ls ${src_out})           	# 展开需要过滤的源文件
srcs := $(filter-out $(src_out),$(srcs))    	# 过滤源文件
OBJS := $(patsubst %.c,%.o,$(srcs))             # 汇编：替换(编译)所有%.c -> %.o

$(TARGET): $(OBJS)
	gcc $^ -o $@ $(LDFLAGS)



# 调试
.PHONY:debug debug_as debug_ld
debug:
	@echo "变量：INCS_P"
	@echo ${INCS_P}
	@echo ${}
	@echo ${srcs}
debug_as:
	@echo "汇编器as："
	@echo $(CC) { $(CFLAGS) } -c -o xx.o xx.c
debug_ld:
	@echo "链接器ld："
	@echo $(CC) xx.o -o $(TARGET) { $(LDFLAGS) }
	

.PHONY:clean clean_obj
clean:
	rm -f $(TARGET) $(OBJS)
clean_obj:
	rm -f $(OBJS)
