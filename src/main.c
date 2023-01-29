#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "../include/seq_globals.h"
#include "../include/seq_gpio.h"
#include "../include/seq_interface.h"
#include "../include/seq_sequencer.h"
#include "../include/seq_seven_segment.h"


int main() {
    stdio_init_all();

    sleep_ms(2000);

    // initialize all modules
    seq_gpio_init();
    seq_interface_init();
    seq_seven_segment_init();
    sequencer_init();

    // register callbacks
    seq_gpio_register_callback(0, 0, 1, sequencer_toggle_running);
    seq_gpio_register_callback(0, 6, 0, sequencer_toggle_joined);
    seq_gpio_register_callback(0, 1, 1, sequencer_terminate);

    // set illumination time of 7 segment display
    seq_seven_segment_time(500000);

    sleep_ms(2000);

    // store time of last refresh event
    uint64_t last_refresh_v = 0;
    uint64_t last_refresh_s = 0;

    // create a list of all available BPM choices
    uint16_t bpm_choices[25] = {0};
    for(uint8_t i = 0; i < 25; ++i) bpm_choices[i] = 60 + 20 * i;

    // create a list of all available BPM choices with fine tuning
    uint16_t bpm_choices_list[25][25] = {0};
    for(uint8_t x = 0; x < 25; ++x) for(uint8_t y = 0; y < 25; ++y) bpm_choices_list[x][y] = (x * 60) + (y * 5);


    while(1) {
        uint64_t current_time = time_us_64();

        // tick all essential modules
        seq_gpio_tick();
        sequencer_tick();

        // refresh all analog values
        if(current_time > last_refresh_v + REFRESH_VALUES_US) {
            last_refresh_v = current_time;

            // update interface
            seq_interface_tick();
            seq_interface_stage_select(seq_gpio_read_setting(3));
            seq_interface_stage_set(seq_gpio_read_setting(4));

            // set bpm
            sequencer_set_bpm(bpm_choices[seq_gpio_read_setting(6)]);

            // update all values specific for a row/step
            for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
                for(uint8_t stage = 0; stage < SEQ_STAGES; ++stage) sequencer_set_value(row * SEQ_STAGES + stage, seq_gpio_read_value(row, stage));
                sequencer_set_stages(row, seq_gpio_read_setting(row));
            }
        }

        // update seven segment display
        if(current_time > last_refresh_s + REFRESH_SEVEN_SEGMENT_US) {
            last_refresh_s = current_time;
            seq_seven_segment_tick();
        }
    }

    return 0;
}
