# Makefile for Building Project
# Features: Automatically captures and compiles/link project files in the specified paths
# COMPILER_*: make external variable
# variables:
INCLUDE := ./ ../lib_modbus_src
SOURCE  := ./ ../lib_modbus_src
LIBRARY  = -L$(COMPILER_LIB_PTAH)
LIB_LINK = -lpthread -lm -lrt -ldl # -lmodbus -lmosquitto -lmqtt -lsqlite3 -lzmq -llua #-lfwlib32 #-lsnap7
SRC_OUT  = ./modbus_slave_epoll.c # ./1.rubbish/*
OBJ_DIR  = .obj

# zmake option
COMPILER_LIB_PTAH ?= ./
COMPILER_PREFIX   ?= # arm-openwrt-linux-
COMPILER_CFLAGS   ?= # -Dx86

# config
TARGET  := modbus_slave
CC      := $(COMPILER_PREFIX)gcc
CXX     := $(COMPILER_PREFIX)g++
STRIP	:= $(COMPILER_PREFIX)strip
DEBUG   := -g -Wall -O2 #-fno-stack-protector #-fsanitize=address
CFLAGS   = -c $(INCPATH) $(DEBUG) $(COMPILER_CFLAGS)
LDFLAGS := $(LIB_LINK) $(LIBRARY)

# build
INCS 	:= $(shell find ${INCLUDE} -name "*.h")
INCS 	:= $(dir ${INCS} hacks)
INCS 	:= $(sort $(INCS))
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
#	$(STRIP) $(TARGET)

# Compilation rules
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) -c $< -o $@


.PHONY: clean debug
clean:
	rm -f ${TARGET}
	rm -rf ${OBJ_DIR}
debug:
	@echo "CC=${CC}  CXX=${CXX}"
	@echo "COMPILER_LIB_PTAH=${COMPILER_LIB_PTAH}  COMPILER_PREFIX=${COMPILER_PREFIX} CFLAGS=${CFLAGS}"