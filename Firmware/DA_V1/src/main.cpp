#include <Arduino.h>

#include "SN_host_system_FZ.h"
#include "SN_host_comm_FZ.h"
#include "SN_host_node_FZ.h"

void setup() {
  SN_host_system_init(SERIAL_DEBUG);
  SN_host_pin_init();
  SN_host_BLE_init();
  SN_host_BLE_startAdv();
  SN_host_interrupt_init();
  SN_host_nodes_init();
}

void loop() {
  // scan the whole network and update the SN_node_addr every 1s.
  if (timer4Interrupt_2s) {
    timer4Interrupt_2s = false;
    SN_update_addr_net();
    SN_get_node_num();
  }

  // read data from each node and send through BLE.
  if (ble_connnected) {
    SN_send_node_data();
  }
}



