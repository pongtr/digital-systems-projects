# Digital Systems Projects

EENG 348 with Professor Roman Kuc in Spring 2017

## (1) Chameleon
Given a colored m&m, detect its color and display it on the RGB LED.

Includes:  
0. Learn program to store color mapping to EEPROM  
1. Using hobbyist commands (eg. analogRead, analogWrite)  
2. Using register commands to replace analogRead to take in analog input  
3. Using Fast PWM for varying intensity of a color  
4. Directly transform measured RGB values to output values of RGB LED

## (2) Optical Communication
Send a message encoded in dibits via an optical channel. Message is received from Matlab through serial.

Three components:  
1. Transmitter  
2. Receiver  
3. Matlab script to send image to transmitter, read data received from receiver, then plot the error for different delay times.

## (3) Reaction time
After user sends *s* via Serial, the program measures reaction time between when the LED turns on and when the button is pressed. Takes 10 readings. 

Three versions:  
1. Using hobbyist commands (eg. digitalRead, digitalWrite)  
2. Using register commands (eg. bit manipulations of DDRB, PORTB, PINB etc.)  
3. Using interrupts (eg. creating an interrupt service routine for button press)



