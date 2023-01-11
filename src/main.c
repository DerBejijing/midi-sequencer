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

    while(1) {
        seq_gpio_tick();
        
        for(uint8_t r = 0; r < SEQ_ROWS; ++r) {
            for(uint8_t i = 0; i < SEQ_STAGES; ++i) printf("%d ", seq_gpio_read_value(r, i));
            printf("\n");
        }
        printf("----------------------------------------\n");
    }

    return 0;
}
