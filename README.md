# pumpkin

Program controls two 10mm three-color common anode LEDs on Attiny85 chip. 
LED represents two eyes of Pumpkin Halloween Glitter: so eyes changes its color 
![pumpkin](./images/ezgif.com-gif-maker.gif)
Design uses five chip pins: 
* Three attached to LEDs cathodes (the same color cathode of each led attached to the same pin);
* Two pins used to control anodes:  each LED anode attached to the dedicated pin.
 
Program uses Adam Dunkels Protothreads library.
