/***************************************************
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#include "RcController.h"

#if MODE == MODE_RC

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

#ifdef MOVE_1_CHANNEL
volatile unsigned long mv1_timer_start;
volatile int mv1_last_interrupt_time;
volatile int mv1_pulse_time;
void mv1ProcessSignal() 
{
    //record the interrupt time so that we can tell if the receiver has a signal from the transmitter 
    mv1_last_interrupt_time = micros(); 
    //if the pin has gone HIGH, record the microseconds since the Arduino started up 
    if(digitalRead(MOVE_1_CHANNEL) == HIGH) 
    { 
        mv1_timer_start = micros();
    } 
    //otherwise, the pin has gone LOW 
    else
    { 
        //only worry about this if the timer has actually started
        if(mv1_timer_start != 0)
        { 
            //record the pulse time
            mv1_pulse_time = ((volatile int)micros() - mv1_timer_start);
            //restart the timer
            mv1_timer_start = 0;
        }
    } 
} 
#endif

#ifdef MOVE_2_CHANNEL
volatile unsigned long mv2_timer_start;
volatile int mv2_last_interrupt_time;
volatile int mv2_pulse_time;
void mv2ProcessSignal() 
{
    //record the interrupt time so that we can tell if the receiver has a signal from the transmitter 
    mv2_last_interrupt_time = micros(); 
    //if the pin has gone HIGH, record the microseconds since the Arduino started up 
    if(digitalRead(MOVE_2_CHANNEL) == HIGH) 
    { 
        mv2_timer_start = micros();
    } 
    //otherwise, the pin has gone LOW 
    else
    { 
        //only worry about this if the timer has actually started
        if(mv2_timer_start != 0)
        { 
            //record the pulse time
            mv2_pulse_time = ((volatile int)micros() - mv2_timer_start);
            //restart the timer
            mv2_timer_start = 0;
        }
    } 
} 
#endif

void RcController::init() {
#ifdef THREE_STATE_CHANNEL
  attachInterrupt(digitalPinToInterrupt(THREE_STATE_CHANNEL), tsProcessSignal, CHANGE);
#endif
#ifdef MOVE_1_CHANNEL
  attachInterrupt(digitalPinToInterrupt(MOVE_1_CHANNEL), mv1ProcessSignal, CHANGE);
#endif
#ifdef MOVE_2_CHANNEL
  attachInterrupt(digitalPinToInterrupt(MOVE_2_CHANNEL), mv2ProcessSignal, CHANGE);
#endif
}
void RcController::run() {

}
unsigned long RcController::getControls() {
    unsigned long controls = CTRL_NONE;
    int pulse;

#ifdef THREE_STATE_CHANNEL
    pulse = ts_pulse_time;
    if(pulse < THREE_STATE_CHANNEL_LIMIT_WIN) {
        controls |= CTRL_WIN;
    }else if(pulse > THREE_STATE_CHANNEL_LIMIT_LOST) {
        controls |= CTRL_LOST;
    }
#endif

#if MOVE_MODE == MOVE_MODE_WHEELS
    int lMove = 0, rMove = 0;
    // Move1: Roue gauche
    #ifdef MOVE_1_CHANNEL
    lMove = mv1_pulse_time - MOVE_1_CENTER;
    #endif
    // Move2: Roue droite
    #ifdef MOVE_2_CHANNEL
    rMove = mv2_pulse_time - MOVE_2_CENTER;
    #endif

    // Si les deux roues avancent, alors on avance
    if(lMove > MOVE_1_LIMITS && rMove > MOVE_2_LIMITS) {
        controls |= CTRL_FORWARD;
    }
    // Si les deux roues reculent, alors on recule
    else if(lMove < MOVE_1_LIMITS && rMove < MOVE_2_LIMITS) {
        controls |= CTRL_BACKWARD;
    }

    // Si la roue gauche avance plus vite que la roue droite
    // ou que la roue droite recule plus vite que la roue gauche
    // => alors on tourne a droite
    if( (lMove - rMove > MOVE_1_LIMITS) || (rMove - lMove < MOVE_2_LIMITS) ) {
        controls |= CTRL_RIGHT;
    }
    // Si la roue droite avance plus vite que la roue gauche
    // ou que la roue gauche recule plus vite que la roue droite
    // => alors on tourne a gauche
    else if( (rMove - lMove > MOVE_2_LIMITS) || (lMove - rMove < MOVE_1_LIMITS) ) {
        controls |= CTRL_LEFT;
    }

#elif MOVE_MODE == MOVE_MODE_DIRECTION
    // Move1: Avant/Arrière
    #ifdef MOVE_1_CHANNEL
    pulse = mv1_pulse_time;
    if( mv1_pulse_time < MOVE_1_CENTER - MOVE_1_LIMITS ) {
        controls |= CTRL_FORWARD;
    } else if( mv1_pulse_time > MOVE_1_CENTER + MOVE_1_LIMITS ) {
        controls |= CTRL_BACKWARD;
    }
    #endif
    // Move2: Gauche/Droite
    #ifdef MOVE_2_CHANNEL
    pulse = mv2_pulse_time;
    if( mv2_pulse_time < MOVE_2_CENTER - MOVE_2_LIMITS ) {
        controls |= CTRL_LEFT;
    } else if( mv2_pulse_time > MOVE_2_CENTER + MOVE_2_LIMITS ) {
        controls |= CTRL_RIGHT;
    }
    #endif
#endif

    return CTRL_NONE;
}

#endif