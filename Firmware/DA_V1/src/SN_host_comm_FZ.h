#pragma once

#include <Arduino.h>
#include <bluefruit.h>

#define MAX_PKG_LEN 200
#define PKG_LEN     100

extern volatile bool ble_connnected;

extern uint8_t pkg2send[PKG_LEN];

extern void update(void);
extern void SN_host_BLE_init(void);
extern void SN_host_BLE_startAdv(void);
extern void SN_host_comm_send_data(uint8_t *data2send, uint8_t len);
extern void SN_host_comm_tst_func(void);
