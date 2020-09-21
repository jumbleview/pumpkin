/*
 * pumpkin.c
 *
 * Created: 8/26/2020 6:25:48 PM
 *  Author: Jumbleview
 *
 *
 * Program controls two 10mm three-color common anode LEDs on Attiny85 chip. 
 * LED represents two eyes of Pumpkin Halloween Glitter: so eyes change its color 
 * Design uses five chip pins: 
 *    Three attached to LEDs cathodes (the same color cathode of each led attached to the same pin);
 *    Two pins used to control anodes:  each LED anode attached to the dedicated pin.
 *
 *    Program uses Adam Dunkels Protothreads library
 */

#include <avr/io.h>
//  CKDIV8 cleared. 8 MHz; if F_CPU   1000000UL // CKDIV8 set. 1 MHz
#define 	F_CPU   8000000UL // CKDIV8 cleared. 8 MHz
#include <util/delay.h>
#include "../pt-1.4/pt.h" // http://dunkels.com/adam/pt/

#define PinOutHigh(bit) PORTB |= (1 << (bit))

#define PinOutLow(bit) PORTB &= ~(1 << (bit))

#define	LEFT_ANOD	DDB3
#define	RIGHT_ANOD	DDB4

#define	RED_LED		DDB0
#define	GREEN_LED	DDB1
#define	BLUE_LED	DDB2

// High voltage stays on one LED anode during this time (MS)
#define TIME_INTERVAL 2

#define TIME_FACTOR(time) (time/(TIME_INTERVAL*2))

// Pure LED color (RGB) will stay 1000 MS ~ 1 Second
#define PURE_LIMIT TIME_FACTOR(1000)

// Mixed LED color (Cyan, Magenta, Yellow, WHite ) will stay 750 MS ~ 0.75 Second.
#define MIXED_LIMIT TIME_FACTOR(750)

// Dark LED  will stay 500 MS  ~ 0.5 Second
#define DARK_LIMIT TIME_FACTOR(500)

// Pure color procedures
void GreenColor() {
	PinOutHigh(RED_LED);
	PinOutLow(GREEN_LED);
	PinOutHigh(BLUE_LED);
}

void RedColor() {
	PinOutLow(RED_LED);
	PinOutHigh(GREEN_LED);
	PinOutHigh(BLUE_LED);
}

void BlueColor() {
	PinOutHigh(RED_LED);
	PinOutHigh(GREEN_LED);
	PinOutLow(BLUE_LED);
}

// Mixed color procedures
void WhiteColor() {
	PinOutLow(RED_LED);
	PinOutLow(GREEN_LED);
	PinOutLow(BLUE_LED);
}

void YellowColor() {
	PinOutLow(RED_LED);
	PinOutLow(GREEN_LED);
	PinOutHigh(BLUE_LED);
}

void CyanColor() {
	PinOutHigh(RED_LED);
	PinOutLow(GREEN_LED);
	PinOutLow(BLUE_LED);
}

void MagentaColor() {
	PinOutLow(RED_LED);
	PinOutHigh(GREEN_LED);
	PinOutLow(BLUE_LED);
}

void NoColor() {
	PinOutHigh(RED_LED);
	PinOutHigh(GREEN_LED);
	PinOutHigh(BLUE_LED);
}

// Function 'DoAndCountdown' invokes function (first argument) to light up LED(s). It returns value which macro PT_WAIT_UNTIL uses as left side condition.
int16_t DoAndCountdown( void (*f)(), int16_t initial, int16_t *counter) {
	(*f)();
	if (*counter == 0) {
		*counter = initial;
	}
	--(*counter);
	return *counter;
}

// Protothread procedure for left eye. It defines consequent change of eye color and how long each color stays
int LeftEye(struct pt* mlpt, int16_t* i) {
	
	PinOutLow(RIGHT_ANOD);
	PinOutHigh(LEFT_ANOD);
	
	PT_BEGIN(mlpt); 
		PT_WAIT_UNTIL(mlpt, DoAndCountdown(RedColor, PURE_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(GreenColor, PURE_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(BlueColor, PURE_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(WhiteColor, MIXED_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(YellowColor, MIXED_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(CyanColor, MIXED_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(GreenColor, PURE_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(MagentaColor, MIXED_LIMIT,  i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(RedColor, PURE_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(YellowColor, MIXED_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(WhiteColor, MIXED_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(NoColor, DARK_LIMIT,  i) == 0);
		PT_YIELD(mlpt);
	PT_END(mlpt);
}

// Protothread procedure for right eye
int RightEye(struct pt* mlpt, int16_t* i) {
	
	PinOutHigh(RIGHT_ANOD);
	PinOutLow(LEFT_ANOD);
	
	PT_BEGIN(mlpt);
		PT_WAIT_UNTIL(mlpt, DoAndCountdown(GreenColor, PURE_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(BlueColor, PURE_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(RedColor, PURE_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(NoColor, DARK_LIMIT, i) == 0);
		PT_YIELD(mlpt);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(YellowColor, MIXED_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(WhiteColor, MIXED_LIMIT, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(CyanColor, MIXED_LIMIT, i) == 0);
	
		PT_WAIT_UNTIL(mlpt, DoAndCountdown(MagentaColor, MIXED_LIMIT, i) == 0);
	
		PT_WAIT_UNTIL(mlpt, DoAndCountdown(BlueColor, PURE_LIMIT, i) == 0);
	
		PT_WAIT_UNTIL(mlpt, DoAndCountdown(YellowColor, MIXED_LIMIT, i) == 0);
	
		PT_WAIT_UNTIL(mlpt, DoAndCountdown(BlueColor, PURE_LIMIT, i) == 0);	
	
		PT_WAIT_UNTIL(mlpt, DoAndCountdown(WhiteColor, MIXED_LIMIT, i) == 0);
	PT_END(mlpt);
}

void initialize() {
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB4);
	PinOutLow(LEFT_ANOD);
	PinOutLow(RIGHT_ANOD);
	NoColor();
	_delay_ms(1000.0);
}

int main(void) {
	initialize();
	struct pt lpt; // protothread descriptor: left
	struct pt rpt; // protothread descriptor: right
	PT_INIT(&lpt);
	PT_INIT(&rpt);
	int16_t li = 0; // function counter: left
	int16_t ri = 0; // function counter: right
	for(;;) { // main loop
		LeftEye(&lpt, &li);
		_delay_ms(TIME_INTERVAL); 
		RightEye(&rpt, &ri);
		_delay_ms(TIME_INTERVAL); 		
	}
}
