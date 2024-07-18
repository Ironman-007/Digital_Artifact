#include "MC_system_FZ.h"
#include "MC_encoder_FZ.h"

int en1_last_reading    = 0;
int en1_current_reading = 0;

int en2_reading = 0;

int tst_encoder_count = 128;

void setup()
{
  MC_systemInit();
  MC_encoder_init();

  MC_interrupt_init();
}

void loop()
{
  if (timer4Interrupt_10ms)
  {
    timer4Interrupt_10ms = false;

    en1_current_reading = digitalRead(ENCODER_1_PIN);

    if (en1_current_reading != en1_last_reading && en1_current_reading == 1)
    {
      en2_reading = digitalRead(ENCODER_2_PIN);

      if (en2_reading != en1_current_reading)
      {
        if (tst_encoder_count > 0) tst_encoder_count--;
      }
      else
      {
        if (tst_encoder_count < 255) tst_encoder_count++;
      }
    }
    else
    {
      tst_encoder_count = tst_encoder_count;
    }

    en1_last_reading = en1_current_reading;
  }

  analogWrite(LED_PIN, tst_encoder_count);
}




