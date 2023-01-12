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
    uint64_t current_time = time_us_64();

    seq_terminate = 0;
    seq_join_step = 0;
    seq_join_last_clock = 0;

    for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
        seq_rows[row].id = row;
        seq_rows[row].type = 0;
        seq_rows[row].stage = 0;
        seq_rows[row].stages = SEQ_STAGES;
        seq_rows[row].active = 1;
        seq_rows[row].render = 1;
        seq_rows[row].last_clock = current_time;
        seq_rows[row].last_ratchet = current_time;
    }
}


void sequencer_tick(void) {
    if(!seq_running) return;

    uint64_t current_time = time_us_64();

    if(seq_join) {
        return;
    }

    for(uint8_t row = 0; row < 1; ++row) {
        if(current_time >= seq_rows[row].last_clock + seq_us_per_beat) {
            struct seq_row* current_row = &seq_rows[row];

            current_row->last_clock = current_time;

            if(current_row->active) {
                ++current_row->stage;

                if(current_row->stage >= current_row->stages) {
                    current_row->stage = 0;
                }

                seq_gpio_matrix_set(row, current_row->stage);

                if(seq_terminate) if(current_row->stage == 0) {
                    current_row->active = 0;
                    return;
                }

                // play new note here
                printf("row [%d] stage [%d], start now: %d\n", row, current_row->stage, seq_values[row * SEQ_STAGES + current_row->stage]);
            } else seq_gpio_matrix_set(row, STAGE_NONE);
        }
    }

    if(seq_terminate) {
        uint8_t run = 0;
        for(uint8_t row = 0; row < SEQ_ROWS; ++row) run += seq_rows[row].active;
        if(run == 0) sequencer_toggle_running();
    }
}


void sequencer_set_stages(uint8_t row_id, uint8_t stages) {
    seq_rows[row_id].stages = stages;
    if(seq_rows[row_id].stage >= stages) seq_rows[row_id].stage = 0;
    
    if(stages == 0) {
        seq_rows[row_id].active = 0;
        seq_rows[row_id].render = 0;
    } else {
        if(seq_join) return;

        seq_rows[row_id].active = 1;
        seq_rows[row_id].render = 1;
    }
}


void sequencer_set_value(uint8_t index, uint8_t value) {
    seq_values[index] = value;
}


// "private"
void sequencer_process_initial(void) {
    if(seq_join) return;

    for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
        if(seq_rows[row].active) {

            // play new note here
            printf("row [%d] stage [%d], start now: %d\n", row, seq_rows[row].stage, seq_values[row * SEQ_STAGES + seq_rows[row].stage]);
        }
    }
}


void sequencer_toggle_running(void) {
    seq_running =! seq_running;
    if(seq_running) {
        sequencer_init();
        sequencer_process_initial();
    }
}


void sequencer_terminate(void) {
    seq_terminate = 1;
}