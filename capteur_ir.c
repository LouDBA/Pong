#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <stdio.h>
#include <string.h>

#include <main.h>
#include <sensors/proximity.h>

#include <capteur_ir.h>


static char cote_ir = 'a';//


static THD_WORKING_AREA(waCapteurIR, 128);
static THD_FUNCTION(CapteurIR, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;

	uint8_t calibrage = 0;

	int ir6_value = 0 , ir1_value = 0;


	while(1){
		time = chVTGetSystemTime();
		++calibrage;
		if(calibrage > 199)
		{
			calibrage = 0;
			calibrate_ir();
		}

		ir1_value = get_prox(1);
		ir6_value = get_prox(6);

		if(ir6_value > IR_DETECT_VALUE)
		{
			cote_ir = 'g'; //gauche 45°
		}
		if(ir1_value > IR_DETECT_VALUE)
		{
			cote_ir = 'd'; //droite 45°
		}
		if((ir6_value < IR_DETECT_VALUE) && (ir1_value < IR_DETECT_VALUE))
		{
			cote_ir = 'a';//aucun
		}

		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}


char get_cote_ir(void){
	return(cote_ir);
}

void capteur_ir_start(void){
	chThdCreateStatic(waCapteurIR, sizeof(waCapteurIR), NORMALPRIO, CapteurIR, NULL);
}

