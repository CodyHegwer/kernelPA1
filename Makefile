obj-m:= codydevice.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

dev:
	mknod /dev/simp_char_dev c 240 0
	chmod 777 /dev/simp_char_dev
