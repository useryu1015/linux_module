# Features: Automatically captures and compiles/link project files in the specified paths
INCLUDE  := ./
SOURCE   := ./
LIBRARY  := -L./library
LIB_LINK := -lpthread -lm 
OBJ_DIR  := .obj
SRC_OUT	 := ./test/*

TARGET  := ditoo
CC      := arm-openwrt-linux-gnueabi-gcc
CXX     := arm-openwrt-linux-gnueabi-g++
DEBUG   = -g -Wall -O2
CFLAGS  = -c $(DEBUG) $(INCPATH)
LDFLAGS = $(LIB_LINK) $(LIBRARY)

INCS	:= $(shell find ${INCLUDE} -name "*.h")    
INCS	:= $(dir ${INCS} hacks)                    
INCS	:= $(sort $(INCS))                         
INCPATH := $(foreach n,$(INCS),-I$(n))   

SRCS	:= $(shell find ${SOURCE} -name "*.c")     
OUTS	:= $(shell ls ${SRC_OUT})           
SRCS	:= $(filter-out $(OUTS),$(SRCS))    
COBJS	:= $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

CPPS    := $(shell find ${SOURCE} -name "*.cpp")  
OUTS	:= $(shell ls ${SRC_OUT})           
CPPS    := $(filter-out $(OUTS),$(CPPS))    
CPPOBJS := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(CPPS))


$(TARGET): $(COBJS) $(CPPOBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) -c $< -o $@


.PHONY: clean clean_obj
clean:
	rm -f ${TARGET}
	rm -rf ${OBJ_DIR}
clean_obj:
	rm -rf ${OBJ_DIR}
