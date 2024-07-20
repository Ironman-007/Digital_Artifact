#include "MC_system_FZ.h"

#include "NRF52TimerInterrupt.h"
#include "NRF52_ISR_Timer.h"

NRF52Timer ITimer(NRF_TIMER_1);
NRF52_ISR_Timer ISR_Timer;

volatile bool timer4Interrupt_10ms = false;

static void TimerHandler(void) {
  ISR_Timer.run();
}

static void timer_handler_10ms(void) {
  timer4Interrupt_10ms = true;
}

void MC_interrupt_init(void) {
  ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_MS * 1000, TimerHandler);
  ISR_Timer.setInterval(TIMER_INTERVAL_10ms, timer_handler_10ms);
}

void MC_systemInit()
{
  pinMode(LED_PIN, OUTPUT);
  MC_off_led(LED_PIN);
}

void MC_flash_led(int pin, int flash_period)
{
  digitalWrite(pin, HIGH);

  if (flash_period > 0)
  {
    delay(flash_period);
    digitalWrite(pin, LOW);
  }
}

void MC_off_led(int pin)
{
  digitalWrite(pin, LOW);
}

