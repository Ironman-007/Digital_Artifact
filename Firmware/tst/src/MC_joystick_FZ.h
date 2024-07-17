#pragma once

#include <Arduino.h>

#define JOYSTICK_X_PIN A4
#define JOYSTICK_Y_PIN A5

extern int joystickX;
extern int joystickY;

extern void MC_joystickInit();
extern void MC_joystickRead();