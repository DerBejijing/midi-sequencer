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



uint8_t seq_join;
uint8_t seq_join_step;
uint8_t seq_join_length;
uint8_t seq_join_type;

uint64_t seq_join_last_clock;
uint64_t seq_join_last_ratchet;
uint64_t seq_us_per_beat;

uint8_t seq_terminate;
uint8_t seq_running;

uint8_t seq_values[SEQ_STAGES * SEQ_ROWS];
uint8_t seq_durations[SEQ_STAGES * SEQ_ROWS];
uint8_t seq_ratchets[SEQ_STAGES * SEQ_ROWS];

struct seq_row {
    uint8_t id;
    uint8_t type;
    uint8_t stage;
    uint8_t stages;
    uint8_t active;
    uint8_t render;
    uint64_t last_clock;
    uint64_t last_ratchet;
};

struct seq_row seq_rows[SEQ_ROWS];

struct midi_event {
    uint8_t type;
    uint8_t value;
    uint8_t channel;
    uint8_t active;
    uint64_t start;
    uint64_t stop;
};

struct midi_event event_stack[SEQ_ROWS];

#endif