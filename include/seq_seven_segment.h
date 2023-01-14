#ifndef SEQ_SEVEN_SEGMENT_H
#define SEQ_SEVEN_SEGMENT_H

#include <pico/stdlib.h>
#include <math.h>

#include "hardware/gpio.h"

#include "seq_globals.h"

void seq_seven_segment_init(void);
void seq_seven_segment_tick(void);
void seq_seven_segment_clear(void);

void seq_seven_segment_time(uint64_t us);
void seq_seven_segment_cycles(uint8_t cycles);

void seq_seven_segment_set(uint16_t data);
void seq_seven_segment_set_prefix(uint8_t prefix, uint16_t data);

#endif