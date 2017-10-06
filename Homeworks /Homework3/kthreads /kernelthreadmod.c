//kernel module to execute kthread_create 
//Code to Profile Posix Functions for kthread_create 
//Author: Arundhathi Swami
//Subject: Advanced Practical Embeddded Systems 
//Date: 30th Sept 2017 
//tuxthink.blogspot.com/2011/02/kernel-thread-creation-1.html 

#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/kthread.h> 
#include <linux/time.h> 
#include <linux/timex.h> 
#include <linux/ktime.h>
#include <linux/timekeeping.h> 


MODULE_LICENSE("GPL"); //sets license 
MODULE_DESCRIPTION("Profiles the time required for kthread_create()"); //sets description macro 
MODULE_AUTHOR("Arundhathi Swami");  //sets author macro 
MODULE_VERSION("0.2"); //sets module version 

static struct task_struct *childthread; //task struct instance declared = this is cicularly doubly linked
cycles_t initiate, terminate, elapsed; 
static struct timeval begin, end; //struct timeval 
unsigned long jstart, jend, jelapsed; 
uint32_t telapsed; 

//function executed by kthread
int thread_function(void* arg)
{
  printk(KERN_INFO "In CHILD THREAD Function\n");
  printk(KERN_INFO "Exiting CHILD THREAD Function\n");
  return 0; 
}

//spawning thread in the kthread init function 
int thread_init (void) 
{
  char thread_to_call[11] = "childthread"; 
 //start profiling 
  printk(KERN_INFO "In MODULE INIT Function\n");
  initiate = get_cycles(); 
  do_gettimeofday(&begin); 
  jstart = jiffies; 
  childthread = kthread_create(thread_function, "hi", thread_to_call); 
  
  /*kthread_create()
 * kthread_create - create a kthread on the current node
 * @threadfn: the function to run in the thread
 * @data: data pointer for @threadfn()
 * @namefmt: printf-style format string for the thread name
 * @...: arguments for @namefmt.
 *
 * This macro will create a kthread on the current node, leaving it in
 * the stopped state
 
#define kthread_create(threadfn, data, namefmt, arg...) \
	kthread_create_on_node(threadfn, data, NUMA_NO_NODE, namefmt, ##arg)*/


  terminate = get_cycles(); 
  do_gettimeofday(&end);
  jend = jiffies; 
  elapsed = (terminate-initiate);
  //end profiling 
  jelapsed = jend - jstart;
  telapsed =  end.tv_usec-begin.tv_usec;
  if ((childthread))
  {
    printk(KERN_INFO "Cycles Used: %llu\n", elapsed);
    printk(KERN_INFO "Time Elapsed in uS: %d\n", telapsed);
    printk(KERN_INFO "Time Elapsed in Jiffies: %lu\n", jelapsed); 
    wake_up_process(childthread);
  }
  return 0; 
}

//module exit function 
void thread_cleanup (void)
{
  int ret = 0; 
  //ret = kthread_stop(childthread); 
  //kthread_stop()
  if (!ret)
    printk(KERN_INFO "Thread Stopped\n");
    printk(KERN_INFO "MODULE EXIT Function Exited\n");
}

MODULE_LICENSE("GPL");
module_init(thread_init); 
module_exit(thread_cleanup);
 

