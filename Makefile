obj-m += mousehole.o
all: jerry
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm jerry
jerry:
	gcc -o jerry jerry.c
