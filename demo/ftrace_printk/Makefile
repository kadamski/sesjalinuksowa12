obj-m += ftrace_printk.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

all:
		make -C $(KDIR) M=$(PWD) modules

install: all
		make -C $(KDIR) M=$(PWD) modules_install

test: ftrace_printk.ko
	sudo sh -c 'echo BEFORE > /sys/kernel/debug/tracing/trace_marker'
	sudo insmod ftrace_printk.ko dyndbg=+p
	sudo sh -c 'echo AFTER > /sys/kernel/debug/tracing/trace_marker'
	sudo rmmod ftrace_printk

clean:
		make -C $(KDIR) M=$(PWD) clean

