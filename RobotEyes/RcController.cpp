/***************************************************
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#include "RcController.h"

//#define CHANNEL_1_PIN 2

#ifdef THREE_STATE_CHANNEL
volatile unsigned long ts_timer_start;
volatile int ts_last_interrupt_time;
volatile int ts_pulse_time;
void tsProcessSignal() 
{
    //record the interrupt time so that we can tell if the receiver has a signal from the transmitter 
    ts_last_interrupt_time = micros(); 
    //if the pin has gone HIGH, record the microseconds since the Arduino started up 
    if(digitalRead(THREE_STATE_CHANNEL) == HIGH) 
    { 
        ts_timer_start = micros();
    } 
    //otherwise, the pin has gone LOW 
    else
    { 
        //only worry about this if the timer has actually started
        if(ts_timer_start != 0)
        { 
            //record the pulse time
            ts_pulse_time = ((volatile int)micros() - ts_timer_start);
            //restart the timer
            ts_timer_start = 0;
        }
    } 
} 
#endif

void RcController::init() {
#ifdef THREE_STATE_CHANNEL
  attachInterrupt(digitalPinToInterrupt(THREE_STATE_CHANNEL), tsProcessSignal, CHANGE);
#endif
}
void RcController::run() {

}
unsigned long RcController::getControls() {
    return CTRL_NONE;
}
