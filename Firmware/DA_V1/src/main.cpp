#include <Arduino.h>

#include "DA_system_FZ.h"
#include "DA_joystick_FZ.h"

void setup() {
  systemInit();
  joystickInit();
}

// the loop function runs over and over again forever
void loop() {
  joystickRead();
  analogWrite(LED_PIN, joystickY / 10);
}




