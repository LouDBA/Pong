#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <pi_regulator.h>
#include <process_image.h>
#include <capteur_ir.h>
#include <sensors/imu.h>

#include <chprintf.h>


static bool panique = false;
static float accelZStart = 0;

void show_gravity(imu_msg_t *imu_values){
	//create a pointer to the array for shorter name
	float *accel = imu_values->acceleration;
	float random = accel[Z_AXIS];
	//chprintf((BaseSequentialStream *) &SD3, "z_accel : %f\r\n",random );
	if((fabs(accel[Z_AXIS]) < (GRAVITYG - THRESHOLD_IMU)) || (fabs(accel[Z_AXIS]) > (GRAVITYG + THRESHOLD_IMU))){ //rajouter des define dans main.h
		panique = true;
	} else {
		panique = false;
	}
}

static THD_WORKING_AREA(waAccelerometer, 512);
static THD_FUNCTION(Accelerometer, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;
	messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");
	imu_msg_t imu_values;

	//wait 1 sec to be sure the e-puck is in a stable position
	chThdSleepMilliseconds(1000);
	//wait for new measures to be published
	messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));

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
	chThdCreateStatic(waAccelerometer, sizeof(waAccelerometer), NORMALPRIO, Accelerometer, NULL);
}

bool get_panique(void){
	return panique;
}





