#ifndef SEQ_GPIO_H
#define SEQ_QPIO_H

#include <stdio.h>
#include <pico/stdlib.h>


void seq_gpio_init(void);
void seq_gpio_tick(void);

void seq_gpio_matrix_tick(void);
void seq_gpio_matrix_clear(void);
void seq_gpio_matrix_cycles(uint8_t cycles);
void seq_gpio_matrix_set(uint8_t row, uint8_t stage);

void seq_gpio_debounce_cycles(uint8_t cycles)

uint8_t seq_gpio_read_button(uint8_t pin);
uint8_t seq_gpio_read_setting(uint8_t pin);
uint8_t seq_gpio_read_interface(uint8_t pin);
uint8_t seq_gpio_read_value(uint8_t row, uint8_t stage);


#endif