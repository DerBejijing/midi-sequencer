#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "../include/seq_globals.h"
#include "../include/seq_gpio.h"
#include "../include/seq_sequencer.h"


int main() {
    stdio_init_all();
    seq_gpio_init();
    sequencer_init();

    seq_gpio_register_callback(0, 0, 1, sequencer_toggle_running);
    seq_gpio_register_callback(0, 1, 1, sequencer_terminate);

    sleep_ms(3000);

    uint64_t last_refresh = 0;

    while(1) {
        uint64_t current_time = time_us_64();

        seq_gpio_tick();
        sequencer_tick();

        if(current_time > last_refresh + VALUES_REFRESH_US) {
            last_refresh = current_time;

            for(uint8_t row = 0; row < SEQ_ROWS; ++row) {
                for(uint8_t stage = 0; stage < SEQ_STAGES; ++stage) sequencer_set_value(row * SEQ_STAGES + stage, seq_gpio_read_value(row, stage));

                //printf("%d: %d\n", row, seq_gpio_read_setting(row));
                sequencer_set_stages(row, seq_gpio_read_setting(row));
            }
        }
    }

    return 0;
}
