/*************************************************** 
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <Arduino.h>

#include "config.h"
#include "RobotBrain.h"
#if MODE == MODE_JOYSTICK
#include "JoystickController.h"
#else
#include "DemoController.h"
#endif

/**
 * Constructeur
 */
RobotBrain::RobotBrain() {
  this->m_left = new RobotEye();
  this->m_right = new RobotEye();
#if MODE == MODE_JOYSTICK
  this->m_control = new JoystickController();
#else
  this->m_control = new DemoController();
#endif
}

/**
 * Initialisation
 */
void RobotBrain::init(uint8_t left_addr, uint8_t right_addr) {
  randomSeed(analogRead(0));  

  m_bstate = BSTATE_NONE;

  m_right->init(right_addr, EYE_RIGHT);
  m_left->init(left_addr, EYE_LEFT);

  m_control->init();

  m_timeline.reset();
  m_blinkTimeline.reset();
  m_feelingTimeline.reset();
}

/**
 * Inversions du sens des yeux
 */
void RobotBrain::reverseEyes() {
  m_right->reverse();
  m_left->reverse();
  RobotEye *tmp=m_right;
  m_right = m_left;
  m_left = tmp;
}

/**
 * Lecture des contrôles
 */
unsigned long RobotBrain::getControls() {
  m_controls = m_control->getControls();
  // Restaure les actions consommées qui ne sont plus actives
  m_actions |= ~m_controls & CTRL_MASK_ACTIONS;
  // Efface les actions consommées
  m_controls &= m_actions | ~CTRL_MASK_ACTIONS;
  return m_controls;
}

/**
 * Récupération d'une action
 */
unsigned long RobotBrain::getActions(unsigned long actions) {
  // Marque les actions consommées
  m_actions &= ~(m_controls & CTRL_MASK_ACTIONS & actions);
  // Extraction des actions
  return m_controls & actions & CTRL_MASK_ACTIONS;
}

/**
 * Démarrage du robot
 */
void RobotBrain::stateStart() {
  m_bstate = BSTATE_START;
  m_left->open();
  m_right->open();
}

/**
 * Arrêt du robot
 */
void RobotBrain::stateStop() {
  m_bstate = BSTATE_STOP;
  m_left->reset();
  m_right->reset();
}

/**
 * Réveil le robot
 */
void RobotBrain::stateWakeup() {
  m_bstate = BSTATE_START;
  m_left->open();
  m_right->open();
}

/**
 * Le robot se met en attente
 */
void RobotBrain::stateWait() {
  m_bstate = BSTATE_WAIT;
  m_left->normal();
  m_right->normal();
  m_timeline.reset();
}

/**
 * Endors le robot
 */
void RobotBrain::stateSleep() {
  m_bstate = BSTATE_SLEEP;
  m_left->close();
  m_right->close();
}

/**
 * Le robot est choqué
 */
void RobotBrain::stateShocked() {
  m_bstate = BSTATE_SHOCKED;
  m_right->isNormal();
  m_left->isNormal();
  m_right->rolling();
  m_left->rolling();
  m_timeline.reset();
}

/**
 * Le robot s'ennuie
 */
void RobotBrain::stateBored() {
  m_bstate = BSTATE_BORED;
  m_left->blink();
  m_right->blink();
}

/**
 * Le robot est mort
 */
void RobotBrain::stateDead() {
  m_bstate = BSTATE_DEAD;
  m_left->dead();
  m_right->dead();
}

/**
 * Le robot a gagné
 */
void RobotBrain::stateWin() {
  m_bstate = BSTATE_WIN;
  m_left->win();
  m_right->win();
}

/**
 * Exécution du cerveau
 */
void RobotBrain::run() {
  // Traitement du contrôleur
  m_control->run();

  // Lecture des états
  unsigned long controls = getControls();
  long eyeState = getEyeState(m_right);
  long eyeFeeling = getEyeFeeling(m_right);
  long eyeAction = getEyeAction(m_right);

  // Inversion ?
  if(getActions(CTRL_REVERSE)) {
    reverseEyes();
  }

  // Traitement de l'état
  String state = "NONE";
  switch (m_bstate)
  {
    // Etat non défini
    case BSTATE_NONE:
      state="NONE";
      if(controls & getActions(CTRL_ACTION1 | CTRL_START)) {
        stateStart();
      }
      break;
    // En cours de démarrage
    case BSTATE_START:
      state="START";
      if(eyeState != EYE_STATE_OPENING) {
        stateWait();
      }
      break;
    // En cours d'arrêt
    case BSTATE_STOP:
      state="STOP";
      if(eyeState == EYE_STATE_NONE) {
        m_bstate = BSTATE_NONE;
      }
      break;
    // En train de dormir
    case BSTATE_SLEEP:
      state="SLEEP";
      if(eyeState != EYE_STATE_CLOSING) {
        if(controls & getActions(CTRL_ACTION2 | CTRL_START)) {
          stateWakeup();
        } else if(controls & getActions(CTRL_ACTION1 | CTRL_STOP)) {
          stateStop();
        }
      }
      break;
    // En état d'attente
    case BSTATE_WAIT:
      state="WAIT";
      if(getActions(CTRL_SHOCK)) {
        stateShocked();
      } else if(getActions(CTRL_LOST | CTRL_ACTION2)) {
        stateDead();
      } else if(getActions(CTRL_WIN | CTRL_ACTION1)) {
        stateWin();
      } else if(getActions(CTRL_STOP)) {
        stateSleep();
      } else {
        bool changed = false;
        int lookAt = m_right->getLookAt();
        int newLookAt = EYE_LOOK_FORWARD;

        if(controls & CTRL_LEFT) {
          newLookAt = EYE_LOOK_LEFT;
        } else if(controls & CTRL_RIGHT) {
          newLookAt = EYE_LOOK_RIGHT;
        } else if((controls & 0xF0)==0) {
          newLookAt = EYE_LOOK_FORWARD;
        }
        if(lookAt != newLookAt) {
          changed = true;
          m_right->lookAt(newLookAt);
          m_left->lookAt(newLookAt);
        }

        if(controls & CTRL_FORWARD && eyeFeeling != EYE_FEELING_ANGRY) {
          changed = true;
          m_right->isAngry();
          m_left->isAngry();
        }else if(controls & CTRL_BACKWARD && eyeFeeling != EYE_FEELING_SCARED) {
          changed = true;
          m_right->isScared();
          m_left->isScared();
        }else if( (controls & 0xF)==0 && eyeFeeling != EYE_FEELING_NORMAL) {
          changed = true;
          m_right->isNormal();
          m_left->isNormal();
        }

        // Un changement a eu lieu ?
        if(changed) {
          m_timeline.reset();
        } else {
          if(m_timeline.isTimePasted(5000)) {
            stateBored();
          }
        }
      }
      break;
    // Etat de choc
    case BSTATE_SHOCKED:
      state="SHOCKED";
      if(eyeAction != EYE_ACTION_ROLLING || m_timeline.isTimePasted(1500)) {
        stateWait();
      }
      break;
    // Ennui
    case BSTATE_BORED:
      state = "BORED";
      if(eyeState != EYE_STATE_BLINKING) {
        stateWait();
      }
      break;
    // Le robot est mort
    case BSTATE_DEAD:
      state = "DEAD";
      if(getActions(CTRL_ACTION1 | CTRL_ACTION2)) {
        stateStart();
      }
      break;
    // Le robot a gagné
    case BSTATE_WIN:
      state = "WIN";
      if(getActions(CTRL_ACTION1 | CTRL_ACTION2)) {
        stateStart();
      }
      break;
  }
#if DEBUG
  Serial.print("Brain state: "); Serial.println(state);
#endif

  // Traitement des yeux
  m_right->run();
  m_left->run();

}
