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
#define WIDTH_SLOPE				10
#define MIN_LINE_WIDTH			50
#define SPEED_MAX				900.0f
#define PXTOCM					1570.0f //experimental value
#define GOAL_DISTANCE_FOND		7.0f	// [cm]
#define IR_DETECT_VALUE			512		// valeur a laquelle les capteur se triggers
#define THRESHOLD_IMU			5.0f
#define GRAVITYG				9.81f
#define PETIT_ANGLE				50  // [ms] temps de sleep pour tourner du bon angle
#define MOYEN_ANGLE				225 // [ms]
#define GRAND_ANGLE				260 // [ms]
#define SLEEP_DEMI_TOUR			700 //temps en [s] pour faire demi tour apres un point
#define BLINKINGTIME			300 // [ms]
#define VALUEFORTOGGLE			2
#define NUMBEROFBLINKING		7
#define TIMEFIRSTNOTE			300
#define TIMESECONDNOTE			500


/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif

#endif
