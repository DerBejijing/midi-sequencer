#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

/* global definitions here*/
#define SEQ_STAGES 8
#define SEQ_ROWS 3

#include "seq_gpio.h"
#include "sequencer.h"
#include "seven_segment.h"


int main() {
    stdio_init_all();


    return 0;
}
