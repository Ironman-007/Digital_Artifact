#pragma once

#include <Arduino.h>

#define SERIAL_DEBUG true

extern void SN_host_system_init(bool serial_en);

#define IND_LED_PIN       13 // LED
#define SN_host_SDA_PIN   26
#define SN_host_SCL_PIN   27

extern void SN_host_pin_init(void);

enum IND_LED_STATE {
  IND_LED_OFF = LOW,
  IND_LED_ON  = HIGH
};

extern void IND_LED(IND_LED_STATE led_state);
extern void flash_IND_LED(int flash_interval_ms);
extern void flash_IND_LED_inv(int flash_interval_ms);

extern void SN_host_tst_func(void);

// parameters for interrupt

#define HW_TIMER_INTERVAL_MS 1
#define TIMER_INTERVAL_2s    3000L

extern volatile bool timer4Interrupt_2s;

extern void SN_host_interrupt_init(void);
extern void SN_host_nodes_init(void);