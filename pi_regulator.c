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
	systime_t rot_time = 0;
	int side_ir = 0;

	int16_t speed_g = 0 , speed_d = 0;
	while(1){
		time = chVTGetSystemTime();

		//computes the speed to give to the motors

		if(get_play() == true){
			side_ir = get_cote_ir();
			switch(side_ir)
			{
			case GAUCHE_AV :
				speed_g = MOTOR_SPEED_LIMIT;
				speed_d = -MOTOR_SPEED_LIMIT;
				rot_time = 325;
				break;
			case DROIT_AV :
				speed_g = -MOTOR_SPEED_LIMIT;
				speed_d = MOTOR_SPEED_LIMIT;
				rot_time = 325;
				break;
			case GAUCHE_45 :
				speed_g = MOTOR_SPEED_LIMIT;
				speed_d = -MOTOR_SPEED_LIMIT;
				rot_time = 250;
				break;
			case DROIT_45 :
				speed_g = -MOTOR_SPEED_LIMIT;
				speed_d = MOTOR_SPEED_LIMIT;
				rot_time = 250;
				break;
			case GAUCHE :
				speed_g = MOTOR_SPEED_LIMIT;
				speed_d = -MOTOR_SPEED_LIMIT;
				rot_time = 75;
				break;
			case DROIT :
				speed_g = -MOTOR_SPEED_LIMIT;
				speed_d = MOTOR_SPEED_LIMIT;
				rot_time = 75;
				break;
			case GAUCHE_AR :
				speed_g = MOTOR_SPEED_LIMIT;
				speed_d = -MOTOR_SPEED_LIMIT;
				break;
			case DROIT_AR :
				speed_g = MOTOR_SPEED_LIMIT;
				speed_d = -MOTOR_SPEED_LIMIT;
				break;
			case AUCUN :
				speed_g = MOTOR_SPEED_LIMIT;
				speed_d = MOTOR_SPEED_LIMIT;
				rot_time = 0;
				break;
			default :
				speed_g = 0;
				speed_d = 0;
				rot_time = 0;
			}
		} else {
			//			speed_g = 0;
			//			speed_d = 0;
			//			right_motor_set_speed(+speed_d);
			//			left_motor_set_speed(speed_g);
			//			chThdSleepMilliseconds(600);
			set_play(true);
		}
		right_motor_set_speed(speed_d);
		left_motor_set_speed(speed_g);


		//Attente du temps de rotation


		chThdSleepUntilWindowed(time,time + MS2ST(10) + MS2ST(rot_time));

	}
}


void pi_regulator_start(void){
	chThdCreateStatic(waPiRegulator, sizeof(waPiRegulator), NORMALPRIO, PiRegulator, NULL);
}
