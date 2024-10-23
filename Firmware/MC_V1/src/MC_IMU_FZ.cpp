#include <Arduino.h>

#include "MC_IMU_FZ.h"
#include "MC_system_FZ.h"

Adafruit_ICM20948 icm;
// uint16_t measurement_delay_us = 65535; // Delay between measurements for testing

float acc_x = 0.0;
float acc_y = 0.0;
float acc_z = 0.0;

float gyro_x = 0.0;
float gyro_y = 0.0;
float gyro_z = 0.0;

float mag_x = 0.0;
float mag_y = 0.0;
float mag_z = 0.0;

void MC_imu_init(void) {
  Wire.setPins(IC2_SDA_pin, IC2_SCL_pin);
  Wire.begin();

  // Try to initialize!
  if (!icm.begin_I2C(0x68)) {
    while (1) {
      delay(10);
    }
  }
}

void MC_read_imu(void) {
  //  /* Get a new normalized sensor event */
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t mag;
  sensors_event_t temp;

  icm.getEvent(&accel, &gyro, &temp, &mag);

  acc_x  = accel.acceleration.x;
  acc_y  = accel.acceleration.y;
  acc_z  = accel.acceleration.z;

  gyro_x = gyro.gyro.x;
  gyro_y = gyro.gyro.y;
  gyro_z = gyro.gyro.z;

  mag_x  = mag.magnetic.x;
  mag_y  = mag.magnetic.y;
  mag_z  = mag.magnetic.z;
}
