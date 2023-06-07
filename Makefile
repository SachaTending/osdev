override CC=gcc
override LD=ld
override AS=nasm
override CPP=g++

# Define options

override CCFLAGS+=       \
    -ffreestanding       \
    -fno-stack-protector \
    -fno-stack-check     \
    -fno-lto             \
    -fno-pie             \
    -fno-pic             \
    -fpermissive         \
    -m64                 \
    -march=x86-64        \
    -mabi=sysv           \
    -mno-80387           \
    -mno-mmx             \
    -mno-sse             \
    -mno-sse2            \
    -mno-red-zone        \
    -mcmodel=kernel      \
    -MMD                 \
    -c  		         \
    -I include			 \
	-I include/cppbinds  \
	-g                   \
    -D NO_GDT            \

override LDFLAGS +=         \
    -nostdlib               \
    -static                 \
    -m elf_x86_64           \
    -z max-page-size=0x1000 \
    -T link.ld              \
    -g

ifeq ($(shell $(LD) --help 2>&1 | grep 'no-pie' >/dev/null 2>&1; echo $$?),0)
    override LDFLAGS += -no-pie
endif

override NASMFLAGS += \
    -f elf64

override QEMU = qemu-system-x86_64

override HEADER_DEPS := $(FILES:.c=.d)

KERNEL = out.kern

# Here is includes

-include targets/*.mk

# Build targets

build: iso

kern: $(FILES) imgs
	@echo LD $(KERNEL)
	@$(LD) $(FILES) font.o img.o $(LDFLAGS) -o out.kern

-include $(HEADER_DEPS)

%.o: %.c
	@echo CC $@
	@$(CC) $(CCFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.cc
	@echo C++ $@
	@$(CPP) $(CCFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.S
	@echo AS $@
	@$(CC) $(CCFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.asm
	@echo NASM $@
	@$(AS) $(NASMFLAGS) $< -o $@

run: iso
	@echo QEMU -cdrom image.iso
	@$(QEMU) -cdrom image.iso -serial stdio -m 512m -smp cores=2 -hda ide_test -device sb16,audiodev=a -audiodev sdl,id=a -device rtl8139,netdev=a -netdev user,hostfwd=udp::1200-:1200,id=a

imgs:
	@echo GEN img.o, font.o
	@cp font*.psf font.psf
	@cp img*.tga img.tga
	@objcopy -O elf64-x86-64 -B i386 -I binary img.tga img.o
	@objcopy -O elf64-x86-64 -B i386 -I binary font.psf font.o
	@rm img.tga font.psf

.PHONY: clean cleanw

clean:
	@echo RM $(FILES) out.kern image.iso
	-@rm $(FILES) out.kern image.iso -f

cleanw:
	@echo DEL $(FILES) out.kern image.iso
	-@del $(FILES) out.kern image.iso

iso: kern
	@echo CP limine/out.kern
	@echo XORRISO image.iso
	@cp out.kern limine/
	@xorriso -as mkisofs -b limine-cd.bin \
	    -no-emul-boot -boot-load-size 4 -boot-info-table \
	    --efi-boot limine-cd-efi.bin \
	    -efi-boot-part --efi-boot-image --protective-msdos-label \
	    limine -o image.iso