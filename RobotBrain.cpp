/*************************************************** 
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <Arduino.h>

#include "config.h"
#include "RobotBrain.h"

/**
 * Constructeur
 */
RobotBrain::RobotBrain() {
  this->m_left = new RobotEye();
  this->m_right = new RobotEye();
}

/**
 * Initialisation
 */
void RobotBrain::init(uint8_t left_addr, uint8_t right_addr) {
  randomSeed(analogRead(0));  
  
  m_right->init(right_addr, EYE_RIGHT);
  m_left->init(left_addr, EYE_LEFT);

  m_timeline.reset();
}

/**
 * Exécution du cerveau
 */
void RobotBrain::run() {
  // Traitement des yeux
  m_right->run();
  m_left->run();
  
  int eyeState = getEyeState(m_right);
  int eyeAction = getEyeAction(m_right);
  
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
        m_right->blink();
        m_left->blink();
        break;
      }
  
      m_right->lookAt(look);
      m_left->lookAt(look);
    }
  }
  
}
