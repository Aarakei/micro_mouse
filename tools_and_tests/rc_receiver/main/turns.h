#ifndef TURNS_H
#define TURNS_H

typedef enum {
    SIMPLE_90,
    WIDE_90,
    WALL_180,
    INTO_45,
    INTO_135,
    OUT_45,
    OUT_135,
    DIAGONAL_90
} TurnSelect;

typedef struct {
    int length;
    int ms_per_tick;
    const int *pwm_short;
    const int *pwm_long;
} MotionProfile;

extern const MotionProfile TURNS[];

#endif


