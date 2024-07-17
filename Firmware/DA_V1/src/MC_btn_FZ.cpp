#include "MC_btn_FZ.h"

uint8_t btn1 = 0;
uint8_t btn2 = 0;
uint8_t btn3 = 0;
uint8_t btn4 = 0;

void MC_btnInit()
{
  pinMode(BTN_1_PIN, INPUT);
  pinMode(BTN_2_PIN, INPUT);
  pinMode(BTN_3_PIN, INPUT);
  pinMode(BTN_4_PIN, INPUT);
}

void MC_read_btn(void)
{
  btn1 = digitalRead(BTN_1_PIN);
  btn2 = digitalRead(BTN_2_PIN);
  btn3 = digitalRead(BTN_3_PIN);
  btn4 = digitalRead(BTN_4_PIN);
}