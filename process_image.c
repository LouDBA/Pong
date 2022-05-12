#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>
#include <chprintf.h>
#include <process_image.h>


static float distance_fond_cm = 0;
static uint8_t imageRed[IMAGE_BUFFER_SIZE] = {0};
static uint8_t imageBlue[IMAGE_BUFFER_SIZE] = {0};
static uint8_t scoreRed = 0;
static uint8_t scoreBlue = 0;
static bool play = true;
static float last_distance_cm = 0;

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

/*
 *  Returns the line's width extracted from the image buffer given
 *  Returns 0 if line not found
 */
uint16_t extract_line_width(uint8_t *buffer, uint32_t mean){
	uint32_t width = 0;
	uint32_t width1 = 0;
	uint32_t width2 = 0;
	uint32_t width3 = 0;
	//uint32_t colorvalue = 0;

	bool flancmontant = false;
	bool flancdescendant = false;

	bool bande = false;
	bool firstbandeok = false;
	bool secondbandeok = false;

	for(int i = 0; i < (IMAGE_BUFFER_SIZE - 1); ++i) {
		if(firstbandeok == false){
			if((bande == false) && (buffer[i] > mean) && (buffer[i+1] < mean)) { //détection des lignes de couleur
				bande = true;
				flancmontant = true;
			}
			if((bande == true) && (buffer[i] < mean)){
				++width1;
				if (buffer[i+1] > mean) {
					bande = false;
					flancdescendant = true;
				}
			}
			if((bande == false) && (width1 < MIN_LINE_WIDTH)) {
				width1 = 0;
			}
			if((bande == false) && (width1 > MIN_LINE_WIDTH)) {
				firstbandeok = true;
				width = width1;
			}
			//width = width1;
		} else {
			if(secondbandeok == false){
				if((bande == false) && (buffer[i] > mean) && (buffer[i+1] < mean)) { //détection des lignes de couleur
					bande = true;
				}
				if((bande == true) && (buffer[i] < mean)){
					++width2;
					if (buffer[i+1] > mean) {
						bande = false;
					}
				}
				if((bande == false) && (width2 < MIN_LINE_WIDTH)) {
					width2 = 0;
				}
				if((bande == false) && (width2 > MIN_LINE_WIDTH)) {
					secondbandeok = true;
					if(width2 > width) {
						width = width2;
					}
				}
			} else {
				if((bande == false) && (buffer[i] > mean) && (buffer[i+1] < mean)) { //détection des lignes de couleur
					bande = true;
				}
				if((bande == true) && (buffer[i] < mean)){
					++width3;
					if (buffer[i+1] > mean) {
						bande = false;
					}
				}
				if((bande == false) && (width2 < MIN_LINE_WIDTH)) {
					width3 = 0;
				}
				if((bande == false) && (width2 > MIN_LINE_WIDTH)) {
					if(width3 > width){
						width = width3;
					}
				}
			}
		}
	}
	if(!flancdescendant || !flancmontant) {
		width = 0;
	}
	if(width > MIN_LINE_WIDTH) {
		return width;
	}else {
		return (PXTOCM/MAX_DISTANCE);
	}
}

//	uint32_t width = 0;
//	uint16_t debut_haut = 0, debut_bas = 0, ecart = 0;
//	bool haut = false, bas = false;
//
//	for(unsigned int i = 0; i < IMAGE_BUFFER_SIZE ; ++i)
//	{
//		if((buffer[i] > (mean * 3/2)) && (!haut))
//		{
//			debut_haut = i;
//			if(bas)
//			{
//				ecart = i - debut_haut;
//			}
//			haut = true;
//			bas = false;
//		} else if((buffer[i] < (mean / 2)) && (!bas)) {
//			debut_bas = i;
//			if(haut)
//			{
//				ecart = i - debut_haut;
//			}
//			haut = false;
//			bas = true;
//		} else {
//			bas = haut = false;
//		}
//
//		if(ecart > width)
//		{
//			width = ecart;
//		}
//
//	}
//
//	if(width < MIN_LINE_WIDTH){
//		width = PXTOCM/MAX_DISTANCE;
//	}
//	//chprintf((BaseSequentialStream *)&SD3, "%6u \r\n", (uint32_t)width);
//	return width;
//}


//	do{
//		wrong_line = 0;
//		//search for a begin
//		while(stop == 0 && i < (IMAGE_BUFFER_SIZE - WIDTH_SLOPE))
//		{
//			//the slope must at least be WIDTH_SLOPE wide and is compared
//			//to the mean of the image
//			if(buffer[i] > mean && buffer[i+WIDTH_SLOPE] < mean)
//			{
//				begin = i;
//				colorvalue = buffer[i];
//				stop = 1;
//			}
//			i++;
//		}
//		//if a begin was found, search for an end
//		if (i < (IMAGE_BUFFER_SIZE - WIDTH_SLOPE) && begin)
//		{
//			stop = 0;
//
//			while(stop == 0 && i < IMAGE_BUFFER_SIZE)
//			{
//				if(buffer[i] > mean && buffer[i-WIDTH_SLOPE] < mean)
//				{
//					end = i;
//					stop = 1;
//				}
//				i++;
//			}
//			//if an end was not found
//			if (i > IMAGE_BUFFER_SIZE || !end)
//			{
//				line_not_found = 1;
//			}
//		}
//		else//if no begin was found
//		{
//			line_not_found = 1;
//		}
//
//		//if a line too small has been detected, continues the search
//		if(!line_not_found && (end-begin) < MIN_LINE_WIDTH){
//			i = end;
//			begin = 0;
//			end = 0;
//			stop = 0;
//			wrong_line = 1;
//		}
//	}while(wrong_line);
//
//	if(line_not_found){
//		begin = 0;
//		end = 0;
//		width = PXTOCM/MAX_DISTANCE;
//	}else{
//		width = (end - begin);
//	}
//
//	//sets a maximum width or returns the measured width
//	if((PXTOCM/width) >= MAX_DISTANCE){
//		return PXTOCM/MAX_DISTANCE;
//	}else{
//		return width;
//	}


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
	//bool sendtocomputer = true;
	uint8_t sendtocomputer = 0;
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
		if((distance_fond_cm < GOAL_DISTANCE_FOND) && (last_distance_cm > GOAL_DISTANCE_FOND + 5)) {
			distance_fond_cm =+ 5;
		}
		last_distance_cm = distance_fond_cm;
		//distance_fond_cm = last_distance;
		//}
		//last_distance = PXTOCM/lineWidth;
		if(sendtocomputer <2 ) {

			++sendtocomputer;
		}else {
			chprintf((BaseSequentialStream *) &SD3, "width : %d\r\n",lineWidth );
			chprintf((BaseSequentialStream *) &SD3, "distance : %f\r\n",distance_fond_cm );
			//SendUint8ToComputer(imageBlue, IMAGE_BUFFER_SIZE);
			sendtocomputer = 0;
		}



		if((distance_fond_cm < GOAL_DISTANCE_FOND) && (play == true)){
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


float get_distance_fond_cm(void){
	return distance_fond_cm;
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
