/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */


#include "System.h"

#include "StellarisWare.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>


#include "SDCard.h"
#include "ADS1248.h"
#include "DesktopUART.h"
#include "MicroCAT.h"
#include "Optode.h"






/*
 * main.c
 */
int main(void)
{
	short error = 0;  //no errors so far

	//variables for keeping track of user text input:
	char inputBuffer[2];
	char input;
	int charCount = 0;
	int validInput = 0;    //loop boolean

	//miscellaneous helper variables:
	unsigned long startTicks;
	unsigned long elapsedTicks;
	unsigned long ulStatus;


	//UARTprintf("hello\n");
	// Enable lazy stacking for interrupt handlers.  This allows floating-point
	// instructions to be used within interrupt handlers, but at the expense of2
	// extra stack usage.  For SD card.
	ROM_FPULazyStackingEnable();

	//enable floats
	ROM_FPUEnable();

	//set the clock...50MHZ
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	//enable processor interrupts
	ROM_IntMasterEnable();

	//set up the stdio UART1 to the desktop
	openUART1();





	//set up the EEPROM (for variable storage in hibernate mode)
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
	ulStatus = EEPROMInit();
	if( ulStatus != EEPROM_INIT_OK)
	{
		UARTprintf("EEPROM Initialization error.\n");
	}





	handleHibernation();








	//hit CTRL-X within 30 seconds to bring up main menu, else deploy:
	startTicks = ROM_HibernateRTCGet();
	UARTprintf("\nWelcome!\n\n***Press CTRL-X within 30 seconds to bring up the Main Menu***\n");
	while(!validInput)
	{
		//only check input if there has been input
		if(ROM_UARTCharsAvail(UART1_BASE))
		{
			//grab whatever the user inputs
			inputBuffer[0] = ROM_UARTCharGetNonBlocking(UART1_BASE);

			if(inputBuffer[0] == 24)
			{
				validInput = 1;
			}
		}

		//update seconds passed, check if 30 seconds passed
		elapsedTicks = ROM_HibernateRTCGet() - startTicks;		if( (elapsedTicks > 30) && (validInput != 1))
		{
			UARTprintf("Deploy()");
		}
	}



	//Main Menu loop
	while(1)
	{
		//print out the menu:
		UARTprintf("\n\n\nMain Menu--SeaFET ARMageddon");
		UARTprintf("\n1 -- Configure");
		UARTprintf("\n2 -- Deploy");
		UARTprintf("\n3 -- Test");
		UARTprintf("\n4 -- Sleep");
		UARTprintf("\n9 -- Exit to PicoDOS\n");
		UARTprintf("\nEnter Selection (1-4, 9): ");


		//store user input in inputBuffer.  charCount stores # of chars sent
		charCount = UARTgets(inputBuffer, sizeof(inputBuffer));

		input = inputBuffer[0];

		if(charCount == 1)
		{
			switch(input)
			{
				case '1':
					configuration();
					break;
				case '2':
					UARTprintf("Deploy()");
					break;
				case '3':
					test();
					break;
				case '4':
					UARTprintf("sleep()");
					break;
				case '9':
					UARTprintf("Return error....Finish this.");
					break;

				default: UARTprintf("\n\nEnter a valid menu choice (1, 2, 3, 4, or 9).\n");
			}
		}

		else
		{
			UARTprintf("\n\nEnter a valid menu choice (1, 2, 3, 4, or 9).\n");
		}

	}
}







/*
 * Configuration Menu.
 */
void configuration()
{
	//variables for keeping track of user text input:
	char userInput[23];
	char input;
	int charCount = 0;

	unsigned long ticks;  //RTC ticks since epoch

	while(1)
	{
		//print out config menu
		UARTprintf("\n\n\nConfiguration Menu--SeaFET ARMageddon");
		UARTprintf("\n1 -- Set Clock");
		UARTprintf("\n2 -- Set Sample Period");
		UARTprintf("\n3 -- Set AD/C Calibration Parameters");
		UARTprintf("\n9 -- Exit to Main Menu");
		UARTprintf("\nEnter Selection (1-3, 9): ");

		//store user input in inputBuffer.  charCount stores # of chars sent
		charCount = UARTgets(userInput, sizeof(userInput));

		input = userInput[0];

		if(charCount == 1)
		{
			switch(input)
			{
				case '1':  //syncs the RTC to the inputted time

					UARTprintf("\n\n\nEnter the time you'd like the clock to be set to (24 hour format).\n\n");
					UARTprintf("**To enter Jan. 7 2012 1:00 PM 0 seconds, for example, type 01/07/12 13:00:00.\n\n");

					//getDateAndTime gets the date and time from the user and sets the RTC
					getDateAndTime();

					ticks = ROM_HibernateRTCGet();

					//print out the time according to the RTC
					UARTprintf("Time is %s", ctime((const time_t*)&ticks));


					break;

				case '2':  //Set timing (query user for values, save them in EEPROM)
					sys_data.sampling_period = getUnsignedLong("\nEnter sampling period (sec): ");
					sys_data.sampling_average = getLong("\nEnter number of samples to average: ");
					sys_data.pumpon_time = getUnsignedLong("\nEnter time pump stays on (sec): ");

					//copy the sys_data struct into EEPROM.  Last argument ensures # of bytes is multiple of 4
					if(EEPROMProgram((unsigned long*) &sys_data, 0x400, (sizeof(sys_data) + 3) & ~3))
					{
						UARTprintf("Error storing sample period variables.\n");
					}

					break;

				case '3':  //Set battery calibration factor, low voltage threshold (query user for values, save them in EEPROM)
					sys_data.batt1_volt_cal = getFloat("\nEnter main battery calibration factor: ");
					sys_data.batt2_volt_cal = getFloat("\nEnter secondary battery calibration factor: ");
					sys_data.low_batt_volt = getFloat("\nEnter low battery voltage: ");


					//copy the sys_data struct into EEPROM.  Last argument ensures # of bytes is multiple of 4
					if(EEPROMProgram((unsigned long*) &sys_data, 0x400, (sizeof(sys_data) + 3) & ~3))
					{
						UARTprintf("Error storing AD/C Calibration variables.\n");
					}

					break;

				case '9': //Exit
					return;

				default: UARTprintf("\n\nEnter a valid menu choice (1, 2, 3 or 9).\n");
			}
		}

		else
		{
			UARTprintf("\n\nEnter a valid menu choice (1, 2, 3 or 9).\n");
		}
	}
}




/*
 * Test menu.
 */
void test()
{
	//variables for keeping track of user text input:
	char userInput[2];
	char input;
	int charCount = 0;


	//helper varaibles
	long charInput;
	signed long counts = 0;

	while(1)
	{
		UARTprintf("\n\n\nTest Menu--SeaFET ARMageddon\n");
		UARTprintf("1 -- Display Battery Voltage and Temperature\n");
		UARTprintf("2 -- Display pH Sensor A/D Data\n");
		UARTprintf("3 -- Communicate with Instrument\n");
		UARTprintf("4 -- Pump ON\n");
		UARTprintf("5 -- Pump OFF\n");
		UARTprintf("9 -- Exit to Main Menu\n");
		UARTprintf("\nEnter Selection (1-5, 9): ");

		//store user input in inputBuffer.  charCount stores # of chars sent
		charCount = UARTgets(userInput, sizeof(userInput));

		input = userInput[0];

		if(charCount == 1)
		{
			switch(input)
			{
				case '1':
				    // This case looks for temperature and battery voltage  this subsystems are sampled by ADS1248 at : Vtherm-AIN1, ISO-BAT_VOLT-AIN7
				    //openADS1248();
					//SysCtlDelay(ONESEC);

					return;
					break;

				case '2':
					UARTprintf("\nExit on any key.");

					openADS1248();
					SysCtlDelay(ONESEC);

					while(1)
					{
						UARTprintf("\n\n");

						counts = ADS1248GetValue();

						UARTprintf("Counts returned: %d", counts);

						//if user types anything, exit
						if(ROM_UARTCharsAvail(UART1_BASE))
						{
							break;
						}

						//delay so results are easy to read, and ADC has time between conversions (ADS1248GetValue does not delay between conversions)
						ROM_SysCtlDelay(ONESEC);
						ROM_SysCtlDelay(ONESEC);
					}

					closeADS1248();

					break;

				case '3': //Communicate with Instruments
					while(1)
					{
						UARTprintf("\n\n\nMicroCAT and Optode Peripheral Communication Menu\n");
						UARTprintf("1 -- SBE\n");
						UARTprintf("2 -- Optode\n");
						UARTprintf("3 -- HON Pressure Sensor\n");
						UARTprintf("9 -- Exit\n");
						UARTprintf("\nEnter peripheral device selection (1-3, 9): ");

						//store user input in inputBuffer.  charCount stores # of chars sent
						charCount = UARTgets(userInput, sizeof(userInput));

						input = userInput[0];

						if(charCount == 1)
						{
							switch(input)
							{
								case '1':  //SBE

									UARTprintf("\n\n*Press CTRL-X to stop communicating with SBE.*\n\n");

									//set up the MicroCAT
									openMicroCAT();

									//don't want to receive data via an interrupt at this point
									ROM_UARTIntDisable(UART2_BASE, UART_INT_RX);

									while(1)
									{
										//if MicroCAT sent anything, print it out on UART1 (the desktop)
										if(ROM_UARTCharsAvail(UART2_BASE))
										{
											ROM_UARTCharPut(UART1_BASE, UARTCharGet(UART2_BASE));
										}

										//if user types anything, send it to the MicroCAT--exit on CTRL-X
										if(ROM_UARTCharsAvail(UART1_BASE))
										{
											charInput = UARTCharGet(UART1_BASE);

											if(charInput == 24)
											{
												break;
											}

											UARTCharPut(UART2_BASE, charInput);
										}
									}
									//close up the MicroCAT
									closeMicroCAT();
									break;


								case '2': //Optode

									UARTprintf("\n\n*Press CTRL-X to stop communicating with the Optode.*\n\n");

									//set up the optode
									openOptode();

									//don't want to receive data via an interrupt at this point
									ROM_UARTIntDisable(UART3_BASE, UART_INT_RX);

									while(1)
									{
										//if Optode sent anything, print it out on UART1 (the desktop)
										if(ROM_UARTCharsAvail(UART3_BASE))
										{
											charInput = UARTCharGet(UART3_BASE);

											//add a line feed if Optode sends a CR
											if(charInput == 13)
											{
												ROM_UARTCharPut(UART1_BASE, charInput);
												ROM_UARTCharPut(UART1_BASE, 10);
											}

											else
											{
												ROM_UARTCharPut(UART1_BASE, charInput);
											}
										}

										//if user types anything, send it to the Optode--exit on CTRL-X
										if(ROM_UARTCharsAvail(UART1_BASE))
										{
											charInput = UARTCharGetNonBlocking(UART1_BASE);

											//exit on CTRL-X:
											if(charInput == 24)
											{
												break;
											}


											//add a line feed when user hits return
											if(charInput == 13)
											{
												ROM_UARTCharPut(UART3_BASE, charInput); //the CR
												ROM_UARTCharPut(UART3_BASE, 10);        //the line feed
												ROM_UARTCharPut(UART1_BASE, 10);
											}

											else
											{
												ROM_UARTCharPut(UART3_BASE, charInput);
											}

											//do local echo over UART1
											ROM_UARTCharPut(UART1_BASE, charInput);
										}
									}
									//close up the Optode
									closeOptode();
									break;

								case '3': //HON Pressure Sensor
									return;
									break;

								case '9': //Exit
									return;

								default: UARTprintf("Enter a valid selection (1, 2, 3, or 9)");
							}
						}

						else
						{
							UARTprintf("Enter a valid selection (1, 2, 3, or 9).");
						}
					}


				case '4':  //Pump on
					return;
					break;

				case '5':  //Pump off
					return;
					break;

				case '9': //Exit
					return;

				default: UARTprintf("Enter a valid selection (1, 2, 3, or 9).");
			}
		}

		else
		{
			UARTprintf("Enter a valid selection (1, 2, 3, or 9).");
		}
	}
}





/*
 * handleHibernation enables the hibernate peripheral then
 * determines what woke the module (wake pin, RTC match, cold boot).
 * On a cold boot (aka first run) the RTC is enabled, but not set.
 */
void handleHibernation()
{
	unsigned long ulStatus;

	// Need to enable the Hibernation peripheral after wake/reset, before using it.
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);

	//HibernateIsActive() returns true if the system woke from hibernate. false on cold restart
	if(ROM_HibernateIsActive())
	{
		//Read the status to determine cause of wake.
		ulStatus = ROM_HibernateIntStatus(false);

		//Wakeup from wake pin?
		if(ulStatus & HIBERNATE_INT_PIN_WAKE)
		{
			UARTprintf("Wake pin\n");
			//change wokenFromHibernate variable to true
		}

		//Wakeup by RTC match?
		if(ulStatus & HIBERNATE_INT_RTC_MATCH_0)
		{
			UARTprintf("RTC Match.\n");
			//change wokenFromHibernate variable to true
		}
	}

	//must be a cold restart, set up the hibernation module (do not set the RTC, though)
	else
	{
		UARTprintf("Cold Restart.\n");

		//enable Hibernation module...it's given the clock rate of the processor
		ROM_HibernateEnableExpClk(SysCtlClockGet());

		//delay for crystal stabalization (check data sheet for actual time needed)
		ROM_SysCtlDelay(ONESEC);

		//use the raw signal from the 32.768 kHz oscillator
		ROM_HibernateClockSelect(HIBERNATE_CLOCK_SEL_RAW);

		ROM_HibernateRTCEnable();
	}
}






/*
 * getDateAndTime queries the user for a date and time in 24 hour format.
 * It syncs the RTC with their given time assuming their time is a valid
 * one.  (See time.h header.)
 */
void getDateAndTime()
{
	char monthString[3];
	char dayString[3];
	char yearString[3];
	char hourString[3];
	char minuteString[3];
	char secondString[3];

	char *p;  //helper variable

	//variables for keeping track of user text input:
	char timeInput[23];
	int charCount;

	int month;
	int day;
	int year;
	int hour;
	int minute;
	int second;

	int validInput = 1; //helper boolean

	//time variables:
	time_t rawtime;
	struct tm * timeStruct;



	//loop until given a valid time string
	while(1)
	{
		UARTprintf("Your time MM/DD/YY HH:MM:SS ");

		validInput = 1;

		//store user input in timeInput.  charCount stores # of chars sent
		charCount = UARTgets(timeInput, sizeof(timeInput));


		//make sure the user has at least entered the 17 chars of MM/DD...
		if( charCount == 17)
		{
			//now check for appropriate punctuation chars
			if(timeInput[2] == '/' && timeInput[5] == '/' && isspace(timeInput[8]) && timeInput[11] == ':' && timeInput[14] == ':')
			{
				//break up the string into it's individual pieces (month, day...):
				strncpy(monthString, &timeInput[0], 2 );
				monthString[2] = '\0';

				strncpy(dayString, &timeInput[3], 2 );
				dayString[2] = '\0';

				strncpy(yearString, &timeInput[6], 2 );
				yearString[2] = '\0';

				strncpy(hourString, &timeInput[9], 2 );
				hourString[2] = '\0';

				strncpy(minuteString, &timeInput[12], 2 );
				minuteString[2] = '\0';

				strncpy(secondString, &timeInput[15], 2 );
				secondString[2] = '\0';


				//Make sure all the strings convert to an int:
				errno = 0;
				p = monthString;
				month = (int)strtol( monthString, &p, 10);
				if( (errno != 0) || (monthString == p) || (*p != 0))
				{
					// conversion failed (EINVAL, ERANGE)
					// conversion failed (no characters consumed)
					// conversion failed (trailing data)
					validInput = 0;
				}

				errno = 0;
				p = dayString;
				day = (int)strtol( dayString, &p, 10);
				if( (errno != 0) || (dayString == p) || (*p != 0))
				{
					validInput = 0;
				}

				errno = 0;
				p = yearString;
				year = (int)strtol( yearString, &p, 10);
				if( (errno != 0) || (yearString == p) || (*p != 0))
				{
					validInput = 0;
				}

				errno = 0;
				p = hourString;
				hour = (int)strtol( hourString, &p, 10);
				if( (errno != 0) || (hourString == p) || (*p != 0))
				{
					validInput = 0;
				}

				errno = 0;
				p = minuteString;
				minute = (int)strtol( minuteString, &p, 10);
				if( (errno != 0) || (minuteString == p) || (*p != 0))
				{
					validInput = 0;
				}

				errno = 0;
				p = secondString;
				second = (int)strtol( secondString, &p, 10);
				if( (errno != 0) || (secondString == p) || (*p != 0))
				{
					validInput = 0;
				}


				//if all the fields are indeed integers...
				if(validInput)
				{
					if( second >= 0 && minute >= 0 && hour >= 0 && month >= 1 && day >= 1 && year >= 0)
					{
						if( second <= 61 && minute <= 59 && hour <= 23 && day <= 31 && month <= 12)
						{
							//fill in time.h's tm struct initially (timeStruct just points to time.h's struct)
							time(&rawtime);
							timeStruct = localtime(&rawtime);

							//modify as user desired
							timeStruct->tm_sec = second;
							timeStruct->tm_min = minute;
							timeStruct->tm_hour = hour;
							timeStruct->tm_mday = day;
							timeStruct->tm_mon = month - 1;
							timeStruct->tm_year = (year + 2000) - 1900;


							//remake the struct
							rawtime = mktime(timeStruct);

							//print out the time the user entered
							//UARTprintf( "\n\nTime is: %s", asctime(timeStruct));

							//set the RTC to this current time so we can keep track of their time
							ROM_HibernateRTCSet(rawtime);

							return;
						}
					}
				}
			}
		}

		UARTprintf("Enter a valid time string as MM/DD/YY HH:MM::SS.\n\n\n");

	}

}











/*
 * getLong reads user input and determines whether or not they
 * entered a long.  If they did, it returns that long. Otherwise
 * it prompts them until they do. It prints out whatever
 * prompt it is passed.
 */
long getLong(char *prompt)
{
	char inputBuffer[24];

	signed long val;     //our return value


	//print out the user's prompt
	UARTprintf("%s", prompt);


	while(1)
	{
		//store user input in inputBuffer
		UARTgets(inputBuffer, sizeof(inputBuffer) );


		//convert to long, query user to enter a new value if not given valid input
		errno = 0;
		char *p  = inputBuffer;
		val = strtol( inputBuffer, &p, 10);

		if( (errno != 0) || (inputBuffer == p) || (*p != 0))
		{
			// conversion failed (EINVAL, ERANGE)
			// conversion failed (no characters consumed)
			// conversion failed (trailing data)
			UARTprintf("Enter a valid value (no decimals allowed for this factor): ");

		}

		//otherwise...conversion was successful...exit loop and return val
		else
		{
			return val;
		}

	}
}









/*
 * getULong reads user input and determines whether or not they
 * entered a ulong.  If they did, it returns that ulong. Otherwise
 * it prompts them until they do. It prints out whatever
 * prompt it is passed.
 */
unsigned long getUnsignedLong(char *prompt)
{
	char inputBuffer[24];
	unsigned long val;     //our return value


	//print out the user's prompt
	UARTprintf("%s", prompt);


	while(1)
	{
		//store user input in inputBuffer
		UARTgets(inputBuffer, sizeof(inputBuffer) );


		//convert to ulong, query user to enter a new value if not given valid input
		errno = 0;
		char *p  = inputBuffer;
		val = strtoul( inputBuffer, &p, 10);

		if( (errno != 0) || (inputBuffer == p) || (*p != 0))
		{
			// conversion failed (EINVAL, ERANGE)
			// conversion failed (no characters consumed)
			// conversion failed (trailing data)
			UARTprintf("Enter a valid value (no decimals allowed for this factor): ");
		}

		//otherwise...conversion was successful...exit loop and return val
		else
		{
			return val;
		}

	}
}




/*
 * getFloat reads user input and determines whether or not they
 * entered a float.  If they did, it returns that float. Otherwise
 * it prompts them until they do. It prints out whatever
 * prompt it is passed.
 */
float getFloat(char *prompt)
{
	char inputBuffer[24];
	float val;           //our return value


	//print out the user's prompt
	UARTprintf("%s", prompt);


	while(1)
	{
		//store user input in inputBuffer
		UARTgets(inputBuffer, sizeof(inputBuffer) );


		//convert to double, query user to enter a new value if not given valid input
		errno = 0;
		char *p  = inputBuffer;
		val = (float)strtod( inputBuffer, &p);

		if( (errno != 0) || (inputBuffer == p) || (*p != 0))
		{
			// conversion failed (EINVAL, ERANGE)
			// conversion failed (no characters consumed)
			// conversion failed (trailing data)
			UARTprintf("Enter a valid value: ");
		}

		//otherwise...conversion was successful...exit loop and return val
		else
		{
			return val;
		}
	}
}
