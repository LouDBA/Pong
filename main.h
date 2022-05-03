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
#define MIN_LINE_WIDTH			50
#define SPEED_MIN				640.0f
#define SPEED_MAX				1100.0f
#define PXTOCM					1570.0f //experimental value
#define GOAL_DISTANCE_FOND		10.0f
#define MAX_DISTANCE 			20.0f
#define ERROR_THRESHOLD			0.1f	//[cm] because of the noise of the camera
#define IR_DETECT_VALUE			110		//valeur a laquelle les capteur se triggers

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

void SendUint8ToComputer(uint8_t* data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
