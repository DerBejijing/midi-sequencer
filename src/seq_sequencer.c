#include "../include/seq_sequencer.h"

uint8_t seq_join = 0;                               // sequence joined or not
uint8_t seq_join_step = 0;                          // current step for joined sequence
uint8_t seq_join_length = 0;                        // length of joined sequence
uint8_t seq_join_type;                              // ?

uint8_t seq_terminate = 0;
uint8_t seq_running = 0;

uint64_t seq_join_last_clock = 0;                   // last clock for joined sequence
uint64_t seq_join_last_ratchet = 0;                 // last ratchet for joined sequence

uint64_t seq_us_per_beat = 500000;                 // speed of the sequence(s)

uint8_t seq_values[SEQ_STAGES * SEQ_ROWS];
uint8_t seq_durations[SEQ_STAGES * SEQ_ROWS];
uint8_t seq_ratchets[SEQ_STAGES * SEQ_ROWS];

struct seq_row {
    uint8_t id;                 // aka row
    uint8_t type;               // note or control change
    uint8_t stage;              // current step
    uint8_t stages;             // number of steps
    uint8_t active;             // active or not
    uint8_t render;             // light led or not (obsolete??)
    uint64_t last_clock;        // last clock
    uint64_t last_ratchet;      // last ratchet
};

struct seq_row seq_rows[SEQ_ROWS];

struct midi_event {
    uint8_t type;               // note or control change
    uint8_t value;              // value
    uint8_t channel;            // channel
    uint8_t active;             // event currently running
    uint64_t start;             // start time
    uint64_t stop;              // stop time
};

struct midi_event event_stack[SEQ_ROWS];


void sequencer_init(void) {
    for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
        seq_rows[row].id = row;
        seq_rows[row].type = 0;
        seq_rows[row].stage = 0;
        seq_rows[row].stages = SEQ_STAGES;
        seq_rows[row].active = 1;
        seq_rows[row].render = 1;
        seq_rows[row].last_clock = 0;
        seq_rows[row].last_ratchet = 0;
    }
}


void sequencer_tick(void) {
    if(!seq_running) return;

    uint64_t current_time = time_us_64();

    if(seq_join) {
        return;
    }

    for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
        if(current_time >= seq_rows[row].last_clock + seq_us_per_beat) {
            struct seq_row* current_row = &seq_rows[row];

            current_row->last_clock = current_time;

            if(current_row->active) {
                ++current_row->stage;

                if(current_row->stage >= current_row->stages) {
                    current_row->stage = 0;
                }

                if(seq_terminate) if(current_row->stage == 0) {
                    current_row->active = 0;
                    return;
                }

                // play new note
                // seq_values[row * SEQ_STAGES + current_row->stage]
                printf("row [%d] stage [%d], start now: %d\n", row, current_row->stage, seq_values[row * SEQ_STAGES + current_row->stage]);
            }
        }
    }
}


void sequencer_toggle_running(void) {
    seq_running =! seq_running;
    printf("running: %d\n", seq_running);
    //if(seq_running) sequencer_init();
}


/*
notes

on every clock, the sequencer plays the next note
-> when starting the first note will be skipped!
-> play first note on activation of the sequencer!!!
*/