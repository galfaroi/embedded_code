/*
 * David Muller; Germán Alfaro
 * davidmuller10@mittymonarch.com; alfaro.germanevera@gmail.com
 */



/*
 * System.h is the main header file.
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_


// Definition of a structure containing the controller's parameters
// These parameters are all modifiable by external user
struct sys_data
{
	float batt1_volt_cal;	// Calibration factor of the controller battery voltage
	float low_batt_volt;	// Low battery voltage threshold
	float ad24_cal;
	float batt2_volt_cal;
	unsigned long sampling_period;	// pH sampling period
	long sampling_average;	// Number of samples to average
	unsigned long averaging_interval;// Number of seconds between each average
	long optode_samples;
	unsigned long optode_sample_interval;
	unsigned long pumpon_time;		// Active pump time
	unsigned long status_period;	// Controller status polling period
	unsigned long output_mode;
	float t_c; //mel 072711 user input
	float eo_int; //mel 072911 user input
	float sal_cal;//mel 080311 user input

	//char *fn;//mel let user name text file
}sys_data;




/*
 * Function prototypes for main.c.
 */
void configuration();
void test();
void handleHibernation();
void getDateAndTime();
long getLong(char *prompt);
unsigned long getUnsignedLong (char* prompt);
float getFloat(char *prompt);




#endif /* SYSTEM_H_ */
