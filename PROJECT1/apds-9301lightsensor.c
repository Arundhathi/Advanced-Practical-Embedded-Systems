#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdint.h> 
#include<sys/ioctl.h> 
#include<linux/i2c-dev.h>

/*Device and Register Addresses*/
#define DEVICE_ADDR  0x39 /*I2C Slave Float  Address*/
#define I2C_NUM 2 /*I2C Instance Number*/

#define MAX_BUFFER_SIZE 2 
#define POWER_UP 0x03
#define POWER_DOWN 0x00
#define REG_TH_LOW_LOWER 0x82
#define REG_TH_LOW_HIGHER 0x83
#define REG_TH_HIGH_LOWER 0x84 
#define REG_TH_HIGH_HIGHER 0x85
#define REG_INTERRUPT 0x86
#define TIMING_REG 0x81
#define CONTROL_REG 0x80 
#define SENSOR0_LOWER 0xAC
#define SENSOR0_HIGHER 0xAD

/*Values for registers*/ 
#define TH_LOW_LOWER 0x0f
#define TH_LOW_HIGHER 0x00
#define TH_HIGH_LOWER 0x00
#define TH_HIGH_HIGHER 0x08
#define PERSISTENCE 0x04 /*if the luminosit crosses threshold for 4 consecutive periods*/ 
#define INTR 0x01/*Interrupts enabled*/
#define GAIN 0x00/*low gain*/
#define INTEGRATION_TIME 0x01/*INtegrattion time per conversion 101ms*/
#define INTERRUPT_VALUE INTR<<4 | PERSISTENCE 
#define TIMING_VALUE GAIN<<4 | INTEGRATION_TIME

uint8_t WR_BUFF[MAX_BUFFER_SIZE] = {0};
uint8_t RD_BUFF[MAX_BUFFER_SIZE] ={0}; 

/*Threshold values for comparison*/
#define BRIGHT_LIGHT 0x0800
#define LOW_LIGHT 0x000f

/*Function to set up the sensor*/
int setupI2C2(char filename[])
{
  int file = open(filename, O_RDWR); 
  if(file<0){
    printf("Error: Cant open file\n");
    return 1;  
  }
  if(ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
  {
    printf("Error: Slave Address Resolution Problem\n"); 
    return 1; 
  }
  return file;

}

int setupLightSensor()
{ 
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
  int file ; 
  char filename[20];
  snprintf(filename, 19, "/dev/i2c-%d", I2C_NUM);
  file = setupI2C2(filename);

  /*writing to the control register*/ 
  WR_BUFF[0] = CONTROL_REG; //command reg to write to control register    
  WR_BUFF[1] = POWER_UP; //3 is equivalent to power up 
  printf("Control Register:\n"); 
  printf("Register Address: %x :: Register Value:  %x\n", WR_BUFF[0], WR_BUFF[1]);
  if (write(file, WR_BUFF, 2) != 2)
  {
    printf("Error: Write Error\n");
    return 1; 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 

  /* writing to the timing register*/
  WR_BUFF[0] = TIMING_REG; //command reg to write to control register   
  WR_BUFF[1] = TIMING_VALUE; //3 is equivalent to power up 
  printf("Timing register:\n"); 
  printf("Register Address: %x :: Register Value:  %x\n", WR_BUFF[0], WR_BUFF[1]);
  if (write(file, WR_BUFF, 2) != 2)
  {
    printf("Error: Write Error\n");
    return 1; 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 

  /* writing to lower byte of lower threshold*/
  WR_BUFF[0] = REG_TH_LOW_LOWER; //command write to lower byte of low th
  WR_BUFF[1] = TH_LOW_LOWER; //3 is equivalent to power up 
  printf("Lower Threshold: Lower Byte Value:\n");
  printf("Register Address: %x :: Register Value:  %x\n", WR_BUFF[0], WR_BUFF[1]);
  if (write(file, WR_BUFF, 2) != 2)
  {
    printf("Error: Write Error\n");
    return 1; 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 

  /* writing to higher byte of lower threshold*/
  WR_BUFF[0] = REG_TH_LOW_HIGHER; //command to higher byte of lower th    
  WR_BUFF[1] = TH_LOW_HIGHER; //3 is equivalent to power up 
  printf("Lower Threshold: Higher Byte Value:\n");
  printf("Register Address: %x :: Register Value:  %x\n", WR_BUFF[0], WR_BUFF[1]);
  if (write(file, WR_BUFF, 2) != 2)
  {
    printf("Error: Write Error\n");
    return 1; 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 

  /* writing to lower byte of higher threshold*/
  WR_BUFF[0] = REG_TH_HIGH_LOWER; //command write to lower byte high th   
  WR_BUFF[1] = TH_HIGH_LOWER; //3 is equivalent to power up 
  printf("Upper Threshold: Lower Byte Value\n"); 
  printf("Register Address: %x :: Register Value:  %x\n", WR_BUFF[0], WR_BUFF[1]);

  if (write(file, WR_BUFF, 2) != 2)
  {
    printf("Error: Write Error\n");
    return 1; 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 

  /* writing to higher byte of higher threshold*/
  WR_BUFF[0] = REG_TH_HIGH_HIGHER; //command write to lower byte high th  
  WR_BUFF[1] = TH_HIGH_HIGHER; //3 is equivalent to power up 
  printf("Upper Threshold: Higher Byte Value:\n"); 
  printf("Register Address: %x :: Register Value:  %x\n", WR_BUFF[0], WR_BUFF[1]);

  if (write(file, WR_BUFF, 2) != 2)
  {
    printf("Error: Write Error\n");
    return 1; 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 

  /* writing to interrupt reg*/
  WR_BUFF[0] = REG_INTERRUPT; //command write to interrupt reg    
  WR_BUFF[1] = INTERRUPT_VALUE; //3 is equivalent to power up 
  printf("Interrupt Register:\n"); 
  printf("Register Address: %x :: Register Value:  %x\n", WR_BUFF[0], WR_BUFF[1]);


  if (write(file, WR_BUFF, 2) != 2)
  {
    printf("Error: Write Error\n");
    return 1; 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  return file; 
}

void LEDOn()
{
  FILE* LEDHandler = NULL; 
  char* LEDPATH = "/sys/class/leds/beaglebone:green:usr3/brightness";
  if(LEDHandler = fopen(LEDPATH, "r+"))
  {
    fwrite("1", 1, 1, LEDHandler); 
    fclose(LEDHandler); 
  }
  else 
  {
    printf("Wrong Handler bruh\n"); 
  }
}

void LEDOff()
{
  FILE* LEDHandler = NULL; 
  char* LEDPATH = "/sys/class/leds/beaglebone:green:usr3/brightness";
  if(LEDHandler = fopen(LEDPATH, "r+"))
  {
    fwrite("0", 1, 1, LEDHandler); 
    fclose(LEDHandler); 
  }
  else 
  {
    printf("Wrong Handler bruh\n"); 
  }

}

int main()
{

  int file;
  file = setupLightSensor();   
  /*Reading adc values from sensor*/
  WR_BUFF[0] = SENSOR0_LOWER; 

  if(ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
  {
    printf("Error: Slave Address Resolution Problem\n"); 
    return 1; 
  }

  if(write(file, WR_BUFF, 1)!=1)
  {
    printf("Error: Writing to control register\n"); 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 

  if(ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
  {
    printf("Error: Slave Address Resolution Problem\n"); 
    return 1; 
  }

  if(read(file, RD_BUFF, 1) != 1)
  {
    printf("Error: read Error\n");
  }

  printf("%x\n", RD_BUFF[0]);
  uint8_t lower = RD_BUFF[0];
  /*higher byte*/
  WR_BUFF[0] = SENSOR0_HIGHER; 

  if(ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
  {
    printf("Error: Slave Address Resolution Problem\n"); 
    return 1; 
  }

  if(write(file, WR_BUFF, 1)!=1)
  {
    printf("Error: Writing to control register\n"); 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 

  if(ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
  {
    printf("Error: Slave Address Resolution Problem\n"); 
    return 1; 
  }

  if(read(file, RD_BUFF, 1) != 1)
  {
    printf("Error: read Error\n");
  }
  uint8_t upper = RD_BUFF[0];
  printf("%x\n", upper);
  uint8_t lum = ((upper << 8) + lower); 
  printf("Lum value: %x\n", lum); 
  if(lum < LOW_LIGHT)
  {
    printf("Its a dark dark world\n"); 
    LEDOn(); 
  }
  else if (lum > BRIGHT_LIGHT)
  {
    printf("Its an occassionally bright  world\n"); 

    LEDOff();
  }


  close(file);
  return 0; 
}

