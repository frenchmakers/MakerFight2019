/***************************************************
 * Utilitaires
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#include <Arduino.h>

#include "Utils.h"

/**
 * Constructeur
 */
Timeline::Timeline() {
  reset();
}

/**
 * Réinitialisation des compteurs
 */
void Timeline::reset() {
  m_time = millis();
  m_elapsed = 0;
}

/**
 * Retour le temps passé actuel
 */
unsigned long Timeline::getElapsed() {
  unsigned long ms = millis();
  m_elapsed += ms - m_time;
  m_time = ms;
  return m_elapsed;
}

/**
 * Indique si temps espéré est passé
 * 
 * Le temps passé est recalculé
 */
bool Timeline::isTimePasted(unsigned long expected) {
  unsigned long et = getElapsed();
  if( et >= expected ) {
    et -= expected;
    m_elapsed = et;
    return true;
  }
  return false;
}
    
/**
 * Compte le nombre de fois que le temps espéré est passé
 * 
 * Le temps passé est recalculé
 */
unsigned long Timeline::timePasted(unsigned long expected) {
  unsigned long et = getElapsed();
  unsigned long count = 0;
  while( et >= expected ) {
    et -= expected;
    count++;
  }
  m_elapsed = et;
  return count;  
}
