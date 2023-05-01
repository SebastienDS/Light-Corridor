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

void drawObjectShadow(Info* info);

void drawWall(Wall* wall);

void drawBall(Ball* ball);

void drawPlayer(Player* player);

void drawBonus(Bonus* bonus);

void drawDigit(Digit* digit);