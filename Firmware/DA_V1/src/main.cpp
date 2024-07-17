#include "MC_system_FZ.h"
#include "MC_joystick_FZ.h"
#include "MC_IMU_FZ.h"
#include "MC_comm_FZ.h"
#include "MC_btn_FZ.h"

void setup()
{
  MC_systemInit();
  MC_joystickInit();
  MC_imu_init();
  MC_BLE_init();
  MC_BLE_startAdv();

  MC_interrupt_init();
}

void loop()
{
  if (timer4Interrupt_10ms)
  {
    timer4Interrupt_10ms = false;

    MC_joystickRead();
    MC_read_imu();
    MC_read_btn();

    MC_pack_ack();
    MC_comm_send_data(pkg2send, PKG_LEN);
  }
}




