/***************************************************
 * Librairie de gestion d'un oeil du robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#include <Arduino.h>

#include <Wire.h>
#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BLINK_OFF 0
#define HT16K33_BLINK_2HZ  1
#define HT16K33_BLINK_1HZ  2
#define HT16K33_BLINK_HALFHZ  3
#define HT16K33_CMD_BRIGHTNESS 0xE0

#include "RobotEye.h"
#include "eye_data.h"

static uint8_t rollingPositions[] = { EYE_LOOK_UP, EYE_LOOK_UP_RIGHT, EYE_LOOK_RIGHT, EYE_LOOK_DOWN_RIGHT, EYE_LOOK_DOWN, EYE_LOOK_DOWN_LEFT, EYE_LOOK_LEFT, EYE_LOOK_UP_LEFT };
#define rollingPositionsCount 8

/**
 * Constructeur
 */
RobotEye::RobotEye() {
  i2c_addr = 0x00;
  m_eyeLip = NULL;
  m_eyeLip_movement = NULL;
  m_eye_animation = NULL;
}

/**
 * Démarrage et initialisation de l'oeil
 */
void RobotEye::init(uint8_t addr, uint8_t type) {
  i2c_addr = addr;
  eye_type = type;
  
  Wire.begin();
  Wire.beginTransmission(i2c_addr);
  Wire.write(0x21);  // turn on oscillator
  Wire.endTransmission();
  
  blinkRate(HT16K33_BLINK_OFF);
  setBrightness(15); // max brightness

  reset();

  m_actionTimeline.reset();
  m_stateTimeline.reset();

  m_reversed = false;
  
  isNormal();
}

/**
 * Définition de l'intensité de l'écran
 */
void RobotEye::setBrightness(uint8_t b) {
  if (b > 15) b = 15;
  Wire.beginTransmission(i2c_addr);
  Wire.write(HT16K33_CMD_BRIGHTNESS | b);
  Wire.endTransmission();  
}

/**
 * Définition de la fréquance de clignotement
 */
void RobotEye::blinkRate(uint8_t b) {
  Wire.beginTransmission(i2c_addr);
  if (b > 3) b = 0; // turn off if not sure
  
  Wire.write(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (b << 1)); 
  Wire.endTransmission();
}

/**
 * Efface la matrice
 */
void RobotEye::clear() {
  Wire.beginTransmission(i2c_addr);
  Wire.write((uint8_t)0x00); // start at address $00

  for (uint8_t i=0; i<8; i++) {
    Wire.write(0);
    Wire.write(0);    
  }

  Wire.endTransmission();    
}

/**
 * Affiche une frame sur la matrice
 */
void RobotEye::displayFrame(frame *f) {
  if(f==NULL) return;
  
  Wire.beginTransmission(i2c_addr);
  Wire.write((uint8_t)0x00); // start at address $00

  bool isReversed = (eye_type == EYE_LEFT && !m_reversed) || (eye_type == EYE_RIGHT && m_reversed);
  uint16_t *pf = *f;
  if(isReversed) {
    pf += 8;
    for (uint8_t i=0; i<8; i++) {
      uint16_t l = (*--pf);
      Wire.write(l & 0xFF);    
      Wire.write(l >> 8);    
    }
  } else {
    for (uint8_t i=0; i<8; i++) {
      uint16_t l = (*pf++);
      Wire.write(l & 0xFF);    
      Wire.write(l >> 8);    
    }
  }
  
  Wire.endTransmission();    
}

/**
 * Retourne la frame de l'oeil ouvert
 */
masked_frame*RobotEye::getOpenedEye() {
  switch(getEyeFeeling(this)) {
    case EYE_FEELING_ANGRY: return &eyelip_angry;
    case EYE_FEELING_SCARED: return &eyelip_scared;
    case EYE_FEELING_NORMAL:
    default:
      return &eyelip;
  }
}

/**
 * Retourne la frame de l'oeil fermé
 */
masked_frame*RobotEye::getClosedEye() {
  return &eyelip_closed;
}

/**
 * Dessine l'oeil
 */
void RobotEye::drawEye() {
  // Cas des yeux non animé
  long state = getEyeState(this);
  if(state==EYE_STATE_DEAD) {
    displayFrame(&eye_dead);
    return;
  }
  // Une frame d'animation ?
  if(m_eye_frame != NULL) {
    displayFrame(m_eye_frame);
    return;
  }

  bool isReversed = (eye_type == EYE_LEFT && !m_reversed) || (eye_type == EYE_RIGHT && m_reversed);
  
  // Dessin du fond
  for(uint8_t i = 0; i<8; i++) {
    displayBuffer[i] = eye_back[i];
  }
  
  // Dessin de la pupille
  uint16_t pixels = eye_pupil_pixels;
  uint8_t line = eye_pupil_line;
  if( m_lookAt & EYE_LOOK_RIGHT ) {
    pixels = !isReversed ? pixels >> 1 : pixels << 1;
  } else if( m_lookAt & EYE_LOOK_LEFT ) {
    pixels = !isReversed ? pixels << 1 : pixels >> 1;
  } 
  if( m_lookAt & EYE_LOOK_UP ) {
    line = line - 1;
  } else if( m_lookAt & EYE_LOOK_DOWN ) {
    line = line + 1;
  } 
  displayBuffer[line] |= pixels;
  
  // Dessin de la paupière
  if( m_eyeLip != NULL ) 
  {
    for(uint8_t i = 0; i<8; i++) {
      displayBuffer[i] &= ~m_eyeLip->mask[i];
      displayBuffer[i] |= m_eyeLip->pixels[i];
    }
  }
  
  // Affichage du buffer
  displayFrame(&displayBuffer);
}

/**
 * Demande à l'oeil de regarder dans une direction
 */
void RobotEye::lookAt(uint8_t direction) {
  m_lookAt = direction;
  m_refreshEye = true;
}

/**
 * Retourne la direction dans laquelle regarder
 */
uint8_t RobotEye::getLookAt() {
  return m_lookAt;
}

/**
 * Inversion de l'oeil
 */
void RobotEye::reverse() {
  m_reversed = !m_reversed;
  m_refreshEye = true;
}

/**
 * Défini l'oeil dans une nouvelle action
 */
void RobotEye::setAction(long action) {
  m_state = (m_state & ~EYE_ACTION_MASK) | (action & EYE_ACTION_MASK);
  m_actionTimeline.reset();
}

/**
 * Défini l'oeil dans un nouvel état
 */
void RobotEye::setState(long state) {
  m_state = (m_state & ~EYE_STATE_MASK) | (state & EYE_STATE_MASK);
  m_stateTimeline.reset();
}

/**
 * Défini le sentiment de l'oeil
 */
void RobotEye::setFeeling(long feeling) {
  m_state = (m_state & ~EYE_FEELING_MASK) | (feeling & EYE_FEELING_MASK);
}

/**
 * Place l'oeil en mode normal
 */
void RobotEye::normal() {
  setAction(EYE_ACTION_NONE);
  lookAt(EYE_LOOK_FORWARD);
  m_eye_frame = NULL;
}

/**
 * Place l'oeil en mode 'rolling'
 */
void RobotEye::rolling() {
  setAction(EYE_ACTION_ROLLING);
  m_actionStep = random(rollingPositionsCount);
}

/**
 * Provoque l'ouverture de l'oeil
 */
void RobotEye::open() {
  setState(EYE_STATE_OPENING);
  m_stateStep = -1;
  m_eyeLip = getClosedEye();
  m_refreshEye = true; 
}

/**
 * Provoque la fermeture de l'oeil
 */
void RobotEye::close() {
  setState(EYE_STATE_CLOSING);
  m_stateStep = -1;
  m_eyeLip = getOpenedEye();
  m_refreshEye = true; 
}

/**
 * Cligne
 */
void RobotEye::blink() {
  setState(EYE_STATE_BLINKING);
  m_stateStep = -1;
  m_eyeLip = getOpenedEye();
  m_refreshEye = true; 
}

/**
 * L'oeil est mort
 */
void RobotEye::dead() {
  setState(EYE_STATE_DEAD);
  m_stateStep = -1;
  m_eyeLip = NULL;
  m_refreshEye = true; 
}

/**
 * L'oeil est en mode vainqueur
 */
void RobotEye::win() {
  setState(EYE_STATE_WIN);
  m_stateStep = -1;
  m_eyeLip = NULL;
  m_eye_animation = &eye_state_win_animation;
  m_refreshEye = true; 
}

/**
 * Réinitialisation de l'oeil
 */
void RobotEye::reset() {
  clear();
  m_refreshEye = false;
  m_lookAt = EYE_LOOK_FORWARD;
  m_state = EYE_STATE_NONE;
  m_stateStep = 0;
  m_eye_animation = NULL;
  m_eyeLip_movement = NULL;
  m_eyeLip = NULL;
}

/**
 * Ce sent normal
 */
void RobotEye::isNormal() {
  setFeeling(EYE_FEELING_NORMAL);
  m_eyeLip = getOpenedEye();
  m_refreshEye = true; 
  m_eyeLip_movement = &eyelip_movement_normal;
}

/**
 * Se sent en colère
 */
void RobotEye::isAngry() {
  setFeeling(EYE_FEELING_ANGRY);
  m_eyeLip = getOpenedEye();
  m_refreshEye = true; 
  m_eyeLip_movement = &eyelip_movement_angry;
}

/**
 * Se sent apeuré
 */
void RobotEye::isScared() {
  setFeeling(EYE_FEELING_SCARED);
  m_eyeLip = getOpenedEye();
  m_refreshEye = true; 
  m_eyeLip_movement = &eyelip_movement_scared;
}

/**
 * Retourne l'état de l'oeil
 */
long RobotEye::getState() {
  return m_state;
}

/**
 * Traitement de l'état opening
 */
void RobotEye::processStateOpening() {
  if(m_eyeLip_movement==NULL) {
      setState(EYE_STATE_OPENED); 
      m_eyeLip = getOpenedEye();
      return;
  }
  if(m_stateTimeline.isTimePasted(m_eyeLip_movement->speed)) {
    m_stateStep++;
    if(m_stateStep >= m_eyeLip_movement->count) {
      setState(EYE_STATE_OPENED); 
      m_eyeLip = getOpenedEye();
    } else {
      m_eyeLip = &m_eyeLip_movement->frames[(m_eyeLip_movement->count-1) - m_stateStep];
    }
    m_refreshEye = true; 
  }
}

/**
 * Traitement de l'état closing
 */
void RobotEye::processStateClosing() {
  if(m_eyeLip_movement==NULL) {
      setState(EYE_STATE_CLOSED); 
      m_eyeLip = getClosedEye();
      return;
  }
  if(m_stateTimeline.isTimePasted(m_eyeLip_movement->speed)) {
    m_stateStep++;
    if(m_stateStep >= m_eyeLip_movement->count) {
      setState(EYE_STATE_CLOSED); 
      m_eyeLip = getClosedEye();
    } else {
      m_eyeLip = &m_eyeLip_movement->frames[m_stateStep];
    }
    m_refreshEye = true; 
  }
}

/**
 * Traitement de l'état clignotement
 */
void RobotEye::processStateBlinking() {
  if(m_eyeLip_movement==NULL) {
      setState(EYE_STATE_OPENED); 
      m_eyeLip = getOpenedEye();
      return;
  }
  if(m_stateTimeline.isTimePasted(m_eyeLip_movement->speed)) {
    m_stateStep++;
    if(m_stateStep < m_eyeLip_movement->count) {
      m_eyeLip = &m_eyeLip_movement->frames[m_stateStep];
    } else if(m_stateStep >= m_eyeLip_movement->count && m_stateStep < 2*m_eyeLip_movement->count) {
      m_eyeLip = &m_eyeLip_movement->frames[(m_eyeLip_movement->count-1) - (m_stateStep-m_eyeLip_movement->count)];
    } else {
      setState(EYE_STATE_OPENED); 
      m_eyeLip = getOpenedEye();
    }
    m_refreshEye = true; 
  }
}

/**
 * Traitement de l'état 'winner'
 */
void RobotEye::processStateWin() {
  if(m_eye_animation==NULL) {
      setState(EYE_STATE_OPENED); 
      m_eyeLip = getOpenedEye();
      return;
  }

  if(m_stateTimeline.isTimePasted(m_eye_animation->speed)) {
    m_stateStep++;
    if(m_stateStep < m_eye_animation->count) {
      m_eye_frame = &m_eye_animation->frames[m_stateStep];
    } else {
      m_stateStep = -1;
    }
    m_refreshEye = true; 
  }
}

/**
 * Traitement de l'action rolling
 */
void RobotEye::processActionRolling() {
  while(m_actionTimeline.isTimePasted(30)) {
    // Prochaine position
    m_actionStep += eye_type == EYE_LEFT ? 1 : -1 ;
    
    if(m_actionStep<0) 
      m_actionStep = rollingPositionsCount-1;
    else if(m_actionStep>=rollingPositionsCount) 
      m_actionStep = 0;
      
    lookAt((uint8_t)rollingPositions[m_actionStep]);
  }
}

/**
 * Exécution de l'oeil
 */
void RobotEye::run() {
  // Si l'état est à NONE alors on ne fait rien
  if(m_state == EYE_STATE_NONE) return;

  // Traitement des états
  switch(getEyeState(this)) {
    case EYE_STATE_OPENING:   processStateOpening(); break;
    case EYE_STATE_CLOSING:   processStateClosing(); break;
    case EYE_STATE_BLINKING:  processStateBlinking(); break;
    case EYE_STATE_WIN:       processStateWin(); break;
  }
  // Traitement des actions
  switch(getEyeAction(this)) {
    case EYE_ACTION_ROLLING: processActionRolling(); break;
  }
  // Traitement des sentiments

  // L'oeil est à redessiner ?
  if(m_refreshEye) {
    drawEye();
    m_refreshEye = false;
  }

}
