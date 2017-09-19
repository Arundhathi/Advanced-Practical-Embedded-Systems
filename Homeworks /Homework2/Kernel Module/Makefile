#makefile for timer manipulaton module 


obj-m += timer_manipulation_module.o

all: 
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) modules 

clean: 
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) clean	
