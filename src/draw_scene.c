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

void drawWall(Wall* wall) {
    glPushMatrix();
        glColor3f(wall->color.r, wall->color.g, wall->color.b);
        glTranslatef(wall->info.position.x, wall->info.position.y, wall->info.position.z);
        glRotatef(90, 1, 0, 0);
        glScalef(wall->info.size.x, wall->info.size.z, 1);
        drawFilledSquare();
    glPopMatrix();
}

void drawBallShadow(Ball* ball) {
    glPushMatrix();
        glTranslatef(ball->info.position.x, ball->info.position.y, 0.01);
        glScalef(ball->info.size.x / 2, ball->info.size.y / 2, ball->info.size.z / 2);
        glColor3f(0.3, 0.3, 0.3);
        drawCircle();
    glPopMatrix();
}

void drawBall(Ball* ball) {
    glPushMatrix();
        glTranslatef(ball->info.position.x, ball->info.position.y, ball->info.position.z);
        glScalef(ball->info.size.x / 2, ball->info.size.y / 2, ball->info.size.z / 2);
        glColor3f(ball->color.r, ball->color.g, ball->color.b);
        drawSphere();
    glPopMatrix();

    drawBallShadow(ball);
}

void drawPlayer(Player* player) {
    glPushMatrix();
        glColor3f(player->color.r, player->color.g, player->color.b);
        glTranslatef(player->info.position.x, player->info.position.y, player->info.position.z);
        glRotatef(90, 1, 0, 0);
        glScalef(player->info.size.x, player->info.size.z, 1);
        drawSquare();
    glPopMatrix();
}