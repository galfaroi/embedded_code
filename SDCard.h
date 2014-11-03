/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */


#ifndef SDCARD_H_
#define SDCARD_H_


#include "fatfs/ff.h" //(so FRESULT is defined in this file)




/*
 * SDCard.c function prototypes:
 */
void openSDCard(void);
void closeSDCard(void);


const char *StringFromFresult(FRESULT fresult);  //For decoding SD card error messages
void SysTickHandler(void);  //FatFs requires this interrupt



#endif /* SDCARD_H_ */
