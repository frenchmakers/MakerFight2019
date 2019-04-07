/***************************************************
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */
 
#ifndef RobotBrain_h
#define RobotBrain_h

#include "RobotController.h"
#include "RobotEye.h"

#define MOVE_NONE     0x0000
#define MOVE_FORWARD  0x0001
#define MOVE_BACKWARD 0x0002
#define MOVE_LEFT     0x0010
#define MOVE_RIGHT    0x0020
#define SHOCK         0x0100
#define ACTION1       0x0200

/**
 * Cerveau du robot
 */
class RobotBrain {
  private:
    RobotEye *m_left;
    RobotEye *m_right;
    RobotController *m_control;
    Timeline m_timeline;
    Timeline m_blinkTimeline;
    Timeline m_feelingTimeline;
#if MODE == MODE_JOYSTICK
    int controllerState;
#endif
  protected:
    void reverseEyes();
#if MODE == MODE_DEMO
    void runDemo();
#elif MODE == MODE_JOYSTICK
    int getMovement();
    void runController();
#endif
  public:
    RobotBrain();

    void init(uint8_t left_addr, uint8_t right_addr);

    void run();    
};

#endif // RobotBrain_h
