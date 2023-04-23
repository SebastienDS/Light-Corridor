#pragma once

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "core.h"



void drawFrame();

void drawCorridorPart(Color color);
void drawSeparator();

void drawWall(Wall* wall);

void drawBall(Ball* ball);

void drawPlayer(Player* player);
