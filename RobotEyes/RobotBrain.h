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

#define BSTATE_NONE     0x0000
#define BSTATE_START    0x0001
#define BSTATE_STOP     0x0002
#define BSTATE_WAIT     0x0003
#define BSTATE_SLEEP    0x0004
#define BSTATE_SHOCKED  0x0005
#define BSTATE_BORED    0x0006
#define BSTATE_DEAD     0x0007
#define BSTATE_WIN      0x0008

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
    int m_bstate;
    unsigned long m_controls, m_actions;

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

    void stateNone();
    void stateStart();
    void stateStop();
    void stateWakeup();
    void stateSleep();
    void stateWait();
    void stateShocked();
    void stateBored();
    void stateDead();
    void stateWin();

    unsigned long getControls();
    unsigned long getActions(unsigned long actions);

  public:
    RobotBrain();

    void init(uint8_t left_addr, uint8_t right_addr);

    void run();    
};

#endif // RobotBrain_h
