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
#define REG_TEMP 0x00	/*read only register that has temp value by default*/ 
#define REG_CONFIGURATION 0x01	/*configuration register*/
#define REG_TLOW 0x02	/*lower byte of threshold*/
#define REG_THIGH 0x03	/*higher byte of threshold*/


/*Register values*/
#define CONFIG_LOWER
#define CONFIG_HIGHER (FAULTBITS<<3 | 

/*TMP102 Attributes*/
#define FAULTBITS 0x02 /*generates alert after 4 consecutive faults*/ 
#define EXTENDED_MODE 0x00 /*0:12 bit temp values (-55C to +128C)*/
#define ALERT_MODE 0x01 /*Alert(ro). Provides information about the comparator status. FOR POL = 0, AL will read as 1 till the temp equals or exceeds Thigh for fault no of times*/ 
#define CONVERSION_RATE 0x02 /*4Hz Conversion rate*/ 
#define SHUTDOWN 0x00 /*device maintains a continuous conversion state*/ 
#define THERMOSTAT_MODE 0X00 /*device is in comparator mode*/ 
#define POLARITY_MODE 0X00 /*Makes AL pin active low*/ 
#define CONVERTER_RESOLUTION 0X03 /*Read only bits, sets temperature to a 12 bit resolution*/ 
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

/*sets extended mode (12/13 bit repesentation of temp) values*/
void setEM(uint8_t mode, int file)
{
	
	WR_BUFF[0] = REG_CONFIGURATION;
 	if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
	{
		printf("Error: SLave Address Resolution in EM\n"); 
	}
	if(write(file,WR_BUFF, 1) != 1)
	{
		printf("Error: Error initiating read in em\n"); 
	}
	if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
	{
		printf("Error: SLave Address Resolution in EM\n"); 
	}
	if(read(file, RD_BUFF, 2) != 2)
	{
		printf("Error: Read Error in EM\n"); 
	}
	WR_BUFF[1] = RD_BUFF[0];
	WR_BUFF[2] = RD_BUFF[1];
	WR_BUFF[2] &= 0xEF;
	WR_BUFF[2] |= mode<<4;
	 
	if(write(file, WR_BUFF, 3)!=3)
	{
		printf("Error: Couldnt write\n"); 
	}  
}

/*sets the rate for conversion of value*/
void setConversionRate(uint8_t rate, int file)
{
	WR_BUFF[0] = REG_CONFIGURATION;
  	if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
	{
		printf("Error: SLave Address Resolution in EM\n"); 
	}
	if(write(file,WR_BUFF, 1) != 1)
	{
		printf("Error: Error initiating read in em\n"); 
	}
	if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
	{
		printf("Error: SLave Address Resolution in EM\n"); 
	}
	if(read(file, RD_BUFF, 2) != 2)
	{
		printf("Error: Read Error in EM\n"); 
	}

	WR_BUFF[1] = RD_BUFF[0];
	WR_BUFF[2] = RD_BUFF[1];
	WR_BUFF[2] &= 0X3F; /*clearing out cr bits */
	WR_BUFF[2] |= rate << 6;
	 
	if(write(file, WR_BUFF, 3)!=3)
	{
		printf("Error: Couldnt write\n"); 
	}  
}

/*sets the shutdown mode - with 0 being continuous conversion or wake up*/ 
void setSD(uint8_t sdmode, int file)
{
	WR_BUFF[0] = REG_CONFIGURATION; 
 	if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
	{
		printf("Error: SLave Address Resolution in EM\n"); 
	}
	if(write(file,WR_BUFF, 1) != 1)
	{
		printf("Error: Error initiating read in em\n"); 
	}

	if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
	{
		printf("Error: SLave Address Resolution in EM\n"); 
	}
	if(read(file, RD_BUFF, 2) != 2)
	{
		printf("Error: Read Error in EM\n"); 
	}
	WR_BUFF[1] = RD_BUFF[0]; 
	WR_BUFF[1] &= 0xFE; //clears out the sd bit  
	WR_BUFF[1] = RD_BUFF[0] | sdmode; 
	if(write(file, WR_BUFF, 2)!=2)
	{
		printf("Error: Couldnt write\n"); 
	}
	memset(WR_BUFF, '\0', MAX_BUFFER); 
	memset(WR_BUFF, '\0', MAX_BUFFER);
}

/*sets sensor in threshold mode (comparator mode) or interrupt mode*/ 
void setTM(uint8_t tm_mode , int file)
{
	WR_BUFF[0] = REG_CONFIGURATION;
  	if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
	{
		printf("Error: SLave Address Resolution in EM\n"); 
	}
	if(write(file,WR_BUFF, 1) != 1)
	{
		printf("Error: Error initiating read in em\n"); 
	}

	if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
	{
		printf("Error: SLave Address Resolution in EM\n"); 
	}
	if(read(file, RD_BUFF, 2) != 2)
	{
		printf("Error: Read Error in EM\n"); 
	}
	WR_BUFF[1] = RD_BUFF[0]; 
	WR_BUFF[1] &= 0xFD; 
	WR_BUFF[1] |= tm_mode<<1; 
	if(write(file, WR_BUFF, 2)!=2)
	{
		printf("Error: Couldnt write\n"); 
	}
	memset(WR_BUFF, '\0', MAX_BUFFER); 
	memset(WR_BUFF, '\0', MAX_BUFFER);
}

/*sets polairty of alert bit (active low or active high)*/ 
void setPOL(uint8_t polarity, int file)
{
	WR_BUFF[0] = REG_CONFIGURATION; 
  	if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
	{
		printf("Error: SLave Address Resolution in Polarity\n"); 
	}
	if(write(file,WR_BUFF, 1) != 1)
	{
		printf("Error: Error initiating read in POL\n"); 
	}

	if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
	{
		printf("Error: SLave Address Resolution in POL\n"); 
	}
	if(read(file, RD_BUFF, 2) != 2)
	{
		printf("Error: Read Error in POL\n"); 
	}
	WR_BUFF[1] = RD_BUFF[0]; 
	WR_BUFF[1] &= 0xFB;
	WR_BUFF[1] |= polarity<<2; 
	if(write(file, WR_BUFF, 2)!=2)
	{
		printf("Error: Couldnt write\n"); 
	}
	memset(WR_BUFF, '\0', MAX_BUFFER); 
	memset(WR_BUFF, '\0', MAX_BUFFER);
}

/*Sets the number of faults (crossing limits) that must occure before alert is set*/
void setFault(uint8_t faultvalue, int file)
{
	WR_BUFF[0] = REG_CONFIGURATION; 
  	if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
	{
		printf("Error: SLave Address Resolution in Polarity\n"); 
	}
	if(write(file,WR_BUFF, 1) != 1)
	{
		printf("Error: Error initiating read in FF\n"); 
	}

	if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0)
	{
		printf("Error: SLave Address Resolution in FF\n"); 
	}
	if(read(file, RD_BUFF, 2) != 2)
	{
		printf("Error: Read Error in FF\n"); 
	}
	WR_BUFF[1] = RD_BUFF[0]; 
	WR_BUFF[1] &= 0xE7;
	WR_BUFF[1] |= faultvalue<<3; 
	if(write(file, WR_BUFF, 2)!=2)
	{
		printf("Error: Couldnt write\n"); 
	}
	memset(WR_BUFF, '\0', MAX_BUFFER); 
	memset(WR_BUFF, '\0', MAX_BUFFER);
}


int setupTempSensor()
{
	memset(WR_BUFF, '\0', MAX_BUFFER_SIZE); 
	memset(RD_BUFF, '\0', MAX_BUFFER_SIZE); 
	int file; 
	char filename[20]; 
	snprintf(filename, 19, "/dev/i2c-%d", I2C_NUM); 
	file = setupI2C2(filename);
	setFault(FAULTBITS, file); 
	setEM(EXTENDED_MODE, file); 
	setConversionRate(CONVERSION_RATE, file); 	
	return file;  
}

int main()
{

	int file; 
	file = setupTempSensor();
	return 0;
}

















