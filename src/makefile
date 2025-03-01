# 中间产物生产目录
BUILD:=./build

${BUILD}/%.bin: %.asm
	$(shell mkdir -p ${BUILD})
	nasm -f bin $< -o $@ -l ${BUILD}/$*.lst

master.img: ${BUILD}/boot.bin ${BUILD}/loader.bin
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat master.img
	dd if=${BUILD}/boot.bin of=master.img bs=512 count=1 conv=notrunc
	dd if=$(BUILD)/loader.bin of=master.img bs=512 count=4  seek=2 conv=notrunc

.PHONY: clean
clean:
	rm -rf ${BUILD}

.PHONY: bochs
bochs: master.img
	bochs -q

.PHONY: qemu
qemu: master.img
	qemu-system-x86_64 -hda master.img