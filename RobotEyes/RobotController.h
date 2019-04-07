/***************************************************
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */
 
#ifndef RobotController_h
#define RobotController_h

#define CTRL_NONE       0x00000000
#define CTRL_FORWARD    0x00000001
#define CTRL_BACKWARD   0x00000002
#define CTRL_LEFT       0x00000010
#define CTRL_RIGHT      0x00000020
#define CTRL_REVERSE    0x00000100
#define CTRL_SHOCK      0x00010000
#define CTRL_WIN        0x00020000
#define CTRL_LOST       0x00030000

/**
 * Contrôleur de robot
 */
class RobotController {
    public:
        // Initialisation du contrôleur
        virtual void init() = 0;
        // Exécution du contrôleur
        virtual void run() = 0;
        // Récupération des informations de contrôle
        virtual long getControls() = 0;
};

#endif // RobotController_h
