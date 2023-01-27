#include "../include/seq_seven_segment.h"

// how many digits the display has
#define SEGMENTS_COUNT 4

// value to represent "no digit"
#define SEGMENT_NONE 10

// all ground connections
#define D0_GND 5
#define D1_GND 4
#define D2_GND 3
#define D3_GND 2

// all segment connections
#define SEG_A 12
#define SEG_B 11
#define SEG_C 10
#define SEG_D 9
#define SEG_E 8
#define SEG_F 7
#define SEG_G 6


// struct to store the data of a single digit
struct ss_digit {
	uint8_t render;
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t f;
	uint8_t g;
};

// array to store all digits
struct ss_digit ss_digits[SEGMENTS_COUNT];


// how many times the display should be re-rendered
uint64_t ss_cycles = 1;

// store time of when illumination has started and how long it should last
uint64_t ss_illumination_started = 0;
uint64_t ss_illumination_time = 10000000;



/* initialize the display */
void seq_seven_segment_init(void) {
    gpio_init(D0_GND);
	gpio_set_dir(D0_GND, GPIO_OUT);
	gpio_init(D1_GND);
	gpio_set_dir(D1_GND, GPIO_OUT);
	gpio_init(D2_GND);
	gpio_set_dir(D2_GND, GPIO_OUT);
	gpio_init(D3_GND);
	gpio_set_dir(D3_GND, GPIO_OUT);

	gpio_init(SEG_A);
	gpio_set_dir(SEG_A, GPIO_OUT);
	gpio_init(SEG_B);
	gpio_set_dir(SEG_B, GPIO_OUT);
	gpio_init(SEG_C);
	gpio_set_dir(SEG_C, GPIO_OUT);
	gpio_init(SEG_D);
	gpio_set_dir(SEG_D, GPIO_OUT);
	gpio_init(SEG_E);
	gpio_set_dir(SEG_E, GPIO_OUT);
	gpio_init(SEG_F);
	gpio_set_dir(SEG_F, GPIO_OUT);
	gpio_init(SEG_G);
	gpio_set_dir(SEG_G, GPIO_OUT);
}


/* update the display */
void seq_seven_segment_tick(void) {
	// check if it should still be rendered
    if(time_us_64() < ss_illumination_started + ss_illumination_time) {

		// redo as long as is desired
        for(uint8_t cycle = 0; cycle < ss_cycles; ++cycle) {
            for(uint8_t i = 0; i < SEGMENTS_COUNT; ++i) {

				// only render, if it should be
                if(ss_digits[i].render) {

                    gpio_put(SEG_A, ss_digits[i].a);
                    gpio_put(SEG_B, ss_digits[i].b);
                    gpio_put(SEG_C, ss_digits[i].c);
                    gpio_put(SEG_D, ss_digits[i].d);
                    gpio_put(SEG_E, ss_digits[i].e);
                    gpio_put(SEG_F, ss_digits[i].f);
                    gpio_put(SEG_G, ss_digits[i].g);
                    
                    gpio_put(D0_GND, i != 0);
                    gpio_put(D1_GND, i != 1);
                    gpio_put(D2_GND, i != 2);
                    gpio_put(D3_GND, i != 3);

					// time is money and money is time. Waste it here
                    sleep_ms(1);

                    gpio_put(D0_GND, 1);
                    gpio_put(D1_GND, 1);
                    gpio_put(D2_GND, 1);
                    gpio_put(D3_GND, 1);

                    gpio_put(SEG_A, 0);
                    gpio_put(SEG_B, 0);
                    gpio_put(SEG_C, 0);
                    gpio_put(SEG_D, 0);
                    gpio_put(SEG_E, 0);
                    gpio_put(SEG_F, 0);
                    gpio_put(SEG_G, 0);
                }
            }
        }
    }
}


/* clear the display */
void seq_seven_segment_clear(void) {
    for(uint8_t i = 0; i < SEGMENTS_COUNT; ++i) {
        ss_digits[i].render = 0;
        ss_digits[i].a = 0;
        ss_digits[i].b = 0;
        ss_digits[i].c = 0;
        ss_digits[i].d = 0;
        ss_digits[i].e = 0;
        ss_digits[i].f = 0;
        ss_digits[i].g = 0;
    }

	// reset illumination time
    ss_illumination_started = 0;
}


/* set how long the display should be illuminated after setting a value
-> time_us: integer value representing how long it should be lit in microseconds */
void seq_seven_segment_time(uint64_t time_us) {
    ss_illumination_time = time_us;
}


/* set how many times the display should be re-rendered
-> cycles: integer value */
void seq_seven_segment_cycles(uint8_t cycles) {
    ss_cycles = cycles;
}


/* set individual digits
-> index: integer value representing the desired digit
-> number: integer value from 0-9 */
void seq_seven_segment_set_digit(uint8_t index, uint8_t number) {
    struct ss_digit* ssd_struct = &ss_digits[index];

	// if it is "no digit", it should not be rendered
    if(number == SEGMENT_NONE) {
        ssd_struct->render = 0;
        return;
    }

    ssd_struct->render = 1;

    if(number == 0) {
		ssd_struct->a = 1;
		ssd_struct->b = 1;
		ssd_struct->c = 1;
		ssd_struct->d = 1;
		ssd_struct->e = 1;
		ssd_struct->f = 1;
		ssd_struct->g = 0;
	} else if(number == 1) {
		ssd_struct->a = 0;
		ssd_struct->b = 1;
		ssd_struct->c = 1;
		ssd_struct->d = 0;
		ssd_struct->e = 0;
		ssd_struct->f = 0;
		ssd_struct->g = 0;
	} else if(number == 2) {
		ssd_struct->a = 1;
		ssd_struct->b = 1;
		ssd_struct->c = 0;
		ssd_struct->d = 1;
		ssd_struct->e = 1;
		ssd_struct->f = 0;
		ssd_struct->g = 1;
	} else if(number == 3) {
		ssd_struct->a = 1;
		ssd_struct->b = 1;
		ssd_struct->c = 1;
		ssd_struct->d = 1;
		ssd_struct->e = 0;
		ssd_struct->f = 0;
		ssd_struct->g = 1;
	} else if(number == 4) {
		ssd_struct->a = 0;
		ssd_struct->b = 1;
		ssd_struct->c = 1;
		ssd_struct->d = 0;
		ssd_struct->e = 0;
		ssd_struct->f = 1;
		ssd_struct->g = 1;
	} else if(number == 5) {
		ssd_struct->a = 1;
		ssd_struct->b = 0;
		ssd_struct->c = 1;
		ssd_struct->d = 1;
		ssd_struct->e = 0;
		ssd_struct->f = 1;
		ssd_struct->g = 1;
	} else if(number == 6) {
		ssd_struct->a = 1;
		ssd_struct->b = 0;
		ssd_struct->c = 1;
		ssd_struct->d = 1;
		ssd_struct->e = 1;
		ssd_struct->f = 1;
		ssd_struct->g = 1;
	} else if(number == 7) {
		ssd_struct->a = 1;
		ssd_struct->b = 1;
		ssd_struct->c = 1;
		ssd_struct->d = 0;
		ssd_struct->e = 0;
		ssd_struct->f = 0;
		ssd_struct->g = 0;
	} else if(number == 8) {
		ssd_struct->a = 1;
		ssd_struct->b = 1;
		ssd_struct->c = 1;
		ssd_struct->d = 1;
		ssd_struct->e = 1;
		ssd_struct->f = 1;
		ssd_struct->g = 1;
	} else if(number == 9) {
		ssd_struct->a = 1;
		ssd_struct->b = 1;
		ssd_struct->c = 1;
		ssd_struct->d = 1;
		ssd_struct->e = 0;
		ssd_struct->f = 1;
		ssd_struct->g = 1;
	}
}


/* calculate the number of digits for a number
-> number: integer value
-> returns number of digits */
uint8_t num_length(uint16_t number) {
	if(number < 10) return 1;
	if(number < 100) return 2;
	if(number < 1000) return 3;
	if(number < 10000) return 4;
	return 5;
}


/* extract a digit from a given number
-> number: integer value
-> digit: integer value representing a digit (least significant first) */
uint8_t num_digit(uint16_t number, uint8_t digit) {
	uint8_t length = num_length(number);
	if(digit >= length || digit < 0) return 0;
	digit = -1 * digit + length -1;
	uint32_t r = 0;
	r = number / pow(10, digit);
	r = r % 10;
	return r; 
}


/* set a number to be displayed
-> data: integer value to be rendered */
void seq_seven_segment_set(uint16_t data) {
	// clear display
	seq_seven_segment_clear();

	// reset illumination time
    ss_illumination_started = time_us_64();

	// calculate length and number of leading zeroes
    uint8_t length = num_length(data);
    uint8_t leading_zeroes = SEGMENTS_COUNT - length;

	// set all digits, ignoring leading zeroes
    for(uint8_t i = leading_zeroes; i < SEGMENTS_COUNT; ++i) {
        uint8_t digit = num_digit(data, i - leading_zeroes);
        seq_seven_segment_set_digit(i, digit);
    }
}


/* set a number with a prefix to be displayed
-> prefix: integer value ranging from 0-9
-> data: integer value to be rendered */
void seq_seven_segment_set_prefix(uint8_t prefix, uint16_t data) {
	// clear display
	seq_seven_segment_clear();
	
	// reset illumination time
	ss_illumination_started = time_us_64();

	// calculate length and number of leading zeroes
    uint8_t length = num_length(data);
    uint8_t leading_zeroes = SEGMENTS_COUNT - length;

	// set the prefix
	seq_seven_segment_set_digit(0, prefix);

	// set all digits, ignoring prefix and leading zeroes
	for(uint8_t i = leading_zeroes; i < SEGMENTS_COUNT; ++i) {
        uint8_t digit = num_digit(data, i - leading_zeroes);
        seq_seven_segment_set_digit(i, digit);
    }
}