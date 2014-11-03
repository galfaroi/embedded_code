/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */


#ifndef ADS1148_H_
#define ADS1148_H_



//toggle ADS1148 GPIO power enable pin (pin 71)--POWER SWITCHING IS DONE FOR YOU IN openADS1248() and closeADS1248()!
#define ADS1148_ON ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4, 0xff)
#define ADS1148_OFF ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4, 0x00)



#define SDATAC 0x16     //stop reading data continuously command for ADS1248
#define RDATA 0x12      //read data command
#define NOP 0xFF        //no operation command
#define SELFOCAL 0x62   //self offset calibration command


/*
 * ADS1148 Delays (these delays assume 50Mhz clock):
 */

//205ms delay. At default 5SPS, digital filter reset takes about 200ms, so this is safe.
#define SPS5DELAY ((50000000/15) + (50000000/600))

//55ms delay. At 20SPS, digital filter reset takes about 50ms, so this is safe.
#define SPS20DELAY ((50000000/60) + (50000000/600))

//805ms delay. At 20SPS, calibration takes about 801ms so this is safe.
#define SPS20CALIBRATIONDELAY ( (50000000/4) + (50000000/60) + (50000000/600))




/*
 * ADS1148.c function prototypes:
 */
void openADS1148(void);
float pollADS1218(short channel, int trials);
void closeADS1218(void);


signed long ADS1148GetValue(void);
void ADS1148ChangeChannel(short channel);
void setUpSSIADS1148(void);






#endif /* ADS1148_H_ */
