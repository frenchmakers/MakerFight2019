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

// Etat général de l'oeil
#define EYE_STATE_NONE      0x00000000      // L'oeil est éteint
#define EYE_STATE_OPENED    0x00000001      // Paupière ouverte
#define EYE_STATE_OPENING   0x00000002      // Paupière en cours d'ouverture
#define EYE_STATE_CLOSED    0x00000003      // Paupière fermée
#define EYE_STATE_CLOSING   0x00000004      // Paupière en cours de fermeture
#define EYE_STATE_BLINKING  0x00000005      // Clignement de l'oeil
#define EYE_STATE_DEAD      0x00000006      // L'oeil est mort
#define EYE_STATE_MASK      0x000000FF      // Masque pour isoler l'état général
#define getEyeState(eye) (eye->getState() & EYE_STATE_MASK)

// Actions
#define EYE_ACTION_NONE     0x00000000      // L'oeil n'a aucun comportement pour le moment
#define EYE_ACTION_ROLLING  0x00000100      // L'oeil est en cours de roulement
#define EYE_ACTION_MASK     0x0FFFFF00      // Masque pour isoler l'état de actions
#define getEyeAction(eye) (eye->getState() & EYE_ACTION_MASK)

// Sentiments
#define EYE_FEELING_NORMAL  0x00000000      // Le regard est normal
#define EYE_FEELING_ANGRY   0x10000000      // Le regard est en colère
#define EYE_FEELING_SCARED  0x20000000      // Le regard est apeuré
#define EYE_FEELING_MASK    0xF0000000      // Masque pour isoler l'état des sentiments
#define getEyeFeeling(eye) (eye->getState() & EYE_FEELING_MASK)

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
 * Animation avec des frames
 */
typedef struct {
  frame *frames;
  int count;
  int speed;
} animation;

/**
 * Animation avec des frames avec masque
 */
typedef struct {
  masked_frame *frames;
  int count;
  int speed;
} masked_animation;

/**
 * Contrôleur d'un oeil
 */
class RobotEye {
  private:
    uint8_t eye_type;
    uint8_t i2c_addr;
    frame displayBuffer;
    bool m_refreshEye;
    bool m_reversed;
    uint8_t m_lookAt;
    masked_frame *m_eyeLip;
    masked_animation *m_eyeLip_movement;
    long m_state;
    int m_stateStep;
    int m_actionStep;
    Timeline m_stateTimeline;
    Timeline m_actionTimeline;
  protected:
    void displayFrame(frame *f);

    void drawEye();
    
    void clear();
    void setAction(long action);
    void setState(long state);
    void setFeeling(long feeling);

    void processStateOpening();
    void processStateClosing();
    void processStateBlinking();

    void processActionRolling();

    masked_frame *getOpenedEye();
    masked_frame *getClosedEye();
  public:
    RobotEye();
    
    void init(uint8_t addr, uint8_t type);
    void setBrightness(uint8_t b);
    void blinkRate(uint8_t b);

    void lookAt(uint8_t direction = EYE_LOOK_FORWARD);
    uint8_t getLookAt();

    void reverse();

    void open();
    void close();
    void blink();
    void dead();
    void reset();

    void normal();
    void rolling();

    void isNormal();
    void isAngry();
    void isScared();

    long getState();
    
    void run();
};

#endif // RobotEye_h
