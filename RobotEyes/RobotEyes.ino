/*************************************************** 
 *  Programme d'animation des yeux du robot Maker Fight
 *  
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "config.h"
#include "RobotBrain.h"

RobotBrain brain = RobotBrain();

#define CHANNEL_1_PIN 2
volatile unsigned long timer_start;
volatile int last_interrupt_time;
volatile int pulse_time;

void calcSignal() 
{
    //record the interrupt time so that we can tell if the receiver has a signal from the transmitter 
    last_interrupt_time = micros(); 
    //if the pin has gone HIGH, record the microseconds since the Arduino started up 
    if(digitalRead(CHANNEL_1_PIN) == HIGH) 
    { 
        timer_start = micros();
    } 
    //otherwise, the pin has gone LOW 
    else
    { 
        //only worry about this if the timer has actually started
        if(timer_start != 0)
        { 
            //record the pulse time
            pulse_time = ((volatile int)micros() - timer_start);
            //restart the timer
            timer_start = 0;
        }
    } 
} 

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  
  attachInterrupt(digitalPinToInterrupt(CHANNEL_1_PIN), calcSignal, CHANGE);

  brain.init(LEFT_EYE_ADDR, RIGHT_EYE_ADDR);  
}

void loop() {
  //unsigned long t = millis();
  brain.run();
  //t = millis() - t;
  
  Serial.println(pulse_time);
 }
