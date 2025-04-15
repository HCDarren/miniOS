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
INCLUDE+= -I${SRC}/application
INCLUDE+= -I${SRC}

${BUILD}/bootloader/%.bin: ${SRC}/bootloader/%.asm
	$(shell mkdir -p ${dir $@})
	nasm -f bin $< -o $@ -l ${BUILD}/bootloader/$*.lst

${BUILD}/%.o: ${SRC}/%.asm
	$(shell mkdir -p ${dir $@})
	nasm -f elf32 ${DEBUG} $< -o $@ -l ${BUILD}/$*.lst

${BUILD}/%.o: ${SRC}/%.c
	$(shell mkdir -p ${dir $@})
	x86_64-elf-gcc ${CFLAGS} ${DEBUG} ${INCLUDE} -c $< -o $@

# 编译 libc 库
$(BUILD)/lib/libc.o: $(BUILD)/lib/dirent.o \
	$(BUILD)/lib/execve.o \
	$(BUILD)/lib/exit.o \
	$(BUILD)/lib/printf.o \
	$(BUILD)/lib/sleep.o \
	$(BUILD)/lib/stdio.o \
	$(BUILD)/lib/syscall.o \
	$(BUILD)/lib/unistd.o \

	x86_64-elf-ld -m elf_i386 -r $^ -o $@

$(BUILD)/%.elf: $(BUILD)/%/main.o \
	$(BUILD)/application/start.o \
	$(BUILD)/lib/libc.o \

	x86_64-elf-ld -m elf_i386 -static $^ -o $@ -Ttext 0x40000000

${BUILD}/kernel/kernel.bin: ${BUILD}/kernel/kernel.o \
	${BUILD}/kernel/drivers/io.o \
	${BUILD}/kernel/drivers/console.o \
	${BUILD}/kernel/drivers/keyborad.o \
	${BUILD}/kernel/drivers/device.o \
	${BUILD}/kernel/drivers/disk.o \
	${BUILD}/kernel/drivers/tty.o \
	${BUILD}/kernel/printk.o \
	${BUILD}/kernel/interrupt.o \
	${BUILD}/kernel/bridge.o \
	${BUILD}/kernel/time.o \
	${BUILD}/kernel/base/assert.o \
	${BUILD}/kernel/task/list.o \
	${BUILD}/kernel/task/task.o \
	${BUILD}/kernel/task/idle_task.o \
	${BUILD}/kernel/task/init_task.o \
	${BUILD}/kernel/gdt.o \
	${BUILD}/kernel/base/string.o \
	${BUILD}/kernel/memory/memory_manager.o \
	${BUILD}/kernel/memory/bitmap.o \
	${BUILD}/kernel/base/asm_instruct.o \
	${BUILD}/kernel/ipc/mutex.o \
	${BUILD}/kernel/kernel_call.o \
	${BUILD}/lib/printf.o \
	${BUILD}/lib/unistd.o \
	${BUILD}/lib/sleep.o \
	${BUILD}/lib/exit.o \
	${BUILD}/lib/execve.o \
	${BUILD}/lib/stdio.o \
	${BUILD}/lib/syscall.o \
	${BUILD}/kernel/fs/fs.o \
	${BUILD}/kernel/fs/file.o \
	${BUILD}/kernel/fs/fat16.o \
	${BUILD}/kernel/elf/elf.o \

	$(shell mkdir -p ${dir $@})
	x86_64-elf-ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRYPOINT)

${BUILD}/kernel/system.bin: ${BUILD}/kernel/kernel.bin
	$(shell mkdir -p ${dir $@})
	x86_64-elf-objcopy -O binary $< $@

${BUILD}/kernel/system.map: ${BUILD}/kernel/kernel.bin
	nm $< | sort > $@

slave.img:
	no | bximage -q -hd=32 -func=create -sectsize=512 -imgmode=flat $@

application.app: $(BUILD)/application/shell.elf\
	$(BUILD)/application/cat.elf\

	# 挂载 slave 从盘设备
	hdiutil attach slave.img -mountpoint /Volumes/mydisk
	# 拷贝 elf 文件到根目录
	cp -r $(BUILD)/application/shell.elf /Volumes/mydisk
	cp -r $(BUILD)/application/cat.elf /Volumes/mydisk
	# 卸载设备
	hdiutil detach /Volumes/mydisk


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

QEMU_DISK:= -drive file=master.img,if=ide,index=0,media=disk,format=raw
QEMU_DISK+= -drive file=slave.img,if=ide,index=1,media=disk,format=raw

.PHONY: qemu
qemu: slave.img application.app master.img 
	qemu-system-i386 -s -S -m 32M ${QEMU_DISK} -boot c
