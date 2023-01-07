#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <stdio.h>
#include <pico/stdlib.h>

void sequencer_init(void);
void sequencer_tick(void);
void sequencer_set_bpm(uint16_t bpm);
void sequencer_set_stages(uint8_t row_id, uint8_t stages);

void sequencer_set_value(uint8_t index, uint8_t value);
void sequencer_set_duration(uint8_t index, uint8_t value);
void sequencer_set_ratchets(uint8_t index, uint8_t value);

void sequencer_event_add(uint8_t row, uint8_t stage);
void sequencer_event_clear(uint8_t stack_index);
void sequencer_event_play(uint8_t stack_index);
void sequencer_event_stop(uint8_t stack_index);
void sequencer_events_clear(void);

void sequencer_terminate(void);
void sequencer_toggle_joined(void);
void sequencer_toggle_running(void);
void sequencer_clear(void);

#endif