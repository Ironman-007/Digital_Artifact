#include "MC_joystick_FZ.h"

int joystickX = 0;
int joystickY = 0;

void MC_joystickInit()
{
  pinMode(JOYSTICK_X_PIN, INPUT);
  pinMode(JOYSTICK_Y_PIN, INPUT);
}

void MC_joystickRead()
{
  joystickX = analogRead(JOYSTICK_X_PIN);
  joystickY = analogRead(JOYSTICK_Y_PIN);
}
