// Author: Fangzheng Liu
// Created: 2024-07-15
// This file is used for the BLE communication.

#include "MC_comm_FZ.h"
#include "MC_system_FZ.h"
#include "MC_IMU_FZ.h"
#include "MC_joystick_FZ.h"
#include "MC_btn_FZ.h"
#include "MC_encoder_FZ.h"

volatile bool ble_connnected = false;

byte * mc_byte;

uint32_t timestamp = 0;
uint8_t  seq_num   = 0;

uint8_t pkg2send[PKG_LEN]          = {0};
uint8_t COBSpkg2send[PKG_LEN_COBS] = {0};

int COBS_encoded_len = 0;

BLEUart bleuart;

// callback invoked when central connects
void connect_callback(uint16_t conn_handle) {
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  // request to update data length
  connection->requestDataLengthUpdate();

  // request mtu exchange
  connection->requestMtuExchange(MAX_PKG_LEN);

  ble_connnected = true;

  // MC_flash_led(LED_PIN, 500);
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  (void) conn_handle;
  (void) reason;

  ble_connnected = false;

  // MC_off_led(LED_PIN);
}

void MC_BLE_init(void) {
  Bluefruit.autoConnLed(true);
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.begin();
  Bluefruit.setTxPower(0); // set to 0dBm first.
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  Bluefruit.setName("MindCube - FZ");

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

void MC_BLE_startAdv(void) {
  // Start Advertising
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.start(0);                   // 0 = Don't stop advertising after n seconds  
}

void MC_pack_ack(void) {
  timestamp = millis();
  mc_byte = (byte *) &timestamp;
  memcpy(&pkg2send[0], mc_byte, 4);

  mc_byte = (byte *) &seq_num;
  memcpy(&pkg2send[4], mc_byte, 1);
  seq_num ++;

  mc_byte = (byte *) &acc_x;
  memcpy(&pkg2send[5], mc_byte, 4);
  mc_byte = (byte *) &acc_y;
  memcpy(&pkg2send[9], mc_byte, 4);
  mc_byte = (byte *) &acc_z;
  memcpy(&pkg2send[13], mc_byte, 4);

  mc_byte = (byte *) &gyro_x;
  memcpy(&pkg2send[17], mc_byte, 4);
  mc_byte = (byte *) &gyro_y;
  memcpy(&pkg2send[21], mc_byte, 4);
  mc_byte = (byte *) &gyro_z;
  memcpy(&pkg2send[25], mc_byte, 4);

  mc_byte = (byte *) &mag_x;
  memcpy(&pkg2send[29], mc_byte, 4);
  mc_byte = (byte *) &mag_y;
  memcpy(&pkg2send[33], mc_byte, 4);
  mc_byte = (byte *) &mag_z;
  memcpy(&pkg2send[37], mc_byte, 4);

  mc_byte = (byte *) &joystickX;
  memcpy(&pkg2send[41], mc_byte, 2);
  mc_byte = (byte *) &joystickY;
  memcpy(&pkg2send[43], mc_byte, 2);

  mc_byte = (byte *) &btn1;
  memcpy(&pkg2send[45], mc_byte, 1);
  mc_byte = (byte *) &btn2;
  memcpy(&pkg2send[46], mc_byte, 1);
  mc_byte = (byte *) &btn3;
  memcpy(&pkg2send[47], mc_byte, 1);
  mc_byte = (byte *) &btn4;
  memcpy(&pkg2send[48], mc_byte, 1);

  mc_byte = (byte *) &encoder_count;
  memcpy(&pkg2send[49], mc_byte, 4);

  mc_byte = (byte *) &bat_v;
  memcpy(&pkg2send[53], mc_byte, 2);
}

// ==================== COBS coding ====================
/// \brief Encode a byte buffer with the COBS encoder.
/// \param buffer A pointer to the unencoded buffer to encode.
/// \param size  The number of bytes in the \p buffer.
/// \param encodedBuffer The buffer for the encoded bytes.
/// \returns The number of bytes written to the \p encodedBuffer.
/// \warning The encodedBuffer must have at least getEncodedBufferSize() allocated.
// size_t COBS_encode(uint8_t* buffer, size_t size, uint8_t* encodedBuffer) {
size_t COBSencode(uint8_t* buffer, size_t size, uint8_t* encodedBuffer) {
  size_t  read_index  = 0;
  size_t  write_index = 1;
  size_t  code_index  = 0;
  uint8_t code        = 1;

  while (read_index < size) {
    if (buffer[read_index] == 0) {
      encodedBuffer[code_index] = code;
      code = 1;
      code_index = write_index++;
      read_index++;
    }
    else {
      encodedBuffer[write_index++] = buffer[read_index++];
      code++;

      if (code == 0xFF) {
        encodedBuffer[code_index] = code;
        code = 1;
        code_index = write_index++;
      }
    }
  }

  encodedBuffer[code_index] = code;

  return write_index;
}

/// \brief Decode a COBS-encoded buffer.
/// \param encodedBuffer A pointer to the \p encodedBuffer to decode.
/// \param size The number of bytes in the \p encodedBuffer.
/// \param decodedBuffer The target buffer for the decoded bytes.
/// \returns The number of bytes written to the \p decodedBuffer.
/// \warning decodedBuffer must have a minimum capacity of size.
// size_t decode(const uint8_t* encodedBuffer, size_t size, uint8_t* decodedBuffer)
size_t COBSdecode(uint8_t* encodedBuffer, size_t size, uint8_t* decodedBuffer) {
  size_t read_index  = 0;
  size_t write_index = 0;
  uint8_t code       = 0;
  uint8_t i          = 0;

  while (read_index < size) {
    code = encodedBuffer[read_index];

    read_index++;

    for (i = 1; i < code; i++) { decodedBuffer[write_index++] = encodedBuffer[read_index++];}

    if (code != 0xFF && read_index != size) {decodedBuffer[write_index++] = '\0';}
  }

  return write_index;
}

void MC_comm_send_data(void) {
  COBS_encoded_len = COBSencode(pkg2send, PKG_LEN, COBSpkg2send);
  bleuart.write(COBSpkg2send, COBS_encoded_len);
}






