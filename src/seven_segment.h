#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H

#include <stdio.h>
#include <pico/stdlib.h>

void seven_segment_init(void);
void seven_segment_tick(void);
void seven_segment_clear(void);

void seven_segment_time(uint64_t us);
void seven_segment_cycles(uint8_t cycles);

void seven_segment_set(uint8_t data, uint8_t dot_pos);

#endif