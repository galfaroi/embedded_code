/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */



#include "SDCard.h"
#include "StellarisWare.h"
#include "fatfs/ff.h"
#include "fatfs/diskio.h"



/*
 * SDCard.c:
 * openSDCard(): sets up the SSI0 lines to the SD card as well as the
 * System Tick interrupt FatFs requires.
 * closeSDCard(): disables SSI0.
 */





//The following are for decoding error messages that FATfs sends:

/*
 *  A structure that holds a mapping between an FRESULT numerical code, and a
 *  string representation.  FRESULT codes are returned from the FatFs FAT file
 *  system driver.
 */
typedef struct
{
	FRESULT fresult;
	char *pcResultStr;
}
tFresultString;


// A macro to make it easy to add result codes to the table.
#define FRESULT_ENTRY(f)        { (f), (#f) }



/*
 * A table that holds a mapping between the numerical FRESULT code and it's
 * name as a string.  This is used for looking up error codes for printing to
 * the console.
 */
tFresultString g_sFresultStrings[] =
{
		FRESULT_ENTRY(FR_OK),
		FRESULT_ENTRY(FR_NOT_READY),
		FRESULT_ENTRY(FR_NO_FILE),
		FRESULT_ENTRY(FR_NO_PATH),
		FRESULT_ENTRY(FR_INVALID_NAME),
		FRESULT_ENTRY(FR_INVALID_DRIVE),
		FRESULT_ENTRY(FR_DENIED),
		FRESULT_ENTRY(FR_EXIST),
		FRESULT_ENTRY(FR_RW_ERROR),
		FRESULT_ENTRY(FR_WRITE_PROTECTED),
		FRESULT_ENTRY(FR_NOT_ENABLED),
		FRESULT_ENTRY(FR_NO_FILESYSTEM),
		FRESULT_ENTRY(FR_INVALID_OBJECT),
		FRESULT_ENTRY(FR_MKFS_ABORTED)
};


// A macro that holds the number of result codes.
#define NUM_FRESULT_CODES (sizeof(g_sFresultStrings) / sizeof(tFresultString))



/*
 *  This function returns a string representation of an error code that was
 *  returned from a function call to FatFs.  It can be used for printing human
 *  readable error messages.
 */
const char *
StringFromFresult(FRESULT fresult)
{
	unsigned int uIdx;

	// Enter a loop to search the error code table for a matching error code.
	for(uIdx = 0; uIdx < NUM_FRESULT_CODES; uIdx++)
	{
		// If a match is found, then return the string name of the error code.
		if(g_sFresultStrings[uIdx].fresult == fresult)
		{
			return(g_sFresultStrings[uIdx].pcResultStr);
		}
	}


	// At this point no matching code was found, so return a string indicating
	// an unknown error.
	return("UNKNOWN ERROR CODE");
}






/*
 * Sets up the SSI0 lines to the SD card, and the System Tick
 * interrupt FatFs requires.
 */
void openSDCard(void)
{
	//enable SSI0 to communicate with SD card:
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);  //SSI0 is on GPIOA


	// Configure SysTick for a 100Hz interrupt.  The FatFs driver wants a 10 ms tick
	ROM_SysTickPeriodSet(SysCtlClockGet() / 100);
	ROM_SysTickEnable();
	ROM_SysTickIntEnable();
}


/*
 * Disable the SSI communications to SD card.
 */
void closeSDCard(void)
{
	ROM_SysCtlPeripheralDisable(SYSCTL_PERIPH_SSI0);
}



/*
 * This is the handler for this SysTick interrupt.  FatFs requires a
 * timer tick every 10 ms for internal timing purposes.
 */
void SysTickHandler(void)
{
    // Call the FatFs tick timer.
    disk_timerproc();
}
