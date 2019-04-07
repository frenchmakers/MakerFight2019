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
 * Exécution du cerveau
 */
void RobotBrain::run() {
  m_control->run();
#if MODE == MODE_JOYSTICK
  runController();
#elif MODE == MODE_DEMO
  runDemo();
#endif
}
