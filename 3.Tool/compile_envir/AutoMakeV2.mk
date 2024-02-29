ifeq ($(CROSS),) 
CROSS=x86
CC=gcc
endif

AT = @
RM = rm -rvf
CP = cp -arfd
MKDIR = mkdir -p 

OPTIONS := -DUSE_DEBUG
OPTIONS += -DUSE_DUMP
OPTIONS += -DUSE_LOGGER

TARGET  := hpsdc_wdd.app

#排除的文件
EXCEPTF := 
#排除的目录
EXCEPTD := libs/

#扩展文件
EXTSRCS := 
#扩展文件夹
EXTPATH := 
#外部头文件
EXTINCS := 
#外部库路径
LIBPATH := libs/websocket/

MODULES := $(filter-out $(EXCEPTD), $(shell ls -F|grep /$))
MODULES += $(EXTPATH)

INCPATH := $(foreach dir, $(MODULES), $(dir)inc/)  
INCPATH += $(foreach dir, $(LIBPATH), $(dir)inc/)  
INCPATH += $(EXTINCS)

SRCFILE := $(wildcard $(foreach dir, $(MODULES), $(dir)src/*.c))
SRCFILE += $(EXTSRCS)

OBJFILE := ${patsubst %.c, %.o, $(filter-out $(EXCEPTF), $(SRCFILE))}

CFLAGS  := -Wall $(OPTIONS) -D_REENTRANT=1
CFLAGS  += -Werror=implicit-function-declaration
CFLAGS  += -Wno-unused-function
CFLAGS  += -Wno-unused-variable
CFLAGS  += -Wno-unused-value
CFLAGS  += -Wno-unused-but-set-variable
CFLAGS  += -Wno-pointer-sign

CFLAGS  += $(foreach dir, $(INCPATH), -I$(dir)) 
LDFLAGS += $(foreach dir, $(LIBPATH), -L$(dir)/$(CROSS)) 
LDFLAGS += $(CFLAGS) -lpthread -lm -lwebsockets
perfix=./out


all: $(TARGET) install

$(TARGET): $(OBJFILE)
	$(CC) $^ $(LDFLAGS) -o $@
#反汇编，生成汇编文件
#	$(CROSS_COMPILE)objdump $(TARGET) -d > $(TARGET).s
#去掉符号连接
#	$(CROSS_COMPILE)strip $(TARGET)
	

install:
ifneq ($(perfix),)
	$(AT) $(MKDIR) $(perfix)/bin
	$(AT) $(CP) $(TARGET) $(perfix)/bin 
	$(AT) $(MKDIR) $(perfix)/lib
	$(AT) ($(foreach i, $(LIBPATH), test ! -d $(i) || $(CP) $(i)/$(CROSS)/lib* $(perfix)/lib/;))
endif

clean:
	$(AT)$(RM) $(OBJFILE)

distclean: clean
	$(AT)$(RM) $(TARGET)
	$(AT)$(RM) $(TARGET).s
	$(AT)$(RM) $(perfix)

.PHONY : clean distclean 

