#include <stdio.h>
#include <pico/stdlib.h>

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