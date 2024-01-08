#			-readme-			#
# 功能：自动捕获指定路径下的工程文件，再进行编译&链接
# 变量：
#	include：头文件路径
#	source： 源文件路径
TARGET	:= test_s7
CC		:= gcc
CXX		:= g++
DEBUG	= -g -Wall -O2 
CFLAGS	= -c $(DEBUG) $(INCS_P)				
LDFLAGS	= -lpthread -lm $(lib) $(LIB_P)  

# 			指定项目头文件和源文件路径		 #
include := ./ #../inc
source  := ./ #../src
#			指定不编译的文件				#
src_out := 
#			指定库文件和路径				#
lib		:= -lsnap7
LIB_P	:= -L.

## 添加链接器-I文件路径
incs := $(shell find ${include} -name "*.h")    
incs := $(dir ${incs} hacks)                    
incs := $(sort $(incs))                         
INCS_P := $(foreach n,$(incs),-I$(n))   

## 汇编目录下.c源文件
srcs := $(shell find ${source} -name "*.c")     
out := $(shell ls ${src_out})           
srcs := $(filter-out $(out),$(srcs))    
OBJS := $(patsubst %.c, %.o, $(srcs))			

cpps := $(shell find ${source} -name "*.cpp")	
out := $(shell ls ${src_out})           
cpps := $(filter-out $(out),$(cpps))    
OBJ_CPP := $(patsubst %.cpp, %.o, $(cpps))	


$(TARGET): $(OBJS) $(OBJ_CPP)
	$(CXX) $^ -o $@ $(LDFLAGS)  

$(OBJ_CPP):%.o : %.cpp			#生成*.o文件
	$(CXX) $(CFLAGS) $< -o $@

$(OBJS):%.o : %.c
	$(CC) $(CFLAGS) $< -o $@




.PHONY:clean clean_obj
clean:
	rm -f $(TARGET) $(OBJS) $(OBJ_CPP)
clean_obj:
	rm -f $(OBJS)

	