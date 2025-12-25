# DMI Decode Extension Makefile
# 在不修改原 dmidecode 文件的情况下，提供扩展功能

# 指向原有 dmidecode 目录
DMIDECODE_DIR = ../dmidecode-3.2

CC = gcc
CFLAGS = -Wall -Wextra -I$(DMIDECODE_DIR) -I.
AR = ar

CFLAGS += -g

# 目标文件
LIB = libdmi_ext.a
OBJS = dmi_decode_ext.o

# 原有 dmidecode 的 .o 文件（需要链接）
UTIL_OBJ = $(DMIDECODE_DIR)/util.o
DMIOEM_OBJ = $(DMIDECODE_DIR)/dmioem.o

.PHONY: all clean example deps

all: $(LIB) example

$(LIB): $(OBJS)
	$(AR) rcs $@ $^

dmi_decode_ext.o: dmi_decode_ext.c dmi_decode_ext.h dmi_paths.h
	$(CC) $(CFLAGS) -c $< -o $@

# 确保原有 .o 文件已编译
deps:
	$(MAKE) -C $(DMIDECODE_DIR) util.o dmioem.o

# 编译示例程序（直接链接 .o 文件，不使用静态库）
example: deps example.o dmi_decode_ext.o
	$(CC) $(CFLAGS) -o example example.o dmi_decode_ext.o $(UTIL_OBJ) $(DMIOEM_OBJ)

example.o: example.c dmi_decode_ext.h dmi_paths.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(LIB) example
