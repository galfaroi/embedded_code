/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */



#include "DesktopUART.h"
#include "StellarisWare.h"




/*
 * USE THE MACRO IN THE HEADER TO TURN UART1 OFF!
 *
 *
 * DesktopUART.c allows you to open ("set-up") and close UART1
 * which is the UARTstdio module and the main serial line to the
 * desktop.
 */





/*
 * Set up UART1 as stdio.
 * 9600, 8, N, 1
 */
void openUART1(void)
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);  //UART1 is on GPIOC
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);  //it's UART1
	ROM_GPIOPinConfigure(GPIO_PC4_U1RX);              //configure the pins
	ROM_GPIOPinConfigure(GPIO_PC5_U1TX);
	ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);


	//UART1 is the stdio module
	UARTStdioInitExpClk(1, 9600);
}



/*
 * Disable UART1.
 */
void closeUART1(void)
{
	ROM_UARTDisable(UART1_BASE);
}
