#pragma once


#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>


#define REQUIRE_NON_NULL(PTR) if ((PTR) == NULL) { \
    fprintf(stderr, "Require_non_null failed"); \
    exit(-1); \
}

typedef struct {
    float x;
    float y;
    float z;
} Vec;

typedef struct {
    float r;
    float g;
    float b;
} Color;

typedef struct {
    Vec position;
    Vec size;
} Info;

typedef struct {
    Info info;
    Color color;
    int hp;
} Player;

typedef struct {
    Info info;
    Vec direction;
    float speed;
    Color color;
} Ball;

typedef enum {
    NONE, MOVEMENT, SCALING
} TransitionType;

typedef struct Transition {
    TransitionType type;
    Vec step;
    int from;
    int to;
    int current;
    int increment;
} Transition;

typedef struct {
    Info info;
    Color color;
    Transition transition;
} Wall;

typedef struct {
    int distance;
    int walls_count;
    int bonus_count;
    Wall* walls;
    Bonus* bonus;
} Level;

typedef struct {
    Player player;
    Ball ball;
    Level level;
    bool set_ball_on_player;
    bool set_ball_on_player_on_the_next_collision;
} GameState;

typedef enum {
    HEAL, MAGNET
} BonusType;

typedef struct {
    BonusType type;
    Info info;
} Bonus;

typedef struct {
    int count;
    Wall* walls;
} Template;

typedef struct {
    int count;
    Template* templates;
} Templates;

Transition createNoTransition();
Transition createMovementTransition(Vec step, int from, int to, int current);
Transition createScalingTransition(Vec step, int from, int to, int current);

void applyTransition(Info* info, Transition* transition);

float normalizeVec(Vec* vec);

Level createLevel(int distance);
