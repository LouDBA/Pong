#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ch.h"
#include "hal.h"
#include "memory_protection.h"


#include <usbcfg.h>
#include <main.h>
#include <motors.h>
#include <camera/po8030.h>
#include <chprintf.h>
#include <sensors/proximity.h>
#include <msgbus/messagebus.h>
#include <i2c_bus.h>
#include <spi_comm.h>
#include <audio/audio_thread.h>
#include <audio/play_melody.h>
#include <sensors/imu.h>


#include <process_image.h>
#include <jeu.h>
#include <capteur_ir.h>
#include <accelerometer.h>
#include "gestionmoteurs.h"

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);



void SendUint8ToComputer(uint8_t* data, uint16_t size) 
{
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);
}

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
			115200,
			0,
			0,
			0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

int main(void)
{

	halInit();
	chSysInit();
	mpu_init();

	//starts the serial communication
	serial_start();
	//start the USB communication
	usb_start();
	//starts the camera
	dcmi_start();
	po8030_start();
	//inits the motors
	motors_init();
	//start the audio
	dac_start();
	// starts the rgb LEDs
	spi_comm_start();

	i2c_start();

	/** Inits the Inter Process Communication bus. */
	messagebus_init(&bus, &bus_lock, &bus_condvar);
	//start gyroscope
	imu_start();
	//start proximity sensors
	proximity_start();




	//stars the threads for the pi regulator and the processing of the image
	gestionmoteurs_start();
	process_image_start();
	jeu_start();
	playMelodyStart();
	capteur_ir_start();
	//accelerometer_start();

	//calibration des périphériques
	calibrate_ir();
	calibrate_gyro();
	calibrate_acc();

	/* Infinite loop. */
	while (1) {
		//waits 1 second
		chThdSleepMilliseconds(1000);
	}
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
	chSysHalt("Stack smashing detected");
}
