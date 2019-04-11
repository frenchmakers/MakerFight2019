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
#define MODE_RC         2

// Définition du mode de fonctionnement
//#define MODE MODE_DEMO
//#define MODE MODE_JOYSTICK
#define MODE MODE_RC

// Configuration du mode joystick
#if MODE == MODE_JOYSTICK
#define JOYSTICK_SWITCH_PIN 5       // port digital connecté ou bouton poussoir
#define JOYSTICK_AXE_X_PIN  1       // port anologique pour l'axe des X
#define JOYSTICK_AXE_Y_PIN  0       // port anologique pour l'axe des Y
#define JOYSTICK_SHOCK_PIN  3       // port digital pour le détecteur de choc
#endif

// Configuration du mode MODE_RC
#if MODE == MODE_RC
#define THREE_STATE_CHANNEL 2           // Channel pour le bouton trois états
#define THREE_STATE_CHANNEL_LIMIT_WIN   1000    // Valeur en dessous de laquelle le bouton est en mode "Gagné"
#define THREE_STATE_CHANNEL_LIMIT_LOST  2000    // Valeur au dessus de laquelle le bouton est en mode "Perdu"

#define MOVE_1_CHANNEL          3       // Channel pour le mouvement 1
//#define MOVE_2_CHANNEL          4       // Channel pour le mouvement 2

#define MOVE_MODE_DIRECTION     0       // Mode de mouvement directionnel   => Move1: Avant/Arriere; Move2: Droite/Gauche
#define MOVE_MODE_WHEELS        1       // Mode de mouvement par roues      => Move1: Roue gauche; Move2: Roue droite 

#define MOVE_MODE   MOVE_MODE_DIRECTION    // Mode de direction
#define MOVE_1_CENTER           1500    // Centre pour le mouvement 1
#define MOVE_1_LIMITS           200     // Limite
#define MOVE_2_CENTER           1500    // Centre pour le mouvement 2
#define MOVE_2_LIMITS           200     // Limite
#endif

#endif // config_h
