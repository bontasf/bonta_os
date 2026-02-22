.PHONY: build install run clean get_dependencies

IMAGE_CREATOR = image_creator/build/image_creator
BOOTLOADER_IMAGE = bootloader/build/BOOTX64.EFI
KERNEL_IMAGE = kernel/build/Kernel.bin

$(IMAGE_CREATOR):
	cd image_creator && $(MAKE) build

$(BOOTLOADER_IMAGE):
	cd bootloader && $(MAKE) build

$(KERNEL_IMAGE):
	cd kernel && $(MAKE) build

build: $(IMAGE_CREATOR) $(BOOTLOADER_IMAGE) $(KERNEL_IMAGE)

install: build
	rm -rf build
	mkdir build
	cp $(IMAGE_CREATOR) build/image_creator
	mkdir build/input_folder
	mkdir build/input_folder/EFI
	mkdir build/input_folder/EFI/BOOT
	cp $(BOOTLOADER_IMAGE) build/input_folder/EFI/BOOT/BOOTX64.EFI
	mkdir build/input_folder/BONTAOS
	cp $(KERNEL_IMAGE) build/input_folder/BONTAOS/KERNEL.BIN
	cd build && ./image_creator input_folder BontaOS.hdd

run: install
	qemu-system-x86_64 \
	-smp 4 \
	-cpu host \
	-drive format=raw,file=build/BontaOS.hdd \
	-bios resources/bios64.bin \
	-m 32G \
	-vga std \
	-name BontaOS \
	-machine type=q35,accel=kvm \
	-usb \
	-serial stdio \
	-serial file:Uart.log \
	-device usb-mouse \
	-rtc base=localtime \
	-nic user,model=virtio-net-pci \
	-device intel-hda -device hda-duplex

clean:
	cd image_creator && $(MAKE) clean
	cd bootloader && $(MAKE) clean
	cd kernel && $(MAKE) clean
	rm -rf build

get_dependencies:
	git clone git@github.com:bontasf/image_creator.git
	git clone git@github.com:bontasf/bootloader.git
	git clone git@github.com:bontasf/kernel.git
