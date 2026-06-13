CC ?= clang

SRC := ../../kernel/fs/mcsfs1j/m16_mcsfs_journal.c

HOST_CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic -DMCSOS_M16_HOST_TEST
FREESTANDING_CFLAGS := -ffreestanding -fno-builtin -std=c11 -Wall -Wextra -Werror

all: host m16_mcsfs_journal.o audit

host:
	$(CC) $(HOST_CFLAGS) $(SRC) -o test_m16

m16_mcsfs_journal.o:
	$(CC) $(FREESTANDING_CFLAGS) -c $(SRC) -o m16_mcsfs_journal.o

audit: m16_mcsfs_journal.o
	nm -u m16_mcsfs_journal.o > nm_undefined.txt
	readelf -h m16_mcsfs_journal.o > readelf_header.txt
	objdump -d m16_mcsfs_journal.o > objdump_disasm.txt
	sha256sum m16_mcsfs_journal.o > sha256sum.txt

clean:
	rm -f test_m16
	rm -f m16_mcsfs_journal.o
	rm -f nm_undefined.txt
	rm -f readelf_header.txt
	rm -f objdump_disasm.txt
	rm -f sha256sum.txt

.PHONY: all host audit clean
