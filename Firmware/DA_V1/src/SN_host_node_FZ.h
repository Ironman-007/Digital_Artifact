#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "SN_host_comm_FZ.h"
#include "SN_host_system_FZ.h"

#define NODE_SCAN_PERIOD    1000 // The period of scanning the nodes in the network
#define SN_MAX_NODE_CNT     100  // The maximum number of nodes in the network
#define NODE_DATA_BYTES_CNT 56   // The number of bytes of data in each node
#define NODE_CNT_BYTE_NUM   2    // The maximum length of the package

#define NODE_START_ADDR     0x01 // The start address of the nodes in the network
#define NODE_STEPS          50

static_assert(NODE_DATA_BYTES_CNT + NODE_CNT_BYTE_NUM <= MAX_PKG_LEN,
              "The number of bytes from each node should be less than 200!");

// The information of the whole sensor network
// store the address of all nodes, how many nodes in the network
class SN_nodes
{
private:
  int SCAN_PERIOD = NODE_SCAN_PERIOD; 

public:
  SN_nodes(/* args */);
  ~SN_nodes();

  // The number of nodes in the network
  volatile int SN_node_cnt;

  // The address of all nodes in the network
  uint8_t SN_node_addr[SN_MAX_NODE_CNT];
  uint8_t SN_node_data[NODE_DATA_BYTES_CNT +  NODE_CNT_BYTE_NUM];

  void reset_node_data(void);
  void reset_node_addr(void);

  // Scan the nodes in the network and update the SN_node_cnt and SN_node_addr
  void SN_host_scan_node(uint8_t node_addr_start, int steps);

  // Get data from A SINGLE NODE and store data in the SN_node_data.
  void SN_host_get_node_data(uint8_t node_addr_start);
};

// extern SN_node
extern SN_nodes SN_nodes_net_FZ;

extern void SN_update_addr_net(void);
extern void SN_send_node_data(void);
extern void SN_get_node_num(void);



