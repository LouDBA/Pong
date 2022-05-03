#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

float get_distance_fond_cm(void);
char get_perdant(void);
uint8_t get_scoreRed(void);
uint8_t get_scoreBlue(void);
bool get_play(void);
void set_play(bool partie);
void set_scoreRed(uint8_t score_rouge);
void set_scoreBlue(uint8_t score_rouge);
void process_image_start(void);

#endif /* PROCESS_IMAGE_H */
