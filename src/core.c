#include "core.h"
#include <math.h>

float magnitude(Vec* vec) {
    return sqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
}

void divide(Vec* vec, float value) {
    vec->x /= value;
    vec->y /= value;
    vec->z /= value;
}

float normalizeVec(Vec* vec) {
    float m = magnitude(vec);
    if (m > 0) {
        divide(vec, m);
    }
    return m;
}