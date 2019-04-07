/*************************************************** 
 *  Configuration
 *  
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 ****************************************************/

#ifndef config_h
#define config_h

// Définition du mode DEBUG
#define DEBUG 1

// Adresses SPI de chaque oeil
#define LEFT_EYE_ADDR 0x71
#define RIGHT_EYE_ADDR 0x70

// Modes de fonctionnement
#define MODE_DEMO       0
#define MODE_JOYSTICK   1
#define MODE_GYRO       2

// Définition du mode de fonctionnement
//#define MODE MODE_DEMO
//#define MODE MODE_JOYSTICK
#define MODE MODE_GYRO

// Configuration du mode joystick
#if MODE == MODE_JOYSTICK
#define JOYSTICK_SWITCH_PIN 5       // port digital connecté ou bouton poussoir
#define JOYSTICK_AXE_X_PIN  1       // port anologique pour l'axe des X
#define JOYSTICK_AXE_Y_PIN  0       // port anologique pour l'axe des Y
#define JOYSTICK_SHOCK_PIN  3       // port digital pour le détecteur de choc
#endif

// Configuration du mode gyroscope
#if MODE == MODE_GYRO
#define GYRO_ADDR 0x68              // Adresse du gyroscope         
#endif

#endif // config_h
