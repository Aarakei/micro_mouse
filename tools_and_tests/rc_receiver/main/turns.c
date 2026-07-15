#include "turns.h"

// PWM Motion Profiles
static const int SIMPLE_90_SHORT[] = {};
static const int SIMPLE_90_LONG[]  = {};

static const int WIDE_90_SHORT[] = {};
static const int WIDE_90_LONG[]  = {};

static const int WALL_180_SHORT[] = {};
static const int WALL_180_LONG[]  = {};

static const int INTO_45_SHORT[] = {};
static const int INTO_45_LONG[]  = {};

static const int INTO_135_SHORT[] = {};
static const int INTO_135_LONG[]  = {};

static const int OUT_45_SHORT[] = {};
static const int OUT_45_LONG[]  = {};

static const int OUT_135_SHORT[] = {};
static const int OUT_135_LONG[]  = {};

static const int DIAGONAL_90_SHORT[] = {};
static const int DIAGONAL_90_LONG[]  = {};




//TODO: actually put everything in




// MotionProfile Array Definition:
const MotionProfile TURNS[] = {
    [SIMPLE_90] = {
        .length = 100,
        .ms_per_tick = 10,
        .pwm_short = SIMPLE_90_SHORT,
        .pwm_long  = SIMPLE_90_LONG
    },
    [WIDE_90] = {
        .length = 100,
        .ms_per_tick = 10,
        .pwm_short = WIDE_90_SHORT,
        .pwm_long  = WIDE_90_LONG 
    },
    [WALL_180] = {
        .length = 100,
        .ms_per_tick = 10,
        .pwm_short = WALL_180_SHORT,
        .pwm_long  = WALL_180_LONG 
    },
    [INTO_45] = {
        .length = 100,
        .ms_per_tick = 10,
        .pwm_short = INTO_45_SHORT,
        .pwm_long  = INTO_45_LONG 
    },
    [INTO_135] = {
        .length = 100,
        .ms_per_tick = 10,
        .pwm_short = INTO_135_SHORT,
        .pwm_long  = INTO_135_LONG 
    },
    [OUT_45] = {
        .length = 100,
        .ms_per_tick = 10,
        .pwm_short = OUT_45_SHORT,
        .pwm_long  = OUT_45_LONG 
    },
    [OUT_135] = {
        .length = 100,
        .ms_per_tick = 10,
        .pwm_short = OUT_135_SHORT,
        .pwm_long  = OUT_135_LONG 
    },
    [DIAGONAL_90] = {
        .length = 100,
        .ms_per_tick = 10,
        .pwm_short = DIAGONAL_90_SHORT,
        .pwm_long  = DIAGONAL_90_LONG 
    },
};