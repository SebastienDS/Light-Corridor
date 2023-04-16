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

void drawWall(float r, float g, float b) {
    glPushMatrix();
        glColor3f(r, g, b);
        glScalef(2, 1, 1);
        drawFilledSquare();
    glPopMatrix();
    glPushMatrix();
        glColor4f(r, g, b, 0.8);
        glTranslatef(1, 0, 0);
        glRotatef(-90, 0, 1, 0);
        glTranslatef(0.5, 0, 0);
        drawFilledSquare();
    glPopMatrix();
    glPushMatrix();
        glColor3f(r, g, b);
        glTranslatef(0, 0, 1);
        glScalef(2, 1, 1);
        drawFilledSquare();
    glPopMatrix();
    glPushMatrix();
        glColor4f(r, g, b, 0.8);
        glTranslatef(-1, 0, 0);
        glRotatef(-90, 0, 1, 0);
        glTranslatef(0.5, 0, 0);
        drawFilledSquare();
    glPopMatrix();
}
