#include<stdio.h>
#include<stdint.h> 
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/ioctl.h> 
#include<linux/i2c-dev.h> 

#define MAX_BUFFER_SIZE 4
/*Device and Register addresses*/ 
#define I2C_NUM 2
#define DEVICE_ADDR 0x48

uint8_t WR_BUFF[MAX_BUFFER_SIZE];
uint8_t RD_BUFF[MAX_BUFFER_SIZE];

/*Register Addresses*/
#define REG_TEMP 0x00 /*read only register that has temp value by default*/ 
#define REG_CONFIGURATION 0x01  /*configuration register*/
#define REG_TLOW 0x02 /*lower byte of threshold*/
#define REG_THIGH 0x03  /*higher byte of threshold*/


/*Register values*/

/*TMP102 Attributes*/
#define FAULTBITS 0x02 /*generates alert after 4 consecutive faults*/ 
#define EXTENDED_MODE 0x00 /*0:12 bit temp values (-55C to +128C)*/
#define ALERT_MODE 0x01 /*Alert(ro). Provides information about the comparator status. FOR POL = 0, AL will read as 1 till the temp equals or exceeds Thigh for fault no of times*/ 
#define CONVERSION_RATE 0x03 /*4Hz Conversion rate*/ 
#define SHUTDOWN 0x00 /*device maintains a continuous conversion state*/ 
#define THERMOSTAT_MODE 0x00 /*device is in comparator mode*/ 
#define POLARITY_MODE 0x00 /*Makes AL pin active low*/ 
#define CONVERTER_RESOLUTION 0x02 /*Read only bits, sets temperature to a 12 bit resolution*/ 
#define ONESHOT_CR 0x00 /*saves power between conversions when 1*/ 


int setupI2C2(char filename[])
{
  int file = open(filename, O_RDWR); 
  if(file<0)
  {
   printf("Error: Cant open file\n"); 
   return 1; 
  }
  if(ioctl(file, I2C_SLAVE, DEVICE_ADDR) <0)
  {
   printf("Error: Slave address resolution Problem\n"); 
   return 1; 
  }
   return file; 
}

void setupReg(int file, uint8_t DevAddr)
{
  if (ioctl(file, I2C_SLAVE, DevAddr) < 0)
  {
    printf("Error: SLave Address Resolution in EM\n"); 
  }
}

uint8_t readI2C(int file, int Register, uint8_t* readbytes)
{
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  WR_BUFF[0] = Register;
  setupReg(file, DEVICE_ADDR); 
  if(write(file,WR_BUFF, 1) != 1)
  {
    printf("Error: Error initiating read in em\n"); 
  }
  setupReg(file, DEVICE_ADDR); 
  if(read(file, RD_BUFF, 3) != 3)
  {
    printf("Error: Read Error in EM\n"); 
  }
  *readbytes = RD_BUFF[0];
  *(readbytes+1) = RD_BUFF[1]; 
  *(readbytes+2) = RD_BUFF[2]; 
  *(readbytes+3) = RD_BUFF[3]; 
  return 1; 
}

/*sets extended mode (12/13 bit repesentation of temp) values*/
void setEM(uint8_t mode, int file)
{

  WR_BUFF[0] = REG_CONFIGURATION;
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  uint8_t readbyte = readI2C(file, WR_BUFF[0], READ_BUFF);

  WR_BUFF[1] = READ_BUFF[0];
  WR_BUFF[2] = READ_BUFF[1];
  WR_BUFF[2] &= 0xEF;
  WR_BUFF[2] |= mode<<4;
  setupReg(file, DEVICE_ADDR); 
  if(write(file, WR_BUFF, 3)!=3)
  {
    printf("Error: Couldnt write\n"); 
  }  
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);

}

/*sets the rate for conversion of value*/
void setConversionRate(uint8_t rate, int file)
{
  WR_BUFF[0] = REG_CONFIGURATION;
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  uint8_t readbyte = readI2C(file, WR_BUFF[0], READ_BUFF);  
  WR_BUFF[1] = READ_BUFF[0];
  WR_BUFF[2] = READ_BUFF[1];
  WR_BUFF[2] &= 0x3F; /*clearing out cr bits */
  WR_BUFF[2] |= rate<< 6;
  setupReg(file, DEVICE_ADDR);
  if(write(file, WR_BUFF, 3)!=3)
  {
    printf("Error: Couldnt write\n"); 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);

}

/*sets the shutdown mode - with 0 being continuous conversion or wake up*/ 
void setSD(uint8_t sdmode, int file)
{
  WR_BUFF[0] = REG_CONFIGURATION;
  uint8_t READ_BUFF[MAX_BUFFER_SIZE]; 
  setupReg(file, DEVICE_ADDR); 
  uint8_t readbyte = readI2C(file, WR_BUFF[0], READ_BUFF);  
  WR_BUFF[1] = READ_BUFF[0]; 
  WR_BUFF[1] &= 0xFE; //clears out the sd bit  
  WR_BUFF[1] |= sdmode; 
  setupReg(file, DEVICE_ADDR); 
  if(write(file, WR_BUFF, 2)!=2)
  {
    printf("Error: Couldnt write\n"); 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
}

/*sets sensor in threshold mode (comparator mode) or interrupt mode*/ 
void setTM(uint8_t tm_mode , int file)
{
  WR_BUFF[0] = REG_CONFIGURATION;
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  setupReg(file, DEVICE_ADDR); 
  uint8_t readbyte = readI2C(file, WR_BUFF[0], READ_BUFF); 
  WR_BUFF[1] = READ_BUFF[0]; 
  WR_BUFF[1] &= 0xFD; 
  WR_BUFF[1] |= tm_mode<<1; 

  if(write(file, WR_BUFF, 2)!=2)
  {
    printf("Error: Couldnt write\n"); 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE);  
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);

}

/*sets polairty of alert bit (active low or active high)*/ 
void setPOL(uint8_t polarity, int file)
{
  WR_BUFF[0] = REG_CONFIGURATION; 
  uint8_t READ_BUFF[MAX_BUFFER_SIZE]; 
  setupReg(file, DEVICE_ADDR); 
  uint8_t readbyte = readI2C(file, WR_BUFF[0], READ_BUFF);  
  WR_BUFF[1] = READ_BUFF[0]; 
  WR_BUFF[1] &= 0xFB;
  WR_BUFF[1] |= polarity<<2; 
  if(write(file, WR_BUFF, 2)!=2)
  {
    printf("Error: Couldnt write\n"); 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
}

/*Sets the number of faults (crossing limits) that must occure before alert is set*/
void setFault(uint8_t faultvalue, int file)
{
  WR_BUFF[0] = REG_CONFIGURATION;
  uint8_t READ_BUFF[MAX_BUFFER_SIZE]; 
  setupReg(file, DEVICE_ADDR); 
  uint8_t readbyte = readI2C(file, WR_BUFF[0], READ_BUFF);  
  WR_BUFF[1] = READ_BUFF[0]; 
  WR_BUFF[1] &= 0xE7;
  WR_BUFF[1] |= faultvalue<<3;
  WR_BUFF[0] = REG_CONFIGURATION; 
  if(write(file, WR_BUFF, 2)!=2)
  {
    printf("Error: Couldnt write\n"); 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
}

/*sets lower threshold for temperature in Celcius*/ 
void setLowerThresholdRegisterC(int file, float lower_threshold)
{
  setupReg(file, DEVICE_ADDR); 
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  /*makes sure temp values stay within range*/
  if (lower_threshold > 150.0f)
  {
    lower_threshold = 150.0f; 
  }
  if(lower_threshold < -55.0f)
  {
    lower_threshold = -55.0f;
  }
  /*read current lower threshold register*/
  WR_BUFF[0] = REG_CONFIGURATION; 

  uint8_t readbyte = readI2C(file, WR_BUFF[0], READ_BUFF);  

  /*Reading the extended bit value from the config register*/
  uint8_t extendedmode = ((READ_BUFF[1] & 0x10) >> 4);
  WR_BUFF[0] = REG_TLOW;

  /*if extended bit is 0, temp data will be 12 bits else 13 bits, so according to that we will adjust register values for lower threshold registers*/ 
  lower_threshold = lower_threshold/0.0625; 
  if (extendedmode) /*13 bit data*/
  {
    WR_BUFF[1] = (int)(lower_threshold) >> 5;
    WR_BUFF[2] = (int)(lower_threshold) << 3;
  }
  else 
  { 
    WR_BUFF[1] = (int)(lower_threshold) >> 4;
    WR_BUFF[2] = (int)(lower_threshold) << 4;
  } 

  if(write(file, WR_BUFF, 3)!=3)
  {
    printf("Error: Couldnt write\n"); 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
}


/*sets higher threshold for temperature in celcius*/
void setHigherThresholdRegisterC(int file, float higher_threshold)
{
  setupReg(file, DEVICE_ADDR); 
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  /*makes sure temp values stay within range*/
  if (higher_threshold > 150.0f)
  {
    higher_threshold = 150.0f; 
  }
  if(higher_threshold < -55.0f)
  {
    higher_threshold = -55.0f;
  }
  /*read current lower threshold register*/
  WR_BUFF[0] = REG_CONFIGURATION;

  uint8_t readbyte = readI2C(file, WR_BUFF[0], READ_BUFF);  

  /*Reading the extended bit value from the config register*/
  uint8_t extendedmode = (READ_BUFF[1] &0x10)>>4;
  WR_BUFF[0] = REG_THIGH;

  /*if extended bit is 0, temp data will be 12 bits else 13 bits, so according to that we will adjust register values for lower threshold registers*/ 
  higher_threshold = higher_threshold/0.0625; 
  if (extendedmode) /*13 bit data*/
  {
    WR_BUFF[1] = (int)(higher_threshold) >> 5;
    WR_BUFF[2] = (int)(higher_threshold) << 3;
  }
  else 
  { 
    WR_BUFF[1] = (int)(higher_threshold) >> 4;
    WR_BUFF[2] = (int)(higher_threshold) << 4;
  } 

  if(write(file, WR_BUFF, 3)!=3)
  {
    printf("Error: Couldnt write\n"); 
  }
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE);
}

/*sets lower threshold for temperature in farenheit*/
void setLowerThresholdRegisterF(int file, float lower_threshold)
{
  lower_threshold = (((lower_threshold -32) *5)/9); 
  setLowerThresholdRegisterC(file, lower_threshold); 
}

/*sets higher threshold for temperature in farenheit*/
void setHigherThresholdRegisterF(int file, float higher_threshold)
{
  higher_threshold = (((higher_threshold -32) *5)/9); 
  setHigherThresholdRegisterC(file, higher_threshold);
}

/*sets lower_threshold for temperature in kelvin*/
void setLowerThresholdRegisterK(int file, float lower_threshold)
{
  lower_threshold = (lower_threshold-273.15) ; 
  setLowerThresholdRegisterC(file, lower_threshold); 
}


/*sets higher_threshold for temperature in kelvin*/
void setHigherThresholdRegisterK(int file, float higher_threshold)
{
  higher_threshold = (higher_threshold-273.15); 
  setHigherThresholdRegisterC(file, higher_threshold);
}



int setupTempSensor()
{
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
  int file; 
  char filename[20]; 
  snprintf(filename, 19, "/dev/i2c-%d", I2C_NUM); 
  file = setupI2C2(filename);
  setEM(EXTENDED_MODE, file);
  setConversionRate(CONVERTER_RESOLUTION, file);  
  setSD(SHUTDOWN, file); 
  setTM(THERMOSTAT_MODE, file); 
  setPOL(POLARITY_MODE, file); 
  setFault(FAULTBITS, file); 
//  setLowerThresholdRegisterF(file, 85.0); 
 // setHigherThresholdRegisterF(file, 80.0); 
  
  return file;  
}

float readLowTempC(int file)
{
  float temperature; 
  int extendedmode;
  int tempfromsensor; 
/*find how to read data coming in from the sensor - 12 or 13 bits*/ 
  WR_BUFF[0] = REG_CONFIGURATION; 
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  uint8_t readbyte = readI2C(file, WR_BUFF[0], READ_BUFF); 
  extendedmode = (READ_BUFF[1]&0x10) >>4; 
  WR_BUFF[0] = REG_TLOW;  
  readbyte = readI2C(file, WR_BUFF[0], READ_BUFF); 
  if(extendedmode) //13 bit temperature
  {
    tempfromsensor = ((READ_BUFF[0]<<5) | (READ_BUFF[1] >> 3)); 
    if(tempfromsensor > 0xFFF) //if temp is negative
    {
      tempfromsensor |= 0xE000; //convert to 16 bit 2's complement
    }
  }
  else //12 bit temperature  
  {
    tempfromsensor = ((READ_BUFF[0] << 4) | (READ_BUFF[1] >> 4)); 
    if (tempfromsensor > 0x7FF) //if temp is negative 
    {
      tempfromsensor |= 0xF000; 
    }
  }
  temperature = tempfromsensor * 0.0625; 
  return temperature; 
  
}

float readHighTempC(int file)
{
  float temperature; 
  int extendedmode; 
  int tempfromsensor; 
  WR_BUFF[0] = REG_CONFIGURATION; 
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  uint8_t readbyte = readI2C(file, WR_BUFF[0], READ_BUFF); 
  extendedmode = (READ_BUFF[1] & 0x10)>> 4;
  WR_BUFF[0] = REG_THIGH; 
  readbyte = readI2C(file, WR_BUFF[0], READ_BUFF);
  if(extendedmode)
  {
    tempfromsensor = (READ_BUFF[0] << 5) | (READ_BUFF[1] >>3); 
    if(tempfromsensor > 0xFFF)
    {
      tempfromsensor |= 0xE000; 
    }
  }
  else
  {
    tempfromsensor = (READ_BUFF[0] << 4) | (READ_BUFF[1] >> 4); 
    if(tempfromsensor > 0x7FF)
    {
      tempfromsensor |= 0xF000; 
    }
  }
  temperature = tempfromsensor * 0.0625; 
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
  return temperature; 
}


float readLowTempF(int file)
{
  float rawtemp = readLowTempC(file); 
  return (rawtemp*(9.0/5.0))+32.0; 	
}

float readHighTempF(int file)
{
  float rawtemp = readHighTempC(file); 
  return (rawtemp*(9.0/5.0))+32.0; 
}

float readLowTempK(int file)
{
  float rawtemp = readLowTempC(file); 
  return (rawtemp + 273.15); 
}

float readHighTempK(int file)
{
  float rawtemp = readHighTempC(file); 
  return (rawtemp + 273.15); 
}

float readTempC(int file)
{
  float temperature; 
  int rawtemp; 
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  WR_BUFF[0] = 0x00; 
  uint8_t readbyte = readI2C(file, WR_BUFF[0], READ_BUFF); 
  
  if(READ_BUFF[1] & 0x01) //13 bit mode
  {
    rawtemp = (READ_BUFF[0]<<5) | (READ_BUFF[1] >>3); 
    //adjusting for  positive/negative  data
    if(rawtemp > 0xFFF)
    {
      rawtemp |= 0xE000; 
    }
  }
  else //12 bit mode
  {
    rawtemp = (READ_BUFF[0] << 4) | (READ_BUFF[1] >> 4); 
    if(rawtemp > 0x7FF)
    {
     rawtemp |= 0xF000; 
    }
  }
  
  temperature = rawtemp *0.0625; 
  memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
  memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
  return temperature; 
}

float readTempF(int file)
{
  float temp = readTempC(file); 
  temp = temp*9.0/5.0+32.0; 
  return temp; 
}

float readTempK(int file)
{
  float temp = readTempC(file); 
  temp = temp + 273.15;
  return temp; 
}
   
int main()
{

  int file; 
  uint8_t READ_BUFF[MAX_BUFFER_SIZE];
  file = setupTempSensor();
  uint8_t res = readI2C(file, REG_CONFIGURATION, READ_BUFF);  
  printf("Config Reg Value: %x, %x\n", READ_BUFF[0], READ_BUFF[1]);
  uint8_t thresh = readI2C(file, REG_TEMP, READ_BUFF); 
  printf("Temp reg values: %x, %x\n", READ_BUFF[0], READ_BUFF[1]); 
  
  float cel = readTempC(file);
  float far = readTempF(file);
  float kel = readTempK(file);   
  printf("Temperature: %0.2f C, %0.2f F, %0.2f K\n", cel, far, kel); 
  return 0;
}

