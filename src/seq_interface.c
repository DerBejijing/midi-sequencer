#include "../include/seq_interface.h"

// currently selected stage
uint8_t seq_ui_stage = 0;

// currently selected value
uint8_t seq_ui_value = 0;

// current mode
uint8_t seq_ui_mode = 0;



/* initialize the interface */
void seq_interface_init(void) {
    seq_gpio_register_callback(1, 0, 0, seq_interface_mode_ratcheting);
    seq_gpio_register_callback(1, 1, 0, seq_interface_mode_duration);
    seq_gpio_register_callback(1, 2, 0, seq_interface_mode_gate);
    seq_gpio_register_callback(1, 6, 0, seq_interface_select_row_toggle_note);
    seq_gpio_register_callback(1, 7, 0, seq_interface_select_row_down);

    seq_gpio_register_callback(0, 4, 1, seq_interface_change_clear);
    seq_gpio_register_callback(0, 5, 1, seq_interface_change_write);
}


/* initialize the interface */
void seq_interface_tick(void) {

}


/* write the currently chosen value to the currently chosen stage*/
void seq_interface_change_write(void) {
    if(seq_ui_mode == 0) sequencer_set_ratchets(seq_ui_stage, seq_ui_value);
    if(seq_ui_mode == 1) sequencer_set_duration(seq_ui_stage, seq_ui_value);
}


/* hmm idk what this should have done */
void seq_interface_change_clear(void) {
    printf("clear\n");
}


/* edit step ratcheting */
void seq_interface_mode_ratcheting(void) {
    seq_ui_mode = 0;
    printf("ratchet\n");
}


/* edit step durations */
void seq_interface_mode_duration(void) {
    seq_ui_mode = 1;
    printf("duration\n");
}


/* edit step gate lengths */
void seq_interface_mode_gate(void) {
    seq_ui_mode = 2;
    printf("gate\n");
}


/* select a stage to edit
stage: integer value representing a stage */
void seq_interface_stage_select(uint8_t stage) {
    seq_seven_segment_set(stage);
    if(stage < 0 || stage >= SEQ_ROWS * SEQ_STAGES) return;
    seq_ui_stage = stage;
}


/* select a value
value: integer value */
void seq_interface_stage_set(uint8_t value) {
    seq_seven_segment_set(value);
    seq_ui_value = value;
}