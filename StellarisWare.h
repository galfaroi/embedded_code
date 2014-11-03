/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */


#ifndef STELLARISWARE_H_
#define STELLARISWARE_H_


//clock is set to run at 50MHZ...SysCtlDelay takes 3 cycles per loop, so 50,000,000/3 gives 1 second delay.
#define ONESEC (50000000/3)



#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_uart.h"
#include "inc/hw_ssi.h"
#include "inc/hw_sysctl.h"
#include "inc/lm4f232h5qd.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "driverlib/fpu.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/hibernate.h"
#include "driverlib/debug.h"
#include "driverlib/eeprom.h"
#include "driverlib/rom.h"
#include "utils/uartstdio.h"


#endif /* STELLARISWARE_H_ */
