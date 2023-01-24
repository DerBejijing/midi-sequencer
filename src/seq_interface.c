#include "../include/seq_interface.h"

void seq_interface_init(void) {
    seq_gpio_register_callback(1, 0, 0, seq_interface_mode_ratcheting);
    seq_gpio_register_callback(1, 1, 0, seq_interface_mode_duration);
    seq_gpio_register_callback(1, 2, 0, seq_interface_mode_gate);
    seq_gpio_register_callback(1, 6, 0, seq_interface_select_row_toggle_note);
    seq_gpio_register_callback(1, 7, 0, seq_interface_select_row_down);

    seq_gpio_register_callback(0, 4, 1, seq_interface_change_clear);
    seq_gpio_register_callback(0, 5, 1, seq_interface_change_write);
}


void seq_interface_tick(void) {

}


void seq_interface_change_write(void) {
    printf("write\n");
}


void seq_interface_change_clear(void) {
    printf("clear\n");
}


void seq_interface_mode_ratcheting(void) {
    printf("ratchet\n");
}


void seq_interface_mode_duration(void) {
    printf("duration\n");
}


void seq_interface_mode_gate(void) {
    printf("gate\n");
}


void seq_interface_select_row_toggle_note(void) {
    printf("toggle note\n");
}


void seq_interface_select_row_down(void) {
    printf("down\n");
}