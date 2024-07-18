#pragma once

#include <Arduino.h>

#include "MC_system_FZ.h"

#define ENCODER_1_PIN 19
#define ENCODER_2_PIN 22

extern long encoder_count;

extern void MC_encoder_init(void);

