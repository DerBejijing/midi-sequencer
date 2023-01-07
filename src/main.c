#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/timer.h"

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

int main() {

}