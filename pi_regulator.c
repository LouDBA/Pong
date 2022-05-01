#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <pi_regulator.h>
#include <process_image.h>

//gestion de la vitesse (de la caméra et autre fonction pour les capteurs ou tout dans celle la ?)

static THD_WORKING_AREA(waPiRegulator, 256);
static THD_FUNCTION(PiRegulator, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;

	int16_t speed = 0;

	while(1){
		time = chVTGetSystemTime();

		//computes the speed to give to the motors
		//distance_cm is modified by the image processing thread
		if(get_play() == true){
			speed = SPEED_MAX;
		} else {
			speed = 0;
		}

		//applies the speed from the PI regulator and the correction for the rotation
		right_motor_set_speed(speed);
		left_motor_set_speed(speed);
		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}

void pi_regulator_start(void){
	chThdCreateStatic(waPiRegulator, sizeof(waPiRegulator), NORMALPRIO, PiRegulator, NULL);
}
