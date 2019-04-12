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
  Serial.begin(115200);
  #endif
  
  brain.init(LEFT_EYE_ADDR, RIGHT_EYE_ADDR);  
}

void loop() {
  #if DEBUG
  unsigned long t = millis();
  #endif
  brain.run();
  #if DEBUG
  t = millis() - t;
  Serial.print("Brain run: ");
  Serial.print(t);
  Serial.println(" ms");
  #endif
}
