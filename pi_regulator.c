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

static THD_WORKING_AREA(waPiRegulator, 512);
static THD_FUNCTION(PiRegulator, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;

	char side_ir = 'a';

	int16_t speed_g = 0 , speed_d = 0;

	while(1){
		time = chVTGetSystemTime();

		//computes the speed to give to the motors

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
			case 'f' :
				speed_g = MOTOR_SPEED_LIMIT;
				speed_d = MOTOR_SPEED_LIMIT;
				break;
			default :
				speed_g = 0;
				speed_d = 0;
			}
		} else {


			//retour au milieu du plateau

			speed_g = MOTOR_SPEED_LIMIT;
			speed_d = MOTOR_SPEED_LIMIT;
			while(side_ir == 'a') { // on le fait avancer jusqu'a ce qu'un capteur s'active
				side_ir = get_cote_ir();
				right_motor_set_speed(speed_d);
				left_motor_set_speed(speed_g);
			}


			//					speed_g = 0;
			//					speed_d = 0;
			//					right_motor_set_speed(speed_d);
			//					left_motor_set_speed(speed_g);
			//					chThdSleepMilliseconds(5000);


			// quand un capteur de distance s'active on le fait tourner sur la droite jusqu'a ce que le capteur de gauche complet s'active
			speed_g = MOTOR_SPEED_LIMIT;
			speed_d = -MOTOR_SPEED_LIMIT;
			while(side_ir != 'f') { // remplacer g car avant g = avant gauche et nous on veut gauche toute
				side_ir = get_cote_ir();
				right_motor_set_speed(speed_d);
				left_motor_set_speed(speed_g);
			}
			chThdSleepMilliseconds(120); // cet angle doit etre exact
			//rajouter un chThdSleepMilliseconds(1000)ou un set step ;

			speed_g = MOTOR_SPEED_LIMIT;
			speed_d = MOTOR_SPEED_LIMIT; //on le fait avancer jusqu'à ce qu'il soit à 10 cm du mur
			//while(side_ir != 'g') { // soit quand le capteur avant gauche/ avant droite soit quand il est à 10 cm du mur de coté (bandes sur le téco)
			while(get_distance_fond_cm() > GOAL_DISTANCE_FOND){
				right_motor_set_speed(speed_d);
				left_motor_set_speed(speed_g);
			}
			//on le fait tourner de 90°
			speed_g = MOTOR_SPEED_LIMIT;
			speed_d = -MOTOR_SPEED_LIMIT;
			right_motor_set_speed(speed_d);
			left_motor_set_speed(speed_g);
			chThdSleepMilliseconds(297); // Temps pour qu'il fasse 90°, remplacer par fct set_step
			// cet angle doit etre exact

			// on le fait avancer jusqu'à la moitié du plateau
			speed_g = MOTOR_SPEED_LIMIT;
			speed_d = MOTOR_SPEED_LIMIT;
			while(get_distance_fond_cm() > (PLATEAU_LENGTH/2)){ //bandes assez grosses ?
				right_motor_set_speed(speed_d);
				left_motor_set_speed(speed_g);
			}

			//on le fait tourner de 90°
			speed_g = MOTOR_SPEED_LIMIT;
			speed_d = -MOTOR_SPEED_LIMIT;
			right_motor_set_speed(speed_d);
			left_motor_set_speed(speed_g);
			chThdSleepMilliseconds(297); // Temps pour qu'il fasse 90°, remplacer par fct set_step
			// cet angle doit etre exact

			// on le fait avancer jusqu'à la moitié du plateau
			speed_g = MOTOR_SPEED_LIMIT;
			speed_d = MOTOR_SPEED_LIMIT;
			while(get_distance_fond_cm() > (PLATEAU_WIDTH/2)){ //bandes assez grosses ?
				right_motor_set_speed(speed_d);
				left_motor_set_speed(speed_g);
			}

			//on le fait tourner de 45°, Boucle for?
			speed_g = MOTOR_SPEED_LIMIT;
			speed_d = -MOTOR_SPEED_LIMIT;
			right_motor_set_speed(speed_d);
			left_motor_set_speed(speed_g);
			chThdSleepMilliseconds(148); // Temps pour qu'il fasse 45°, remplacer par fct set_step
			// on le fait s'arrêter pour indiquer que le jeu va repartir variable debut? LEDs vertes qui clignotent ?
			speed_g = 0;
			speed_d = 0;
			right_motor_set_speed(speed_d);
			left_motor_set_speed(speed_g);
			chThdSleepMilliseconds(5000);

			set_play(true);
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
