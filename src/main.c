#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "3D_tools.h"
#include "draw_scene.h"
#include "transition.h"

/* Window properties */
static const unsigned int WINDOW_WIDTH = 1000;
static const unsigned int WINDOW_HEIGHT = 500;
static const char WINDOW_TITLE[] = "Light Corridor";
static float aspectRatio = 1.0;

/* Minimal time wanted between two images */
static const double FRAMERATE_IN_SECONDS = 1. / 30.;

/* IHM flag */
static bool flagTransitions = false;

static int currentDirection = 0;


/* Error handling function */
void onError(int error, const char* description)
{
	fprintf(stderr, "GLFW Error: %s\n", description);
}

void onWindowResized(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / (float) height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0,aspectRatio,Z_NEAR,Z_FAR);
	glMatrixMode(GL_MODELVIEW);
}

void onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		switch(key) {
			case GLFW_KEY_A :
			case GLFW_KEY_ESCAPE :
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			case GLFW_KEY_L :
				glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
				break;
			case GLFW_KEY_P :
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				break;
			case GLFW_KEY_T :
				flagTransitions = !flagTransitions;
				break;
			case GLFW_KEY_KP_9 :
				if(dist_zoom<100.0f) dist_zoom*=1.1;
				printf("Zoom is %f\n",dist_zoom);
				break;
			case GLFW_KEY_KP_3 :
				if(dist_zoom>1.0f) dist_zoom*=0.9;
				printf("Zoom is %f\n",dist_zoom);
				break;
			case GLFW_KEY_UP :
				if (phy>2) phy -= 2;
				printf("Phy %f\n",phy);
				break;
			case GLFW_KEY_DOWN :
				if (phy<=88.) phy += 2;
				printf("Phy %f\n",phy);
				break;
			case GLFW_KEY_LEFT :
				theta -= 5;
				break;
			case GLFW_KEY_RIGHT :
				theta += 5;
				break;
			case GLFW_KEY_W:
				currentDirection = 1;
				break;
			case GLFW_KEY_S:
				currentDirection = -1;
				break;
			default: fprintf(stdout,"Touche non gérée (%d)\n",key);
		}
	} else if (action == GLFW_RELEASE) {
		switch(key) {
			case GLFW_KEY_W:
			case GLFW_KEY_S:
				currentDirection = 0;
				break;
		}
	}
}

float sumRadiusX(Info cube, Info cube2) {
	return cube.size_x / 2 + cube2.size_x / 2;
}

float sumRadiusY(Info cube, Info cube2) {
	return cube.size_y / 2 + cube2.size_y / 2;
}

float sumRadiusZ(Info cube, Info cube2) {
	return cube.size_z / 2 + cube2.size_z / 2;
}

float distanceX(Info cube, Info cube2) {
	return fabs(cube.x - cube2.x);
}

float distanceY(Info cube, Info cube2) {
	return fabs(cube.y - cube2.y);
}

float distanceZ(Info cube, Info cube2) {
	return fabs(cube.z - cube2.z);
}

bool intersectRect(Info cube, Info cube2) {
	return distanceX(cube, cube2) <= sumRadiusX(cube, cube2)
		&& distanceY(cube, cube2) <= sumRadiusY(cube, cube2)
		&& distanceZ(cube, cube2) <= sumRadiusZ(cube, cube2);
}

bool intersectCorridorX(Info ball) {
	return ball.x - ball.size_x / 2 <= -1 || ball.x + ball.size_x / 2 >= 1;
}

bool intersectCorridorZ(Info ball) {
	return ball.z - ball.size_z / 2 <= 0 || ball.z + ball.size_z / 2 >= 1;
}


int main(int argc, char** argv)
{
	/* GLFW initialisation */
	GLFWwindow* window;
	if (!glfwInit()) return -1;

	/* Callback to a function if an error is rised by GLFW */
	glfwSetErrorCallback(onError);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (!window)
	{
		// If no context created : exit !
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSetWindowSizeCallback(window,onWindowResized);
	glfwSetKeyCallback(window, onKey);

	onWindowResized(window,WINDOW_WIDTH,WINDOW_HEIGHT);

	glPointSize(5.0);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );


	Info ball = {0, 0.5, 0.5, 0.2, 0.2, 0.2};
	Vec speed = {0.02, 0.02, 0.02}; // TODO normalize + norme
	Color color = {0, 1, 0};

	int count = 7;
	Info walls[] = {
		{-0.75, 1, 0.75, 0.5, 0, 0.5},
		{0.75, 1, 0.25, 0.5, 0, 0.5},

		{-0.75, 3, 0.5, 0.5, 0, 1},
		{0.75, 3, 0.5, 0.5, 0, 1},

		{0, 5, 0.5, 0.5, 0, 1},
		{0, 7, 0.5, 2, 0, 1},

		{0, 9, 0.5, 2, 0, 1}
	};
	Color colors[] = {
		{1, 0.4, 0.5},
		{1, 0.4, 0.5},

		{0.5, 0.5, 0.5},
		{0.5, 0.5, 0.5},

		{1, 0, 0},
		{0, 0, 1},
		{0.5, 0.5, 1}
	};

	Transition transitions[] = {
		createMovementTransition((Vec){0, 0, -0.01}, -25, 25, -25),
		createMovementTransition((Vec){0, 0, 0.01}, -25, 25, -25),

		createMovementTransition((Vec){0.01, 0, 0}, -25, 25, 25),
		createMovementTransition((Vec){0.01, 0, 0}, -25, 25, -25),
		
		createMovementTransition((Vec){0.01, 0, 0}, -75, 75, 0),
		createScalingTransition((Vec){0.025, 0, 0.01}, -30, 30, 30),

		createNoTransition()
	};


	Info player = {0, 0, 0.5, 0.35, 0, 0.35};
	Color playerColor = {0, 0, 1};



	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Get time (in second) at loop beginning */
		double startTime = glfwGetTime();

		/* Cleaning buffers and setting Matrix Mode */
		glClearColor(0.0,0.0,0.0,0.0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		setCamera();

		/* Initial scenery setup */
		glPushMatrix();
		glTranslatef(0.0,0.0,-0.01);
		glScalef(10.0,10.0,1.0);
		glColor3f(0.0,0.0,0.1);
		drawSquare();
		glBegin(GL_POINTS);
			glColor3f(1.0,1.0,0.0);
			glVertex3f(0.0,0.0,0.0);
		glEnd();
		glPopMatrix();

		/* Scene rendering */
		Color corridorColor = {0.4, 0.4, 0.4};
		glPushMatrix();
			float position = player.y - (int)player.y;

			glTranslatef(0, 0.5 - position, 0);

			for (int i = 0; i < 5; i++) {
				drawCorridorPart(corridorColor);
				drawSeparator();

				glTranslatef(0, 1, 0);
			}
		glPopMatrix();


		glPushMatrix();
			glTranslatef(0, -player.y, 0);

			drawBall(ball, color);

			for (int i = count - 1; i >= 0; i--) {
				drawWall(walls[i], colors[i]);
			}

			drawPlayer(player, playerColor);
		glPopMatrix();


		if (intersectCorridorX(ball)) {
			speed.x *= -1;
		} else if (intersectCorridorZ(ball)) {
			speed.z *= -1;
		}
		if (intersectRect(ball, player)) {
			speed.y = fabs(speed.y);
			printf("player\n");
		} else if (ball.y <= player.y) {
			// vie--;
		}
		
		for (int i = 0; i < count; i++) {
			if (intersectRect(ball, walls[i])) {
				speed.y *= -1;
			}
		}


		player.y += currentDirection * 0.1;

		ball.x += speed.x;
		ball.y += speed.y;
		ball.z += speed.z;

		if (flagTransitions) {
			for (int i = 0; i < count; i++) {
				applyTransition(&walls[i], &transitions[i]);
			}
		}


		// shadow
		glPushMatrix();
			glTranslatef(0, 4.5, 0);
			glScalef(2, 1, 1);
			glColor4f(0, 0, 0, 1);
			glRotatef(-90, 1, 0, 0);
			glTranslatef(0, -0.5, 0);
			glScalef(0.99, 0.99, 0.99);
			drawFilledSquare();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0, 3, 0);
			glScalef(2, 1, 1);
			glColor4f(0, 0, 0, 0.5);
			glRotatef(-90, 1, 0, 0);
			glTranslatef(0, -0.5, 0);
			glScalef(0.99, 0.99, 0.99);
			drawFilledSquare();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0, 2, 0);
			glScalef(2, 1, 1);
			glColor4f(0, 0, 0, 0.25);
			glRotatef(-90, 1, 0, 0);
			glTranslatef(0, -0.5, 0);
			glScalef(0.99, 0.99, 0.99);
			drawFilledSquare();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0, 1.5, 0);
			glScalef(2, 1, 1);
			glColor4f(0, 0, 0, 0.1);
			glRotatef(-90, 1, 0, 0);
			glTranslatef(0, -0.5, 0);
			glScalef(0.99, 0.99, 0.99);
			drawFilledSquare();
		glPopMatrix();
		


		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		/* Elapsed time computation from loop begining */
		double elapsedTime = glfwGetTime() - startTime;
		/* If to few time is spend vs our wanted FPS, we wait */
		if(elapsedTime < FRAMERATE_IN_SECONDS)
		{
			glfwWaitEventsTimeout(FRAMERATE_IN_SECONDS-elapsedTime);
		}

		/* Animate scenery */
	}

	glfwTerminate();
	return 0;
}
