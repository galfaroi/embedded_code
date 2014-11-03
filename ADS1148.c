/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */



#include "ADS1148.h"
#include "StellarisWare.h"





/*
 * ADS1148.c is the "driver" for the ADS1148.  It assumes 20SPS is
 * desired and a 50MHZ processor clock is used.
 *
 * openADS1148(): gets the ADS1148 ready for conversions; POWERS THE ADC, ties
 * start and reset high, sets up the SSI lines, sets ADC to 20SPS,
 * default channel, and SDATAC mode. 24 bit counts can be asked for immediately
 * upon openADS1148()s return.
 * pollADS1148(short channel, int trials): gets trials # conversions from the specified
 * channel (0,1,2 or 3).  The counts from all the trials are averaged and returned
 * as a float.
 * closeADS1148(): POWERS OFF THE ADC, ties start and reset to low, and disables
 * SSI3.
 *
 * ADS1148GetValue() returns just 1 count. ADS1148GetValue() does not include
 * any delays...the user should be sure to give appropriate delays between conversions.
 * ADS1148ChangeChannel(...) allows you to switch between the 4 differential
 * channels (0,1,2,3).
 */





/*
 * openADS1148() performs initial set up of the ADS1148.
 * POWERS THE ADC, ties start and reset high, sets up the SSI, sets
 * the sampling rate to 20SPS, sets up the internal
 * reference for use, and puts the ADS1148 in SDATAC mode. Conversions
 * can be asked for immediately after this function returns (no additional
 * delays are needed).
 *
 * For safety, it assumes the ADS1148 was just booted and gives the necessary
 * delays as the ADS1148 boots up.
 */
void openADS1148(void)
{
	//set up ADS1148's GPIO enable pin (pin 71)
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_4);


	//set up start and reset pins
	//THESE ARE NOT THE SAME AS THE ONES CURRENTLY ON THE SCHEMATIC
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_4);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_5);

	//tie start and reset high
	ROM_GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_4, 0xff);
    ROM_GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_5, 0xff);




	//turn power on to ADS1148
	ADS1148_ON;

	//set up the SSI3 line
	setUpSSIADS1148();


	//Assume we're at default 5SPS and START pin just went high, this delay lets the filter reset
	ROM_SysCtlDelay(SPS5DELAY);

	//Send the stop reading data continuously command (so we can read registers and get conversions by sending RDATA)
	ROM_SSIDataPut(SSI3_BASE, SDATAC);

	//let the filter reset
	ROM_SysCtlDelay(SPS5DELAY);

	//write to SYS0 to change SPS
	ROM_SSIDataPut(SSI3_BASE, 0x43);
	ROM_SSIDataPut(SSI3_BASE, 0x00);  //0x00 is # of bytes to write minus 1
	ROM_SSIDataPut(SSI3_BASE, 0x02);  //20 SPS

	//let digital filter reset
	ROM_SysCtlDelay(SPS20DELAY);

	//read back that register
	ROM_SSIDataPut(SSI3_BASE, 0x23);
	ROM_SSIDataPut(SSI3_BASE, 0x00); //0x00 is # of bytes to write minus 1
	ROM_SSIDataPut(SSI3_BASE, NOP);  //1st NOP issues 8 SCLK's to get 1 byte (should get 0xFF back)



	//write to MUX1 to set up internal reference
	ROM_SSIDataPut(SSI3_BASE, 0x42);
	ROM_SSIDataPut(SSI3_BASE, 0x00); //0x00 is # of bytes to write minus 1
	ROM_SSIDataPut(SSI3_BASE, 0x30); //0x30 sets up internal reference for use

	//let filter reset before trying to get conversions. THIS DELAY MAY BE BREAKING EVERYTHING
	//ROM_SysCtlDelay(SPS20DELAY);

	//read back that register
	ROM_SSIDataPut(SSI3_BASE, 0x22);
	ROM_SSIDataPut(SSI3_BASE, 0x00); //0x00 is # of bytes to write minus 1
	ROM_SSIDataPut(SSI3_BASE, NOP);  //1st NOP issues 8 SCLK's to get 1 byte (should get 0xFF back)



	/*
	 * USE REF1 Input pair
	 */
	/*
	//write to MUX1
	ROM_SSIDataPut(SSI3_BASE, 0x42);
	ROM_SSIDataPut(SSI3_BASE, 0x00); //0x00 is # of bytes to write minus 1
	ROM_SSIDataPut(SSI3_BASE, 0x28); //0x28 sets up Ref1 for the ref, otherwise normal operation

	//let filter reset before trying to get conversions
	ROM_SysCtlDelay(SPS20DELAY);
	 */

	/*
	 * TEST READ REGISTER
	 */
/*
	//write to MUX1
	ROM_SSIDataPut(SSI3_BASE, 0x42);
	ROM_SSIDataPut(SSI3_BASE, 0x00); //0x00 is # of bytes to write minus 1
	ROM_SSIDataPut(SSI3_BASE, 0x30); //should read ox30 back

	//let filter reset before trying to get conversions
	//ROM_SysCtlDelay(SPS20DELAY);




	//send read register command
	ROM_SSIDataPut(SSI3_BASE, 0x22);
	ROM_SSIDataPut(SSI3_BASE, 0x00); //0x00 is # of bytes to write minus 1
	ROM_SSIDataPut(SSI3_BASE, NOP);  //1st NOP issues 8 SCLK's to get 1 byte (should get 0xFF back)

	ROM_SysCtlDelay(SPS20DELAY);
*/





	/*
	 * TEST WITH THE SYSTEM MONITOR.
	 */
/*
	//write to MUX1
	ROM_SSIDataPut(SSI3_BASE, 0x42);
	ROM_SSIDataPut(SSI3_BASE, 0x00); //0x00 is # of bytes to write minus 1
	ROM_SSIDataPut(SSI3_BASE, 0x33); //0x33 uses int ref and does temp diode

	//let filter reset before trying to get conversions
	ROM_SysCtlDelay(SPS20DELAY);



	UARTprintf("SysMonitor: \n");
	UARTprintf("Taking 3 measurements...\n");
	signed long w;
	int x = 3;
	while(x)
	{
		SysCtlDelay(ONESEC);
		w = ADS1148GetValue();
		UARTprintf("%d\n",w);
		--x;
	}

	closeADS1148();
	while(1)
	{
	}
*/



	/*
	 * Calibration commands
	 */
	/*
    //send the SELFOCAL command for calibration
    ROM_SSIDataPut(SSI3_BASE, SELFOCAL);

    //allow the ADS1148 proper calibration time
    ROM_SysCtlDelay(SPS20CALIBRATIONDELAY);
	 */

}




/*
 * pollADS1148() returns the average number of counts from a user
 * specified number of conversions (trials).  User can specify
 * the channel (0,1,2, or 3).
 * This function ASSUMES THE ADC IS RUNNING AT 20SPS and
 * delays appropriately between conversions.
 */
float pollADS1148(short channel, int trials)
{
	//change to the specified channel
	ADS1148ChangeChannel(channel);

	long sample = 0;
	int i;


	//make trials # of conversions
	for (i = 0; i < trials; i++)
	{
		sample += ADS1148GetValue();

		//next conversion will be ready after about 50ms
		ROM_SysCtlDelay(SPS20DELAY);
	}

	return (float) (sample/trials);
}





/*
 * closeADS1148 turns off start, reset, power and disables
 * SSI3.
 */
void closeADS1148(void)
{
	//disable SSI3
	ROM_SSIDisable(SSI3_BASE);


	//TIE START AND RESET LOW
	//THESE ARE NOT THE SAME PINS THAT ARE CURRENTLY ON THE SCHEMATIC
	ROM_GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_4, 0x00);
    ROM_GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_5, 0x00);

	//turn off power
	ADS1218_OFF;
}









/*
 * Selects between the 4 differential channels and delays to let
 * the filter reset (ASSUMES 20SPS OPERATION).
 * The only acceptable arguments are 0, 1, 2, and 3:
 * 0 selects AIN0 as positive, AIN1 as negative.
 * 1 selects AIN2 as positive, AIN3 as negative.
 * 2 selects AIN4 as positive, AIN5 as negative...
 */
void ADS1148ChangeChannel(short channel)
{
	switch(channel)
	{
		//+AIN0, -AIN1
		case 0:
		{
			ROM_SSIDataPut(SSI3_BASE, 0x40); //write to MUX0 register to change channels
			ROM_SSIDataPut(SSI3_BASE, 0x00); //0x00 is the # of bytes to write minus 1
			ROM_SSIDataPut(SSI3_BASE, 0x01); //0x00 means AIN0 is positive, AIN1 is negative (default)
			break;
		}

		//+AIN2, -AIN3
		case 1:
		{
			ROM_SSIDataPut(SSI3_BASE, 0x40);
			ROM_SSIDataPut(SSI3_BASE, 0x00);
			ROM_SSIDataPut(SSI3_BASE, 0x13); //0x13 means +AIN2, -AIN3
			break;
		}

		//+AIN4, -AIN5
		case 2:
		{
			ROM_SSIDataPut(SSI3_BASE, 0x40);
			ROM_SSIDataPut(SSI3_BASE, 0x00);
			ROM_SSIDataPut(SSI3_BASE, 0x25); //0x25 means +AIN4, -AIN5
			break;
		}

		//+AIN6, -AIN7
		case 3:
		{
			ROM_SSIDataPut(SSI3_BASE, 0x40);
			ROM_SSIDataPut(SSI3_BASE, 0x00);
			ROM_SSIDataPut(SSI3_BASE, 0x37); //0x37 means +AIN6, -AIN7
			break;
		}

	}

	//let the digital filter reset
	ROM_SysCtlDelay(SPS20DELAY);

}



/*
 * ADS1148GetValue() retrieves a conversion from the ADC.  It assumes
 * the ADC is operating in SDATAC mode.  It does not include any delays;
 * instead it assumes the ADC will be given proper wait/set-up delays
 * elsewhere in the code.
 */
signed long ADS1148GetValue(void)
{
	//the last 3 cells each store a byte of the 24 bit count
	unsigned long counts[4];


	unsigned long garbage[1];

	//our 24 bit count
	signed long result;

	int i;


	// Flush receive FIFO
	while(ROM_SSIDataGetNonBlocking(SSI3_BASE, &garbage[0] ))
	{
	}

	//send RDATA command
	ROM_SSIDataPut(SSI3_BASE, RDATA);
	ROM_SSIDataPut(SSI3_BASE, NOP);  //1st NOP issues 8 SCLK's to get 1st byte of the count
	ROM_SSIDataPut(SSI3_BASE, NOP);  //2nd NOP for 2nd...
	ROM_SSIDataPut(SSI3_BASE, NOP);


	// Wait until SSI is done transferring all the data in the transmit FIFO.
	while(ROM_SSIBusy(SSI3_BASE))
	{
	}

	//get the 4 bytes that come back (0x00 is received after we send RDATA--it is stored in counts[0])
	for(i = 0; i < 4; ++i)
	{
		//This fcn waits until there is data in the receive FIFO before returning.
		ROM_SSIDataGet(SSI3_BASE, &counts[i]);

		//Since we are using 8-bit data, mask off the MSB.
		counts[i] &= 0x00FF;
	}



	//concatenate the 3 bytes of the 24 bit count (the 6 nibbles from the ADC)
	result =  (counts[1] << 16) | (counts[2] << 8 ) | counts[3];


	// we need to sign the extra two nibbles from the 32 bit long in case of being a signed number,  because bit 23 could be the signed "-" of the negative voltage
	//the bit to check either 1 or 0
  //  long result_;
//
/*	long bitcheck;
	bitcheck = 0x00800000;


		if ( result & (1<<23) == bitcheck )
	{

*/
		result = 0xff000000 | result;



//	}


	return(result);
}






/*
 * Configures the SSI2 line to the ADS1148.
 */
void setUpSSIADS1148(void)
{
	unsigned long ulDataRx[1];

	//SSI2 runs to ADS1148
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);


	// Configure the pin muxing
	ROM_GPIOPinConfigure(GPIO_PK0_SSI3CLK);
	ROM_GPIOPinConfigure(GPIO_PK1_SSI3FSS);
	ROM_GPIOPinConfigure(GPIO_PK2_SSI3RX);
	ROM_GPIOPinConfigure(GPIO_PK3_SSI3TX);


	// Configure the GPIO settings for the SSI pins.
	ROM_GPIOPinTypeSSI(GPIO_PORTK_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 |
			GPIO_PIN_0);


	// Configure and enable the SSI port for SPI master mode.
	//MOTO_MODE_1 is what we want to use for the ADS1148!!
	ROM_SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_1,
			SSI_MODE_MASTER, 1000000, 8);


	//Enable the SSI2 module.
	ROM_SSIEnable(SSI3_BASE);



	// Flush receive FIFO before starting to talk to ADS1148.
	while(ROM_SSIDataGetNonBlocking(SSI3_BASE, &ulDataRx[0]))
	{
	}
}
