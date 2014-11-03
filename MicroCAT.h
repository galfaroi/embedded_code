/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */
#ifndef MICROCAT_H_
#define MICROCAT_H_



//toggle the MicroCAT GPIO power enable pin (pin 80)--POWER SWITCHING IS DONE FOR YOU IN openMicroCAT() and closeMicroCAT()!
#define MICROCAT_ON ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0xff)
#define MICROCAT_OFF ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x00)




/*
 * MicroCAT.c function prototypes:
 */
void openMicroCAT(void);
void pollMicroCAT(void);
void closeMicroCAT(void);


void UART2IntHandler(void);
void UART2Init(void);
void UARTSendMicroCAT(unsigned long ulBase, const char *pucBuffer);






#endif /* MICROCAT_H_ */
