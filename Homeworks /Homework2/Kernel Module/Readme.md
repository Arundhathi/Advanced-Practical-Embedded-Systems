Readme for Adding Self Defined and Individually Loadable Kernel Modules 

Loadable Kernel Module Description: 
Name: timer_manipulation_module 
Function: Timer based Kernel Module that resets Kernel Timer every 500 ms and logs it in the kernel logger 

Process: 
1. Navigate to untarred linux kernel 
2. Make a new folder that will contain source files, Makefile and other build byproducts of the module 
3. Create a .c source file to code the module in 
4. Include GPL License name as not doing so creates a tainted module 
5. Add other necessary data such as name of author, version number and description. 
6. All of the above can be accessed usng predefined macro functions that take in strings as arguments. 
7. Each module needs to have the following two functions: 
	a. static int name_of_module_init(void)
	b. static void name_of_module_exit(void)
   These modules entail the initialisation and exit functions that are invoked when module is added and removed respectively. 
8. Create a Makefile for the same in the same format  as the included Makefile 
9. Performing a make will create other dependencies and a .ko extension file. This means that the module has been compiled properly and is ready to be installed. 

INSTALLING THE MODULE 
1. sudo insmod name_of_module.ko 
 

CHECKING IF MODULE IS WORKING 
1. dmesg | tail -1
2. dmesg 

CHECKING IF MODULE IS DISPLAYED IN LIST OF WORKING MODULES 
1. lsmod | grep name_of_module 
2. modinfo name_of_module 

REMOVING MODULE
1. sudo rmmod name_of_module (doesnt include .ko extension)

Referense Sources: 
https://www.ibm.com/developerworks/linux/library/l-timers-list/?ca=drs-
https://wiki.ubuntu.com/Booting
http://tldp.org/LDP/lkmpg/2.6/html/index.html
https://github.com/yuanhui-yang/Linux-Kernel-Development/blob/master/Linux%20Kernel%20Development%20-%203rd%20Edition.pdf
http://www.thegeekstuff.com/2013/07/write-linux-kernel-module/

