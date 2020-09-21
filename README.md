# pumpkin

Program controls two 10mm three-color common anode LEDs on Attiny85 chip. 
Program uses Adam Dunkels Protothreads library. Hope this simple project gives example of how
to this small but efficient C framework. (I am in no way affiliated with Adam Dunkels, apart of be 
user of his library). 
LED represents two eyes of Pumpkin Halloween Glitter which change its color. 
![pumpkin](./images/ezgif.com-gif-maker.gif)
Design uses five chip pins: 
* Three attached to LEDs cathodes (the same color cathode of each led attached to the same pin);
* Two pins used to control anodes:  each LED anode attached to the dedicated pin.
![circut](./images/pumpkin_crop.png)
Note. Design can't use all six chip in/out pins becasue pin PB5 (RESET) is
defined in the datasheet as a weak pin capable to provide just ~2 mA of load.
![timing](./images/pumpkin_diagram.jpeg)
Timing diagram gives some help with understanding how program swicthes LEDs colors by changing level of voltage on chip pins.
