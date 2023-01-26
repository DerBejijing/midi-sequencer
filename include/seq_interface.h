#ifndef SEQ_INTERFACE_H
#define SEQ_INTERFACE_H

#include "seq_globals.h"
#include "seq_gpio.h"
#include "seq_sequencer.h"

void seq_interface_init(void);
void seq_interface_tick(void);

void seq_interface_change_write(void);
void seq_interface_change_clear(void);

void seq_interface_mode_ratcheting(void);
void seq_interface_mode_duration(void);
void seq_interface_mode_gate(void);

void seq_interface_select_row_toggle_note(void);
void seq_interface_select_row_down(void);

void seq_interface_stage_select(uint8_t stage);
void seq_interface_stage_set(uint8_t value);

#endif