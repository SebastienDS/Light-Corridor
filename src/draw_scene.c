#include "draw_scene.h"
#include "3D_tools.h"

void drawFrame() {
    glBegin(GL_LINES);
	glColor3f(1., 0., 0.);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glColor3f(0., 1., 0.);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);
    glColor3f(0., 0., 1.);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);
	glEnd();
}

void drawCorridorPart(Color color) {
    glPushMatrix();
        glColor3f(color.r, color.g, color.b);
        glScalef(2, 1, 1);
        drawFilledSquare();
    glPopMatrix();
    glPushMatrix();
        glColor4f(color.r, color.g, color.b, 0.8);
        glTranslatef(1, 0, 0);
        glRotatef(-90, 0, 1, 0);
        glTranslatef(0.5, 0, 0);
        drawFilledSquare();
    glPopMatrix();
    glPushMatrix();
        glColor3f(color.r, color.g, color.b);
        glTranslatef(0, 0, 1);
        glScalef(2, 1, 1);
        drawFilledSquare();
    glPopMatrix();
    glPushMatrix();
        glColor4f(color.r, color.g, color.b, 0.8);
        glTranslatef(-1, 0, 0);
        glRotatef(-90, 0, 1, 0);
        glTranslatef(0.5, 0, 0);
        drawFilledSquare();
    glPopMatrix();
}

void drawSeparator() {
    glPushMatrix();
        glTranslatef(0, 0.5, 0);
        glScalef(2, 1, 1);
        glColor3f(1, 1, 1);
        glRotatef(-90, 1, 0, 0);
        glTranslatef(0, -0.5, 0);
        glScalef(0.99, 0.99, 0.99);
        drawSquare();
    glPopMatrix();
}

void drawWall(Info info, Color color) {
    glPushMatrix();
        glColor4f(color.r, color.g, color.b, 0.8);
        glTranslatef(info.x, info.y, info.z);
        glRotatef(90, 1, 0, 0);
        glScalef(info.size_x, info.size_z, 1);
        drawFilledSquare();
    glPopMatrix();
}

void drawBallShadow(Info info) {
    glPushMatrix();
        glTranslatef(info.x, info.y, 0.01);
        glScalef(info.size_x / 2, info.size_y / 2, info.size_z / 2);
        glColor3f(0.3, 0.3, 0.3);
        drawCircle();
    glPopMatrix();
}

void drawBall(Info info, Color color) {
    glPushMatrix();
        glTranslatef(info.x, info.y, info.z);
        glScalef(info.size_x / 2, info.size_y / 2, info.size_z / 2);
        glColor3f(color.r, color.g, color.b);
        drawSphere();
    glPopMatrix();

    drawBallShadow(info);
}