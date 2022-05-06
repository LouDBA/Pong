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
#include <accelerometer.h>

//gestion de la vitesse (de la caméra et autre fonction pour les capteurs ou tout dans celle la ?)

static THD_WORKING_AREA(waPiRegulator, 512);
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
//		if(get_panique()) {
//			speed_g = 0;
//			speed_d = 0;
//		} else {

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
			
				//retour au milieu du plateau

				speed_g = MOTOR_SPEED_LIMIT;
				speed_d = MOTOR_SPEED_LIMIT;
				while(side_ir == AUCUN) { // on le fait avancer jusqu'a ce qu'un capteur s'active
					side_ir = get_cote_ir();
					right_motor_set_speed(speed_d);
					left_motor_set_speed(speed_g);
				}

				// quand un capteur de distance s'active on le fait tourner sur la droite jusqu'a ce que le capteur de gauche complet s'active
				speed_g = MOTOR_SPEED_LIMIT;
				speed_d = -MOTOR_SPEED_LIMIT;
				while(side_ir != GAUCHE) {
					side_ir = get_cote_ir();
					right_motor_set_speed(speed_d);
					left_motor_set_speed(speed_g);
				}
				chThdSleepMilliseconds(90); // cet angle doit etre exact
				//rajouter un chThdSleepMilliseconds(1000)ou un set step ;
				speed_g = 0;
				speed_d = 0;
				right_motor_set_speed(speed_d);
				left_motor_set_speed(speed_g);
				chThdSleepMilliseconds(5000);



				//
				//			speed_g = MOTOR_SPEED_LIMIT;
				//			speed_d = MOTOR_SPEED_LIMIT; //on le fait avancer jusqu'à ce qu'il soit à 5 cm du mur
				//			//while(side_ir != 'g') { // soit quand le capteur avant gauche/ avant droite soit quand il est à 10 cm du mur de coté (bandes sur le téco)
				//			while(get_distance_fond_cm() > GOAL_DISTANCE_FOND/2){
				//				right_motor_set_speed(speed_d);
				//				left_motor_set_speed(speed_g);
				//			}
				//			//on le fait tourner de 90°
				//			speed_g = MOTOR_SPEED_LIMIT;
				//			speed_d = -MOTOR_SPEED_LIMIT;
				//			right_motor_set_speed(speed_d);
				//			left_motor_set_speed(speed_g);
				//			chThdSleepMilliseconds(320); // Temps pour qu'il fasse 90°, remplacer par fct set_step 297
				//			// cet angle doit etre exact
				//
				//			// on le fait avancer jusqu'à la moitié du plateau
				//			speed_g = MOTOR_SPEED_LIMIT;
				//			speed_d = MOTOR_SPEED_LIMIT;
				//			while(get_distance_fond_cm() > (PLATEAU_LENGTH/2)){ //bandes assez grosses ?
				//				right_motor_set_speed(speed_d);
				//				left_motor_set_speed(speed_g);
				//			}
				//
				//			//on le fait tourner de 90°
				//			speed_g = MOTOR_SPEED_LIMIT;
				//			speed_d = -MOTOR_SPEED_LIMIT;
				//			right_motor_set_speed(speed_d);
				//			left_motor_set_speed(speed_g);
				//			chThdSleepMilliseconds(297); // Temps pour qu'il fasse 90°, remplacer par fct set_step
				//			// cet angle doit etre exact
				//
				//			// on le fait avancer jusqu'à la moitié du plateau
				//			speed_g = MOTOR_SPEED_LIMIT;
				//			speed_d = MOTOR_SPEED_LIMIT;
				//
				//			right_motor_set_speed(speed_d);
				//			left_motor_set_speed(speed_g);
				//			chThdSleepMilliseconds(300);
				//
				//			//on le fait tourner de 45°, Boucle for?
				//			speed_g = MOTOR_SPEED_LIMIT;
				//			speed_d = -MOTOR_SPEED_LIMIT;
				//			right_motor_set_speed(speed_d);
				//			left_motor_set_speed(speed_g);
				//			chThdSleepMilliseconds(148); // Temps pour qu'il fasse 45°, remplacer par fct set_step

				// on le fait s'arrêter pour indiquer que le jeu va repartir variable debut? LEDs vertes qui clignotent ?
				//			speed_g = 0;
				//			speed_d = 0;
				//			right_motor_set_speed(speed_d);
				//			left_motor_set_speed(speed_g);
				//			chThdSleepMilliseconds(5000);

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
