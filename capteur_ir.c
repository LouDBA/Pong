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


static int cote_ir = 0;//

static THD_WORKING_AREA(waCapteurIR, 128);
static THD_FUNCTION(CapteurIR, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;

	unsigned int ir_value[8];
	calibrate_ir();
	bool wall = false;
	while(1){
		time = chVTGetSystemTime();
		wall = false;
		for(unsigned int i = 0; i<8; i++)
		{
			ir_value[i] = get_prox(i);
			if(ir_value[i] > IR_DETECT_VALUE)
			{
				wall = true;
			}
		}
		if((wall) && ((ir_value[3] < IR_DETECT_VALUE)) && (ir_value[4] < IR_DETECT_VALUE)){
			if(ir_value[6] > IR_DETECT_VALUE)
			{
				cote_ir = GAUCHE_45;
			}
			if(ir_value[1] > IR_DETECT_VALUE)
			{
				cote_ir = DROIT_45;
			}
			if(ir_value[2] > IR_DETECT_VALUE)
			{
				cote_ir = DROIT; //droite 90°
			}
			if(ir_value[5] > IR_DETECT_VALUE)
			{
				cote_ir = GAUCHE; //gauche 90°
			}
			if((ir_value[0] > ir_value[7]) && (ir_value[0] > IR_DETECT_VALUE)
					&& (ir_value[1] < IR_DETECT_VALUE))
			{
				cote_ir = DROIT_AV; //legerement a droite
			}
			if ((ir_value[7] > ir_value[0]) && (ir_value[7] > IR_DETECT_VALUE)
					&& (ir_value[6] < IR_DETECT_VALUE))
			{
				cote_ir = GAUCHE_AV; //legerement a gauche
			}
		} else {
			cote_ir = AUCUN;//aucun
		}

		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}


int get_cote_ir(void){
	return(cote_ir);
}

void capteur_ir_start(void){
	chThdCreateStatic(waCapteurIR, sizeof(waCapteurIR), NORMALPRIO, CapteurIR, NULL);
}

