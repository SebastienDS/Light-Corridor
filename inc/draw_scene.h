#pragma once

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


typedef struct {
    float x;
    float y;
    float z;
    float size_x;
    float size_y;
    float size_z;
} Info;

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


void drawFrame();

void drawCorridorPart(Color color);
void drawSeparator();

void drawWall(Info info, Color color);

void drawBall(Info info, Color color);

void drawPlayer(Info info, Color color);
