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

    seq_gpio_init();
    seq_interface_init();
    seq_seven_segment_init();
    sequencer_init();

    seq_gpio_register_callback(0, 0, 1, sequencer_toggle_running);
    seq_gpio_register_callback(0, 6, 0, sequencer_toggle_joined);
    seq_gpio_register_callback(0, 1, 1, sequencer_terminate);

    seq_seven_segment_time(500000);

    sleep_ms(2000);

    seq_seven_segment_set(1337);

    uint64_t last_refresh_v = 0;
    uint64_t last_refresh_s = 0;
    uint16_t bpm_choices[25] = {0};
    for(uint8_t i = 0; i < 25; ++i) bpm_choices[i] = 60 + 20 * i;


    while(1) {
        uint64_t current_time = time_us_64();

        seq_gpio_tick();
        sequencer_tick();

        if(current_time > last_refresh_v + REFRESH_VALUES_US) {
            last_refresh_v = current_time;

            seq_interface_tick();

            for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
                for(uint8_t stage = 0; stage < SEQ_STAGES; ++stage) sequencer_set_value(row * SEQ_STAGES + stage, seq_gpio_read_value(row, stage));

                sequencer_set_bpm(bpm_choices[seq_gpio_read_setting(6)]);
                sequencer_set_stages(row, seq_gpio_read_setting(row));

                seq_interface_stage_select(seq_gpio_read_setting(3));
                seq_interface_stage_set(seq_gpio_read_setting(4));
            }
        }

        if(current_time > last_refresh_s + REFRESH_SEVEN_SEGMENT_US) {
            last_refresh_s = current_time;
            seq_seven_segment_tick();
        }
    }

    return 0;
}
