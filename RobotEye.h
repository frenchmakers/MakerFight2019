/***************************************************
 * Librairie de gestion d'un oeil du robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#ifndef RobotEye_h
#define RobotEye_h

// Types d'oeil
#define EYE_LEFT 0
#define EYE_RIGHT 1

// Etats d'un oeil
#define EYE_STATE_NONE 0

/**
 * Image d'une animation
 */
typedef uint16_t frame[8];

/**
 * Contrôleur d'un oeil
 */
class RobotEye {
  private:
    uint8_t eye_type;
    uint8_t i2c_addr;
    frame displayBuffer;
    int m_state;
    int m_stateStep;
  protected:
  public:
    RobotEye();
    
    void init(uint8_t addr, uint8_t type);
    void setBrightness(uint8_t b);
    void blinkRate(uint8_t b);

    void clear();
    void displayFrame(frame *f);

    void drawEye();
    
    void run();
};

#endif // RobotEye_h
