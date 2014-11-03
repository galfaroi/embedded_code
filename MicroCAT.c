/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */


#include "MicroCAT.h"
#include "StellarisWare.h"
#include <string.h>






/*
 * MicroCAT.c:
 * openMicroCAT() POWERS and sets up the UART to the MicroCAT
 * pollMicroCAT() polls the MicroCAT
 * closeMicroCAT() REMOVES POWER and disables the UART to the MicroCAT
 *
 * Replies from the MicroCAT are received via the UART2 interrupt handler
 * which is defined
 * here too.
 */





/*
 * Set up the MicroCAT (give it power and set up UART2).
 * DO THIS BEFORE POLLING.
 */
void openMicroCAT(void)
{
	//set up the MicroCAT's GPIO enable pin (pin 80)
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

	//turn on power
	MICROCAT_ON;

	//UART2 runs to the MicroCAT, set it up:
	UART2Init();
}



/*
 * pollMicroCAT() wakes up the microCAT with a carriage return
 * and then sends "tss" to the microCAT.  Data is received
 * via the UART2 interrupt handler.
 */
void pollMicroCAT()
{
	UARTSendMicroCAT(UART2_BASE, (char *)"\n\r");


	//give MicroCAT a second to wake up
	ROM_SysCtlDelay(ONESEC);


	//send "tss\n\r"
	UARTSendMicroCAT(UART2_BASE, (char *)"tss\n\r");

	//make sure MicroCAT has time to reply
	ROM_SysCtlDelay(ONESEC);
	ROM_SysCtlDelay(ONESEC);
	ROM_SysCtlDelay(ONESEC);
}



/*
 * closeMicroCAT turns off the MicroCAT (removes power
 * and disables UART2).
 */
void closeMicroCAT(void)
{
	//disable UART2 and its interrupt
	ROM_UARTIntDisable(UART2_BASE, UART_INT_RX);
	ROM_UARTDisable(UART2_BASE);

	//turn off power
	MICROCAT_OFF;
}






/*
 * This is the UART2RX interrupt handler for the MicroCAT
 * The handler simply writes characters out to the desktop.
 */
void UART2IntHandler(void)
{
	unsigned long ulStatus;

	// Get the interrupt status.
	ulStatus = ROM_UARTIntStatus(UART2_BASE, true);

	// Clear the asserted interrupts.
	ROM_UARTIntClear(UART2_BASE, ulStatus);


	// Loop while there are characters in the receive FIFO
	while(ROM_UARTCharsAvail(UART2_BASE))
	{
		// Read the next character from the UART and write it to desktop.
		ROM_UARTCharPutNonBlocking(UART1_BASE, ROM_UARTCharGetNonBlocking(UART2_BASE));
	}
}



/*
 * UART2 is used to communicate with the MicroCAT.  Set UART2
 * to 9600 baud, 8, N, 1.  Also, set up and enable the RX FIFO interrupt
 * to receive data.
 */
void UART2Init(void)
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);   //enable the GPIO where the UART is
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);  //UART2
	ROM_GPIOPinConfigure(GPIO_PG4_U2RX);			  //set up the pins
	ROM_GPIOPinConfigure(GPIO_PG5_U2TX);
	ROM_GPIOPinTypeUART(GPIO_PORTG_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	//9600 baud, 8, N, 1
	ROM_UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	ROM_UARTEnable(UART2_BASE);



	//RX interrupt will occur when FIFO is 1/8 full
	ROM_UARTFIFOLevelSet(UART2_BASE, UART_FIFO_TX7_8, UART_FIFO_RX1_8);


	//enable the interrupt
	ROM_IntEnable(INT_UART2);

	//flush the RX fifo before enabling the RX interrupt to get rid of residual data
	while(ROM_UARTCharsAvail(UART2_BASE))
	{
		ROM_UARTCharGetNonBlocking(UART2_BASE);
	}


	//Enable the UART peripheral RX interrupt
	ROM_UARTIntEnable(UART2_BASE, UART_INT_RX);
}







/*
 * UARTSend sends a string to the specified UART module.
 * The first argument is the module you'd like to send from
 * (UART0_BASE, UART1_BASE...). Here is an example function
 * call that sends "Enter text: " through UART2:
 * UARTSend(UART2_BASE, (char *)"Enter text: ");
 */
void UARTSendMicroCAT(unsigned long ulBase, const char *pucBuffer)
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

