#pragma once

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>


typedef struct {
    int w;
    int h;
    int c;
    unsigned char* image;
    GLuint texture;
} Texture;

typedef struct {
    Texture digits[10];
} Assets;

Assets loadAssets();
void freeAssets(Assets* assets);