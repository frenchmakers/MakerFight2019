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

#if MODE == MODE_JOYSTICK
  pinMode(JOYSTICK_SWITCH_PIN, INPUT);
  pinMode(JOYSTICK_SHOCK_PIN, INPUT);
  digitalWrite(JOYSTICK_SWITCH_PIN, HIGH);

  controllerState = 0;
 #endif
 
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

#if MODE == MODE_DEMO
/**
 * Exécution en mode démo
 */
void RobotBrain::runDemo() {
  // Traitement des yeux
  m_right->run();
  m_left->run();
  
  long eyeState = getEyeState(m_right);
  long eyeAction = getEyeAction(m_right);
  long eyeFeeling = getEyeFeeling(m_right);
  
  /**
   * Cligne des yeux toutes les 3 secondes 
   */
  if(eyeState != EYE_STATE_NONE && m_blinkTimeline.isTimePasted(3000)) {
    int r = random(20000);
    if(r % 3 != 1)
      m_right->blink();
    if(r % 3 != 0)
      m_left->blink();
  }
  
  /**
   * On change de sentiment toutes les 10 secondes
   */
  if(eyeState != EYE_STATE_NONE && m_feelingTimeline.isTimePasted(10000)) {
    if(eyeFeeling==EYE_FEELING_NORMAL) {
      m_right->isAngry();
      m_left->isAngry();
    } else if(eyeFeeling==EYE_FEELING_ANGRY) {
      m_right->isScared();
      m_left->isScared();
    } else if(eyeFeeling==EYE_FEELING_SCARED) {
      m_right->isNormal();
      m_left->isNormal();
      reverseEyes();
    }
  }
  
  // Si l'oeil n'est pas encore ouvert on provoque son ouverture
  if(eyeState == EYE_STATE_NONE) {
    m_right->open();
    m_left->open();
  } 
  // Si une action est en cours on attend 5 secondes
  else if(eyeAction != EYE_ACTION_NONE) {
    if(m_timeline.isTimePasted(5000)) {
      m_right->normal();
      m_left->normal();
    }
  }
  // Sinon on anime les yeux
  else {
    while(m_timeline.isTimePasted(1000)) 
    {
      uint8_t look = m_right->getLookAt();
      
      if(look == EYE_LOOK_FORWARD)            look = EYE_LOOK_RIGHT;
      else if(look == EYE_LOOK_RIGHT)         look = EYE_LOOK_LEFT; 
      else if(look == EYE_LOOK_LEFT)          look = EYE_LOOK_UP; 
      else if(look == EYE_LOOK_UP)            look = EYE_LOOK_DOWN; 
      else if(look == EYE_LOOK_DOWN)          look = EYE_LOOK_UP_RIGHT; 
      else if(look == EYE_LOOK_UP_RIGHT)      look = EYE_LOOK_DOWN_LEFT; 
      else if(look == EYE_LOOK_DOWN_LEFT)     look = EYE_LOOK_DOWN_RIGHT; 
      else if(look == EYE_LOOK_DOWN_RIGHT)    look = EYE_LOOK_UP_LEFT; 
      //else if(look == EYE_LOOK_UP_LEFT)       look = EYE_LOOK_FORWARD; 
      else {
        //look = EYE_LOOK_FORWARD; 
        m_right->rolling();
        m_left->rolling();
        break;
      }
  
      m_right->lookAt(look);
      m_left->lookAt(look);
    }
  }
}

#elif MODE == MODE_JOYSTICK

/**
 * Lecture du mouvement
 */
int RobotBrain::getMovement() {
  // Contrôle par joystick
  int x = analogRead(JOYSTICK_AXE_X_PIN);
  int y = analogRead(JOYSTICK_AXE_Y_PIN);
  int move = MOVE_NONE;
  if(y<200) {
    move |= MOVE_FORWARD;
  } else if(y>800) {
    move |= MOVE_BACKWARD;
  }
  if(x<200) {
    move |= MOVE_RIGHT;
  } else if(x>800) {
    move |= MOVE_LEFT;
  }
  if(digitalRead(JOYSTICK_SHOCK_PIN)==HIGH) {
    if(!(controllerState & SHOCK)) {
      move |= SHOCK;
    }
    controllerState |= SHOCK;
  } else {
    controllerState &= ~SHOCK;
  }
  if(digitalRead(JOYSTICK_SWITCH_PIN)==LOW) {
    if(!(controllerState & ACTION1)) {
      move |= ACTION1;
    }
    controllerState |= ACTION1;
  } else {
    controllerState &= ~ACTION1;
  }
  return move;
}

/**
 * Exécution du cerveau avec un contrôleur
 */
void RobotBrain::runController() {
  // Traitement des yeux
  m_right->run();
  m_left->run();
  
  long eyeState = getEyeState(m_right);
  long eyeFeeling = getEyeFeeling(m_right);
  long eyeAction = getEyeAction(m_right);
  
  // Si les yeux ne sont pas encore ouverts on provoque l'ouverture
  if(eyeState == EYE_STATE_NONE) {
    m_right->open();
    m_left->open();
    return;
  }

  // On récupère le mouvement
  int move = getMovement();
  if(eyeAction == EYE_ACTION_NONE) {
    if(move & MOVE_LEFT) {
        m_right->lookAt(EYE_LOOK_LEFT);
        m_left->lookAt(EYE_LOOK_LEFT);
    } else if(move & MOVE_RIGHT) {
        m_right->lookAt(EYE_LOOK_RIGHT);
        m_left->lookAt(EYE_LOOK_RIGHT);
    } else if((move & 0xF0)==0) {
        m_right->lookAt(EYE_LOOK_FORWARD);
        m_left->lookAt(EYE_LOOK_FORWARD);
    }
    if(move & MOVE_FORWARD && eyeFeeling != EYE_FEELING_ANGRY) {
        m_right->isAngry();
        m_left->isAngry();
    }else if(move & MOVE_BACKWARD && eyeFeeling != EYE_FEELING_SCARED) {
        m_right->isScared();
        m_left->isScared();
    }else if( (move & 0xF)==0 && eyeFeeling != EYE_FEELING_NORMAL) {
        m_right->isNormal();
        m_left->isNormal();
    }
    // On a un choc ?
    if(move & SHOCK) {
      m_right->isNormal();
      m_left->isNormal();
      m_right->rolling();
      m_left->rolling();
      m_timeline.reset();
    }
    // On a une action
    if(move & ACTION1) {
      if(eyeState != EYE_STATE_DEAD) {
        m_right->dead();
        m_left->dead();
      } else {
        m_right->open();
        m_left->open();
      }
    }
  } else if(eyeAction == EYE_ACTION_ROLLING) {
    if(m_timeline.isTimePasted(1500)) {
      m_right->normal();
      m_left->normal();
    }
  }
  
}
#endif

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
  m_left->dead();
  m_right->dead();
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
  long eyeAction = getEyeAction(m_right);Serial.println(eyeAction);

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
      } else if(getActions(CTRL_LOST | CTRL_ACTION1)) {
        stateDead();
      } else if(getActions(CTRL_WIN | CTRL_ACTION2)) {
        stateWin();
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

#if MODE == MODE_JOYSTICK
  //runController();
#elif MODE == MODE_DEMO
  //runDemo();
#endif
}
