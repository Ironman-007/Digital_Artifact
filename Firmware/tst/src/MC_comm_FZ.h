#pragma once

#include <Arduino.h>
#include <bluefruit.h>

#define MAX_PKG_LEN 200
#define PKG_LEN     49

extern volatile bool ble_connnected;

extern uint32_t timestamp;
extern uint8_t  seq_num;

extern uint8_t pkg2send[PKG_LEN];

extern void MC_BLE_init(void);
extern void MC_BLE_startAdv(void);
extern void MC_pack_ack(void);
extern void MC_comm_send_data(uint8_t *data2send, uint8_t len);



