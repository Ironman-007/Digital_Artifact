#pragma once

#include <Arduino.h>

#define BTN_1_PIN 7
#define BTN_2_PIN 9
#define BTN_3_PIN 10
#define BTN_4_PIN 14

extern uint8_t btn1;
extern uint8_t btn2;
extern uint8_t btn3;
extern uint8_t btn4;

extern void MC_btnInit(void);
extern void MC_read_btn(void);
