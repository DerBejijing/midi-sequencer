#include "../include/seq_sequencer.h"

#define MIDI_NOTE 0
#define MIDI_VALUE 1


// 
uint8_t seq_do_init = 1;

// variables to store data about serial operation
uint8_t seq_join = 0;                               // sequence joined or not
uint8_t seq_join_step = 0;                          // current step for joined sequence
uint8_t seq_join_length = 0;                        // length of joined sequence
uint8_t seq_join_type;                              // ?

// store last event of clock and ratchet
uint64_t seq_join_last_clock = 0;                   // last clock for joined sequence
uint64_t seq_join_last_ratchet = 0;                 // last ratchet for joined sequence


// stores if the sequence(s) should terminate
uint8_t seq_terminate = 0;

// stores if the sequencer is running
uint8_t seq_running = 0;


// speed of the sequence(s) is microseconds
uint64_t seq_us_per_beat = 500000;


// store all values for notes, durations and ratcheting
uint8_t seq_values[SEQ_STAGES * SEQ_ROWS] = {0};
uint8_t seq_durations[SEQ_STAGES * SEQ_ROWS] = {0}; // haha nice you cannot initialize it to 1 like this...
uint8_t seq_ratchets[SEQ_STAGES * SEQ_ROWS] = {0};


// struct to store all data about an independent row
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

// array to store all sequencer rows
struct seq_row seq_rows[SEQ_ROWS];

// struct to store all data about a midi event
struct midi_event {
    uint8_t type;               // note or control change
    uint8_t value;              // value
    uint8_t channel;            // channel
    uint8_t active;             // event currently running
    uint64_t start;             // start time
    uint64_t stop;              // stop time
};

// stack to store all midi events
// events will be added and removed dynamically
struct midi_event event_stack[SEQ_ROWS];


void sequencer_midi_play(uint8_t channel, uint8_t value, uint8_t type, uint64_t gate);
void sequencer_midi_update(void);


/* initialize the sequencer */
void sequencer_init(void) {
    uint64_t current_time = time_us_64();

    // set all variables that should be reset to default
    seq_terminate = 0;
    seq_join_step = 0;
    seq_join_last_clock = current_time;
    seq_join_last_ratchet = current_time;

    // reset all rows
    for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
        seq_rows[row].id = row;
        seq_rows[row].type = 0;
        seq_rows[row].stage = 0;
        seq_rows[row].stages = SEQ_STAGES;
        seq_rows[row].render = 1;
        seq_rows[row].last_clock = current_time;
        seq_rows[row].last_ratchet = current_time;
    }

    // things to be done only once
    if(seq_do_init) {
        // reset all values for ratchets and durations
        for(uint8_t i = 0; i < SEQ_ROWS * SEQ_STAGES; ++i) {
            seq_ratchets[i] = 1;
            seq_durations[i] = 1;
        }
        seq_do_init = 0;
    }
}


/* update the sequencer */
void sequencer_tick(void) {
    // if the sequencer is not running, do not do anything
    if(!seq_running) return;
    
    uint64_t current_time = time_us_64();
    
    // sequencer running in serial mode
    if(seq_join) {
        // store if the note should be played now
        uint8_t play_note = 0;


        // check if the note should be played
        // and if the current step should be incremented
        if(current_time >= seq_join_last_clock + seq_us_per_beat) {
            // reset time of last clock and ratchet
            seq_join_last_clock = current_time;
            seq_join_last_ratchet = current_time;

            // increment the stage
            ++seq_join_step;

            // reset stage if it has exceeded it's length
            if(seq_join_step >= seq_join_length) seq_join_step = 0;

            // if the sequence has just reset and should terminate, stop the sequencer
            if(seq_terminate) if(seq_join_step == 0) {
                sequencer_toggle_running();
                return;
            }

            // note should be played
            play_note = 1;
        }


        // store the note to be played
        uint8_t note = 0;

        // store the row of the note
        uint8_t play_row = 0;

        // store the stage of the note
        uint8_t play_stage = 0;


        // search for the desired stage
        uint8_t index = 0;
        for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
            for(uint8_t stage = 0; stage < seq_rows[row].stages; ++stage) {
                if(index == seq_join_step) {
                    // note was found, set values accordingly
                    play_row = row;
                    play_stage = stage; 
                    note = seq_values[row * SEQ_STAGES + stage];
                }
                ++index;
            }
        }


        // set LED matrix
        seq_gpio_matrix_clear();
        seq_gpio_matrix_set(play_row, play_stage);


        // calculate how many ratchets should happen on the current step
        // and how long the delay between the events needs to be
        uint8_t ratchets = seq_ratchets[SEQ_STAGES * play_row + play_stage];
        uint64_t ratchet_time = seq_us_per_beat / ratchets;


        // check if ratchet event should happen
        if(current_time >= seq_join_last_ratchet + ratchet_time) {
            // reset last time of ratcheting
            seq_join_last_ratchet = current_time;

            // note should be played
            play_note = 1;
        }

        // if note should be played, play it
        if(play_note) {
            printf("row [%d] stage [%d], start now: %d\n", play_row, play_stage, seq_values[play_row * SEQ_STAGES + play_stage]);
        }
    } 
    // sequencer running in parallel mode
    else {
        // iterate all rows and care about them separately
        for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
            // keep a pointer to the current row
            struct seq_row* current_row = &seq_rows[row];

            // store if note should be played
            uint8_t play_note = 0;

            // check if the note should be played
            // and if the current step should be incremented
            if(current_time >= current_row->last_clock + seq_us_per_beat) {
                // reset time of last clock and ratchet
                current_row->last_clock = current_time;
                current_row->last_ratchet = current_time;

                // continue only, if the current row is active
                if(current_row->active) {
                    // increment the stage
                    ++current_row->stage;

                    // reset stage if it has exceeded it's length
                    if(current_row->stage >= current_row->stages) current_row->stage = 0;

                    // set LED matrix
                    seq_gpio_matrix_set(row, current_row->stage);

                    // assume play note
                    play_note = 1;

                    // if the sequence has just reset and should terminate, stop the sequencer 
                    if(seq_terminate) if(current_row->stage == 0) {
                        current_row->active = 0;
                        seq_gpio_matrix_set(row, STAGE_NONE);

                        // row is dead, do not play note
                        play_note = 0;
                    }
                } else seq_gpio_matrix_set(row, STAGE_NONE);
            }

            // calculate how many ratchets should happen on the current step
            // and how long the delay between the events needs to be
            uint8_t ratchets = seq_ratchets[SEQ_STAGES * row + current_row->stage];
            uint64_t ratchet_time = seq_us_per_beat / ratchets;

            // check if ratchet event should happen
            if(current_time >= current_row->last_ratchet + ratchet_time) {
                // reset time of last ratcheting
                current_row->last_ratchet = current_time;

                // note should be played
                if(current_row->active) play_note = 1;
            }

            // if note should be played, play it
            if(play_note) {
                printf("row [%d] stage [%d], start now: %d\n", row, current_row->stage, seq_values[row * SEQ_STAGES + current_row->stage]);
            }
        }
    }

    // if the sequencer is running in parallel mode and should terminate,
    // check if all sequences have reached their end, and then stop the sequencer
    if(seq_terminate) if(!seq_join) {
        uint8_t run = 0;
        for(uint8_t row = 0; row < SEQ_ROWS; ++row) run += seq_rows[row].active;
        if(run == 0) sequencer_toggle_running();
    }
}


/* set speed of the sequencer in beats per minute
-> bpm: integer value representing the desired speed */
void sequencer_set_bpm(uint16_t bpm) {
    seq_us_per_beat = 1000000 * (60.0f / bpm);
}


/* set the stages for each row
-- if the sequence is about to terminate, it may no longer be resized
-> row_id: integer value representing the row
-> stages: integer value representing the number of stages */
void sequencer_set_stages(uint8_t row_id, uint8_t stages) {
    // when sequences terminate, their lengths may no longer be altered
    if(seq_terminate) return;

    seq_rows[row_id].stages = stages;
    if(seq_rows[row_id].stage >= stages) seq_rows[row_id].stage = 0;
    
    seq_join_length = 0;
    for(uint8_t i = 0; i < SEQ_ROWS; ++i) seq_join_length += seq_rows[i].stages;

    if(stages == 0) {
        seq_rows[row_id].active = 0;
        seq_rows[row_id].render = 0;
    } else {
        seq_rows[row_id].active = 1;
        
        if(seq_join) return;

        seq_rows[row_id].render = 1;
    }
}


/* set the value for a given stage 
-> index: integer value representing the desired stage
-> value: integer value */
void sequencer_set_value(uint8_t index, uint8_t value) {
    seq_values[index] = value;
}


/* set the duration for a given stage
-> index: integer value representing the desired stage
-> value: integer value as multiplier of default BPM */
void sequencer_set_duration(uint8_t index, uint8_t value) {
    seq_durations[index] = value;
}


/* set the number of ratchets for a given stage 
-> index: integer value representing the desired stage
-> value: integer value specifying how often the note will be triggered */
void sequencer_set_ratchets(uint8_t index, uint8_t value) {
    seq_ratchets[index] = value;
}


/* call when starting the sequencer to play the first stage(s) */
void sequencer_process_initial(void) {
    if(seq_join) {
        for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
            if(seq_rows[row].active) {
                printf("row [%d] stage [%d], start now: %d\n", row, 0, seq_values[row * SEQ_STAGES]);
                return;
            }
        }
    } else {
        for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
            if(seq_rows[row].active) {
                printf("row [%d] stage [%d], start now: %d\n", row, seq_rows[row].stage, seq_values[row * SEQ_STAGES + seq_rows[row].stage]);
            }
        }
    }
}


/* terminate the sequencer */
void sequencer_terminate(void) {
    seq_terminate = 1;
}


/* toggle between serial and parallel mode */
void sequencer_toggle_joined(void) {
    seq_join =! seq_join;
    seq_running = 0;
    
    // set indicator LED
    seq_gpio_indicator_joined(seq_join);
    
    // reset the sequencer
    sequencer_init();
}


/* toggle running */
void sequencer_toggle_running(void) {
    seq_running =! seq_running;
    
    // if now running, reset it before
    if(seq_running) {
        sequencer_init();
        sequencer_process_initial();
    }
}


/* play a midi value */
void sequencer_midi_play(uint8_t channel, uint8_t value, uint8_t type, uint64_t gate) {
    /*struct midi_event {
    uint8_t type;               // note or control change
    uint8_t value;              // value
    uint8_t channel;            // channel
    uint8_t active;             // event currently running
    uint64_t start;             // start time
    uint64_t stop;              // stop time
    };*/

    uint64_t current_time = time_us_64();

    struct midi_event* midi_current = &event_stack[channel];

    // check if there is still an event, because the speed has been changed, if so cancel it

    midi_current->channel = channel;
    midi_current->value = value;
    midi_current->type = type;
    midi_current->start = current_time;
    midi_current->stop = current_time + gate;
    midi_current->active = 1;

    if(type == MIDI_NOTE) {
        // play midi note
        return;
    }

    // issue a control change
}


/* update the midi stack, check play time of values */
void sequencer_midi_update(void) {

}