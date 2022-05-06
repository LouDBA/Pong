#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"


//constants for the differents parts of the project
#define IMAGE_BUFFER_SIZE		640
#define WIDTH_SLOPE				5
#define MIN_LINE_WIDTH			40
#define SPEED_MIN				640.0f
#define SPEED_MAX				1100.0f
#define PXTOCM					1570.0f //experimental value
#define GOAL_DISTANCE_FOND		15.0f	// [cm]
#define MAX_DISTANCE 			25.0f	// [cm]
//#define ERROR_THRESHOLD			0.1f	// [cm] because of the noise of the camera
#define IR_DETECT_VALUE			512		// valeur a laquelle les capteur se triggers
#define PLATEAU_LENGTH			56 	// [cm]
#define THRESHOLD_IMU			0.2f
#define NB_SAMPLES_OFFSET    	200

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

void SendUint8ToComputer(uint8_t* data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
