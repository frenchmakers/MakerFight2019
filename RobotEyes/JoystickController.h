/***************************************************
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#ifndef JoystickController_h
#define JoystickController_h

#include "RobotController.h"

/**
 * Contrôleur basé sur un joystick
 */
class JoystickController : public RobotController {
    private:
        int controllerState;
    public:
        virtual void init() {
            pinMode(JOYSTICK_SWITCH_PIN, INPUT);
            pinMode(JOYSTICK_SHOCK_PIN, INPUT);
            digitalWrite(JOYSTICK_SWITCH_PIN, HIGH);
            controllerState = 0;
        }
        virtual void run() {

        }
        virtual unsigned long getControls() { 
            int x = analogRead(JOYSTICK_AXE_X_PIN);
            int y = analogRead(JOYSTICK_AXE_Y_PIN);
            unsigned long control = CTRL_NONE;
            if(y<200) {
                control |= CTRL_FORWARD;
            } else if(y>800) {
                control |= CTRL_BACKWARD;
            }
            if(x<200) {
                control |= CTRL_RIGHT;
            } else if(x>800) {
                control |= CTRL_LEFT;
            }
            Serial.println(digitalRead(JOYSTICK_SHOCK_PIN));
            if(digitalRead(JOYSTICK_SHOCK_PIN)==HIGH) {
                if(!(controllerState & CTRL_SHOCK)) {
                    control |= CTRL_SHOCK;
                }
                controllerState |= CTRL_SHOCK;
            } else {
                controllerState &= ~CTRL_SHOCK;
            }
            if(digitalRead(JOYSTICK_SWITCH_PIN)==LOW) {
                if(!(controllerState & CTRL_ACTION1)) {
                    control |= CTRL_ACTION1;
                }
                controllerState |= CTRL_ACTION1;
            } else {
                controllerState &= ~CTRL_ACTION1;
            }
            return control;
        }
};

#endif // JoystickController_h
