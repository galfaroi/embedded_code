/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */




#include "Optode.h"
#include "StellarisWare.h"
#include "string.h"





//SEE OPTODE MANUAL PAGES 17-19 TO LEARN MORE ABOUT RESPONSE TIMES
//AND ABOUT XON XOFF HANDSHAKES


/*
 * Optode.c:
 * openMicroCAT() POWERS and sets up the UART to the MicroCAT
 * pollMicroCAT() polls the MicroCAT
 * closeMicroCAT() REMOVES POWER and disables the UART to the MicroCAT
 *
 * Replies from the Optode are received via the UART3
 * interrupt handler which is defined here too.
 */







/*
 * Set up the Optode (give it power and set up UART3).
 * DO THIS BEFORE POLLING.
 */
void openOptode(void)
{
	//set up the Optode's GPIO enable pin (pin 81)
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

	//turn on power
	OPTODE_ON;

	//UART3 runs to the Optode, set it up:
	UART3Init();
}



/*
 * pollOptode() tells the Optode to "do_sample."  Data is received
 * via the UART3 interrupt handler.
 */
void pollOptode()
{
	//send "do_sample\n\r" to Optode
	UARTSendOptode(UART3_BASE, (char *)"do_sample\n\r");


	//SEE OPTODE MANUAL PAGES 17-19 TO LEARN MORE ABOUT RESPONSE TIMES
	//AND ABOUT XON XOFF HANDSHAKES
	//give optode time to respond
	ROM_SysCtlDelay(ONESEC);
	ROM_SysCtlDelay(ONESEC);
}



/*
 * closeOptode turns off the Optode (removes power
 * and disables UART3).
 */
void closeOptode(void)
{
	//disable UART3 and its interrupt
	ROM_UARTIntDisable(UART3_BASE, UART_INT_RX);
	ROM_UARTDisable(UART3_BASE);

	//turn off power
	OPTODE_OFF;
}






/*
 * This is the UART3RX interrupt handler for the Optode
 * The handler simply writes characters out to the desktop.
 */
void UART3IntHandler(void)
{
	unsigned long ulStatus;

	// Get the interrupt status.
	ulStatus = ROM_UARTIntStatus(UART3_BASE, true);

	// Clear the asserted interrupts.
	ROM_UARTIntClear(UART3_BASE, ulStatus);


	// Loop while there are characters in the receive FIFO
	while(ROM_UARTCharsAvail(UART3_BASE))
	{
		// Read the next character from the UART and write it to desktop.
		ROM_UARTCharPutNonBlocking(UART1_BASE, ROM_UARTCharGetNonBlocking(UART3_BASE));
	}
}



/*
 * UART3 is used to communicate with the Optode.  Set UART3
 * to 9600 baud, 8, N, 1.  Also, set up and enable the RX FIFO interrupt
 * to receive data.
 */
void UART3Init(void)
{
	// set up UART3 to the optode
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
	ROM_GPIOPinConfigure(GPIO_PC6_U3RX);              //set up the pins
	ROM_GPIOPinConfigure(GPIO_PC7_U3TX);
	ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

	//9600 baud, 8, N, 1
	ROM_UARTConfigSetExpClk(UART3_BASE, ROM_SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	// Enable the UART.
	ROM_UARTEnable(UART3_BASE);


	//RX interrupt will occur when FIFO is 1/8 full
	ROM_UARTFIFOLevelSet(UART3_BASE, UART_FIFO_TX7_8, UART_FIFO_RX1_8);

	//enable the interrupt
	ROM_IntEnable(INT_UART3);

	//flush the RX fifo before enabling the RX interrupt to get rid of residual data
	while(ROM_UARTCharsAvail(UART3_BASE))
	{
		ROM_UARTCharGetNonBlocking(UART3_BASE);
	}


	//Enable the UART peripheral interrupt...receive
	ROM_UARTIntEnable(UART3_BASE, UART_INT_RX);
}







/*
 * UARTSend sends a string to the specified UART module.
 * The first argument is the module you'd like to send from
 * (UART0_BASE, UART1_BASE...). Here is an example function
 * call that sends "Enter text: " through UART2:
 * UARTSend(UART2_BASE, (char *)"Enter text: ");
 */
void UARTSendOptode(unsigned long ulBase, const char *pucBuffer)
{

	//ulCount is the length of the passed in string
	unsigned long ulCount;

	ulCount = strlen(pucBuffer);


	// Loop while there are more characters to send.
	while(ulCount--)
	{
		// Write the next character to the UART.
		ROM_UARTCharPut(ulBase, *pucBuffer++);
	}
}

