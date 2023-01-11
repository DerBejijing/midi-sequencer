#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "../include/seq_globals.h"
#include "../include/seq_gpio.h"
//#include "../include/seq_sequencer.h"
//#include "../include/seq_seven_segment.h"


int main() {
    stdio_init_all();

    seq_gpio_init();

    seq_gpio_matrix_set(0, 0);
    seq_gpio_matrix_set(1, 1);
    seq_gpio_matrix_set(2, 2);

    while(1) seq_gpio_matrix_tick();

    return 0;
}
