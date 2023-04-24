#include "3D_tools.h"

/* Camera parameters and functions */
float theta = -90.0f; // Angle between x axis and viewpoint
float phy = 90.0f; // Angle between z axis and viewpoint
float dist_zoom = 2.0f; // Distance between origin and viewpoint

// void setCamera() {
// 	gluLookAt(dist_zoom*cos(toRad(theta))*sin(toRad(phy)),
// 			  dist_zoom*sin(toRad(theta))*sin(toRad(phy)),
// 			  dist_zoom*cos(toRad(phy)),
// 			  0.0,0.5,0.0,
// 			  0.0,0.0,1.0);
// }

void setCamera() {
	gluLookAt(0, -0.8, 0.5, 0, 0, 0.5, 0, 0, 1);
}

/* Convert degree to radians */
float toRad(float deg) {
	return deg*M_PI/180.0f;
}

void drawSquare()
{
	glBegin(GL_LINE_LOOP);
	glVertex2f(-0.5, 0.5);
	glVertex2f(0.5, 0.5);
	glVertex2f(0.5, -0.5);
	glVertex2f(-0.5, -0.5);
	glEnd();
}


void drawFilledSquare() {
	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(-0.5,-0.5,0.0);
		glVertex3f(0.5,-0.5,0.0);
		glVertex3f(0.5,0.5,0.0);
		glVertex3f(-0.5,0.5,0.0);
	glEnd();
}

void drawCircle() {
	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(0.0,0.0,0.0);
		float step_rad = 2*M_PI/(float)NB_SEG_CIRCLE;
		for(int i=0;i<=NB_SEG_CIRCLE;i++) {
			glVertex3f(cos(i*step_rad),sin(i*step_rad),0.0f);
		}
	glEnd();
}

void drawCone() {
	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(0.0,0.0,1.0);
		float step_rad = 2*M_PI/(float)NB_SEG_CIRCLE;
		for(int i=0;i<=NB_SEG_CIRCLE;i++) {
			glVertex3f(cos(i*step_rad),sin(i*step_rad),0.0f);
		}
	glEnd();
}

void drawSphere() {
	gluSphere(gluNewQuadric(),1.0,NB_SEG_CIRCLE,NB_SEG_CIRCLE);
}

void drawPyramid() {
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glPushMatrix();
		glRotatef(90, 1, 0, 0);
		glBegin(GL_TRIANGLES);
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f,-1.0f, 1.0f);
		glVertex3f( 1.0f,-1.0f, 1.0f);

		glVertex3f( 0.0f, 1.0f, 0.0f);
		glVertex3f( 1.0f,-1.0f, 1.0f);
		glVertex3f( 1.0f,-1.0f,-1.0f);

		glVertex3f( 0.0f, 1.0f, 0.0f);
		glVertex3f( 1.0f,-1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);

		glVertex3f( 0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f, 1.0f);
		glEnd();
	glPopMatrix();

}

void drawLosange() {
	glPushMatrix();
		drawPyramid();
		glRotatef(180, 1, 0, 0);
		glTranslatef(0.0f, 0.0f, 2.0f);
		drawPyramid();
	glPopMatrix();
}

void drawCube() {
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	drawSquare();
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 1.0f);
		drawSquare();
	glPopMatrix();
	glPushMatrix();
		glRotatef(90, 0, 1, 0);
		glTranslatef(-0.5f, 0.0f, 0.5f);
		drawSquare();
		glTranslatef(0.0f, 0.0f, -1.0f);
		drawSquare();
	glPopMatrix();
	glPushMatrix();
		glColor3f(0.5, 0.0, 0.5);
		glRotatef(90, 1, 0, 0);
		glTranslatef(0.0f, 0.5f, -0.5f);
		drawSquare();
		glTranslatef(0.0f, 0.0f, 1.0f);
		drawSquare();
	glPopMatrix();
}
