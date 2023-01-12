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

    seq_gpio_register_callback(0, 0, 1, sequencer_toggle_running);

    sleep_ms(3000);

    while(1) {
        seq_gpio_tick();
        //sequencer_tick();
    }

    return 0;
}
