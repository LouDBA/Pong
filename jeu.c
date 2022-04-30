#include "ch.h"
#include "hal.h"
#include "leds.h"
#include <audio/play_melody.h>
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <pi_regulator.h>
#include <process_image.h>
#include <jeu.h>
// vérifier les include


static THD_WORKING_AREA(waJeu, 256); // vérifier la taille de la mémoire allouée
static THD_FUNCTION(Jeu, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;
	uint8_t score_rouge = 0;
	uint8_t score_bleu = 0;

	while(1){
		time = chVTGetSystemTime();
		score_rouge = get_scoreRed();
		score_bleu = get_scoreBlue();

		//retour milieu
		switch(score_rouge)
		{
		case 0:
			set_led(LED1, 0);
			set_led(LED3, 0);
			set_led(LED5, 0);
			set_led(LED7, 0);
			//retour milieu
			break;

		case 1:
			set_led(LED1, 1);
			set_led(LED3, 0);
			set_led(LED5, 0);
			set_led(LED7, 0);
			//retour milieu
			break;

		case 2:
			set_led(LED1, 1);
			set_led(LED3, 1);
			set_led(LED5, 0);
			set_led(LED7, 0);
			//retour milieu
			break;

		case 3:
			set_led(LED1, 1);
			set_led(LED3, 1);
			set_led(LED5, 1);
			set_led(LED7, 0);
			//retour milieu
			break;
		case 4:
			set_led(LED1, 1);
			set_led(LED3, 1);
			set_led(LED5, 1);
			set_led(LED7, 1);
			//playMelody(WE_ARE_THE_CHAMPIONS, ML_FORCE_CHANGE, NULL);
			//blink pdt 5 sec
			//retour au milieu du plateau
			break;

			// operator doesn't match any case
		default:
			score_rouge = 0;
			set_scoreRed(score_rouge);
		}

		switch(score_bleu)
		{
		case 0:
			set_rgb_led(LED2, 0, 0, 0);
			set_rgb_led(LED4, 0, 0, 0);
			set_rgb_led(LED6, 0, 0, 0);
			set_rgb_led(LED8, 0, 0, 0);
			break;
			//retour milieu

		case 1:
			set_rgb_led(LED2, 0, 0, RGB_MAX_INTENSITY);
			set_rgb_led(LED4, 0, 0, 0);
			set_rgb_led(LED6, 0, 0, 0);
			set_rgb_led(LED8, 0, 0, 0);
			//retour milieu
			break;

		case 2:
			set_rgb_led(LED2, 0, 0, RGB_MAX_INTENSITY);
			set_rgb_led(LED4, 0, 0, RGB_MAX_INTENSITY);
			set_rgb_led(LED6, 0, 0, 0);
			set_rgb_led(LED8, 0, 0, 0);
			//retour milieu
			break;

		case 3:
			set_rgb_led(LED2, 0, 0, RGB_MAX_INTENSITY);
			set_rgb_led(LED4, 0, 0, RGB_MAX_INTENSITY);
			set_rgb_led(LED6, 0, 0, RGB_MAX_INTENSITY);
			set_rgb_led(LED8, 0, 0, 0);
			//retour milieu
			break;
		case 4:
			set_rgb_led(LED2, 0, 0, RGB_MAX_INTENSITY);
			set_rgb_led(LED4, 0, 0, RGB_MAX_INTENSITY);
			set_rgb_led(LED6, 0, 0, RGB_MAX_INTENSITY);
			set_rgb_led(LED8, 0, 0, RGB_MAX_INTENSITY);
			//playMelody(STARWARS, ML_FORCE_CHANGE, NULL);
			//retour milieu
			break; // directement remettre le score à 0 ? Mettre un wait ? Faire clignoter toutes les LEDs pdt 5 secondes puis score_rouge = score_bleu = 0

			// operator doesn't match any case
		default:
			score_bleu = 0;
			set_scoreBlue(score_bleu);
		}

		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}

void jeu_start(void){
	chThdCreateStatic(waJeu, sizeof(waJeu), NORMALPRIO, Jeu, NULL);
}
