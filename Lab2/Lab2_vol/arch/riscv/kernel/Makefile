#在Makefile中，它被展开为已经存在的、使用空格分开的、匹配此模式的所有文件列表。一般我们可以使用“$(wildcard *.c)”来获取工作目录下的所有的.c文件列表。
ASM_SRC		= $(sort $(wildcard *.S))
C_SRC     = $(sort $(wildcard *.c))
#$(patsubst 原模式， 目标模式， 文件列表)
OBJ		    = $(patsubst %.S,%.o,$(ASM_SRC)) $(patsubst %.c,%.o,$(C_SRC))

all: $(OBJ) 

%.o:%.S
	${CC}  ${CFLAG}  -c $<
%.o:%.c
	${CC}  ${CFLAG}  -c $<

clean:
	$(shell rm *.o 2>/dev/null)
