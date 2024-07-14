// Author: Fangzheng Liu
// Created: 2024-05-24
// This file is used for the system control, like serialport initialization etc.

#include "SN_host_system_FZ.h"
#include "SN_host_node_FZ.h"

#include "NRF52TimerInterrupt.h"
#include "NRF52_ISR_Timer.h"

NRF52Timer ITimer(NRF_TIMER_1);
NRF52_ISR_Timer ISR_Timer;

volatile bool timer4Interrupt_2s = false;

// ===================== Interrupt functions =====================
static void TimerHandler(void) {
  ISR_Timer.run();
}

static void timer_handler_2s(void) {
  timer4Interrupt_2s = true;
}

void SN_host_interrupt_init(void) {
  ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_MS * 1000, TimerHandler);
  ISR_Timer.setInterval(TIMER_INTERVAL_2s, timer_handler_2s);
}

void SN_host_system_init(bool serial_en) {
  if (serial_en) {
    Serial.begin(115200);
    while (!Serial) {
      delay(10); // wait for serial port to connect. Needed for native USB port only
    }
  }
}

void SN_host_pin_init(void) {
  pinMode(IND_LED_PIN, OUTPUT);
  digitalWrite(IND_LED_PIN, LOW);
}

void IND_LED(IND_LED_STATE led_state) {
  digitalWrite(IND_LED_PIN, led_state);
}

void flash_IND_LED(int flash_interval_ms) {
  digitalWrite(IND_LED_PIN, HIGH);
  delay(flash_interval_ms);
  digitalWrite(IND_LED_PIN, LOW);
}

void flash_IND_LED_inv(int flash_interval_ms) {
  digitalWrite(IND_LED_PIN, LOW);
  delay(flash_interval_ms);
  digitalWrite(IND_LED_PIN, HIGH);
}

void SN_host_nodes_init(void) {
  Wire.setPins(SN_host_SDA_PIN, SN_host_SCL_PIN);
  Wire.begin();
}

void SN_host_tst_func(void) {
  digitalWrite(IND_LED_PIN, LOW);
  delay(100);
  digitalWrite(IND_LED_PIN, HIGH);
  delay(100);
}



