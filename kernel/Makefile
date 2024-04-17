include ../Makefile.config

KEYMAP_SRC=$(wildcard keymap.*.c)
BOOTBLOCK=bootblock.S
SRC=$(filter-out $(KEYMAP_SRC), $(wildcard *.c))
ASM=$(filter-out $(BOOTBLOCK), $(wildcard *.S))
HDR=$(wildcard *.h)
OBJ=$(SRC:.c=.o) $(ASM:.S=.o)

basekernel.img: bootblock kernel
	cat bootblock kernel /dev/zero | head -c 1474560 > basekernel.img

kernel: kernel.elf
	${OBJCOPY} -O binary $< $@

bootblock: bootblock.elf
	${OBJCOPY} -O binary $< $@

kernel.elf: ${OBJ}
	${LD} ${KERNEL_LDFLAGS} -Ttext 0x10000 ${OBJ} -o $@

bootblock.elf: bootblock.o
	${LD} ${KERNEL_LDFLAGS} -Ttext 0 $< -o $@

%.o: %.c
	${CC} ${KERNEL_CCFLAGS} -I ../include $< -o $@

%.o: %.S
	${CC} ${KERNEL_CCFLAGS} -I ../include $< -o $@

clean:
	rm -rf basekernel.img *.o *.elf kernel bootblock bootblock.o
