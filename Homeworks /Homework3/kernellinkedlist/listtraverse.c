//kernel module to traverse back the list of processes running cuurently on the machine 
//Code to Profile Posix Functions for kthread_create 
//Author: Arundhathi Swami
//Subject: Advanced Practical Embeddded Systems 
//Date: 2nd Oct 2017 

#include<linux/sched.h> 
#include<linux/init.h> 
#include<linux/module.h> 
#include<linux/kernel.h> 
#include<linux/sched/signal.h>
#include<linux/list.h> 


MODULE_LICENSE("GPL"); 
MODULE_DESCRIPTION("Lists all the processes and their children currently running "); //sets description macro 
MODULE_AUTHOR("Arundhathi Swami");  //sets author macro 
MODULE_VERSION("0.1"); //sets module version 

struct task_struct *childthread; 

int module_initialization(void)
{
  struct list_head* traverse_ptr;
  //list_head is a kernel defined struct. 
  //struct list_head{ 
  //  struct mylist *next; 
  //  struct mylist *prev; 
  //  }; 
  int num_child = 0; 
  printk(KERN_INFO "Loading Task Module\n"); 
  for(childthread = current; childthread!=&init_task; childthread = childthread->parent)
  {
    num_child=0; 
    list_for_each(traverse_ptr, &(childthread->children))
    {
      num_child++; 
    }
    //list_for_each(position, &mylist.list)-function to iterate over a list 
    //first parameter is used as the counter in the for loop - inside the loop it points to the current items list head 
    //second parameter is the pointer to the list
    printk("%s [%d] state = %ld children = %d\n", childthread->comm, childthread->pid, childthread->state, num_child);
  }

  printk(KERN_ALERT "\nPrinting All Processes\n"); 

  for_each_process(childthread)
  {
    num_child=0; 
    list_for_each(traverse_ptr, &(childthread->children))
    {
      num_child++; 
    }
   /*for_each_process(p):
		p = process where traversal needs to start
      it is a macro function defined as "for(p=&init_task; (p=next_task(p)) != &ini_task; )
here next_task is also a macro defined in the linux/sched.h library. 
#define next_task(p) list_entry((p)->tasks.next, struct task_struct, tasks)
list_entry is a macro defined in linux/list.h
list entry has 3 parameters:
function: gets the struct for this entry
paramters: @ptr: the &struct list_head pointer 
           @type: the type of struct this is embedded in 
           @member: the name of the list_struct within the struct 
*/  
    printk("%s [%d] state = %ld children = %d\n", childthread->comm, childthread->pid, childthread->state, num_child);
  }

  return 0; 
}


void module_exitroutine(void)
{
  printk(KERN_INFO "Removing Module\n"); 
}

module_init(module_initialization); 
module_exit(module_exitroutine); 
MODULE_LICENSE("GPL"); 
