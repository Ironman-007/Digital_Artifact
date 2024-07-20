#pragma once

#include <Arduino.h>

#define LED_PIN 8

#define HW_TIMER_INTERVAL_MS 1
#define TIMER_INTERVAL_10ms  2L

extern volatile bool timer4Interrupt_10ms;

extern void MC_systemInit(void);
extern void MC_interrupt_init(void);

extern void MC_flash_led(int pin, int flash_period);
extern void MC_off_led(int pin);