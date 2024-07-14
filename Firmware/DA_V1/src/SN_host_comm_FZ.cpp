// Author: Fangzheng Liu
// Created: 2024-05-24
// This file is used for the BLE communication.

#include "SN_host_comm_FZ.h"
#include "SN_host_node_FZ.h"
#include "SN_host_system_FZ.h"

volatile bool ble_connnected = false;

// char pkg2send[PKG_LEN] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
//                           20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};

// BLEDis  bledis;
BLEUart bleuart;

// callback invoked when central connects
void connect_callback(uint16_t conn_handle) {
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  // request to update data length
  // Serial.println("Request to change Data Length");
  connection->requestDataLengthUpdate();

  // request mtu exchange
  // Serial.println("Request to change MTU");
  connection->requestMtuExchange(MAX_PKG_LEN);

  // char central_name[32] = { 0 };
  // connection->getPeerName(central_name, sizeof(central_name));

  // if (Serial_Debug) Serial.print("BLE connected.");

  IND_LED(IND_LED_ON);
  ble_connnected = true;
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  (void) conn_handle;
  (void) reason;

  IND_LED(IND_LED_OFF);
  ble_connnected = false;
}

void SN_host_BLE_init(void) {
  Bluefruit.autoConnLed(true);
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.begin();
  Bluefruit.setTxPower(0); // set to 0dBm first.
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  Bluefruit.setName("SensorNet - Host");

  bleuart.begin();

  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  Bluefruit.ScanResponse.addName();

  // Start Advertising
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
}

void SN_host_BLE_startAdv(void) {
  // // Start Advertising
  Bluefruit.Advertising.restartOnDisconnect(true);
  // Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  // Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                   // 0 = Don't stop advertising after n seconds  
}

void SN_host_comm_send_data(uint8_t *data2send, uint8_t len) {
  bleuart.write(data2send, len);
}

void SN_host_comm_tst_func(void) {
  bleuart.write(pkg2send, PKG_LEN);
}








