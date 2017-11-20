/*
 * FILE: testchardriver.c
 * AUTHOR: Arundhathi Swami 
 * DATE: 13 November 2017 
 * DESCRIPTION: Basic character driver for BeagleBone Green 
 * REFERENCE: Derek Malloy
 */

#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/device.h> 
#include <linux/kernel.h> 
#include <linux/fs.h> 
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/gpio.h> 

#define DEVICE_NAME "chardriver"
#define CLASS_NAME "userdrivers"

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Arundhathi Swami"); 
MODULE_DESCRIPTION("Character Driver for BBG");
MODULE_VERSION("0.1");

/*declaring private variables to store led driver values*/ 
static uint8_t GpioLed = 53; //maps to led usr led 0
static bool  LedState = false; //leddefaults to off state 

static uint32_t LedOnPeriod = 1000; //defaults dutycycle on time to 1 s
static uint32_t LedOffPeriod = 1000;//defaults off time to 1 s
static uint32_t LedFrequency = 1; 

/*declaring instance of kernel timer module struct*/ 
struct timer_list LedTimer; 

/*kernel expiration(callback) count*/ 
uint16_t RealiveCount = 1; 

/*Flag to check if time has been added*/ 
uint8_t TimerPresent = 1; 

/*Declaring struct used to store pwm characteristisc*/
typedef struct pwm_opt{
  uint8_t power; 
  uint8_t period; 
  uint8_t dutycycle; 
}pwm_characteristics; 

/*creating global private instance of pwm struct*/ 
static pwm_characteristics pwm_rec; 
static pwm_characteristics pwm_send;

static int DevMajorNum; 
//static char Message[256] = {0}; //maximum memory for the string passed in from user space 
static short MaxMsgSize; //stores size of string 
static int InstanceCount = 0; //stores number of times character driver is utilised
static struct class* CharDriverClass = NULL; //the device driver class truct class struct pointer 
static struct device* CharDriverDevice = NULL; //device driver device struct pointer



/*functions defined as part of the char driver and led driver and timer */ 
/*function to open an instance of the character driver*/
static int CharDev_open(struct inode*, struct file*);

/* function to release an instance of  the character driver*/
static int CharDev_release(struct inode*, struct file*); 

/*function to read using the char driver*/ 
static ssize_t CharDev_read(struct file*, char*, size_t, loff_t*); 

/* function to write using the char driver*/ 
static ssize_t CharDev_write(struct file*, const char* __user, size_t, loff_t*); 

/*call back function for the timer*/
void LedTimer_callback(unsigned long data)
{
  printk(KERN_INFO "Progress: In callback\n"); 
  RealiveCount += 1; 
  if (pwm_rec.power == 0)//led is off
  {
    printk(KERN_INFO "Progress: Led is off\n");
    LedState = false; //switches off led 
    RealiveCount = 1; 
    TimerPresent = 1; //added timer 
  }
  else if (pwm_rec.power == 1) //led is on 
  {
    if(RealiveCount % 2 == 0)
    {
      int8_t ret = mod_timer(&LedTimer, jiffies + msecs_to_jiffies(LedOnPeriod)); 
      if(ret != 0)
      {
        printk(KERN_ALERT "Error: Timer Modifying Error\n");  
      
      }
      LedState = true; 
    }
    else 
    {
      int8_t ret = mod_timer(&LedTimer, jiffies + msecs_to_jiffies(LedOffPeriod)); 
      if(ret != 0)
      {
        printk(KERN_ALERT "Error: Timer Modifying Error\n");  
      
      }
      LedState = false; //switches off led 
    }
  }
}//callback ends


/* struct to list all functions associated with the current driver*/ 
static struct file_operations fops = 
{
  .open = CharDev_open, 
  .read = CharDev_read, 
  .write = CharDev_write, 
  .release = CharDev_release, 
}; 


/* function to initiate the module:
 * __init: keyword macro, function is only used at the initialization time and that it can be discarded and its memory freed up after that point
 * returns 0 if successful*/ 
static int __init CharDriver_Init(void)
{
  printk(KERN_ALERT "Progress: Char Driver Initialization Started\n"); 

  /*dynamic allocation of major number to the char driver device*/
  DevMajorNum = register_chrdev(0, DEVICE_NAME, &fops); 
  if(DevMajorNum < 0)
  {
    printk(KERN_ALERT "Error: Char Driver Major Number Allocation Failed\n");    
    return DevMajorNum; 
  }
  printk(KERN_INFO "Progress: Char Driver Registered Correctly with Major Number: %d\n", DevMajorNum); 

  /*register the new device class*/
  CharDriverClass = class_create(THIS_MODULE, CLASS_NAME); 
  if(IS_ERR(CharDriverClass))
  {
    unregister_chrdev(DevMajorNum, DEVICE_NAME);
    printk(KERN_ALERT "Error: Failed to Register the Device Classs\n");
    return PTR_ERR(CharDriverClass);
  }
  printk(KERN_INFO "Progress: Char Driver class registered correctly\n");

  /*Register the device driver*/ 
  CharDriverDevice = device_create(CharDriverClass, NULL, MKDEV(DevMajorNum, 0), NULL, DEVICE_NAME);
  if(IS_ERR(CharDriverDevice)) //clean up if there is an error in any of the above steps 
  {
    class_destroy(CharDriverClass); 
    unregister_chrdev(DevMajorNum, DEVICE_NAME);
    printk(KERN_ALERT "Error: Failed to Register the Device Driver\n");
    return PTR_ERR(CharDriverDevice);
  }
  printk(KERN_INFO "Progress: Char Driver Device registered correctly\n");

  /*initialising timer module*/ 
  init_timer(&LedTimer); 
  setup_timer(&LedTimer, LedTimer_callback, 0); 

  printk(KERN_INFO "Progress: Timer Module for Led Initialized\n"); 
  
  /*checking if provided led gpio is valid*/ 
  if(!gpio_is_valid(GpioLed))
  {
    printk(KERN_INFO "Error: Gpio LED invalid.Try the right one for a change, why dont you?\n"); 
    return -ENODEV; 
  }

  /*initialising led to start at off*/ 
  LedState = false; 
  gpio_request(GpioLed, "sysfs"); //opens led driver
  gpio_direction_output(GpioLed, LedState); //pulls pin low to switch off led 
  gpio_export(GpioLed, false); //creates dir for pin 53 in led and switches it off 
  printk(KERN_INFO "Progress: Led Initialised to Default Value off\n"); 

  /*initialising default values for pwm struct*/ 
  /*default values*/ 
  pwm_rec.power = 180; 
  pwm_rec.period = 190; 
  pwm_rec.dutycycle = 200; 


  printk(KERN_ALERT "Progress: Device Registered Correctly. Leaving Init\n"); 
  return 0; 
}

/*Function to implement when removing module*/
static void __exit CharDriver_Exit(void)
{
  int ret;

  /* led driver released from kernel space*/ 
  gpio_set_value(GpioLed, 0); //pulls pin
  gpio_unexport(GpioLed); //removes dir led 
  gpio_free(GpioLed); //release pin from association 

  /*releasing the timer module */
  ret = del_timer(&LedTimer); 
  if(ret)
  {
    printk(KERN_INFO "Error: Timer Failed to Delete\n"); 
  }

  /*releasing character driver from kernel space*/ 
  device_destroy(CharDriverClass, MKDEV(DevMajorNum, 0)); 
  class_unregister(CharDriverClass); 
  class_destroy(CharDriverClass); 
  unregister_chrdev(DevMajorNum, DEVICE_NAME); 
  printk(KERN_ALERT "Progress: Done with Cleanup. Exiting exit\n"); 
}

/*Function to initiate an instance of the device driver
 * It increments the Instance count
 */
static int CharDev_open(struct inode* inodeDev, struct file* fileptr)
{
  InstanceCount++; 
  printk(KERN_INFO "Progress: Device Opened: %d times\n", InstanceCount); 
  return 0; 
}

/*Function to send data from device to user. uses the copy to user to send data from user space to kernel space in the buffer format
 * fileptr -> a pointer to  file object
 * bufferptr -> a pointer to the buffer that contains data to send to the user
 * length -> length of the data needed to be transferred
 * offset -> an offset for the buffer if required
 * */
static ssize_t CharDev_read(struct file* fileptr, char* buffer, size_t length, loff_t* offset)
{
  int errors = 0;
  pwm_send.power = pwm_rec.power; 
  pwm_send.period = pwm_rec.period; 
  pwm_send.dutycycle = pwm_rec.dutycycle; 
  //copies data to user
  errors = copy_to_user(buffer, &pwm_send, sizeof(pwm_send));
  if(errors == 0)
  {
    printk(KERN_INFO "PWM Parameter Values from Read:\n");
    printk(KERN_INFO "PWM Power: %d\n", pwm_send.power); 
    printk(KERN_INFO "PWM Period: %d\n", pwm_send.period); 
    printk(KERN_INFO "PWM Duty Cycle: %d\n", pwm_send.dutycycle); 
    printk(KERN_INFO "Progress: Sent to user space\n"); 
    return (MaxMsgSize = 0); 
  }
  else
  {
    printk(KERN_ALERT "Error: Failed to send chars to user\n");
    return EFAULT; 
  }
}

/*Function is called whenever user needs to write to the device from userspace
 * fileptr -> pointer to a file object
 * buffer -> buffer that receives data from the userspace to write to the device
 * length -> length of data in bytes
 * offset -> optional
 * */
static ssize_t CharDev_write(struct file* fileptr, const char* __user buffer, size_t length, loff_t* offset)
{
  int errors = 0;
  errors = copy_from_user(&pwm_rec, buffer, length); 
  if (errors != 0)
  {
    printk(KERN_INFO "Error: Failed to read from user\n");
    return -EFAULT; 
  }
  else 
  {
    printk(KERN_INFO "Progress: Successfully read from user space\n"); 
  }
  //sprintf(Message, "%s(%zu letters)", buffer, length); //appends received string with length 
  
  LedFrequency = pwm_rec.period * 1000; 
  LedOnPeriod = (pwm_rec.dutycycle * LedFrequency)/ 100; 
  LedOffPeriod = (LedFrequency - LedOnPeriod); 

  printk(KERN_INFO "Received PWM Power State: %d\n", pwm_rec.power); 
  printk(KERN_INFO "Received PWM Period: %d\n", pwm_rec.period);
  printk(KERN_INFO "Received PWM Duty Cycle:%d\n", pwm_rec.dutycycle);

  if (pwm_rec.power == 1 && TimerPresent == 1)
  {
    printk(KERN_INFO "Progress: Entered On struct\n");
    LedTimer.expires = jiffies+msecs_to_jiffies(LedOnPeriod); 
    LedTimer.data = 0; 
    LedTimer.function = LedTimer_callback; 
    add_timer(&LedTimer); 
    TimerPresent = 0; 
  }
  
  printk(KERN_INFO "Progress: Successfully written to device %d\n", sizeof(pwm_rec));
  return length;
}

/*Function called whenever device is closed form user space*/
static int CharDev_release(struct inode* inodedev, struct file* fileptr)
{
  InstanceCount--;
  printk(KERN_INFO "Progress: Number of Active Devices now %d\n", InstanceCount);
  printk(KERN_ALERT "Progress: Device Successfully closed from userspace");
  return 0; 
}

module_init(CharDriver_Init);
module_exit(CharDriver_Exit);






