/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */

#ifndef DESKTOPUART_H_
#define DESKTOPUART_H_



//Use this macro to turn UART1 OFF!
#define UART1_OFF ROM_UARTDisable(UART1_BASE)
#define UART1_ON openUART1(void)


/*
 * DesktopUART.c function prototypes:
 */
void openUART1(void);
void closeUART1(void);





#endif /* DESKTOPUART_H_ */
