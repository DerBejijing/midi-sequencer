#ifndef SEQ_GLOBALS
#define SEQ_GLOBALS

// number of stages and rows
#define SEQ_STAGES 8
#define SEQ_ROWS 3

// delay in microseconds between refreshing the values
#define REFRESH_VALUES_US 1000
#define REFRESH_SEVEN_SEGMENT_US 5000

// how often the measured analog signal has to remain constant
// used for analog debouncing
#define ANALOG_CERTAINTY_TARGET 10

#define STAGE_NONE SEQ_STAGES + 1

#endif