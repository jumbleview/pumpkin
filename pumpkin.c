/*
 * pumpkin.c
 *
 * Created: 8/26/2020 6:25:48 PM
 *  Author: Jumbleview
 *
 *
 * Program controls two 10mm three-color LED on Attiny85 chip.
 * LED represents two eyes of Pumpkin Halloween Glitter
 * Design uses five chip pins: 
 *    Three attached to LEDs cathodes (the same color cathode of each led to the same pi,n);
 *    Two pins used to control anodes:  each LED anode attached to the dedicated pin
 *    Program uses Adam Dunkels Protothreads library
 *
 */

#include <avr/io.h>
//#define 	F_CPU   1000000UL // CKDIV8 set. 1 MHz
#define 	F_CPU   8000000UL // CKDIV8 cleared. 8 MHz
#include <util/delay.h>
#include "../pt-1.4/pt.h" // http://dunkels.com/adam/pt/

#define setAllPinOut() DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB4)

#define pinHigh(bit) PORTB |= (1 << (bit))

#define pinLow(bit) PORTB &= ~(1 << (bit))

#define	LEFT_ANOD	DDB3
#define	RIGHT_ANOD	DDB4

#define	RED_LED		DDB0
#define	GREEN_LED	DDB1
#define	BLUE_LED	DDB2

#define PURE_LIMIT 199
#define MIXED_LIMIT 151
#define DARK_LIMIT 97

typedef enum {EYE_LEFT, EYE_RIGHT} EYE;

EYE eye = EYE_LEFT;
 
struct pt lpt; // protothread descriptor: left
struct pt rpt; // protothread descriptor: right

// Pure color procedures
void greenEye() {
	pinHigh(RED_LED);
	pinLow(GREEN_LED);
	pinHigh(BLUE_LED);
}

void redEye() {
	pinLow(RED_LED);
	pinHigh(GREEN_LED);
	pinHigh(BLUE_LED);
}

void blueEye() {
	pinHigh(RED_LED);
	pinHigh(GREEN_LED);
	pinLow(BLUE_LED);
}

// Mixed color procedures
void whiteEye() {
	pinLow(RED_LED);
	pinLow(GREEN_LED);
	pinLow(BLUE_LED);
}

void yellowEye() {
	pinLow(RED_LED);
	pinLow(GREEN_LED);
	pinHigh(BLUE_LED);
}

void cyanEye() {
	pinHigh(RED_LED);
	pinLow(GREEN_LED);
	pinLow(BLUE_LED);
}

void magentaEye() {
	pinLow(RED_LED);
	pinHigh(GREEN_LED);
	pinLow(BLUE_LED);
}

void darkEye() {
	pinHigh(RED_LED);
	pinHigh(GREEN_LED);
	pinHigh(BLUE_LED);
}

// Protothread procedure for left eye
int eyeLeft(struct pt* mlpt) {
	static int16_t i=0;
	PT_BEGIN(mlpt); 	//
	for (i=0; i < PURE_LIMIT; i++) {
		redEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < PURE_LIMIT; i++) {
		greenEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < PURE_LIMIT; i++) {
		blueEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < MIXED_LIMIT; i++) {
		whiteEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < PURE_LIMIT; i++) {
		redEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < MIXED_LIMIT; i++) {
		yellowEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < MIXED_LIMIT; i++) {
		cyanEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < PURE_LIMIT; i++) {
		greenEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < MIXED_LIMIT; i++) {
		magentaEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < PURE_LIMIT; i++) {
		greenEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < DARK_LIMIT; i++) {
		darkEye();
		PT_YIELD(mlpt);
	}
	PT_RESTART(mlpt);	//
	PT_END(mlpt);
}

// Protothread procedure for right eye
int eyeRight(struct pt* mlpt) {
	static int16_t i=0;
	PT_BEGIN(mlpt); 	
	for (i=0; i < DARK_LIMIT; i++) {
		darkEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < PURE_LIMIT; i++) {
		greenEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < MIXED_LIMIT; i++) {
		magentaEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < PURE_LIMIT; i++) {
		redEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < MIXED_LIMIT; i++) {
		yellowEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < PURE_LIMIT; i++) {
		blueEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < MIXED_LIMIT; i++) {
		whiteEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < PURE_LIMIT; i++) {
		greenEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < PURE_LIMIT; i++) {
		redEye();
		PT_YIELD(mlpt);
	}
	for (i=0; i < MIXED_LIMIT; i++) {
		cyanEye();
		PT_YIELD(mlpt);
	}
	PT_RESTART(mlpt);	
	PT_END(mlpt);
}

// Function changeEye switches voltage between LEDs and call protothread functions to apply programmed color
void changeEye() {
	pinHigh(RED_LED);
	pinHigh(GREEN_LED);
	pinHigh(BLUE_LED);
	if (eye == EYE_LEFT) {
		eye = EYE_RIGHT;
		pinHigh(RIGHT_ANOD);
		pinLow(LEFT_ANOD);
		// Voltage applied to Right LED 
		eyeRight(&rpt);
	} else {
		eye = EYE_LEFT;
		pinLow(RIGHT_ANOD);
		pinHigh(LEFT_ANOD);
		// Voltage applied to Left LED 		
		eyeLeft(&lpt);
	}
}

int main(void) {
	// initiate protothread structures
	PT_INIT(&lpt); 
	PT_INIT(&rpt); 
	// Set all pins to output mode
	setAllPinOut();
	// main loop
	while(1) {
			_delay_ms(2.0);
			changeEye();
	}
}




