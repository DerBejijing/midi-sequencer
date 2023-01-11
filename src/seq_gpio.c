#ifndef SEQ_GPIO_H
#define SEQ_QPIO_H

#include "../include/seq_gpio.h"
#include <stdio.h>

#define INDEX_REVERSE(i) SEQ_STAGES - 1 - i

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

#define ADDR_ROW_0 0
#define ADDR_ROW_1 1
#define ADDR_ROW_2 5
#define ADDR_ROW_3 4
#define ADDR_SETTINGS 2
#define ADDR_BUTTONS 3

#define SETTINGS_BUTTONS 7
#define SETTINGS_VALUES 7


struct analog_reading {
    uint8_t value;              // last value that was certain
    uint8_t value_new;          // recently measured but invalidated value
    uint8_t error;              // how heavily the value is fluctuating
    uint8_t certainty;          // number of equal readings in series
};

struct analog_reading sg_values_stages[SEQ_ROWS * SEQ_STAGES];
struct analog_reading sg_values_settings[SETTINGS_BUTTONS];

uint8_t sg_matrix_cycles = 1;
uint8_t sg_debounce_cycles = 10;                                                            // change back

uint8_t sg_matrix[SEQ_ROWS] = {0};

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
    gpio_init(MAIN_MUX_CH_B);
    gpio_set_dir(MAIN_MUX_CH_B, GPIO_OUT);
    gpio_init(MAIN_MUX_CH_C);
    gpio_set_dir(MAIN_MUX_CH_C, GPIO_OUT);

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

            if(sg_matrix[row] != STAGE_NONE) {
                uint8_t stage = INDEX_REVERSE(sg_matrix[row]);

                gpio_put(MUX_CH_A, stage >> 0 & 1);
                gpio_put(MUX_CH_B, stage >> 1 & 1);
                gpio_put(MUX_CH_C, stage >> 2 & 1);

                gpio_put(MATRIX_PWR, 1);

                sleep_ms(2);

                gpio_put(MATRIX_PWR, 0);
            }
        }
    }
}


void seq_gpio_matrix_clear(void) {
    for(uint8_t i = 0; i < SEQ_ROWS; ++i) sg_matrix[i] = STAGE_NONE;
}


void seq_gpio_matrix_cycles(uint8_t cycles) {
    sg_matrix_cycles = cycles;
}


void seq_gpio_matrix_set(uint8_t row, uint8_t stage) {
    if(row < 0 || row > SEQ_ROWS) return;
    sg_matrix[row] = stage;
}


// "private"
uint8_t seq_gpio_debounce(uint8_t value_new, struct analog_reading* debounce_struct) {
    if(debounce_struct->value != value_new) {
        if(debounce_struct->value_new == value_new) {
            ++debounce_struct->certainty;
        
            if(debounce_struct->certainty == ANALOG_CERTAINTY_TARGET) {
                debounce_struct->value = debounce_struct->value_new;
                debounce_struct->certainty = 0;
            }

        } else {
            debounce_struct->value_new = value_new;
            debounce_struct->certainty = 0;
        }
    } else debounce_struct->certainty = 0;
    
    return debounce_struct->value;
}

uint8_t seq_gpio_read_button(uint8_t pin) {
    gpio_put(MAIN_MUX_CH_A, ADDR_BUTTONS >> 0 & 1);
    gpio_put(MAIN_MUX_CH_B, ADDR_BUTTONS >> 1 & 1);
    gpio_put(MAIN_MUX_CH_C, ADDR_BUTTONS >> 2 & 1);

    gpio_put(MUX_CH_A, pin >> 0 & 1);
    gpio_put(MUX_CH_B, pin >> 1 & 1);
    gpio_put(MUX_CH_C, pin >> 2 & 1);

    uint16_t readings_sum = 0;

    for(uint8_t cycle = 0; cycle < sg_debounce_cycles; ++cycle) {
        readings_sum += adc_read() * ADC_CONV_24;
    }

    return readings_sum / sg_debounce_cycles > (ADC_CONV_24 * ADC_RANGE / 2);
}



uint8_t seq_gpio_read_setting(uint8_t pin);
uint8_t seq_gpio_read_interface(uint8_t pin);
uint8_t seq_gpio_read_value(uint8_t row, uint8_t stage);
uint8_t seq_gpio_read_certainty(void); 

#endif