/***************************************************
 * Librairie de gestion du comportement de robot Maker Fight 2019
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#include "GyroController.h"
#include "Gyro.h"

void GyroController::init()
{
    int error;
    uint8_t c;

    m_state = 0;

    //Serial.begin(19200);
    /*
  Serial.println(F("InvenSense MPU-6050"));
  Serial.println(F("June 2012"));
  */
    // Initialize the 'Wire' class for the I2C-bus.
    Wire.begin();

    // default at power-up:
    //    Gyro at 250 degrees second
    //    Acceleration at 2g
    //    Clock source at internal 8MHz
    //    The device is in sleep mode.
    //

    error = MPU6050_read(GYRO_ADDR, MPU6050_WHO_AM_I, &c, 1);
    /*
  Serial.print(F("WHO_AM_I : "));
  Serial.print(c,HEX);
  Serial.print(F(", error = "));
  Serial.println(error,DEC);
  */

    // According to the datasheet, the 'sleep' bit
    // should read a '1'. But I read a '0'.
    // That bit has to be cleared, since the sensor
    // is in sleep mode at power-up. Even if the
    // bit reads '0'.
    error = MPU6050_read(GYRO_ADDR, MPU6050_PWR_MGMT_2, &c, 1);
    /*
  Serial.print(F("PWR_MGMT_2 : "));
  Serial.print(c,HEX);
  Serial.print(F(", error = "));
  Serial.println(error,DEC);
  */

    // Clear the 'sleep' bit to start the sensor.
    MPU6050_write_reg(GYRO_ADDR, MPU6050_PWR_MGMT_1, 0);

    //Initialize the angles
    calibrate_sensors(GYRO_ADDR);
    set_last_read_angle_data(millis(), 0, 0, 0, 0, 0, 0);
}

void GyroController::run()
{
    int error;
    double dT;
    accel_t_gyro_union accel_t_gyro;

    /*
  Serial.println(F(""));
  Serial.println(F("MPU-6050"));
  */

    // Read the raw values.
    error = read_gyro_accel_vals(GYRO_ADDR, (uint8_t *)&accel_t_gyro);

    // Get the time of reading for rotation computations
    unsigned long t_now = millis();

    /*
  Serial.print(F("Read accel, temp and gyro, error = "));
  Serial.println(error,DEC);
  

  // Print the raw acceleration values
  Serial.print(F("accel x,y,z: "));
  Serial.print(accel_t_gyro.value.x_accel, DEC);
  Serial.print(F(", "));
  Serial.print(accel_t_gyro.value.y_accel, DEC);
  Serial.print(F(", "));
  Serial.print(accel_t_gyro.value.z_accel, DEC);
  Serial.println(F(""));
*/

    // The temperature sensor is -40 to +85 degrees Celsius.
    // It is a signed integer.
    // According to the datasheet:
    //   340 per degrees Celsius, -512 at 35 degrees.
    // At 0 degrees: -512 - (340 * 35) = -12412
    /*  
  Serial.print(F("temperature: "));
  dT = ( (double) accel_t_gyro.value.temperature + 12412.0) / 340.0;
  Serial.print(dT, 3);
  Serial.print(F(" degrees Celsius"));
  Serial.println(F(""));
  

  // Print the raw gyro values.
  Serial.print(F("raw gyro x,y,z : "));
  Serial.print(accel_t_gyro.value.x_gyro, DEC);
  Serial.print(F(", "));
  Serial.print(accel_t_gyro.value.y_gyro, DEC);
  Serial.print(F(", "));
  Serial.print(accel_t_gyro.value.z_gyro, DEC);
  Serial.print(F(", "));
  Serial.println(F(""));
*/

    // Convert gyro values to degrees/sec
    float FS_SEL = 131;
    /*
  float gyro_x = (accel_t_gyro.value.x_gyro - base_x_gyro)/FS_SEL;
  float gyro_y = (accel_t_gyro.value.y_gyro - base_y_gyro)/FS_SEL;
  float gyro_z = (accel_t_gyro.value.z_gyro - base_z_gyro)/FS_SEL;
  */
    m_gyro_x = (accel_t_gyro.value.x_gyro - base_x_gyro) / FS_SEL;
    m_gyro_y = (accel_t_gyro.value.y_gyro - base_y_gyro) / FS_SEL;
    m_gyro_z = (accel_t_gyro.value.z_gyro - base_z_gyro) / FS_SEL;

    // Get raw acceleration values
    //float G_CONVERT = 16384;
    //float accel_x = accel_t_gyro.value.x_accel;
    //float accel_y = accel_t_gyro.value.y_accel;
    //float accel_z = accel_t_gyro.value.z_accel;
    m_accel_x = accel_t_gyro.value.x_accel;
    m_accel_y = accel_t_gyro.value.y_accel;
    m_accel_z = accel_t_gyro.value.z_accel;

    // Get angle values from accelerometer
    float RADIANS_TO_DEGREES = 180 / 3.14159;
    //  float accel_vector_length = sqrt(pow(accel_x,2) + pow(accel_y,2) + pow(accel_z,2));
    //float accel_angle_y = atan(-1*accel_x/sqrt(pow(accel_y,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;
    //float accel_angle_x = atan(accel_y/sqrt(pow(accel_x,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;
    //float accel_angle_z = 0;
    m_accel_angle_y = atan(-1 * m_accel_x / sqrt(pow(m_accel_y, 2) + pow(m_accel_z, 2))) * RADIANS_TO_DEGREES;
    m_accel_angle_x = atan(m_accel_y / sqrt(pow(m_accel_x, 2) + pow(m_accel_z, 2))) * RADIANS_TO_DEGREES;
    m_accel_angle_z = 0;

    // Compute the (filtered) gyro angles
    float dt = (t_now - get_last_time()) / 1000.0;
    //float gyro_angle_x = gyro_x*dt + get_last_x_angle();
    //float gyro_angle_y = gyro_y*dt + get_last_y_angle();
    //float gyro_angle_z = gyro_z*dt + get_last_z_angle();
    m_gyro_angle_x = m_gyro_x * dt + get_last_x_angle();
    m_gyro_angle_y = m_gyro_y * dt + get_last_y_angle();
    m_gyro_angle_z = m_gyro_z * dt + get_last_z_angle();

    // Compute the drifting gyro angles
    //float unfiltered_gyro_angle_x = gyro_x*dt + get_last_gyro_x_angle();
    //float unfiltered_gyro_angle_y = gyro_y*dt + get_last_gyro_y_angle();
    //float unfiltered_gyro_angle_z = gyro_z*dt + get_last_gyro_z_angle();
    m_unfiltered_gyro_angle_x = m_gyro_x * dt + get_last_gyro_x_angle();
    m_unfiltered_gyro_angle_y = m_gyro_y * dt + get_last_gyro_y_angle();
    m_unfiltered_gyro_angle_z = m_gyro_z * dt + get_last_gyro_z_angle();

    // Apply the complementary filter to figure out the change in angle - choice of alpha is
    // estimated now.  Alpha depends on the sampling rate...
    float alpha = 0.96;
    //float angle_x = alpha*gyro_angle_x + (1.0 - alpha)*accel_angle_x;
    //float angle_y = alpha*gyro_angle_y + (1.0 - alpha)*accel_angle_y;
    //float angle_z = gyro_angle_z;  //Accelerometer doesn't give z-angle
    m_angle_x = alpha * m_gyro_angle_x + (1.0 - alpha) * m_accel_angle_x;
    m_angle_y = alpha * m_gyro_angle_y + (1.0 - alpha) * m_accel_angle_y;
    m_angle_z = m_gyro_angle_z; //Accelerometer doesn't give z-angle

    // Update the saved data with the latest values
    set_last_read_angle_data(t_now, m_angle_x, m_angle_y, m_angle_z, m_unfiltered_gyro_angle_x, m_unfiltered_gyro_angle_y, m_unfiltered_gyro_angle_z);

    // Send the data to the serial port
#ifdef DEBUG
    Serial.print(F("DEL:")); //Delta T
    Serial.print(dt, DEC);
    Serial.print(F("#ACC:")); //Accelerometer angle
    Serial.print(m_accel_angle_x, 2);
    Serial.print(F(","));
    Serial.print(m_accel_angle_y, 2);
    Serial.print(F(","));
    Serial.print(m_accel_angle_z, 2);
    Serial.print(F("#GYR:"));
    Serial.print(m_unfiltered_gyro_angle_x, 2); //Gyroscope angle
    Serial.print(F(","));
    Serial.print(m_unfiltered_gyro_angle_y, 2);
    Serial.print(F(","));
    Serial.print(m_unfiltered_gyro_angle_z, 2);
    Serial.print(F("#FIL:")); //Filtered angle
    Serial.print(m_angle_x, 2);
    Serial.print(F(","));
    Serial.print(m_angle_y, 2);
    Serial.print(F(","));
    Serial.print(m_angle_z, 2);
    Serial.println(F(""));
#endif

    // Delay so we don't swamp the serial port
    //delay(5);
}

unsigned long GyroController::getControls()
{
    switch (m_state)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            m_state++;
            return CTRL_NONE;
        case 6:
            m_state++;
            return CTRL_START;
        case 7:
        {
            float x = m_accel_angle_x - m_last_accel_angle_x;
            float y = m_accel_angle_y - m_last_accel_angle_y;
            m_last_accel_angle_x = m_accel_angle_x;
            m_last_accel_angle_y = m_accel_angle_y;
#ifdef DEBUG
            Serial.print(F("CTRL:"));
            Serial.print(F("#ACC:")); //Accelerometer angle
            Serial.print(x, 2);
            Serial.print(F(","));
            Serial.print(y, 2);
            Serial.println(F(""));
#endif
            if(x>3) return CTRL_LEFT;
            if(x<-3) return CTRL_RIGHT;
            break;
        }
        default:
            break;
    }
    return CTRL_NONE;
}
