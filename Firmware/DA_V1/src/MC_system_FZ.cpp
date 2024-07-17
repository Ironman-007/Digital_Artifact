#include "MC_system_FZ.h"

#include "NRF52TimerInterrupt.h"
#include "NRF52_ISR_Timer.h"

NRF52Timer ITimer(NRF_TIMER_1);
NRF52_ISR_Timer ISR_Timer;

volatile bool timer4Interrupt_2ms  = false;
volatile bool timer4Interrupt_20ms = false;

static void TimerHandler(void) {
  ISR_Timer.run();
}

static void timer_handler_2ms(void) {
  timer4Interrupt_2ms = true;
}

static void timer_handler_20ms(void) {
  timer4Interrupt_20ms = true;
}

void MC_interrupt_init(void) {
  ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_MS * 1000, TimerHandler);
  ISR_Timer.setInterval(TIMER_INTERVAL_20ms, timer_handler_20ms);
  ISR_Timer.setInterval(TIMER_INTERVAL_2ms, timer_handler_2ms);
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

