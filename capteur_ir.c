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

	unsigned int ir_value[8];
	calibrate_ir();

	while(1){
		time = chVTGetSystemTime();
		for(unsigned int i = 0; i<8; i++)
		{
			ir_value[i] = get_prox(i);
		}

		if(ir_value[6] > IR_DETECT_VALUE)
		{
			cote_ir = 'g'; //gauche activé
		}
		if(ir_value[1] > IR_DETECT_VALUE)
		{
			cote_ir = 'd'; //droite 45°
		}
		if((ir_value[0] > ir_value[7]) && (ir_value[0] > IR_DETECT_VALUE)
				&& (ir_value[1] < IR_DETECT_VALUE))
		{
			cote_ir = 'u'; //legerement a droite
		}
		if ((ir_value[7] > ir_value[0]) && (ir_value[0] > IR_DETECT_VALUE)
				&& (ir_value[6] < IR_DETECT_VALUE))
		{
			cote_ir = 'n'; //legerement a gauche
		}
		if((ir_value[1] < IR_DETECT_VALUE) && (ir_value[6] < IR_DETECT_VALUE)
				&& (ir_value[0] < IR_DETECT_VALUE) && (ir_value[7] < IR_DETECT_VALUE))
		{
			cote_ir = 'a';//aucun
		}
		for(unsigned int i = 0; i < 7 ; i++)
		{

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

