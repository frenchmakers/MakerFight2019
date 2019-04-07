/***************************************************
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#ifndef GyroController_h
#define GyroController_h

#include "config.h"
#include "RobotController.h"

/**
 * Contrôleur basé sur un gyroscope
 */
class GyroController : public RobotController {
    private:
        int m_state;
        float m_gyro_x, m_gyro_y, m_gyro_z;
        float m_accel_x, m_accel_y, m_accel_z;
        float m_accel_angle_x, m_accel_angle_y, m_accel_angle_z;
        float m_gyro_angle_x, m_gyro_angle_y, m_gyro_angle_z;
        float m_unfiltered_gyro_angle_x, m_unfiltered_gyro_angle_y, m_unfiltered_gyro_angle_z;
        float m_angle_x, m_angle_y, m_angle_z;

        float m_last_accel_angle_x, m_last_accel_angle_y;
    public:
        virtual void init();
        virtual void run();
        virtual unsigned long getControls();
};

#endif // GyroController_h
