#include "MC_system_FZ.h"
#include "MC_joystick_FZ.h"
#include "MC_IMU_FZ.h"
#include "MC_comm_FZ.h"
#include "MC_btn_FZ.h"
#include "MC_encoder_FZ.h"

void setup() {
  MC_systemInit();
  MC_joystickInit();
  MC_btn_init();
  MC_encoder_init();
  MC_imu_init();
  MC_BLE_init();
  MC_BLE_startAdv();

  MC_interrupt_init();

  MC_flash_led(LED_PIN, 10);
}

void loop() {
  if (ble_connnected) {
    if (timer4Interrupt_2ms) {
      timer4Interrupt_2ms = false;

      MC_read_encoder();
    }

    if (timer4Interrupt_20ms) {
      timer4Interrupt_20ms = false;

      MC_joystickRead();
      MC_read_imu();
      MC_read_btn();
      MC_read_bat_v();

      MC_pack_ack();
      MC_comm_send_data();
    }
  }
}







