# $(wildcard *.c) 可以获取当前目录下的所有的 .c 文件列表
ASM_SRC   = $(sort $(wildcard *.S))
C_SRC     = $(sort $(wildcard *.c))

# $(patsubst 原模式, 目标模式, 文件列表)
OBJ		  = $(patsubst %.S,%.o,$(ASM_SRC)) $(patsubst %.c,%.o,$(C_SRC))

# 你可以通过 @echo 输出这些变量, 来查看他们起到了什么样的作用

all: $(OBJ)

%.o:%.S
	$(CC) $(CFLAG) -c $<
%.o:%.c
	$(CC) $(CFLAG) -c $<

clean:
	$(shell rm *.o 2>/dev/null)
