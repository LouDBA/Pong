#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <pi_regulator.h>
#include <process_image.h>
#include <capteur_ir.h>

//gestion de la vitesse (de la caméra et autre fonction pour les capteurs ou tout dans celle la ?)

static THD_WORKING_AREA(waPiRegulator, 256);
static THD_FUNCTION(PiRegulator, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;

	char side_ir = 'a';

	int16_t speed_g = 0 , speed_d = 0;

	while(1){
		time = chVTGetSystemTime();

		//computes the speed to give to the motors
		//distance_cm is modified by the image processing thread
		if(get_play() == true){
			side_ir = get_cote_ir();
			switch(side_ir)
			{
			case 'a' :
				speed_g = MOTOR_SPEED_LIMIT;
				speed_d = MOTOR_SPEED_LIMIT;
				break;
			case 'd' :
				speed_g = -MOTOR_SPEED_LIMIT;
				speed_d = MOTOR_SPEED_LIMIT;
				break;
			case 'g' :
				speed_g = MOTOR_SPEED_LIMIT;
				speed_d = -MOTOR_SPEED_LIMIT;
				break;
			default :
				speed_g = 0;
				speed_d = 0;
			}
		} else {
			speed_g = 0;
			speed_d = 0;
		}

		//applies the speed from the PI regulator and the correction for the rotation
		right_motor_set_speed(speed_d);
		left_motor_set_speed(speed_g);
		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}

void pi_regulator_start(void){
	chThdCreateStatic(waPiRegulator, sizeof(waPiRegulator), NORMALPRIO, PiRegulator, NULL);
}
