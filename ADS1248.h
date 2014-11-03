/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */


#ifndef ADS1248_H_
#define ADS1248_H_



//toggle ADS1248 GPIO power enable pin (pin 71)--POWER SWITCHING IS DONE FOR YOU IN openADS1248() and closeADS1248()!
#define ADS1248_ON ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4, 0xff)
#define ADS1248_OFF ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4, 0x00)



#define SDATAC 0x16     //stop reading data continuously command for ADS1248
#define RDATA 0x12      //read data command
#define NOP 0xFF        //no operation command
#define SELFOCAL 0x62   //self offset calibration command


/*
 * ADS1248 Delays (these delays assume 50Mhz clock):
 */

//205ms delay. At default 5SPS, digital filter reset takes about 200ms, so this is safe.
#define SPS5DELAY ((50000000/15) + (50000000/600))

//55ms delay. At 20SPS, digital filter reset takes about 50ms, so this is safe.
#define SPS20DELAY ((50000000/60) + (50000000/600))

//805ms delay. At 20SPS, calibration takes about 801ms so this is safe.
#define SPS20CALIBRATIONDELAY ( (50000000/4) + (50000000/60) + (50000000/600))




/*
 * ADS1248.c function prototypes:
 */
void openADS1248(void);
float pollADS1248(short channel, int trials);
void closeADS1248(void);


signed long ADS1248GetValue(void);
void ADS1248ChangeChannel(short channel);
void setUpSSIADS1248(void);






#endif /* ADS1248_H_ */
