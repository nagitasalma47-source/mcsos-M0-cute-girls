.PHONY: meta check smoke qemu-version tree clean

meta:
	bash tools/check_env.sh

check:
	bash tools/check_env.sh
	shellcheck tools/check_env.sh

smoke:
	mkdir -p build/smoke
	clang --target=x86_64-unknown-none -ffreestanding -fno-stack-protector -fno-pic -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Wall -Wextra -Werror -std=c17 -c smoke/freestanding.c -o build/smoke/freestanding.o
	readelf -h build/smoke/freestanding.o | tee build/smoke/readelf-header.txt
	objdump -drwC build/smoke/freestanding.o | tee build/smoke/objdump.txt
	file build/smoke/freestanding.o | tee build/smoke/file.txt

qemu-version:
	qemu-system-x86_64 --version
	echo "QEMU exists. M0 does not boot a kernel image."

tree:
	tree -a -L 3

clean:
	rm -rf build/smoke
