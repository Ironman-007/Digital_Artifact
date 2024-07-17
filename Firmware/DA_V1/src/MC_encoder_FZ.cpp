#include "MC_encoder_FZ.h"

int lastStateCLK    = 0;
int currentStateCLK = 0;

long encoder_count  = 0;

void MC_encoder_init()
{
  pinMode(ENCODER_1_PIN, INPUT);
  pinMode(ENCODER_2_PIN, INPUT);

  lastStateCLK = digitalRead(ENCODER_1_PIN);
}

void MC_read_encoder(void)
{
  if (timer4Interrupt_2ms)
  {
    timer4Interrupt_2ms = false;

	  currentStateCLK = digitalRead(ENCODER_1_PIN);

    if (currentStateCLK != lastStateCLK  && currentStateCLK == 1)
    {
      if (digitalRead(ENCODER_2_PIN) != currentStateCLK) {
        encoder_count --;
      } else {
        encoder_count ++;
      }
    }

    // Remember last CLK state
    lastStateCLK = currentStateCLK;

  }
}