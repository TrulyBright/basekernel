include ../Makefile.config

SRC=$(wildcard *.c)
ASM=$(wildcard *.S)
OBJ=$(SRC:.c=.o) $(ASM:.S=.o)

all: user-start.o baselib.a

%.o: %.c
	${CC} ${KERNEL_CCFLAGS} -I ../include $< -o $@

baselib.a: ${OBJ}
	${AR} rv $@ ${OBJ}

clean:
	rm -rf *.a *.o
