/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */

#ifndef OPTODE_H_
#define OPTODE_H_


//toggle the Optode GPIO power enable pin (pin 81)--POWER SWITCHING IS DONE FOR YOU IN openOptode() and closeOptode()!
#define OPTODE_ON ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0xff)
#define OPTODE_OFF ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x00)



/*
 * Optode.c function prototypes:
 */
void openOptode(void);
void pollOptode(void);
void closeOptode(void);


void UART3IntHandler(void);
void UART3Init(void);
void UARTSendOptode(unsigned long ulBase, const char *pucBuffer);




#endif /* OPTODE_H_ */
