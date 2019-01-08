/***************************************************
 * Librairie de gestion d'un oeil du robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#ifndef RobotEye_h
#define RobotEye_h

#include "Utils.h"

// Types d'oeil
#define EYE_LEFT 0
#define EYE_RIGHT 1

// Directions
#define EYE_LOOK_FORWARD      0
#define EYE_LOOK_RIGHT        1
#define EYE_LOOK_LEFT         2
#define EYE_LOOK_UP           4
#define EYE_LOOK_DOWN         8
#define EYE_LOOK_UP_LEFT      (EYE_LOOK_UP | EYE_LOOK_LEFT)
#define EYE_LOOK_UP_RIGHT     (EYE_LOOK_UP | EYE_LOOK_RIGHT)
#define EYE_LOOK_DOWN_LEFT    (EYE_LOOK_DOWN | EYE_LOOK_LEFT)
#define EYE_LOOK_DOWN_RIGHT   (EYE_LOOK_DOWN | EYE_LOOK_RIGHT)

// Etats d'un oeil
#define EYE_STATE_NONE      0     // L'oeil ne fait rien
#define EYE_STATE_NORMAL    1     // l'oeil est dans un état normal c'est à dire sans animation
#define EYE_STATE_ROLLING   2     // l'oeil roule

/**
 * Image d'une animation
 */
typedef uint16_t frame[8];

/**
 * Image d'une animation de paupière
 */
typedef struct {
  frame pixels;
  frame mask;
} masked_frame;

/**
 * Contrôleur d'un oeil
 */
class RobotEye {
  private:
    uint8_t eye_type;
    uint8_t i2c_addr;
    frame displayBuffer;
    bool m_refreshEye;
    uint8_t m_lookAt;
    int m_state;
    int m_stateStep;
    Timeline m_eyeTimeline;
  protected:
    void clear();
    void displayFrame(frame *f);

    void processStateRolling();
  public:
    RobotEye();
    
    void init(uint8_t addr, uint8_t type);
    void setBrightness(uint8_t b);
    void blinkRate(uint8_t b);

    void drawEye();
    
    void lookAt(uint8_t direction = EYE_LOOK_FORWARD);
    uint8_t getLookAt();

    void normal();
    void rolling();

    int getState();
    
    void run();
};

#endif // RobotEye_h
