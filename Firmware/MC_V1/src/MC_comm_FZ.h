#pragma once

#include <Arduino.h>
#include <bluefruit.h>

#define MAX_PKG_LEN   200
#define PKG_LEN       55
#define PKG_LEN_COBS  55

extern volatile bool ble_connnected;

extern uint32_t timestamp;
extern uint8_t  seq_num;

extern uint8_t pkg2send[PKG_LEN];
extern uint8_t COBSpkg2send[PKG_LEN_COBS];

extern int COBS_encoded_len;

extern void MC_BLE_init(void);
extern void MC_BLE_startAdv(void);
extern void MC_pack_ack(void);
extern void MC_comm_send_data(void);

extern size_t COBSencode(uint8_t* buffer, size_t size, uint8_t* encodedBuffer);
extern size_t COBSdecode(uint8_t* buffer, size_t size, uint8_t* decodedBuffer);

