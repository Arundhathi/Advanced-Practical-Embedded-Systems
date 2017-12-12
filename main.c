/* FreeRTOS 8.2 Tiva Demo
 *
 * main.c
 *
 *Author:: Arundhathi Swami, Vignesh Jayaram
 *
 * Credits: Andy Kobyljanec
 *
 * This is a simple demonstration project of FreeRTOS 8.2 on the Tiva Launchpad
 * EK-TM4C1294XL.  TivaWare driverlib sourcecode is included.
 */

/*PIN CONNECTIONS
 * 1. ALcohol Sensor - Pin 4: PE3
 *                   - Power = 5v
 * 2. Gyroscope - CS - Vcc - 5v
 *              - INT1 - PC7
 *              - SDA - PG1
 *              - SCL - PG0
 * 3. Ultrasonic Sensor - Power - 5v
 *                      - Trig - PC6
 *                      - Echo - 1KOhm - PL3
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"


// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"


// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*user defined libraries included*/
#include "UserI2CLib.h"



/* macro definitions for various tasks*/

/*Ultrasonic Sensor Task*/
#define MAX_TIME 7500

/*ADXLSensor Task*/
/*register set definitions*/
#define DEVADDR 0x53
#define DEVID 0x00
#define PWRCTRL 0x2D
#define INTENABLE 0x2E
#define INTMAP 0x2F
#define INTSOURCE 0x30
#define DATAFORMAT 0x31
#define THRESHACT 0x24
#define THRESHINACT 0x25
#define TIMEINACT 0x26
#define ACTINACTCTL 0x27
#define DATAX0 0x32
#define DATAX1 0x33
#define DATAY0 0x34
#define DATAY1 0x35
#define DATAZ0 0x36
#define DATAZ1 0x37



//global variable declaration
int long rawdistance[1];
uint32_t distancecounter = 0;
float distance = 0;
char dist[8];
uint8_t adxlvalues[10] = {0};
int x = 0, y = 0, z = 0;
double xg = 0, yg = 0, zg = 0;

float cleangasconstant = 0;
uint32_t alcraw[1];
float alcvoltvalue = 0.0;
float alcgasvalue = 0.0;
float alcratio = 0.0;
char printalc[10];


// Demo Task declarations
void demoLEDTask(void *pvParameters);
void demoSerialTask(void *pvParameters);
void USSTask(void *pvParameters);
void COSTask(void *pvParameters);
void ADXLTask(void *pvParameters);

void delayMs(uint32_t ui32Ms) {

    // 1 clock cycle = 1 / SysCtlClockGet() second
    // 1 SysCtlDelay = 3 clock cycle = 3 / SysCtlClockGet() second
    // 1 second = SysCtlClockGet() / 3
    // 0.001 second = 1 ms = SysCtlClockGet() / 3 / 1000

    SysCtlDelay(ui32Ms * (SysCtlClockGet() / 3 / 1000));
}

void delayUs(uint32_t ui32Us) {
    SysCtlDelay(ui32Us * (SysCtlClockGet() / 3 / 1000000));
}

void USSGPIOInit(void)
{
    //Port C and E for GPIO r/w
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);


    //enable the gpio peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);//gpio c set
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);//gpio c set


    //wait for gpio modules to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC))
    {
    }
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL))
    {
    }


    //sets pin pc6 as digital output to trig pin of sensor
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);

    //sets pin pl3 as analog input to echo pin of sensor
    GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_3);
}

void UARTtoCOMInit(void)
{
    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, 57600, SYSTEM_CLOCK);
}

void COSADCInit(void)
{

    SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlDelay(10);

    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);    //ADC Channel-0 for the echo pin of USS Only


    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.  Each ADC module has 4 programmable sequences, sequence 0
    // to sequence 3.
    //
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // information on the ADC sequences and steps, reference the datasheet.
    //
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                                 ADC_CTL_END);
    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    //
    ADCIntClear(ADC0_BASE, 3);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    //
    ADCSequenceEnable(ADC0_BASE, 3);
}

float calibrateCOS(void)
{
    COSADCInit();
    float alsvolt = 0.0;
    float alsair = 0.0;
    float ratio = 0.0;
    char cratio[10];
    uint32_t alsvalue[1] = {0.0};
    float cumulativeval = 0.0;
    int i =0;
    for(i =0; i<100; i++)
    {
        /*Trigger ADC conversion*/
        ADCProcessorTrigger(ADC0_BASE, 3);

        /*Wait for conversion to complete*/
        while(!ADCIntStatus(ADC0_BASE, 3, false));

        /*Clear ADC interrupt flag*/
        ADCIntClear(ADC0_BASE, 3);

        /*Read ADC value*/
        ADCSequenceDataGet(ADC0_BASE, 3, alsvalue);

        cumulativeval += (float)alsvalue[0];
        memset(alsvalue, 0, sizeof(alsvalue));

     }
    cumulativeval = cumulativeval/100.0;
    alsvolt = (cumulativeval/1024)*5.0;
    alsair = (5.0 - alsvolt)/alsvolt;
    ratio = alsair/60.0;
    sprintf(cratio, "%f", ratio);
    UARTprintf("Clean Air Constant: %s\n", cratio);
    return ratio;
}


void ADXLI2CInit(void)
{
    /* Using I2C1 I2C on board at the muxed pins of base PORT G
     * Pin 0 - SCL
     * Pin 1 - SDA
     *  */

    /*Enabling the peripheral*/
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

    /* Idling till peripheral is ready to be used */
    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOG));

    /*Enabling the use of GPIO pins PG0 and PG1 as I2C1 SCL and I2C1 SDA using muxing*/
    ROM_GPIOPinConfigure(GPIO_PG0_I2C1SCL);
    ROM_GPIOPinConfigure(GPIO_PG1_I2C1SDA);

    /*Setting up these pins to perform I2C functioning (sets them as open drain pins with weak pull up
     * Ref Consult the data sheet to see which functions are allocated per pin.
     */
    ROM_GPIOPinTypeI2C(GPIO_PORTG_BASE, GPIO_PIN_1);
    GPIOPinTypeI2CSCL(GPIO_PORTG_BASE, GPIO_PIN_0);

    /* Disable, Reset and reEnable I2C Module in Master configuration */
    ROM_SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C1);
    ROM_SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

    /* Idling till the Peripheral is ready */
    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_I2C1));

    /* Initializing and Configuring the Master I2C1 */
    ROM_I2CMasterInitExpClk(I2C1_BASE, SYSTEM_CLOCK, true);     //400kbps

        //
        // Enable Interrupts for Arbitration Lost,  Stop, NAK, Clock Low
        // Timeout and Data.
        //
    //  ROM_I2CMasterIntEnableEx(I2C2_BASE, (I2C_MASTER_INT_ARB_LOST |
    //  I2C_MASTER_INT_STOP | I2C_MASTER_INT_NACK |
    //  I2C_MASTER_INT_TIMEOUT | I2C_MASTER_INT_DATA));

        //
        // Enable the Interrupt in the NVIC from I2C Master
        //
    //  ROM_IntEnable(INT_I2C2);

}

void ADXLSInit(void)
{
    /*set up i2c  for adxl input*/
        ADXLI2CInit();
        UARTprintf("\n\n\n");
        /*reading device id*/
        uint32_t deviceid = readReg(DEVADDR, DEVID);
        UARTprintf("Device ID: 0x%x\n", deviceid);

        /*data format register*/
        /*reading data format reg*/
        uint32_t df = readReg(DEVADDR, DATAFORMAT);
        UARTprintf("Initial Data Format Reg: 0x%x\n", df);

        /*write 0x01 to the data format register to put it into +/- 4G range*/
       uint32_t value = writeReg(DEVADDR,DATAFORMAT, 0x01);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", DATAFORMAT, 0x01);

        /*reading data format reg*/
        df = readReg(DEVADDR, DATAFORMAT);
        UARTprintf("Data Format Reg: 0x%x\n", df);


        /*int enable register*/
        /*reading int enable reg*/
        uint32_t ie = readReg(DEVADDR, INTENABLE);
        UARTprintf("Initial Int Enable Reg: 0x%x\n", ie);

        /*write 0x00 to disable interrupts while setting thresholds*/
        value = writeReg(DEVADDR, INTENABLE, 0x00);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", INTENABLE, 0x00);

        /*reading int enable reg*/
        ie = readReg(DEVADDR, INTENABLE);
        UARTprintf("Int Enable Reg: 0x%x\n", ie);

        /*Set a threshold of 1g for inactivity*/
        uint32_t inactthresh = readReg(DEVADDR, THRESHINACT);
        UARTprintf("InActivity Threshold Value Reg: 0x%x\n", inactthresh);

        value = writeReg(DEVADDR, THRESHINACT, 0x08);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", THRESHINACT, 0x08);

        inactthresh = readReg(DEVADDR, THRESHINACT);
        UARTprintf("InActivity Threshold Value Reg: 0x%x\n", inactthresh);

        /*Set a threshold of 3g for activity*/
        uint32_t actthresh = readReg(DEVADDR, THRESHACT);
        UARTprintf("Activity Threshold Value Reg: 0x%x\n", actthresh);

        value = writeReg(DEVADDR, THRESHACT, 0x0F);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", THRESHACT, 0x0F);

        actthresh = readReg(DEVADDR, THRESHINACT);
        UARTprintf("Activity Threshold Value Reg: 0x%x\n", actthresh);

        /*Raise ACT event immediately (defaults to a 1 second delay)*/
        uint32_t tinact = readReg(DEVADDR, TIMEINACT);
        UARTprintf("Timer for Inactivity Value Reg: 0x%x\n", tinact);

        value =  writeReg(DEVADDR, TIMEINACT,0);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", TIMEINACT, 0x00);

        tinact = readReg(DEVADDR, TIMEINACT);
        UARTprintf("Timer for Inactivity Value Reg: 0x%x\n", tinact);

        /*Consider all axes AND compare THRESH_INACT from 0 acceleration (otherwise it would be differential)*/
        uint32_t actinact = readReg(DEVADDR, ACTINACTCTL);
        UARTprintf("Act Inact Control Reg: 0x%x\n", actinact);

        value = writeReg(DEVADDR, ACTINACTCTL, 0x77);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", ACTINACTCTL, 0x77);

        actinact = readReg(DEVADDR, ACTINACTCTL);
        UARTprintf("Act Inact Control Reg: 0x%x\n", actinact);

        /*Set all interrupts for activity to INT 1 (PIN 2) by writing 0 to all bits. If 1 is written, all interrupts are sent to INT2*/
        uint32_t intmapping = readReg(DEVADDR, INTMAP);
        UARTprintf("Interrupt Map: 0x%x\n",intmapping);

        value = writeReg(DEVADDR, INTMAP,0x00);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", INTMAP, 0x00);

        intmapping = readReg(DEVADDR, INTMAP);
        UARTprintf("Interrupt Map: 0x%x\n",intmapping);


        /* Enable interrupts for activity and inactivity only */
        uint32_t enabledints = readReg(DEVADDR, INTENABLE);
        UARTprintf("Enabled Interrupts: 0x%x\n", enabledints);

        value = writeReg(DEVADDR, INTENABLE, 0x18);
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", INTENABLE, 0x18);

        enabledints = readReg(DEVADDR, INTENABLE);
        UARTprintf("Enabled Interrupts: 0x%x\n", enabledints);

        /*Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register
         * and using linked (serial) activity inactivity detection*/
        value = writeReg(DEVADDR, PWRCTRL, 0x28);  //Measurement mode
        if(value == 0)
            UARTprintf("Finished writing to the 0x%x register value 0x%x.\n", PWRCTRL, 0x28);

        /*read only register is cleared when read*/
        uint8_t intsrc = readReg(DEVADDR, INTSOURCE); //Clear the interrupts from the INT_SOURCE register.
        UARTprintf("Active Interrupts: 0x%x\n", intsrc);

}
// Main function
int main(void)
{
    // Initialize system clock to 120 MHz
    uint32_t output_clock_rate_hz;
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    /*configure uart*/
    UARTtoCOMInit();
    UARTprintf("\n ---- WELCOME ----\n", dist);

    /*configure USS*/
    /*set up gpio*/
    USSGPIOInit();


    /*initialise accelerometer*/
    ADXLSInit();

    /*Set up adc 0 channel 3 - pin e-3*/
    cleangasconstant = calibrateCOS();
    char cac[10];
    sprintf(cac, "%f", cleangasconstant);
    UARTprintf("Clean Air Ratio in Main: %s\n\n\n\n", cac);
    //COSADCInit();

    SysCtlDelay(120000); //Delay by 10us

    // Create tasks
    xTaskCreate(demoLEDTask, (const portCHAR *)"LEDs",
                configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    xTaskCreate(USSTask, (const portCHAR *)"USS",
                configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    xTaskCreate(COSTask, (const portCHAR *)"ALS",
                    configMINIMAL_STACK_SIZE, NULL, 1, NULL);


    xTaskCreate(ADXLTask, (const portCHAR *)"ADXLGYS",
                configMINIMAL_STACK_SIZE, NULL, 1, NULL);



    vTaskStartScheduler();
    return 0;
}

void COSTask(void *pvParameters)
{
    for ( ; ;)
    {
        /*Trigger ADC conversion*/

        ADCProcessorTrigger(ADC0_BASE, 3);

        /*Wait for conversion to complete*/
        while(!ADCIntStatus(ADC0_BASE, 3, false));

        /*Clear ADC interrupt flag*/
        ADCIntClear(ADC0_BASE, 3);

        /*Read ADC value*/
        ADCSequenceDataGet(ADC0_BASE, 3, alcraw);

        /*convert*/
        alcvoltvalue = (float)alcraw[0]/1024.0 * 5.0;
        alcgasvalue = (5.0 - alcvoltvalue)/alcvoltvalue;
        alcratio = alcgasvalue/cleangasconstant;

        /*print the alcohol content*/
        memset(printalc, 0, sizeof(printalc));
        sprintf(printalc, "%f", alcratio);
        /*
         * 280 - sober
         * 280-350 - 1 beer
         * 350- 450 - few beers
         * 450 above - get outa the fucking car
         */

        UARTprintf("Alcohol Content = %s\n", printalc);
        vTaskDelay(1000);
    }
}

//get readings from Ultrasonic Sensor
void USSTask(void *pvParameters)
{


    /*code to trigger input for 10us and read echo output*/
    for( ; ; )
    {
        /*trigger*/
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, ~(GPIO_PIN_6));
       // delayUs(10); //10usDelay
        SysCtlDelay(1200); //Delay by 10us

        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_PIN_6);
       // delayUs(10); //10usDelay
        SysCtlDelay(1200); //Delay by 10us

        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, ~(GPIO_PIN_6));

        /*read echo*/
        distancecounter = 0;
        while(GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_3) == 0)
        {

        }
        while ((GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_3) != 0) & (distancecounter<MAX_TIME))
        {
            distancecounter++;
            SysCtlDelay(120); //Delay by 1us

           // delayUs(1); //1usDelay
        }

        /*convert*/
        distance = ((float)distancecounter/(float)58.2);
        sprintf(dist, "%f", (distance*6));

        /*print the distance in cms*/
        UARTprintf("distance = %s cm\n", dist);

        /*print*/
        vTaskDelay(1000);
    }
}

void ADXLTask(void *pvParameters)
{
    uint8_t intsrc;

    for( ; ;)
    {
        intsrc = 0;
        memset(adxlvalues, 0, sizeof(adxlvalues));

        adxlvalues[0] = readReg(DEVADDR,DATAX0);
        adxlvalues[1] = readReg(DEVADDR,DATAX1);
        adxlvalues[2] = readReg(DEVADDR,DATAY0);
        adxlvalues[3] = readReg(DEVADDR,DATAY1);
        adxlvalues[4] = readReg(DEVADDR,DATAZ0);
        adxlvalues[5] = readReg(DEVADDR,DATAZ1);
        intsrc = readReg(DEVADDR, INTSOURCE); //Clear the interrupts from the INT_SOURCE register.
        if(intsrc == 0x2)
            UARTprintf("Sensed Interrupts: INACTIVITY\n");
        else
            UARTprintf("Sensed Interrupts: ACTIVITY\n"    );
        x = (((int)adxlvalues[1] <<  8) | (int)adxlvalues[0]);
        y = (((int)adxlvalues[3] <<  8) | (int)adxlvalues[2]);
        z = (((int)adxlvalues[5] <<  8) | (int)adxlvalues[4]);
        xg = x * 0.0078;
        yg = y * 0.0078;
        zg = z * 0.0078;
//        UARTprintf("Sensed Values = x:%d, y:%d, z:%d\n", xg, yg, zg);

        vTaskDelay(1000);
    }




}


// Flash the LEDs on the launchpad
void demoLEDTask(void *pvParameters)
{
    for (;;)
    {
        // Turn on LED 1
        LEDWrite(0x0F, 0x01);
        vTaskDelay(1000);

        // Turn on LED 2
        LEDWrite(0x0F, 0x02);
        vTaskDelay(1000);

        // Turn on LED 3
        LEDWrite(0x0F, 0x04);
        vTaskDelay(1000);

        // Turn on LED 4
        LEDWrite(0x0F, 0x08);
        vTaskDelay(1000);
    }
}


// Write text over the Stellaris debug interface UART port
void demoSerialTask(void *pvParameters)
{
    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, 57600, SYSTEM_CLOCK);


    for (;;)
    {
        UARTprintf("\r\nHello, world from FreeRTOS 9.0!");
        vTaskDelay(4000 / portTICK_PERIOD_MS);
    }
}




/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
