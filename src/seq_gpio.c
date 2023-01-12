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

struct callback {
    void (*callback)();
    uint8_t is_interface;
    uint8_t button;
    uint8_t momentary;
    uint8_t state;
};

struct analog_reading sg_values_stages[SEQ_ROWS * SEQ_STAGES];
struct analog_reading sg_values_settings[SETTINGS_BUTTONS];

struct callback sg_button_callback[SEQ_STAGES + SETTINGS_BUTTONS];

uint8_t sg_values_interface[SEQ_STAGES];
uint8_t sg_values_buttons[SETTINGS_BUTTONS];
uint8_t sg_matrix[SEQ_ROWS] = {0};

uint8_t sg_matrix_cycles = 1;
uint8_t sg_debounce_cycles = 1;

uint64_t sg_buttons_read_delay = 1000;
uint64_t sg_settings_read_delay = 2000;
uint64_t sg_values_read_delay = 1000;

uint64_t sg_buttons_read_last = 0;
uint64_t sg_settings_read_last = 0;
uint64_t sg_values_read_last = 0;



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

    // initialize callback pointer
    for(uint8_t i = 0; i < SEQ_STAGES + SETTINGS_BUTTONS; ++i) sg_button_callback[i].callback = seq_gpio_callback_void;
    
    // initialize values for switches
    gpio_put(MAIN_MUX_CH_A, ADDR_ROW_3 >> 0 & 1);
    gpio_put(MAIN_MUX_CH_B, ADDR_ROW_3 >> 1 & 1);
    gpio_put(MAIN_MUX_CH_C, ADDR_ROW_3 >> 2 & 1);

    for(uint8_t i = 0; i < SEQ_STAGES; ++i) {
        gpio_put(MUX_CH_A, i >> 0 & 1);
        gpio_put(MUX_CH_B, i >> 1 & 1);
        gpio_put(MUX_CH_C, i >> 2 & 1);

        sg_values_interface[i] = adc_read() > (ADC_CONV_24 * ADC_RANGE / 2);
    }
}


void seq_gpio_tick(void) {
    uint64_t current_time = time_us_64();

    if(current_time > sg_buttons_read_last + sg_buttons_read_delay) {
        sg_buttons_read_last = current_time;
        seq_gpio_tick_buttons();
    }
    if(current_time > sg_settings_read_last + sg_settings_read_delay) {
        sg_settings_read_last = current_time;
        seq_gpio_tick_settings();
    }
    if(current_time > sg_values_read_last + sg_values_read_delay) {
        sg_values_read_last = current_time;
        seq_gpio_tick_values();
    }

    seq_gpio_matrix_tick();
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


void seq_gpio_debounce_cycles(uint8_t cycles) {
    sg_debounce_cycles = cycles;
}


// "private"
uint8_t seq_gpio_debounce(uint8_t read, struct analog_reading* debounce_struct) {
    if(debounce_struct->value != read) {
        if(debounce_struct->value_new == read) {
            ++debounce_struct->certainty;
        
            if(debounce_struct->certainty == ANALOG_CERTAINTY_TARGET) {
                debounce_struct->value = debounce_struct->value_new;
                debounce_struct->certainty = 0;
            }

        } else {
            debounce_struct->value_new = read;
            debounce_struct->certainty = 0;
        }
    } else debounce_struct->certainty = 0;
    
    return debounce_struct->value;
}


uint8_t seq_gpio_read_button(uint8_t pin) {
    return sg_values_buttons[pin];
}


uint8_t seq_gpio_read_setting(uint8_t pin) {
    return sg_values_settings[pin].value;
}


uint8_t seq_gpio_read_interface(uint8_t pin) {
    return sg_values_interface[pin];
}


uint8_t seq_gpio_read_value(uint8_t row, uint8_t stage) {
    return sg_values_stages[row * SEQ_STAGES + stage].value;
}


void seq_gpio_tick_values(void) {
    gpio_put(MAIN_MUX_CH_A, ADDR_ROW_0 >> 0 & 1);
    gpio_put(MAIN_MUX_CH_B, ADDR_ROW_0 >> 1 & 1);
    gpio_put(MAIN_MUX_CH_C, ADDR_ROW_0 >> 2 & 1);

    for(uint8_t i = 0; i < SEQ_STAGES; ++i) {
        gpio_put(MUX_CH_A, i >> 0 & 1);
        gpio_put(MUX_CH_B, i >> 1 & 1);
        gpio_put(MUX_CH_C, i >> 2 & 1);

        seq_gpio_debounce(adc_read() * ADC_CONV_24, &sg_values_stages[0 * SEQ_STAGES + INDEX_REVERSE(i)]);
    }

    gpio_put(MAIN_MUX_CH_A, ADDR_ROW_1 >> 0 & 1);
    gpio_put(MAIN_MUX_CH_B, ADDR_ROW_1 >> 1 & 1);
    gpio_put(MAIN_MUX_CH_C, ADDR_ROW_1 >> 2 & 1);

    for(uint8_t i = 0; i < SEQ_STAGES; ++i) {
        gpio_put(MUX_CH_A, i >> 0 & 1);
        gpio_put(MUX_CH_B, i >> 1 & 1);
        gpio_put(MUX_CH_C, i >> 2 & 1);

        seq_gpio_debounce(adc_read() * ADC_CONV_24, &sg_values_stages[1 * SEQ_STAGES + INDEX_REVERSE(i)]);
    }

    gpio_put(MAIN_MUX_CH_A, ADDR_ROW_2 >> 0 & 1);
    gpio_put(MAIN_MUX_CH_B, ADDR_ROW_2 >> 1 & 1);
    gpio_put(MAIN_MUX_CH_C, ADDR_ROW_2 >> 2 & 1);

    for(uint8_t i = 0; i < SEQ_STAGES; ++i) {
        gpio_put(MUX_CH_A, i >> 0 & 1);
        gpio_put(MUX_CH_B, i >> 1 & 1);
        gpio_put(MUX_CH_C, i >> 2 & 1);

        // No, this is the only row I did not mess up when wiring ._.
        // So no reverse here
        seq_gpio_debounce(adc_read() * ADC_CONV_24, &sg_values_stages[2 * SEQ_STAGES + i]);
    }
}


void seq_gpio_tick_buttons(void) {
    gpio_put(MAIN_MUX_CH_A, ADDR_ROW_3 >> 0 & 1);
    gpio_put(MAIN_MUX_CH_B, ADDR_ROW_3 >> 1 & 1);
    gpio_put(MAIN_MUX_CH_C, ADDR_ROW_3 >> 2 & 1);

    for(uint8_t i = 0; i < SEQ_STAGES; ++i) {
        gpio_put(MUX_CH_A, i >> 0 & 1);
        gpio_put(MUX_CH_B, i >> 1 & 1);
        gpio_put(MUX_CH_C, i >> 2 & 1);

        uint16_t readings_sum = 0;

        for(uint8_t cycle = 0; cycle < sg_debounce_cycles; ++cycle) {
            readings_sum += adc_read() * ADC_CONV_24;
        }

        uint8_t value_new = readings_sum / sg_debounce_cycles > (ADC_CONV_24 * ADC_RANGE / 2);
        if(sg_values_interface[i] != value_new) seq_gpio_process_callback(1, i);
        sg_values_interface[i] = value_new;
    }
    
    gpio_put(MAIN_MUX_CH_A, ADDR_BUTTONS >> 0 & 1);
    gpio_put(MAIN_MUX_CH_B, ADDR_BUTTONS >> 1 & 1);
    gpio_put(MAIN_MUX_CH_C, ADDR_BUTTONS >> 2 & 1);

    for(uint8_t i = 0; i < SETTINGS_BUTTONS; ++i) {
        gpio_put(MUX_CH_A, i >> 0 & 1);
        gpio_put(MUX_CH_B, i >> 1 & 1);
        gpio_put(MUX_CH_C, i >> 2 & 1);

        uint16_t readings_sum = 0;

        for(uint8_t cycle = 0; cycle < sg_debounce_cycles; ++cycle) {
            readings_sum += adc_read() * ADC_CONV_24;
        }

        uint8_t value_new = readings_sum / sg_debounce_cycles > (ADC_CONV_24 * ADC_RANGE / 2);
        if(sg_values_buttons[i] != value_new) seq_gpio_process_callback(0, i);
        sg_values_buttons[i] = value_new;
    }
    
}


void seq_gpio_tick_settings(void) {
    gpio_put(MAIN_MUX_CH_A, ADDR_SETTINGS >> 0 & 1);
    gpio_put(MAIN_MUX_CH_B, ADDR_SETTINGS >> 1 & 1);
    gpio_put(MAIN_MUX_CH_C, ADDR_SETTINGS >> 2 & 1);

    for(uint8_t i = 0; i < SETTINGS_VALUES; ++i) {
        gpio_put(MUX_CH_A, i >> 0 & 1);
        gpio_put(MUX_CH_B, i >> 1 & 1);
        gpio_put(MUX_CH_C, i >> 2 & 1);

        seq_gpio_debounce(adc_read() * ADC_CONV_24, &sg_values_settings[i]);
    }
}


void seq_gpio_register_callback(uint8_t is_interface, uint8_t button, uint8_t momentary, void (*callback)()) {
    uint8_t index = is_interface * SEQ_ROWS + button;

    sg_button_callback[index].callback = callback;
    sg_button_callback[index].is_interface = is_interface;
    sg_button_callback[index].button = button;
    sg_button_callback[index].momentary = momentary;
    sg_button_callback[index].state = 0;
}


void seq_gpio_process_callback(uint8_t is_interface, uint8_t button) {
    uint8_t index = is_interface * (SEQ_STAGES - 1) + button;

    sg_button_callback[index].state =! sg_button_callback[index].state;

    if(sg_button_callback[index].momentary) {
        if(sg_button_callback[index].state) sg_button_callback[index].callback();
        return;
    }
    sg_button_callback[index].callback();                                           // apparently called as soon as seq starts -> initialize buttons
}


void seq_gpio_callback_void(void) {
    printf("void\n");
    return;
}
