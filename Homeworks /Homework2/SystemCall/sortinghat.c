/* Kernel Sys Call Source File 
Author: Arundhathi Swami
Subject:Advanced Practical Embeddded Systems 
Date Started: 16 Sept 2017 
Date Last Modified: 18 Sept 2017 
Function: This System calls accepts an array of 256 uit32_T sized elements and sorts them in ascending order in the kernel space and return them in the right order to the user space. 
Algorithm Credit: http://www.sanfoundry.com/c-program-sort-array-descending-order/
*/

#include <linux/export.h>
#include <linux/mm.h>
#include <linux/utsname.h>
#include <linux/mman.h>
#include <linux/reboot.h>
#include <linux/prctl.h>
#include <linux/highuid.h>
#include <linux/fs.h>
#include <linux/kmod.h>
#include <linux/perf_event.h>
#include <linux/resource.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/capability.h>
#include <linux/device.h>
#include <linux/key.h>
#include <linux/times.h>
#include <linux/posix-timers.h>
#include <linux/security.h>
#include <linux/dcookies.h>
#include <linux/suspend.h>
#include <linux/tty.h>
#include <linux/signal.h>
#include <linux/cn_proc.h>
#include <linux/getcpu.h>
#include <linux/task_io_accounting_ops.h>
#include <linux/seccomp.h>
#include <linux/cpu.h>
#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/fs_struct.h>
#include <linux/file.h>
#include <linux/mount.h>
#include <linux/gfp.h>
#include <linux/syscore_ops.h>
#include <linux/version.h>
#include <linux/ctype.h>

#include <linux/compat.h>
#include <linux/syscalls.h>
#include <linux/kprobes.h>
#include <linux/user_namespace.h>
#include <linux/binfmts.h>

#include <linux/sched.h>
#include <linux/sched/autogroup.h>
#include <linux/sched/loadavg.h>
#include <linux/sched/stat.h>
#include <linux/sched/mm.h>
#include <linux/sched/coredump.h>
#include <linux/sched/task.h>
#include <linux/sched/cputime.h>
#include <linux/rcupdate.h>
#include <linux/uidgid.h>
#include <linux/cred.h>

#include <linux/kmsg_dump.h>
/* Move somewhere else to avoid recompiling? */
#include <generated/utsrelease.h>

#include <linux/uaccess.h>
#include <asm/io.h>
#include <asm/unistd.h>

/*SYSCALLDEFINEn is a family of macros that facilitates in defining a system call with 'n' arguments. Mandated format has arguments listed as - name of system call, type_of_argument, argument_name .. respectively. It also uses asmlinkage to relate the system call to assembly operation. This essentially means that arguments must be read from registers instead of using copys of arguments as you would in any other c source code*/

SYSCALL_DEFINE3(sortinghat, int32_t*, inbuffer, int32_t*, outbuffer, uint32_t, length)
{
	if (length == 0 || inbuffer == NULL)//validates length and input pointer parameters 
	{
		printk(KERN_ALERT "Parameter: Invalid\n");
		return -EINVAL; //invalid arguemnts error from errno.h (22) 
	}

	int32_t* incoming_buffer; //initialises buffer to accept input parameters input buffer
	uint32_t length_of_buffer = length; 
	size_t malloc_length = (length*(sizeof(int32_t)));  
	incoming_buffer = (int32_t*)kmalloc(malloc_length, GFP_KERNEL);		//delegated memory in kernel memory
	
	if (incoming_buffer == NULL) //validates allocation of memory
	{
		printk(KERN_ALERT "Error: Memory Alloction Failed\n"); 
		return -EFAULT; //bad address fault (14)
	}	
	
	uint32_t i;
	uint32_t j;
	int32_t temp_var;
 	 
	printk(KERN_ALERT "System Call Entered\n"); 

	printk("Length of file is %d", length_of_buffer); 
/*FUnction to accept inputs from user memory space. This cannot be done by passing paramters. This function has three parameters namely - 1. destination pointer- incoming values are allocated to this pointer, 2. source - this is the input parameter to the sysdefine function that points to the value we have to copy from address space 3. lenth - no of bytes that need to copied. Returns 1 if copying was incomplete or faulty*/
	if (copy_from_user (incoming_buffer, inbuffer, malloc_length))
	{
		printk(KERN_ALERT "Error: Copy From User Space Failed\n"); 
		return -EFAULT; 
	}
/*Sorting Algo*/	
	printk("Sorting Started\n"); 
	for ( i = 0; i< length_of_buffer; ++i)
	{
		for (j =i+1; j< length_of_buffer; ++j)
		{
			if ( *(incoming_buffer+i) < *(incoming_buffer+j))
			{
				temp_var = *(incoming_buffer+i); 
				*(incoming_buffer+i) = *(incoming_buffer+j);
				*(incoming_buffer+j) = temp_var; 
			}
		}
	}
	
	 
  	printk("Sorting Completed\n");
/*FUnction to accept send values from kernel memory space to user accessible memory space. This cannot be done by passing paramters. This function has three parameters namely - 1. destination pointer- pointer that was passed as a parameter to the sysdefine macro to send back inputs to the user space, 2. source - This is the pointer which points to the address whos data we have to send back to user space  3. lenth - no of bytes that need to copied. Returns 1 if copying was incomplete or faulty*/
	
	if (copy_to_user(outbuffer, incoming_buffer, malloc_length))
	{
		printk(KERN_ALERT "Error: Copy to User Space Failed\n"); 
		return -EFAULT; 
	}
	kfree(incoming_buffer); //frees allocated memory
		
	printk(KERN_ALERT "System Call Exited"); 
	return 0; 
}
