#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>

#include <sensors/imu.h>


static bool robotLeve = false;

void show_gravity(imu_msg_t *imu_values){
	//create a pointer to the array for shorter name
	float *accel = imu_values->acceleration;
	if((accel[Z_AXIS] < (- GRAVITYG - THRESHOLD_IMU)) || (accel[Z_AXIS] > (- GRAVITYG + THRESHOLD_IMU))){
		robotLeve = true;
	} else {
		robotLeve = false;
	}
}

static THD_WORKING_AREA(waAccelerometer, 512);
static THD_FUNCTION(Accelerometer, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;
	messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");
	imu_msg_t imu_values;

	while(1){
		time = chVTGetSystemTime();

		//wait 1 sec to be sure the e-puck is in a stable position
		chThdSleepMilliseconds(1000);

		//wait for new measures to be published
		messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));

		show_gravity(&imu_values);

		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}

void accelerometer_start(void){
	chThdCreateStatic(waAccelerometer, sizeof(waAccelerometer), (NORMALPRIO + 1), Accelerometer, NULL);
}

bool get_robotLeve(void){
	return robotLeve;
}





