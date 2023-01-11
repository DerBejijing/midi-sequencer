#ifndef SEQ_GPIO_H
#define SEQ_QPIO_H

#include <stdio.h>
#include <pico/stdlib.h>

#define INDEX_REVERSE(i) SEQ_STAGES - i

#define ADC_RANGE (1 << 12)
#define ADC_CONV_24 (24.0 / (ADC_RANGE - 1))

#define MATRIX_GND_0 18
#define MATRIX_GND_1 19
#define MATRIX_GND_2 20

#define MATRIX_PWR 27

#define MUX_CH_A 26
#define MUX_CH_B 22
#define MUX_CH_C 21

#define MAIN_MUX_CH_A 15
#define MAIN_MUX_CH_B 14
#define MAIN_MUX_CH_C 13

#define MAIN_MUX_READ 28

#define SETTINGS_BUTTONS 7
#define SETTINGS_VALUES 7


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


uint8_t sg_matrix_cycles;
uint8_t sg_debounce_cycles;

void (*sg_button_callbacks_state[SETTINGS_BUTTONS])() = {seq_gpio_callback_void};
void (*sg_button_callbacks_toggle[SETTINGS_BUTTONS])() = {seq_gpio_callback_void};



void seq_gpio_init(void) {
    sg_matrix_cycles = 1;
    sg_debounce_cycles = 1;

    gpio_init(MATRIX_GND_0);
    gpio_set_dir(MATRIX_GND_0, GPIO_OUT);
    gpio_init(MATRIX_GND_1);
    gpio_set_dir(MATRIX_GND_1, GPIO_OUT);
    gpio_init(MATRIX_GND_2);
    gpio_set_dir(MATRIX_GND_2, GPIO_OUT);

    gpio_init(MATRIX_PWR);
    gpio_set_dir(MATRIX_PWR, GPIO_OUT);

    gpio_init(MUX_CH_A);
    gpio_set_dir(MUX_CH_A, GPIO_OUT);
    gpio_init(MUX_CH_B);
    gpio_set_dir(MUX_CH_B, GPIO_OUT);
    gpio_init(MUX_CH_C);
    gpio_set_dir(MUX_CH_C, GPIO_OUT);

    gpio_init(MAIN_MUX_CH_A);
    gpio_set_dir(MAIN_MUX_CH_A, GPIO_OUT);
    gpio_init(MAIN_MUX_CH_A);
    gpio_set_dir(MAIN_MUX_CH_A, GPIO_OUT);
    gpio_init(MAIN_MUX_CH_A);
    gpio_set_dir(MAIN_MUX_CH_A, GPIO_OUT);

    adc_init();
    adc_gpio_init(MAIN_MUX_READ);
    adc_select_input(2);
}


void seq_gpio_tick(void) {

}


void seq_gpio_matrix_tick(void) {
    for(uint8_t cycle = 0; cycle < sg_matrix_cycles; ++cycle) {
        for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
            gpio_put(MATRIX_GND_0, row != 0);
            gpio_put(MATRIX_GND_1, row != 1);
            gpio_put(MATRIX_GND_2, row != 2);

            for(uint8_t i = 0; i < SEQ_STAGES; ++i) {
                gpio_put(MATRIX_PWR, 0);

                gpio_put(MUX_CH_A, i >> 0 & 1);
                gpio_put(MUX_CH_B, i >> 1 & 1);
                gpio_put(MUX_CH_C, i >> 2 & 1);

                if(0) gpio_put(MATRIX_PWR, 1);
            }
        }
    }
}

#endif