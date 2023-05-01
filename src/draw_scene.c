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

void drawObjectShadow(Info* info) {
    glPushMatrix();
        glTranslatef(info->position.x, info->position.y, 0.01);
        glScalef(info->size.x / 2, info->size.y / 2, info->size.z / 2);
        glColor3f(0.3, 0.3, 0.3);
        drawCircle();
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

void drawBall(Ball* ball) {
    glPushMatrix();
        glTranslatef(ball->info.position.x, ball->info.position.y, ball->info.position.z);
        glScalef(ball->info.size.x / 2, ball->info.size.y / 2, ball->info.size.z / 2);
        glColor3f(ball->color.r, ball->color.g, ball->color.b);
        drawSphere();
    glPopMatrix();

    drawObjectShadow(&ball->info);
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

void drawBonus(Bonus* bonus) {
	glPushMatrix();
        glTranslatef(bonus->info.position.x, bonus->info.position.y, bonus->info.position.z);
        glScalef(bonus->info.size.x / 2, bonus->info.size.y / 2, bonus->info.size.z / 2);
		glRotatef(bonus->angle, 0, 0, 1);
        glColor3f(1, 1, 1);

		if (bonus->type == HEAL) {
			glTranslatef(0, 0, -0.5);
			drawCube();
		} else if (bonus->type == MAGNET) {
			glTranslatef(0, 0, 1);
			drawLosange();
		} else if (bonus->type == CLAIRVOYANCE) {
			drawPyramid();
        } else {
			fprintf(stderr, "Unknown Bonus");
			exit(1);
		}
    glPopMatrix();

	drawObjectShadow(&bonus->info);
}

void drawDigit(Digit* digit) {
    glPushMatrix();
        glTranslatef(0, 0, 0.5);
        glRotatef(-90, 1, 0, 0);
        glScalef(0.25, 0.25, 0.25);

        glBegin(GL_LINES);
            if (digit->have_line[0]) {
                glVertex2f(-0.23, -0.5);
                glVertex2f(0.23, -0.5);
            }
            if (digit->have_line[1]) {
                glVertex2f(-0.25, -0.48);
                glVertex2f(-0.25, -0.02);
            }
            if (digit->have_line[2]) {
                glVertex2f(0.25, -0.48);
                glVertex2f(0.25, -0.02);
            }
            if (digit->have_line[3]) {
                glVertex2f(-0.25, 0.02);
                glVertex2f(-0.25, 0.48);
            }
            if (digit->have_line[4]) {
                glVertex2f(0.25, 0.02);
                glVertex2f(0.25, 0.48);
            }
            if (digit->have_line[5]) {
                glVertex2f(-0.23, 0.5);
                glVertex2f(0.23, 0.5);
            }
            if (digit->have_line[6]) {
                glVertex2f(-0.23, 0);
                glVertex2f(0.23, 0);
            }
        glEnd();
    glPopMatrix();
}