# YOUR MAKEFILE CODE

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

# 所以追加 2>/dev/null 在命令末尾，表示：把错误输出到 “黑洞”。

# 1、Linux系统预留可三个文件描述符：0、1、2，意义如下：
# 0——标准输入（stdin）
# 1——标准输出（stdout）即输入到终端
# 2——标准错误（stderr）

# 2、重定向的符号有两个：> 和 >>，两者的区别是：前者是直接覆盖原有内容，即先清空文件，然后再写入内容；后者会将重定向的内容追加到现有文件的尾部。

# 3、/dev/null 是一个特殊的设备文件，这个文件接收到任何数据都会被丢弃。因此，null 这个设备通常也被称为位桶（bit bucket）或黑洞。

# 综上，2>/dev/null 的意思就是将标准错误 stderr 删掉。