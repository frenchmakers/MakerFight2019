/***************************************************
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */
 
#ifndef RobotBrain_h
#define RobotBrain_h

#include "RobotEye.h"

/**
 * Cerveau du robot
 */
class RobotBrain {
  private:
    RobotEye *m_left;
    RobotEye *m_right;
    Timeline m_timeline;
    Timeline m_blinkTimeline;
    Timeline m_feelingTimeline;
  protected:
  public:
    RobotBrain();

    void init(uint8_t left_addr, uint8_t right_addr);

    void run();    
};

#endif // RobotBrain_h
