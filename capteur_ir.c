#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <stdio.h>
#include <string.h>

#include <main.h>
#include <sensors/proximity.h>
#include <leds.h>
#include <motors.h>

#include <capteur_ir.h>


static char cote_ir = 'a';


static BSEMAPHORE_DECL(capteur_ir_ready_sem, TRUE);

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
		if(calibrage > 99)
		{
			calibrage = 0;
			calibrate_ir();
		}

		ir1_value = get_prox(1);
		ir6_value = get_prox(6);

		if(ir6_value > 60)
		{
			cote_ir = 'g';
		}
		if(ir1_value > 60)
		{
			cote_ir = 'd';
		}
		if((ir6_value < 60) && (ir1_value < 60))
		{
			cote_ir = 'a';
		}
		chBSemSignal(&capteur_ir_ready_sem);

		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}


static THD_WORKING_AREA(waProcessIR, 128);
static THD_FUNCTION(ProcessIR, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;


	while(1){
		//waits until an image has been captured
		chBSemWait(&capteur_ir_ready_sem);

		switch(cote_ir)
		{
		case 'a' :
			left_motor_set_speed(MOTOR_SPEED_LIMIT);
			right_motor_set_speed(MOTOR_SPEED_LIMIT);
			set_body_led(0);
			set_front_led(1);
			break;
		case 'd' :
			left_motor_set_speed(-MOTOR_SPEED_LIMIT);
			right_motor_set_speed(MOTOR_SPEED_LIMIT);
			set_body_led(1);
			set_front_led(0);
			break;
		case 'g' :
			left_motor_set_speed(MOTOR_SPEED_LIMIT);
			right_motor_set_speed(-MOTOR_SPEED_LIMIT);
			set_body_led(1);
			set_front_led(0);
			break;
		default :
			left_motor_set_speed(0);
			right_motor_set_speed(0);
			set_body_led(0);
			set_front_led(1);
		}
	}

}


void capteur_ir_start(void){
	chThdCreateStatic(waCapteurIR, sizeof(waCapteurIR), NORMALPRIO, CapteurIR, NULL);
	chThdCreateStatic(waProcessIR, sizeof(waProcessIR), NORMALPRIO, ProcessIR, NULL);
}

