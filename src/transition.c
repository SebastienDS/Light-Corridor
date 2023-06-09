#include "core.h"


Transition createNoTransition() {
    return (Transition){NONE};
}

Transition createMovementTransition(Vec step, int from, int to, int current) {
    return (Transition){MOVEMENT, step, from, to, current, current == to ? -1 : 1};
}

Transition createScalingTransition(Vec step, int from, int to, int current) {
    return (Transition){SCALING, step, from, to, current, current == to ? -1 : 1};
}

void stepTransition(Transition* transition) {
    transition->current += transition->increment;

    if (transition->current <= transition->from || transition->current >= transition->to) {
        transition->increment *= -1;
    }
}

void applyMovementTransition(Info* info, Transition* transition) {
    info->position.x += transition->increment * transition->step.x;
    info->position.y += transition->increment * transition->step.y;
    info->position.z += transition->increment * transition->step.z;

    stepTransition(transition);
}

void applyScalingTransition(Info* info, Transition* transition) {
    info->size.x += transition->increment * transition->step.x;
    info->size.y += transition->increment * transition->step.y;
    info->size.z += transition->increment * transition->step.z;

    stepTransition(transition);
}

void applyTransition(Info* info, Transition* transition) {
    if (transition->type == MOVEMENT) {
        applyMovementTransition(info, transition);
    } else if (transition->type == SCALING) {
        applyScalingTransition(info, transition);
    }
}