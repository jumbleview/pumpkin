# Pumpkin

This project is about controlling two 10mm three-color common anode LEDs with Attiny85 chip. 
Project program uses Adam Dunkels Protothreads library and you can see this program as illustration of  how
to use protothreads: small but efficient C framework. (I am in no way affiliated with Adam Dunkels apart of to be 
user of his library). 
LED represents two multicolored eyes of Pumpkin Halloween Glitter.
![pumpkin](./images/ezgif.com-gif-maker.gif)
Design uses five chip pins: 
* Two pins used to control anodes:  each LED anode attached to the dedicated pin.
* Three pinns attached (throuhg resistors) to LEDs cathodes (the same color cathode of each led attached to the same pin);
![circut](./images/pumpkin_crop.png)
Note. Design can't use all six in/out pins because pin PB5 (RESET) is
a weak pin capable to provide just ~2 mA of the current, while there is need to have ~20 mA.
![timing](./images/pumpkin_diagram.jpeg)
Timing diagram help to understand how program swicthes LEDs colors by changing level of voltage on pins.

