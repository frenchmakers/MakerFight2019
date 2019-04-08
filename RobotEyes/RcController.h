/***************************************************
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#ifndef RcController_h
#define RcController_h

#include "RobotController.h"
#include "Utils.h"

/**
 * Contrôleur RC
 */
class RcController : public RobotController {
    private:
    public:
        virtual void init();
        virtual void run();
        virtual unsigned long getControls();
};

#endif // RcController_h
