Readme for adding User-Defined System Calls 

User Defined System Call: sortinghat 
Source File Name: sortinghat.c 
User Space File Name: userspace.c
Executable Name: godricgryffindor 
Functions: Accepts pointer to a user space buffer of 256 uint32_t sized elements. Sorts them in a descending order. Returns the sorted buffer.Arguments: int32_t* input buffer
	   int32_t* outpur buffer 
	   uint32_t length of buffer 
returns sported buffer by pointing it to the output buffer argument and returning them to user space. 

Process of Adding a user defined system call: 

1. Create a folder to house the new system call in  
2. Create a source file nameofsyscall.c 
3. create the system call using "SYSDEFINEn" 
4. in the same folder create a Makefile 
	add line "obj-y:= nameofexecutable.o"
	This makes sure that an executable of added system call is created and added to the list of available sys call executables 
5. Add the folder(directory) to the kernels main Makefile. This Makefile s found in the kernels untarred image 
	add line at (944) "core-y += kernel/ mm/ fs/ ipc/ security/ crypto/ block/foldername" 
	this helps the makefile to locate the makefile of the syscall by navigating to that directory 
6. Add the new system call to the sys call table to it gets included as a valid system call thatc can be called from kernel space 
	cd /linux_untarred/arch/x86/entry/syscalls/ 
	vim syscall_64.tbl 
	navigate to the end of the syscall lit (333) 
	add the next number 
	use the following format: next_serial_no common nameofsyscall sys_nameofsyscalll
7. Add the new system call to the system call header file 
	cd /linux_untarred/include/linux
	vim syscalls.h 
	at the end add the line: asmlinkage long sys_nameofsyscall(paramters);
	asmlinkage is a key word used to indicate all paramters of the function would be available on the stack 
8. Compile and build the kernel 
9. write a userspace command that calls the system call with the right parameters. 
10. on successful completion, syscall returns 0 
11. use "dmesg" to read the kernel log that will print information about the function of the syscall in the kernel space 



References Sources:
https://www.quora.com/What-happens-once-a-system-call-is-made-by-a-process-in-user-space
https://tssurya.wordpress.com/2014/08/19/adding-a-hello-world-system-call-to-linux-kernel-3-16-0/
https://brennan.io/2016/11/14/kernel-dev-ep3/
http://www.virtsync.com/c-error-codes-include-errno
https://www.fsl.cs.sunysb.edu/kernel-api/re257.html //copy from user
https://www.fsl.cs.sunysb.edu/kernel-api/re256.html //copy to user
https://www.kernel.org/doc/htmldocs/kernel-api/API-kmalloc.html //kamlloc 
https://www.kernel.org/doc/htmldocs/kernel-api/API-kfree.html //kfree


Syscall Sorting Algorithm Credit: http://www.sanfoundry.com/c-program-sort-array-descending-order/
Useful commands: 
1. dmesg - kernel logger 
2. sudo dmesg -c -clears the kernel log buffer 

	    



