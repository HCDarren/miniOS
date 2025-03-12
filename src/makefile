# 中间产物生产目录
BUILD:=./.build
SRC:=.
ENTRYPOINT:=0x10000

CFLAGS:= -m32
#去掉GCC的版本信息
CFLAGS+= -Qn
# debug 编译环境
CFLAGS+= -DDEBUG
CFLAGS+= -fno-builtin
CFLAGS+= -nostdinc
CFLAGS+= -fno-pic
CFLAGS+= -fno-pie
CFLAGS+= -fno-stack-protector
CFLAGS:=$(strip ${CFLAGS})

DEBUG:= -g
INCLUDE:= -I${SRC}/kernel/include

${BUILD}/bootloader/%.bin: ${SRC}/bootloader/%.asm
	$(shell mkdir -p ${dir $@})
	nasm -f bin $< -o $@ -l ${BUILD}/bootloader/$*.lst

${BUILD}/kernel/%.o: ${SRC}/kernel/%.asm
	$(shell mkdir -p ${dir $@})
	nasm -f elf32 ${DEBUG} $< -o $@ -l ${BUILD}/kernel/$*.lst

${BUILD}/kernel/%.o: ${SRC}/kernel/%.c
	$(shell mkdir -p ${dir $@})
	x86_64-elf-gcc ${CFLAGS} ${DEBUG} ${INCLUDE} -c $< -o $@

${BUILD}/kernel/kernel.bin: ${BUILD}/kernel/kernel.o \
	${BUILD}/kernel/drivers/io.o \
	${BUILD}/kernel/drivers/console.o \
	${BUILD}/kernel/printk.o \
	${BUILD}/kernel/interrupt.o \
	${BUILD}/kernel/bridge.o \
	${BUILD}/kernel/time.o \
	${BUILD}/kernel/base/assert.o \
	${BUILD}/kernel/scheduler.o \
	${BUILD}/kernel/gdt.o \
	${BUILD}/kernel/base/string.o \
	${BUILD}/kernel/memory/memory_manager.o

	$(shell mkdir -p ${dir $@})
	x86_64-elf-ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRYPOINT)

${BUILD}/kernel/system.bin: ${BUILD}/kernel/kernel.bin
	$(shell mkdir -p ${dir $@})
	x86_64-elf-objcopy -O binary $< $@

${BUILD}/kernel/system.map: ${BUILD}/kernel/kernel.bin
	nm $< | sort > $@

master.img: ${BUILD}/bootloader/boot.bin ${BUILD}/bootloader/loader.bin \
	${BUILD}/kernel/system.bin \
	${BUILD}/kernel/system.map \

	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
	dd if=${BUILD}/bootloader/boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD)/bootloader/loader.bin of=$@ bs=512 count=2 seek=2 conv=notrunc
	dd if=$(BUILD)/kernel/system.bin of=$@ bs=512 count=100 seek=4 conv=notrunc

.PHONY: clean
clean:
	rm -rf ${BUILD}

.PHONY: bochs
bochs: master.img
	bochs -q -debugger

.PHONY: qemu
qemu: master.img
	qemu-system-i386 -s -S -m 32M -boot c -hda $<

