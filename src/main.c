#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "../include/seq_globals.h"
#include "../include/seq_gpio.h"
#include "../include/seq_sequencer.h"
#include "../include/seq_seven_segment.h"


int main() {
    stdio_init_all();
    seq_gpio_init();
    seq_seven_segment_init();
    sequencer_init();

    seq_gpio_register_callback(0, 0, 1, sequencer_toggle_running);
    seq_gpio_register_callback(0, 1, 1, sequencer_terminate);

    sleep_ms(3000);
    

    uint64_t last_refresh = 0;
    uint16_t bpm_choices[25] = {0};
    for(uint8_t i = 0; i < 25; ++i) bpm_choices[i] = 60 + 20 * i;


    while(1) {
        uint64_t current_time = time_us_64();

        seq_gpio_tick();
        seq_seven_segment_tick();
        sequencer_tick();


        if(current_time > last_refresh + VALUES_REFRESH_US) {
            last_refresh = current_time;

            for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
                for(uint8_t stage = 0; stage < SEQ_STAGES; ++stage) sequencer_set_value(row * SEQ_STAGES + stage, seq_gpio_read_value(row, stage));

                sequencer_set_bpm(bpm_choices[seq_gpio_read_setting(6)]);
                sequencer_set_stages(row, seq_gpio_read_setting(row));
            }
        }
    }

    return 0;
}
