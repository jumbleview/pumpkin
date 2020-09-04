/*
 * pumpkin.c
 *
 * Created: 8/26/2020 6:25:48 PM
 *  Author: Jumbleview
 *
 *
 * Program controls two 10mm three-color common anode LEDs on Attiny85 chip. 
 * LED represents two eyes of Pumpkin Halloween Glitter: so eyes changes its color 
 * Design uses five chip pins: 
 *    Three attached to LEDs cathodes (the same color cathode of each led attached to the same pin);
 *    Two pins used to control anodes:  each LED anode attached to the dedicated pin.
 *
 *    Program uses Adam Dunkels Protothreads library
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

// It is the time (mls) which high voltage stays on one LED anode
#define TAKT_TIME 2.5

// Pure LED color (RGB) will stay ~ 1 second
#define PURE_LIMIT 200

// Mixed LED color (Cyan, Magenta, Yellow, WHite ) will stay ~ 0.75 second.
#define MIXED_LIMIT 150

// Dark LED  will stay for 100 takts (~ 0.5 second)
#define DARK_LIMIT 100


typedef enum {EYE_LEFT, EYE_RIGHT} EYE;
 
struct pt lpt; // protothread descriptor: left
struct pt rpt; // protothread descriptor: right

// Procedures below controls states of LED cathodes. 
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

// Function 'doAndCountdown' invokes function (first argument) to light up LED(s). It returns value which macro PT_WAIT_UNTIL uses as left side condition.
int16_t doAndCountdown( void (*f)(), int16_t *counter) {
	(*f)();
	--(*counter);
	return *counter;	
}

// Protothread procedure for left eye. It defines consequent change of eye color and how long each color stays
int eyeLeft(struct pt* mlpt) {
	static int16_t i=0;
	PT_BEGIN(mlpt); 
	// first step made by 'for' loop to invoke PT_YIELD to avoid warning  of unused PT_YELD_FLAG
	for (i=0; i < PURE_LIMIT; i++) {
		blueEye();
		PT_YIELD(mlpt);
	}
	// Other steps  use PT_WAIT_UNTIL macro providing the same functionality as 'for' loop above
	i = PURE_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(redEye, &i) <= 0);

	i = PURE_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(greenEye, &i) <= 0);

	i = PURE_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(blueEye, &i) <= 0);

	i = MIXED_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(whiteEye, &i) <= 0);

	i = PURE_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(redEye, &i) <= 0);

	i = MIXED_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(yellowEye, &i) <= 0);

	i = MIXED_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(cyanEye, &i) <= 0);

	i = PURE_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(greenEye, &i) <= 0);

	i = MIXED_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(magentaEye, &i) <= 0);

	i = PURE_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(greenEye, &i) <= 0);


	i = DARK_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(darkEye, &i) <= 0);

	PT_RESTART(mlpt);	
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
	
	i = PURE_LIMIT; 
	PT_WAIT_UNTIL(mlpt, doAndCountdown(greenEye, &i) <= 0);

	i = MIXED_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(magentaEye, &i) <= 0);

	i = PURE_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(redEye, &i) <= 0);

	i = MIXED_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(yellowEye, &i) <= 0);

	i = PURE_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(blueEye, &i) <= 0);

	i = MIXED_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(whiteEye, &i) <= 0);

	i = PURE_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(greenEye, &i) <= 0);

	i = PURE_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(redEye, &i) <= 0);

	i = MIXED_LIMIT;
	PT_WAIT_UNTIL(mlpt, doAndCountdown(cyanEye, &i) <= 0);

	PT_RESTART(mlpt);	
	PT_END(mlpt);
}

// Function controlLEDs switches voltage between LEDs anodes and call protothread functions to set cathodes states
void controlLEDs() {
	static EYE eye = EYE_LEFT;
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
		_delay_ms(TAKT_TIME); 
		controlLEDs();
	}
}
