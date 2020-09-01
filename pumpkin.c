/*
 * pumpkin85.c
 *
 * Created: 8/26/2020 6:25:48 PM
 *  Author: Jumbleview
 */ 

#include <avr/io.h>
//#define 	F_CPU   1000000UL // CKDIV8 set. 1 MHz
#define 	F_CPU   8000000UL // CKDIV8 cleared. 8 MHz
#include <util/delay.h>
#include "../pt-1.4/pt.h" // http://dunkels.com/adam/pt/


#define setAllPinOut() \
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB4);


#define pinHigh(bit) PORTB |= (1 << (DDB##bit));

#define pinLow(bit) PORTB &= ~(1 << (DDB##bit));

//  LEFT ANOD bit 3;
//  RIGHT ANOD bit 4;

//  RED LED bit 0;
//  GREEN LED bit 1;
//  BLUE LED bit 2;

typedef enum {EYE_LEFT, EYE_RIGHT} EYE;

EYE eye = EYE_LEFT;
void changeEye() {
	pinHigh(2);
	pinHigh(1);
	pinHigh(0);
	if (eye == EYE_LEFT) {
		eye = EYE_RIGHT;
		pinHigh(4);
		pinLow(3);
	} else {
		eye = EYE_LEFT;
		pinLow(4);
		pinHigh(3);
	}
}
 
struct pt lpt; // protothread descriptor: left
struct pt rpt; // protothread descriptor: right

void greenEye() {
	pinLow(1);
	pinHigh(2);
	pinHigh(0);
}

void redEye() {
	pinHigh(2);
	pinHigh(1);
	pinLow(0);
}

void blueEye() {
	pinLow(2);
	pinHigh(1);
	pinHigh(0);
}


void whiteEye() {
	pinLow(2);
	pinLow(1);
	pinLow(0);
}

void yellowEye() {
	pinHigh(2);
	pinLow(1);
	pinLow(0);
}


void darkEye() {
	pinHigh(2);
	pinHigh(1);
	pinHigh(0);
}

int eyeLeft(struct pt* mlpt)
{
	static int16_t i=0;
	PT_BEGIN(mlpt); 	//
	for (i=0; i < 180; i++) {
		blueEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < 180; i++) {
		redEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < 180; i++) {
		greenEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < 180; i++) {
		whiteEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < 180; i++) {
		yellowEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < 90; i++) {
		darkEye();
		PT_YIELD(mlpt);
	}
	PT_RESTART(mlpt);	//
	PT_END(mlpt);
}

int eyeRight(struct pt* mlpt)
{
	static int16_t i=0;
	PT_BEGIN(mlpt); 	
	for (i=0; i < 200; i++) {
		redEye();
		PT_YIELD(mlpt);		
	}
	for (i=0; i < 200; i++) {
		greenEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < 200; i++) {
		blueEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < 200; i++) {
		yellowEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < 200; i++) {
		whiteEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < 100; i++) {
		darkEye();
		PT_YIELD(mlpt);
	}
	PT_RESTART(mlpt);	
	PT_END(mlpt);
}

int main(void)
{
	// initiate protothread structure...	
	PT_INIT(&lpt); 
	PT_INIT(&rpt); 
	// program pins for output
	setAllPinOut();
	// main loop
	while(1) {
			_delay_ms(2.0);
			changeEye();
			if (eye == EYE_LEFT) {
				eyeLeft(&lpt);
			} else {
				eyeRight(&rpt);
			}
	}
}




