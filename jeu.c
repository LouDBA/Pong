#include "ch.h"
#include "hal.h"
#include "leds.h"
#include <audio/play_melody.h>


#include <main.h>
#include <process_image.h>
#include <jeu.h>
#include <accelerometer.h>


// gestion des LEDs et du son


static THD_WORKING_AREA(waJeu, 256);
static THD_FUNCTION(Jeu, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;
	uint8_t score_rouge = 0;
	uint8_t score_bleu = 0;

	while(1){
		time = chVTGetSystemTime();
		// si on le lève il panique, réinitialise tout et allume toutes les LEDs en rouge
		if(get_robotLeve()) {
			score_rouge = 0;
			set_scoreRed(score_rouge);
			score_bleu = 0;
			set_scoreBlue(score_bleu);
			set_led(LED1, 0);
			set_led(LED3, 0);
			set_led(LED5, 0);
			set_led(LED7, 0);
			set_rgb_led(LED2, 0,  0, 0);
			set_rgb_led(LED4, 0,  0, 0);
			set_rgb_led(LED6, 0,  0, 0);
			set_rgb_led(LED8, 0,  0, 0);
			while (get_robotLeve()) {
				set_led(LED1, VALUEFORTOGGLE);
				set_led(LED3, VALUEFORTOGGLE);
				set_led(LED5, VALUEFORTOGGLE);
				set_led(LED7, VALUEFORTOGGLE);
				toggle_rgb_led(LED2, RED_LED, RGB_MAX_INTENSITY);
				toggle_rgb_led(LED4, RED_LED, RGB_MAX_INTENSITY);
				toggle_rgb_led(LED6, RED_LED, RGB_MAX_INTENSITY);
				toggle_rgb_led(LED8, RED_LED, RGB_MAX_INTENSITY);
				chThdSleepMilliseconds(BLINKINGTIME);
			}
			set_play(true);

		} else {
			if(!get_play()) {
				playNote(NOTE_A4  , TIMEFIRSTNOTE);
				playNote(NOTE_B4  , TIMESECONDNOTE);
				chThdSleepMilliseconds(2 * SLEEP_DEMI_TOUR);
			}
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
				break;

			case 1:
				set_led(LED1, 1);
				set_led(LED3, 0);
				set_led(LED5, 0);
				set_led(LED7, 0);
				break;

			case 2:
				set_led(LED1, 1);
				set_led(LED3, 1);
				set_led(LED5, 0);
				set_led(LED7, 0);
				break;

			case 3:
				set_led(LED1, 1);
				set_led(LED3, 1);
				set_led(LED5, 1);
				set_led(LED7, 0);
				break;
			case 4:
				set_led(LED1, 1);
				set_led(LED3, 1);
				set_led(LED5, 1);
				set_led(LED7, 1);
				for(int i = 0 ; i < NUMBEROFBLINKING ; ++i){
					set_led(LED1, VALUEFORTOGGLE);
					set_led(LED3, VALUEFORTOGGLE);
					set_led(LED5, VALUEFORTOGGLE);
					set_led(LED7, VALUEFORTOGGLE);
					chThdSleepMilliseconds(BLINKINGTIME);
				}
				score_rouge = 0;
				set_scoreRed(score_rouge);
				score_bleu = 0;
				set_scoreBlue(score_bleu);
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

			case 1:
				set_rgb_led(LED2, 0, 0, RGB_MAX_INTENSITY);
				set_rgb_led(LED4, 0, 0, 0);
				set_rgb_led(LED6, 0, 0, 0);
				set_rgb_led(LED8, 0, 0, 0);
				break;

			case 2:
				set_rgb_led(LED2, 0, 0, RGB_MAX_INTENSITY);
				set_rgb_led(LED4, 0, 0, RGB_MAX_INTENSITY);
				set_rgb_led(LED6, 0, 0, 0);
				set_rgb_led(LED8, 0, 0, 0);
				break;

			case 3:
				set_rgb_led(LED2, 0, 0, RGB_MAX_INTENSITY);
				set_rgb_led(LED4, 0, 0, RGB_MAX_INTENSITY);
				set_rgb_led(LED6, 0, 0, RGB_MAX_INTENSITY);
				set_rgb_led(LED8, 0, 0, 0);
				break;
			case 4:
				set_rgb_led(LED2, 0, 0, RGB_MAX_INTENSITY);
				set_rgb_led(LED4, 0, 0, RGB_MAX_INTENSITY);
				set_rgb_led(LED6, 0, 0, RGB_MAX_INTENSITY);
				set_rgb_led(LED8, 0, 0, RGB_MAX_INTENSITY);
				for(int i = 0 ; i < NUMBEROFBLINKING ; ++i){
					toggle_rgb_led(LED2, BLUE_LED, RGB_MAX_INTENSITY);
					toggle_rgb_led(LED4, BLUE_LED, RGB_MAX_INTENSITY);
					toggle_rgb_led(LED6, BLUE_LED, RGB_MAX_INTENSITY);
					toggle_rgb_led(LED8, BLUE_LED, RGB_MAX_INTENSITY);
					chThdSleepMilliseconds(BLINKINGTIME);
				}
				score_rouge = 0;
				set_scoreRed(score_rouge);
				score_bleu = 0;
				set_scoreBlue(score_bleu);
				break;

			default:
				score_bleu = 0;
				set_scoreBlue(score_bleu);
			}
		}

		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}

void jeu_start(void){
	chThdCreateStatic(waJeu, sizeof(waJeu), NORMALPRIO, Jeu, NULL);
}
