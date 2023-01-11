#ifndef SEQ_GPIO
#define SEQ_GPIO

#include <pico/stdlib.h>

#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/timer.h"

#include "seq_globals.h"

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

void seq_gpio_callback_state(uint8_t button, void (*callback)());
void seq_gpio_callback_toggle(uint8_t button, void (*callback)());
void seq_gpio_callback_void(void);

#endif