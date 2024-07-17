#pragma once

#include <Arduino.h>
#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20948.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

extern Adafruit_ICM20948 icm;

extern float acc_x;
extern float acc_y;
extern float acc_z;

extern float gyro_x;
extern float gyro_y;
extern float gyro_z;

extern float mag_x;
extern float mag_y;
extern float mag_z;

extern void MC_imu_init(void);
extern void MC_read_imu(void);
