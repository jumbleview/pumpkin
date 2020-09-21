# pumpkin

Program controls two 10mm three-color common anode LEDs on Attiny85 chip. 
Program uses Adam Dunkels Protothreads library. Hope this simple project gives example how
to use this small but efficient C framework. (I am in no way affiliated with Adam Dunkels, apart of be 
user of his library). 
LED represents two eyes of Pumpkin Halloween Glitter which change its color. 
![pumpkin](./images/ezgif.com-gif-maker.gif)
Design uses five chip pins: 
* Two pins used to control anodes:  each LED anode attached to the dedicated pin.
* Three attached to LEDs cathodes (the same color cathode of each led attached to the same pin);
![circut](./images/pumpkin_crop.png)
Note. Design can't use all six chip in/out pins because pin PB5 (RESET) is
a weak pin capable to provide just ~2 mA of the current.
![timing](./images/pumpkin_diagram.jpeg)
Timing diagram help to understand how program swicthes LEDs colors by changing level of voltage on chip pins.

