#include "gestionmoteurs.h"

#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <process_image.h>
#include <capteur_ir.h>
#include <accelerometer.h>
#include <selector.h>

//gestion de la vitesse des moteurs

static THD_WORKING_AREA(waGestionMoteurs, 512);
static THD_FUNCTION(GestionMoteurs, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;
	systime_t rot_time = 0;
	int side_ir = 0;

	int16_t speed_g = 0 , speed_d = 0;
	while(1){
		time = chVTGetSystemTime();

		if((get_robotLeve()) || (get_selector() & 1) ) {
			speed_g = 0;
			speed_d = 0;
			right_motor_set_speed(speed_d);
			left_motor_set_speed(speed_g);
		} else {

			if(get_play() == true){
				side_ir = get_cote_ir();
				switch(side_ir)
				{
				case GAUCHE_AV :
					speed_g = SPEED_MAX;
					speed_d = -SPEED_MAX;
					rot_time = 260;
					break;
				case DROIT_AV :
					speed_g = -SPEED_MAX;
					speed_d = SPEED_MAX;
					rot_time = 260;
					break;
				case GAUCHE_45 :
					speed_g = SPEED_MAX;
					speed_d = -SPEED_MAX;
					rot_time = 225;
					break;
				case DROIT_45 :
					speed_g = -SPEED_MAX;
					speed_d = SPEED_MAX;
					rot_time = 225;
					break;
				case GAUCHE :
					speed_g = SPEED_MAX;
					speed_d = -SPEED_MAX;
					rot_time = 50;
					break;
				case DROIT :
					speed_g = -SPEED_MAX;
					speed_d =SPEED_MAX;
					rot_time = 50;
					break;
				case GAUCHE_AR :
					speed_g = SPEED_MAX;
					speed_d = -SPEED_MAX;
					break;
				case DROIT_AR :
					speed_g = SPEED_MAX;
					speed_d = -SPEED_MAX;
					break;
				case AUCUN :
					speed_g = SPEED_MAX;
					speed_d = SPEED_MAX;
					rot_time = 0;
					break;
				default :
					speed_g = 0;
					speed_d = 0;
					rot_time = 0;
				}
				right_motor_set_speed(speed_d);
				left_motor_set_speed(speed_g);
			} else {
				right_motor_set_speed(SPEED_MAX);
				left_motor_set_speed(-SPEED_MAX);
				chThdSleepMilliseconds(700); // le robot fait demi-tour
				right_motor_set_speed(0);
				left_motor_set_speed(0);
				chThdSleepMilliseconds(1000); // il marque un arrêt d'une seconde avant de repartir
				set_play(true);
			}
		}
		chThdSleepUntilWindowed(time,time + MS2ST(10) + MS2ST(rot_time));
	}
}


void gestionmoteurs_start(void){
	chThdCreateStatic(waGestionMoteurs, sizeof(waGestionMoteurs), NORMALPRIO, GestionMoteurs, NULL);
}
