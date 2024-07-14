// Author: Fangzheng Liu
// Created: 2024-05-24
// This is used to communicate with the node.

#include "SN_host_node_FZ.h"
#include "SN_host_comm_FZ.h"
#include "SN_host_system_FZ.h"

SN_nodes SN_nodes_net_FZ;

SN_nodes::SN_nodes(/* args */) {
  // The number of nodes in the network
  this -> SN_node_cnt = 0;

  // // The address of all nodes in the network
  memset(this -> SN_node_addr, 0, SN_MAX_NODE_CNT);
  memset(this -> SN_node_data, 0, NODE_DATA_BYTES_CNT +  NODE_CNT_BYTE_NUM);
}

SN_nodes::~SN_nodes() {
  ;
}

void SN_nodes::reset_node_addr(void) {
  this -> SN_node_cnt = 0;
  memset(this -> SN_node_addr, 0, SN_MAX_NODE_CNT);
}

void SN_nodes::reset_node_data(void) {
  memset(this -> SN_node_data, 0, NODE_DATA_BYTES_CNT +  NODE_CNT_BYTE_NUM);
}

// Scan the nodes in the network and update the SN_node_cnt and SN_node_addr
void SN_nodes::SN_host_scan_node(uint8_t node_addr_start, int steps) {
  this -> reset_node_addr();
  this -> reset_node_data();

  uint8_t addr_i   = 0;
  uint8_t err_code = 0;

  for (addr_i = 0; addr_i < steps; addr_i++) {
    Wire.beginTransmission(addr_i);
    err_code = Wire.endTransmission();

    if (err_code == 0) {
      this -> SN_node_addr[this -> SN_node_cnt] = addr_i;
      this -> SN_node_cnt ++;
    }
  }
}

// Get data from A SINGLE NODE and store data in the SN_node_data.
void SN_nodes::SN_host_get_node_data(uint8_t node_addr_start) {
  Wire.beginTransmission(node_addr_start);
  Wire.endTransmission();

  // request 56 bytes from the current node
  Wire.requestFrom(node_addr_start, NODE_DATA_BYTES_CNT);

  int byte_i = 2;

  while (Wire.available()) {
    this -> SN_node_data[byte_i] = Wire.read();
    byte_i ++;
  }
}

void SN_update_addr_net(void) {
  SN_nodes_net_FZ.SN_host_scan_node(NODE_START_ADDR, NODE_STEPS);

  if (!ble_connnected) {
    flash_IND_LED(10);
  }
  else {
    flash_IND_LED_inv(10);
  }
}

void SN_get_node_num(void) {
  int node_cnt = SN_nodes_net_FZ.SN_node_cnt;
  byte * node_cnt_byte = (byte *) &node_cnt;
  memcpy(SN_nodes_net_FZ.SN_node_data, node_cnt_byte, 2);
}

void SN_send_node_data(void) {
  if (SN_nodes_net_FZ.SN_node_cnt > 0) {
    for (uint8_t addr_i = 0; addr_i < SN_nodes_net_FZ.SN_node_cnt; addr_i++) {
      SN_nodes_net_FZ.SN_host_get_node_data(SN_nodes_net_FZ.SN_node_addr[addr_i]);
      SN_host_comm_send_data(SN_nodes_net_FZ.SN_node_data, NODE_DATA_BYTES_CNT + NODE_CNT_BYTE_NUM);
    }
  }
}








