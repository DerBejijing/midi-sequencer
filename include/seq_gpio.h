#ifndef SEQ_GPIO
#define SEQ_GPIO

#include <pico/stdlib.h>

#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/timer.h"

#include "seq_globals.h"
#include "seq_seven_segment.h"

void seq_gpio_init(void);
void seq_gpio_tick(void);

void seq_gpio_matrix_tick(void);
void seq_gpio_matrix_clear(void);
void seq_gpio_matrix_cycles(uint8_t cycles);
void seq_gpio_matrix_set(uint8_t row, uint8_t stage);

void seq_gpio_debounce_cycles(uint8_t cycles);

uint8_t seq_gpio_read_button(uint8_t pin);
uint8_t seq_gpio_read_setting(uint8_t pin);
uint8_t seq_gpio_read_interface(uint8_t pin);
uint8_t seq_gpio_read_value(uint8_t row, uint8_t stage);

void seq_gpio_tick_values(void);
void seq_gpio_tick_buttons(void);
void seq_gpio_tick_settings(void);

void seq_gpio_register_callback(uint8_t is_interface, uint8_t button, uint8_t momentary, void (*callback)());
void seq_gpio_process_callback(uint8_t is_interface, uint8_t button);
void seq_gpio_callback_void(void);


#endif