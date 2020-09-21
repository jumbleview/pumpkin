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
Top two rows on diagram shows voltage change on LED anodes. Voltage on pins connected to LED anodes oscillate with frequency ~ 250 Hz.  This voltage oscillation on anode for left eye is an opposite to the oscillation on right eye. When voltage on anode is high corresponding LED may be lighted. When it is  low corresponding LED is dark. That means each LED is dark during 2 milliseconds interval and may be bright during another 2 milliseconds. Because human eye has some inertia, 250 Hz blinking is not noticeable by observer.
Bottom three rows on diagram shows change of voltage on pins connected to LEDs cathodes when lighting up LEDs in various  colors. Let us look for example on the first diagram column. It shows the case when left LED is in red color and right LED in green color. Here RED cathodes stays low while left anode is high, GREEN cathode stays low while right anode is high, and BLUE cathode stays low all time. Other columns on diagram shows combinations of cathode and anode voltage for various colors.
## Programming
This section  contains highlights related to protothreads usage.
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
		_delay_ms(TIME_INTERVAL); 
		RightEye(&rpt, &ri);
		_delay_ms(TIME_INTERVAL); 		
	}
}
```
Function main after doing some initialization stays in forever loop. In that loop it makes next steps:
* Invokes protothread routing for Left Eye. It changes some pin voltage.
* Make 2 MS delay.  There is no change in pin voltage
* Invokes protothread for Right Eye. It changes some pin voltage.
* Make 2 MS delay.  There is no change in pin voltage

### Auxiliary functions
Functions to set particular color are straightforward. Here is an example
``` C
void GreenColor() {
	PinOutHigh(RED_LED);
	PinOutLow(GREEN_LED);
	PinOutHigh(BLUE_LED);
}

```
There are seven more functions like this: each function per color and it includes case of dark  LED (NoColor).

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

Function above is directly invoked by protothread routing. Technically speaking usage of such a function is not mandatory but I found it convenient.
It has three arguments:
* Address of function setting LED color (like GreenColor or RedColor etc)
* Number of how many times this function must be invoked on particular protothread step.
* Address of reverse counter of this function invocation at particular protothread step.

Function returns value reverse counter.

### Protothreads routings

And here is core of framework. Protothread routing. For the sake of simplicity I limited exmple here only to three steps: to change color from RED, to GREEN, and to BLUE.

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

```
THis function invoked with two arguments:
* Address if protothread structure. It was initialized by *main* before main loop started.
* Address of reverse counter. It was set to 0 by *main*  before main loop started.
At first function set voltages to make left LED active and then it starts protothread steps. Each step of routing presented by protothread macros PT_WAIT_UNTIL. This macros hides next actions:
* Invoke function *DoAndCountdown*. That sets voltage on LED cathodes in particular color order. 
* It compare returned result with 0. If condition is 'false' it immediately returns from the function. That yield code execution to main loop. 
* When protothred routing is invoked next time it, after execution code before PT_BEGIN jump inside teh PT_WAIT macros from which it returned last time.
* That process is repeated until result of *DoAndCountdown* returns 0. In that case there is no return out of protothread routing and program executes next line of the code. In our case it is next PT_WAIT_UNTIL but generally speaking it could be almost any C code.
* WHen protothread reaches PT_END macros state of struct pt is reset. When protothread routing is invoked next time the code will program will start execute line of code right after PT_BEGIN

If you would like to know more about protothreads you may consult Adam Dunkels site: http://dunkels.com/adam/pt/ . Protothread internals revealed
here: http://dunkels.com/adam/pt/expansion.html




