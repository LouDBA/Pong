#include "ch.h"
#include "hal.h"

#include <main.h>
#include <camera/po8030.h>
#include <process_image.h>
#include <selector.h>

static float distance_fond_cm = 0;
static uint8_t imageRed[IMAGE_BUFFER_SIZE] = {0};
static uint8_t imageBlue[IMAGE_BUFFER_SIZE] = {0};
static uint8_t scoreRed = 0;
static uint8_t scoreBlue = 0;
static bool play = true;


//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

/*
 *  Returns the line's width extracted from the image buffer given
 */
uint16_t extract_line_width(uint8_t *buffer, uint32_t mean){ //extrait l'epaisseur de la ligne la plus grosse qui est vue
	uint32_t finalwidth = 0;
	uint32_t actualcolorwidth = 0;
	uint32_t actualwhitewidth = 0;
	bool colorbande = false;
	bool whitebande = false;

	for(uint16_t i = 0 ; i < (IMAGE_BUFFER_SIZE - 1); ++i) {
		// regarde l'épaisseur de toutes les bandes de couleur
		if((colorbande == false) && (buffer[i] > mean) && (buffer[i+1] < mean)) { //flanc descendant
			colorbande = true;
		}
		if((colorbande == true) && (buffer[i] < mean)){
			++actualcolorwidth;
			if (buffer[i+1] > mean) { // flanc montant
				colorbande = false;
			}
		}
		if(colorbande == false) {
			if((actualcolorwidth > finalwidth) && (actualcolorwidth > finalwidth)) {
				finalwidth = actualcolorwidth;
			}
			actualcolorwidth = 0;
		}

		// regarde l'épaisseur de toutes les bandes blanches
		if((whitebande == false) && (buffer[i] < mean) && (buffer[i+1] > mean)) { //flanc montant
			whitebande = true;
		}
		if((whitebande == true) && (buffer[i] > mean)){
			++actualwhitewidth;
			if (buffer[i+1] < mean) { // flanc descendant
				whitebande = false;
			}
		}
		if(whitebande == false)  {
			if((actualwhitewidth > finalwidth) && (actualwhitewidth > MIN_LINE_WIDTH)) {
				finalwidth = actualwhitewidth;
			}
			actualwhitewidth = 0;
		}
	}

	if(finalwidth > MIN_LINE_WIDTH) {
		return finalwidth;
	}else {
		return MIN_LINE_WIDTH;
	}
}


static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	//Takes pixels 0 to IMAGE_BUFFER_SIZE of the line 300 + 301 (minimum 2 lines because reasons)
	po8030_advanced_config(FORMAT_RGB565, 0, 300, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
	dcmi_enable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();

	while(1){
		//starts a capture
		dcmi_capture_start();
		//waits for the capture to be done
		wait_image_ready();
		//signals an image has been captured
		chBSemSignal(&image_ready_sem);
	}
}


static THD_WORKING_AREA(waProcessImage, 1024);
static THD_FUNCTION(ProcessImage, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	uint8_t *img_buff_ptr;
	uint16_t lineWidth = 0;
	uint32_t meanRed = 0;
	uint32_t meanBlue = 0;

	while(1){
		//waits until an image has been captured
		chBSemWait(&image_ready_sem);
		//gets the pointer to the array filled with the last image in RGB565
		img_buff_ptr = dcmi_get_last_image_ptr();

		//Extracts only the red pixels
		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){
			//extracts first 5bits of the first byte
			//takes nothing from the second byte
			imageRed[i/2] = (uint8_t)(img_buff_ptr[i]&0xF8/8); //red pixels on décale de 3 pour avoir des valeurs entre 0 et 32 comme le bleu
			//extracts last 5bits of the first byte
			//takes nothing from the first byte
			imageBlue[i/2] =  (uint8_t)img_buff_ptr[i+1]&0x1F; //blue
		}

		//performs an average
		for(uint16_t i = 0 ; i < IMAGE_BUFFER_SIZE ; i++){
			meanRed += imageRed[i];
			meanBlue += imageBlue[i];
		}
		meanRed /= IMAGE_BUFFER_SIZE;
		meanBlue /= IMAGE_BUFFER_SIZE;

		//search for a line in the image and gets its width in pixels
		if(meanRed < meanBlue) {
			lineWidth = extract_line_width(imageRed, meanRed);
		} else {
			lineWidth = extract_line_width(imageBlue, meanBlue);
		}
		distance_fond_cm = PXTOCM/lineWidth;


		if((distance_fond_cm < GOAL_DISTANCE_FOND) && (play == true) && !(get_selector() & 1) ){
			if(meanRed < meanBlue) {
				++scoreRed;
				play = false;
			} else {
				++scoreBlue;
				play = false;
			}
		}
	}
}



uint8_t get_scoreRed(void){
	return scoreRed;
}

uint8_t get_scoreBlue(void){
	return scoreBlue;
}

bool get_play(void){
	return play;
}

void set_play(bool partie){
	play = partie;
	return;
}

void set_scoreRed(uint8_t score_rouge){
	scoreRed = score_rouge;
}

void set_scoreBlue(uint8_t score_bleu){
	scoreBlue = score_bleu;
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}
