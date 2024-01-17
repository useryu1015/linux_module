#			-readme-			#
# 功能：自动捕获指定路径下的工程文件，再进行编译&链接
# 变量：
#	include：头文件路径
#	source： 源文件路径
#	library：库文件路径
TARGET  := ditoo
CC      := arm-openwrt-linux-gnueabi-gcc
CXX     := arm-openwrt-linux-gnueabi-g++
DEBUG   = -g -Wall -O2 
CFLAGS  = -c $(DEBUG) $(INCS_P)             
LDFLAGS = -lpthread -lm $(liblink) $(library)  

# 指定项目头文件和源文件路径
include := ./ #../inc
source  := ./ #../src
# 指定不编译的文件
src_out := #./test/*
# 指定库文件和路径
library	:= -L./library
liblink	:= -lrt -ldl -lmodbus -lsnap7 -lmosquitto -lmqtt -lsqlite3 -lzmq -llua 
# 存放中间文件的目录
obj_dir := .obj

# 添加链接器-I文件路径
incs := $(shell find ${include} -name "*.h")    
incs := $(dir ${incs} hacks)                    
incs := $(sort $(incs))                         
INCS_P := $(foreach n,$(incs),-I$(n))

# 汇编目录下.c源文件
srcs := $(shell find ${source} -name "*.c")     
out := $(shell ls ${src_out})           
srcs := $(filter-out $(out),$(srcs))    
OBJS := $(patsubst %.c, $(obj_dir)/%.o, $(srcs))

# 汇编目录下.cpp源文件
cpps := $(shell find ${source} -name "*.cpp")  
out := $(shell ls ${src_out})           
cpps := $(filter-out $(out),$(cpps))    
OBJCPPS := $(patsubst %.cpp, $(obj_dir)/%.o, $(cpps))


$(TARGET): $(OBJS) $(OBJCPPS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# 编译规则
$(obj_dir)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(obj_dir)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) -c $< -o $@


.PHONY: clean clean_obj
clean:
	rm -f $(TARGET)
	rm -rf ${obj_dir}
clean_obj:
	rm -f $(OBJS)
