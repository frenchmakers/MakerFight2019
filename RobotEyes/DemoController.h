/***************************************************
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#ifndef DemoController_h
#define DemoController_h

#include "RobotController.h"

/**
 * Contrôleur de démonstration
 */
class DemoController : public RobotController {
    public:
        virtual void init() {

        }
        virtual void run() {

        }
        virtual unsigned long getControls() { 
            return CTRL_NONE;
        }
};

#endif // DemoController_h
