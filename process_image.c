#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>

#include <process_image.h>


static float distance_fond_cm = 0;
static uint8_t imageRed[IMAGE_BUFFER_SIZE] = {0};
static uint8_t imageBlue[IMAGE_BUFFER_SIZE] = {0};
static char perdant = 'x'; // r = red, b = blue, x = none
static uint8_t scoreRed = 0;
static uint8_t scoreBlue = 0;
static bool play = true;

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

/*
 *  Returns the line's width extracted from the image buffer given
 *  Returns 0 if line not found
 */
uint16_t extract_line_width(uint8_t *buffer, uint32_t mean){

	uint16_t i = 0, begin = 0, end = 0, width = 0;
	uint8_t stop = 0, wrong_line = 0, line_not_found = 0;


	do{
		wrong_line = 0;
		//search for a begin
		while(stop == 0 && i < (IMAGE_BUFFER_SIZE - WIDTH_SLOPE))
		{ 
			//the slope must at least be WIDTH_SLOPE wide and is compared
			//to the mean of the image
			if(buffer[i] > mean && buffer[i+WIDTH_SLOPE] < mean)
			{
				begin = i;
				stop = 1;
			}
			i++;
		}
		//if a begin was found, search for an end
		if (i < (IMAGE_BUFFER_SIZE - WIDTH_SLOPE) && begin)
		{
			stop = 0;

			while(stop == 0 && i < IMAGE_BUFFER_SIZE)
			{
				if(buffer[i] > mean && buffer[i-WIDTH_SLOPE] < mean)
				{
					end = i;
					stop = 1;
				}
				i++;
			}
			//if an end was not found
			if (i > IMAGE_BUFFER_SIZE || !end)
			{
				line_not_found = 1;
			}
		}
		else//if no begin was found
		{
			line_not_found = 1;
		}

		//if a line too small has been detected, continues the search
		if(!line_not_found && (end-begin) < MIN_LINE_WIDTH){
			i = end;
			begin = 0;
			end = 0;
			stop = 0;
			wrong_line = 1;
		}
	}while(wrong_line);

	if(line_not_found){
		begin = 0;
		end = 0;
		width = PXTOCM/MAX_DISTANCE;
	}else{
		width = (end - begin);
	}

	//sets a maximum width or returns the measured width
	if((PXTOCM/width) >= MAX_DISTANCE){
		return PXTOCM/MAX_DISTANCE;
	}else{
		return width;
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
	bool send_to_computer = true;

	while(1){
		//waits until an image has been captured
		chBSemWait(&image_ready_sem);
		//gets the pointer to the array filled with the last image in RGB565
		img_buff_ptr = dcmi_get_last_image_ptr();

		//Extracts only the red pixels
		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){
			//extracts first 5bits of the first byte
			//takes nothing from the second byte
			imageRed[i/2] = (uint8_t)(img_buff_ptr[i]&0xF8)/8; //red pixels
			//extracts last 5bits of the first byte
			//takes nothing from the first byte
			imageBlue[i/2] =  (uint8_t)img_buff_ptr[i+1]&0x1F; //blue
		}

		if(send_to_computer){
			//sends to the computer the image
			SendUint8ToComputer(imageRed, IMAGE_BUFFER_SIZE);
		}
		//invert the bool
		send_to_computer = !send_to_computer;

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

		if((distance_fond_cm < GOAL_DISTANCE_FOND) && (play == true)){
			if(meanRed < meanBlue) {
				perdant ='b';
				++scoreRed;
				play = false;
			} else {

				perdant ='r';
				++scoreBlue;
				play = false;

			}
		}
	}
}

float get_distance_fond_cm(void){
	return distance_fond_cm;
}

char get_perdant(void){
	return perdant;
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
