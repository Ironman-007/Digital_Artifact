#include "MC_btn_FZ.h"

uint8_t btn1 = 0;
uint8_t btn2 = 0;
uint8_t btn3 = 0;
uint8_t btn4 = 0;

int btn_reading = 0;

void MC_btn_init()
{
  pinMode(BTN_1_PIN, INPUT);
  pinMode(BTN_2_PIN, INPUT);
  pinMode(BTN_3_PIN, INPUT);
  pinMode(BTN_4_PIN, INPUT);
}

void MC_read_btn(void)
{
  btn_reading = digitalRead(BTN_1_PIN);
  btn1 = btn_reading & 0xFF;

  btn_reading = digitalRead(BTN_2_PIN);
  btn2 = btn_reading & 0xFF;

  btn_reading = digitalRead(BTN_3_PIN);
  btn3 = btn_reading & 0xFF;

  btn_reading = digitalRead(BTN_4_PIN);
  btn4 = btn_reading & 0xFF;
}