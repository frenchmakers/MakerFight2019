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
        virtual long getControls() { 
            return CTRL_NONE;
        }
};

#endif // JoystickController_h
