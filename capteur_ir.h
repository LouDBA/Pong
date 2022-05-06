#ifndef CAPTEUR_IR_H_
#define CAPTEUR_IR_H_


//start
void capteur_ir_start(void);

//get triggered IR captor
int get_cote_ir(void);

enum Detection_mur {
	GAUCHE_AV,
	DROIT_AV,
	GAUCHE_45,
	DROIT_45,
	GAUCHE,
	DROIT,
	GAUCHE_AR,
	DROIT_AR,
	AUCUN
};
#endif /* CAPTEUR_IR_H_ */
