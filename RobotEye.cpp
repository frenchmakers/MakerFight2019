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

/**
 * Constructeur
 */
RobotEye::RobotEye() {
  i2c_addr = 0x00;
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

  clear();
  m_state = EYE_STATE_NONE;
  m_stateStep = 0;
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

  uint16_t *pf = *f;
  if(eye_type == EYE_LEFT) {
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
 * Dessine l'oeil
 */
void RobotEye::drawEye() {
  for(uint8_t i = 0; i<8; i++) {
    displayBuffer[i] = eye_back[i];
  }
  displayBuffer[eye_pupil_line] |= eye_pupil_pixels;
  displayFrame(&displayBuffer);
}

/**
 * Exécution de l'oeil
 */
void RobotEye::run() {
}
