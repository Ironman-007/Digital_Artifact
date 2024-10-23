#pragma once

#include <Arduino.h>

#define LED_PIN   8
#define BAT_V_PIN A0

#define IC2_SDA_pin 26
#define IC2_SCL_pin 27

#define HW_TIMER_INTERVAL_MS 1
#define TIMER_INTERVAL_2ms   5L
#define TIMER_INTERVAL_20ms  50L

extern volatile bool timer4Interrupt_2ms;
extern volatile bool timer4Interrupt_20ms;

extern int bat_v;

extern void MC_systemInit(void);
extern void MC_interrupt_init(void);

extern void MC_flash_led(int pin, int flash_period);
extern void MC_off_led(int pin);

extern void MC_read_bat_v(void);
