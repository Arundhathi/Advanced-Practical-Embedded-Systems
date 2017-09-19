#include <linux/module.h>
#include <linux/init.h> 
#include <linux/timer.h> 

MODULE_LICENSE("GPL"); //uses Lnux Mandated GPL License  
MODULE_DESCRIPTION("Manipulates the Kernel Timer"); //function of the module 
MODULE_AUTHOR("Arundhathi Swami"); 
MODULE_VERSION("0.1");

#define EXPIRATION_in_ms 500 //defines time of expiration for timer in ms 
static struct timer_list modified_timer;  //instantiates an instance of the timer_list struct which contains callback funtion, expiration time and user provided context


/* CALL-BACK FUNCTION : Called every time the timer completes it rotation
* Input Parameters: unsigned long - sent with the mod_timer_function
* Return Type: void 
*/
void modified_timer_callback( unsigned long data)
{
	static uint8_t count = 0;
	int return_value;  
	return_value = mod_timer(&modified_timer, jiffies + msecs_to_jiffies(EXPIRATION_in_ms)); //mod_time returns a 0 while modifying an unintialised timer else returns 1
	printk(KERN_ALERT "In Callback\n");
	count  += 1; //keeps count of number of times program counter has entered callback
	printk("%d: Count Incremented to: %d\n", count, count); 
	printk(KERN_ALERT "Exiting Callback\n"); 
}

/*INITIALIZATION FUNCTION: This is a mandatory function is the first to be executed when a module is invoked. Analogous to main. 
*Input Paramters: Predefined to void  
*return TYpe: Predefined to void
*/
static int timer_manipulation_init(void) 
{
	int return_value; 
	printk(KERN_ALERT "Module Initiated\n"); 
	
	//modified_timer.funtion amd modified_timer.data 
	setup_timer(&modified_timer, modified_timer_callback, 0); //inherently calls init_timer and assigns value to associated call back function and initialised timer_list type struct i.e. timer 
	printk("Starting timer to fire in 500ms (%ld)\n", jiffies);
	add_timer(&modified_timer);//initiates timer operation
	return_value = mod_timer(&modified_timer, jiffies + msecs_to_jiffies(EXPIRATION_in_ms));//sets values afer which timer should expire after converting them to jiffies (no of processor ticks) and offsetting them by jiffies 
	if (return_value == 0)//mod timer returns 0 for modifying parametrs of uninitialized timers 
	{
		printk(KERN_ALERT "%d:Timer Init Failed\n", return_value); 
	}



	return 0; 
}

/*EXIT FUNCTION: Function invoked when module is is removed from list of loadable modules 
*Both, Input Parameters and return typr are mandatorily predefined as 0
*/
static void timer_manipulation_exit(void)
{
	int return_value; 
	return_value = del_timer(&modified_timer); //syscall to delete initialized timer 
	if (return_value)
	{
		printk(KERN_ALERT "Timer not Deleted\n"); 
	}
 
	printk(KERN_ALERT "Module Exit\n"); 
	return; 
}

module_init(timer_manipulation_init);  //initialises init function
module_exit(timer_manipulation_exit); //initialises exit function
MODULE_LICENSE("GPL");
