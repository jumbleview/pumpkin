# Pumpkin

This project is about controlling two 10mm three-color common anode LEDs with Attiny85 chip. 
Project program uses Adam Dunkels Protothreads library and you can see this program as illustration of  how
to use protothreads: small but efficient C framework. (I am in no way affiliated with Adam Dunkels apart of to be 
user of his library). 
LED represents two multicolored eyes of Pumpkin Halloween Glitter.
![pumpkin](./images/ezgif.com-gif-maker.gif)
## Circut 
Design uses five chip pins: 
* Two pins used to control anodes:  each LED anode attached to the dedicated pin.
* Three pins attached (through resistors) to LEDs cathodes (the same color cathode of each led attached to the same pin);
![circut](./images/pumpkin_crop.png)
Note. Design can't use all six in/out pins because pin PB5 (RESET) is
a weak pin capable to provide just ~2 mA of the current, while there is need to have ~20 mA.
## Timing diagram
![timing](./images/pumpkin_diagram.jpeg)
Top two rows on diagram shows voltage change on LED anodes. Voltage on pins connected to LED anodes oscillates with frequency ~ 250 Hz.  This voltage oscillation for left eye is an opposite to the oscillation on right eye. When voltage on anode is high corresponding LED may be lighted. When it is  low corresponding LED is dark. That means each LED may be bright during  2 milliseconds interval and is dark during another 2 milliseconds. Because human eye has some inertia, 250 Hz blinking is not noticeable by observer.
Bottom three rows on diagram shows change of voltage on pins connected to LEDs cathodes when lighting up LEDs in various  colors. Let us look on the first diagram column. It shows the case when left LED is in red color and right LED in green color. Here RED cathodes stays low while left anode is high, GREEN cathode stays low while right anode is high, and BLUE cathode stays low all the time. Other columns on diagram shows combinations of cathode and anode voltage for various colors.
## Programming
This section  contains highlights related to protothreads usage. I simplified code a little to make it shorter and replaced some defined constant with numbers to be more explicit. 

### Includes and Definitions
Here are some basic code which is not specific to protothreads:
``` C

#include <avr/io.h>
//  CKDIV8 cleared. 8 MHz; if F_CPU   1000000UL // CKDIV8 set. 1 MHz
#define 	F_CPU   8000000UL
#include <util/delay.h>
#include "../pt-1.4/pt.h" // http://dunkels.com/adam/pt/
#define PinOutHigh(bit) PORTB |= (1 << (bit))
#define PinOutLow(bit) PORTB &= ~(1 << (bit))
#define	LEFT_ANOD	DDB3
#define	RIGHT_ANOD	DDB4
#define	RED_LED		DDB0
#define	GREEN_LED	DDB1
#define	BLUE_LED	DDB2

```
In addition includes of files coming with Atmel Studio there is include of protothread library header.
Next there are to macros to manipulate pins and definitions  to give logical names to pin numbers.

### Main Loop
``` C
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
		_delay_ms(2); 
		RightEye(&rpt, &ri);
		_delay_ms(2); 		
	}
}
```
Function main after doing some initialization stays in forever loop. In that loop it makes next steps:
* Invokes protothread routing for Left Eye. It changes some pin voltage.
* Make 2 MS delay.  There is no change in pin voltage.
* Invokes protothread for Right Eye. It changes some pin voltage.
* Make 2 MS delay.  There is no change in pin voltage.

### Auxiliary functions
Functions to set particular color are straightforward. Here is an example
``` C

void RedColor() {
	PinOutLow(RED_LED);
	PinOutHigh(GREEN_LED);
	PinOutHigh(BLUE_LED);
}
void GreenColor() {
	PinOutHigh(RED_LED);
	PinOutLow(GREEN_LED);
	PinOutHigh(BLUE_LED);
}
...
```
There are more functions like this: as many function as number of supported colors (seven) and one more for the dark  LED (NoColor).

Here is function which  is directly invoked by protothread routine.
``` C

int16_t DoAndCountdown( void (*f)(), int16_t initial, int16_t *counter) {
	(*f)();
	if (*counter == 0) {
		*counter = initial;
	}
	--(*counter);
	return *counter;
}
```

 Technically speaking usage of such a function is not mandatory but I found it convenient. It has three arguments:
* Address of function setting LED color (like GreenColor or RedColor etc.)
* Number of how many times this function must be invoked at particular protothread stage.
* Address of reverse counter of this function invocation at particular protothread stage.

Function returns value of reverse counter.

### Protothreads routine

And here is core of framework: protothread routine. For the sake of simplicity example limited only to three steps: for color  change from RED, to GREEN, and to BLUE.

``` C

int LeftEye(struct pt* mlpt, int16_t* i) {
	PinOutLow(RIGHT_ANOD);
	PinOutHigh(LEFT_ANOD);
	PT_BEGIN(mlpt); 
		PT_WAIT_UNTIL(mlpt, DoAndCountdown(RedColor, 250, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(GreenColor, 250, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(BlueColor, 250, i) == 0);
	PT_END(mlpt);
}
int RightEye(struct pt* mlpt, int16_t* i) {
	PinOutLow(LEFT_ANOD);
	PinOutHigh(RIGHT_ANOD);
	PT_BEGIN(mlpt); 
		PT_WAIT_UNTIL(mlpt, DoAndCountdown(GreenColor, 250, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(BlueColor, 250, i) == 0);

		PT_WAIT_UNTIL(mlpt, DoAndCountdown(RedColor, 250, i) == 0);
	PT_END(mlpt);
}


```
Function is invoked with two arguments:
* Address of protothread structure. That structure was initialized by *main* before main loop started.
* Address of reverse counter. It was set to 0 by *main*  before main loop started.

After setting voltages to make left LED active function performs protothread segment. Each step of this segment presented by protothread macros PT_WAIT_UNTIL. This macros hides next actions:
* Invocation of  function *DoAndCountdown*. That sets voltage on LED cathodes in order for particular color . 
* Returned result compared with 0. If condition is 'false' protothread function immediately returns. That yields control to the main loop. 
* When protothread is invoked next time it, after execution code before PT_BEGIN jumps directly inside the PT_WAIT macros from which it returned last time.
* Such a process is repeated until result of *DoAndCountdown* is 0. In that case there is no return out of protothread and program executes next line of the code. In our case it is next PT_WAIT_UNTIL but generally speaking it could be almost any C code.
* State of *struct* *pt* is reset as soon as control reaches PT_END macros. When function is invoked next time protothread segment starts execute line of the code right after PT_BEGIN.

As far as pumpkin has two eyes, there is one more protothread routine for the right eye. It is similar to described above apart of the fact that it enforces different order of colors. Whole program is less then 200 lines of code and takes less than 20% of Atinny85 code memory. If needed it is possible to utilize in single program several more protothread routines and assign much more complicated logic to them.

## Further Reading
* If you would like to know more about protothreads I advise you directly consult with Adam Dunkels site: http://dunkels.com/adam/pt/ . 
* Protothread internals revealed here: http://dunkels.com/adam/pt/expansion.html




