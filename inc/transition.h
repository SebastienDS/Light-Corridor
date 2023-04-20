#pragma once

#include "draw_scene.h"


typedef enum {
    NONE, MOVEMENT, SCALING
} TransitionType;

typedef struct {
    TransitionType type;
    Vec step;
    int from;
    int to;
    int current;
    int increment;
} Transition;


Transition createNoTransition();
Transition createMovementTransition(Vec step, int from, int to, int current);
Transition createScalingTransition(Vec step, int from, int to, int current);

void applyTransition(Info* info, Transition* transition);