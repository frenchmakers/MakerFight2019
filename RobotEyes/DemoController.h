/***************************************************
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#ifndef DemoController_h
#define DemoController_h

#include "RobotController.h"
#include "Utils.h"

/**
 * Contrôleur de démonstration
 */
class DemoController : public RobotController {
    private:
        Timeline m_timeline;
        unsigned long m_controls, m_state_reverse, m_state_end;
        int m_state;
        int m_state_delay, m_reverse_delay;
    public:
        virtual void init() {
            m_timeline.reset();
            m_state = 0;
            m_state_reverse = 0;
            m_state_end = CTRL_LOST;
            m_state_delay = 1500;
            m_reverse_delay = 8000;
        }
        virtual void run() {
            // Si le délai n'est pas dépassé on quitte
            if(!m_timeline.isTimePasted(m_state_delay)) {
                return;
            }
            m_reverse_delay -= m_state_delay;
            m_state_delay = 1500;
            // On traite l'état
            switch(m_state++) {
                // On démarre le robot
                case 0:
                    m_controls = CTRL_START;
                    m_state = 100;
                    break;
                // On arrête le robot
                case 1:
                    m_controls = CTRL_STOP;
                    m_state = 2;
                    m_state_delay = 1000;
                    break;
                case 2:
                    m_controls = CTRL_NONE;
                    m_state_delay = 1000;
                    m_state = 3;
                    break;
                case 3:
                    m_controls = CTRL_STOP;
                    m_state_delay = 2000;
                    m_state = 0;    // On redémarre le robot
                    break;

                // Regard à droite
                case 100:
                    m_controls = CTRL_RIGHT;
                    m_state_delay = 500;
                    break;
                // Regard centre
                case 101:
                    m_controls = CTRL_NONE;
                    m_state_delay = 500;
                    break;
                // Regard à gauche
                case 102:
                    m_controls = CTRL_LEFT;
                    m_state_delay = 500;
                    break;
                // Regard centre
                case 103:
                    m_controls = CTRL_NONE;
                    m_state_delay = 8000;
                    break;

                // Avance
                case 104:
                    m_controls = CTRL_FORWARD;
                    m_state_delay = 1000;
                    break;
                case 105:
                    m_controls = CTRL_FORWARD | CTRL_LEFT;
                    m_state_delay = 500;
                    break;
                case 106:
                    m_controls = CTRL_FORWARD | CTRL_RIGHT;
                    m_state_delay = 500;
                    break;
                case 107:
                    m_controls = CTRL_FORWARD;
                    m_state_delay = 500;
                    break;

                // Prend un choc
                case 108:
                    m_controls = CTRL_FORWARD | CTRL_SHOCK;
                    m_state_delay = 2000;
                    break;

                // Recule
                case 109:
                    m_controls = CTRL_BACKWARD;
                    m_state_delay = 1000;
                    break;
                case 110:
                    m_controls = CTRL_BACKWARD | CTRL_LEFT;
                    m_state_delay = 500;
                    break;
                case 111:
                    m_controls = CTRL_BACKWARD | CTRL_RIGHT;
                    m_state_delay = 500;
                    break;
                case 112:
                    m_controls = CTRL_BACKWARD;
                    m_state_delay = 1000;
                    break;

                // Mort/Victoire
                case 113:
                    m_controls = CTRL_BACKWARD | m_state_end;
                    m_state_end = m_state_end == CTRL_LOST ? CTRL_WIN : CTRL_LOST;
                    m_state_delay = 2500;
                    break;
                case 114:
                    m_controls = CTRL_BACKWARD | CTRL_ACTION1;
                    m_state_delay = 1500;
                    break;

                // On arrête le robot
                default:
                    m_state = 1;
                    m_state_delay = 1500;
                    break;
            }
            if(m_reverse_delay<=0) {
                //m_state_reverse ^= CTRL_REVERSE;
                //m_controls |= m_state_reverse;
                m_reverse_delay += 8000;
            }
            // On reset la Timeline
            m_timeline.reset();
        }
        virtual unsigned long getControls() { 
            return m_controls;
        }
};

#endif // DemoController_h
