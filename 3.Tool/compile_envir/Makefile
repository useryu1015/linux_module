# Makefile for Building Project
# Features: Automatically captures and compiles/link project files in the specified paths
# COMPILER_*: make external variable
# Variables:
INCLUDE := ./
SOURCE  := ./
LIBRARY ?= -L$(COMPILER_LIB_PTAH) # -L./lib
LIB_LINK:= -lpthread -lm -lrt -ldl # -lmodbus -lmosquitto -lmqtt -lsqlite3 -lzmq -llua #-lfwlib32 #-lsnap7
SRC_OUT := # ./1.rubbish/*
OBJ_DIR := .obj

# config
TARGET  := ditoo
CC      := $(COMPILER_PREFIX)gcc
CXX     := $(COMPILER_PREFIX)g++
STRIP	:= $(COMPILER_PREFIX)strip
DEBUG   := -g -Wall -O2 #-fno-stack-protector #-fsanitize=address
CFLAGS   = -c $(INCPATH) $(DEBUG) $(COMPILER_CFLAGS)
LDFLAGS := $(LIB_LINK) $(LIBRARY)

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