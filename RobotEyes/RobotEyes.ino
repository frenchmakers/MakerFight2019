/*************************************************** 
 *  Programme d'animation des yeux du robot Maker Fight
 *  
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "config.h"
#include "RobotBrain.h"

RobotBrain brain = RobotBrain();

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  
  brain.init(LEFT_EYE_ADDR, RIGHT_EYE_ADDR);  
}

void loop() {
  //unsigned long t = millis();
  brain.run();
  //t = millis() - t;
}
