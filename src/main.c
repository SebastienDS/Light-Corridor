#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "3D_tools.h"
#include "draw_scene.h"
#include "core.h"
#include "texture.h"

/* Window properties */
static const unsigned int WINDOW_WIDTH = 1000;
static const unsigned int WINDOW_HEIGHT = 500;
static const char WINDOW_TITLE[] = "Light Corridor";
static float aspectRatio = 1.0;

/* Minimal time wanted between two images */
static const double FRAMERATE_IN_SECONDS = 1. / 30.;

/* IHM flag */
static bool flagTransitions = true;

static int currentDirection = 0;
static bool playerDirection[4] = {false, false, false, false}; // top bottom left right

static GameState gs;

static const float BALL_SPEED = 0.05;
static const float PLAYER_SPEED = 0.025;
static const int PLAYER_BASE_HP = 5;

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
			case GLFW_KEY_R :
			case GLFW_KEY_SPACE:
				gs.set_ball_on_player = false;
				break;
			case GLFW_KEY_KP_9 :
				if(dist_zoom<100.0f) dist_zoom*=1.1;
				printf("Zoom is %f\n",dist_zoom);
				break;
			case GLFW_KEY_KP_3 :
				if(dist_zoom>1.0f) dist_zoom*=0.9;
				printf("Zoom is %f\n",dist_zoom);
				break;
			case GLFW_KEY_UP:
				currentDirection = 1;
				break;
			case GLFW_KEY_DOWN:
				currentDirection = -1;
				break;
			case GLFW_KEY_W:
				playerDirection[0] = true;
				break;
			case GLFW_KEY_S:
				playerDirection[1] = true;
				break;
			case GLFW_KEY_A:
				playerDirection[2] = true;
				break;
			case GLFW_KEY_D:
				playerDirection[3] = true;
				break;
			
			default: fprintf(stdout,"Touche non gérée (%d)\n",key);
		}
	} else if (action == GLFW_RELEASE) {
		switch(key) {
			case GLFW_KEY_UP:
			case GLFW_KEY_DOWN:
				currentDirection = 0;
				break;
			case GLFW_KEY_W:
				playerDirection[0] = false;
				break;
			case GLFW_KEY_S:
				playerDirection[1] = false;
				break;
			case GLFW_KEY_A:
				playerDirection[2] = false;
				break;
			case GLFW_KEY_D:
				playerDirection[3] = false;
				break;
		}
	}
}

void onClick(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT)
			currentDirection = 1;
		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
			gs.set_ball_on_player = false;
	} else if (action == GLFW_RELEASE) {
		if (button == GLFW_MOUSE_BUTTON_LEFT)
        	currentDirection = 0;
	}
}

float sumRadiusX(Info cube, Info cube2) {
	return cube.size.x / 2 + cube2.size.x / 2;
}

float sumRadiusY(Info cube, Info cube2) {
	return cube.size.y / 2 + cube2.size.y / 2;
}

float sumRadiusZ(Info cube, Info cube2) {
	return cube.size.z / 2 + cube2.size.z / 2;
}

float distanceX(Info cube, Info cube2) {
	return fabs(cube.position.x - cube2.position.x);
}

float distanceY(Info cube, Info cube2) {
	return fabs(cube.position.y - cube2.position.y);
}

float distanceZ(Info cube, Info cube2) {
	return fabs(cube.position.z - cube2.position.z);
}

bool intersectRect(Info cube, Info cube2) {
	return distanceX(cube, cube2) <= sumRadiusX(cube, cube2)
		&& distanceY(cube, cube2) <= sumRadiusY(cube, cube2)
		&& distanceZ(cube, cube2) <= sumRadiusZ(cube, cube2);
}

bool intersectCorridorX(Info ball) {
	return ball.position.x - ball.size.x / 2 <= -1 || ball.position.x + ball.size.x / 2 >= 1;
}

bool intersectCorridorZ(Info ball) {
	return ball.position.z - ball.size.z / 2 <= 0 || ball.position.z + ball.size.z / 2 >= 1;
}

bool collisionWithBonus(Player player, Bonus bonus) {
	return intersectRect(player.info, bonus.info);
}

GameState createGameState() {
	return (GameState){
		(Player){
			(Info){{0, 0, 0.5}, {0.25, 0.25, 0.25}},
			(Color){0, 0, 1},
			5
		},
		(Ball){
			(Info){{0, 0.5, 0.5}, {0.2, 0.2, 0.2}},
			(Vec){0, 1, 0},
			BALL_SPEED,
			(Color){0, 1, 0}
		},
		createLevel(200 + 1, 3),
		true,
		false
	};
}

void drawShadow() {
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
}

void drawDigit(Texture* texture) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->texture);

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(-0.5, 0.5);
		
		glTexCoord2f(1, 0);
		glVertex2f(0.5, 0.5);
		
		glTexCoord2f(1, 1);
		glVertex2f(0.5, -0.5);
		
		glTexCoord2f(0, 1);
		glVertex2f(-0.5, -0.5);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
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
		} else {
			fprintf(stderr, "Unknown Bonus");
			exit(1);
		}
    glPopMatrix();

}

void draw(GameState* gs, Assets* assets) {
	Color corridorColor = {0.4, 0.4, 0.4};
	glPushMatrix();
		float position = gs->player.info.position.y - (int)gs->player.info.position.y;

		glTranslatef(0, 0.5 - position, 0);

		for (int i = 0; i < 5; i++) {
			drawCorridorPart(corridorColor);
			drawSeparator();

			glTranslatef(0, 1, 0);
		}
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, -gs->player.info.position.y, 0);

		drawBall(&gs->ball);

		for (int i = gs->level.walls_count - 1; i >= 0; i--) {
			drawWall(&gs->level.walls[i]);
		}

		for (int i = 0; i < gs->level.bonus_count; i++) {
			if (gs->level.bonus[i].taken) continue;
			drawBonus(&gs->level.bonus[i]);
		}

		drawPlayer(&gs->player);
	glPopMatrix();

	drawShadow();

	// glPushMatrix();
	// 	glTranslatef(0, 0.2, 0.2);
	// 	drawDigit(&assets->digits[1]);
	// glPopMatrix();
}

bool playerIntersectWall(GameState* gs) {
	for (int i = 0; i < gs->level.walls_count; i++) {
		if (intersectRect(gs->player.info, gs->level.walls[i].info)) {
			return true;
		}
	}
	return false;
}

bool canPlayerAdvance(GameState* gs) {
	return !gs->set_ball_on_player && !playerIntersectWall(gs);
}

void keepPlayerOnCorridor(Player* player) {
	if (player->info.position.x < -0.8) {
		player->info.position.x = -0.8;
	} else if (player->info.position.x > 0.8) {
		player->info.position.x = 0.8;
	}
	if (player->info.position.z > 0.85) {
		player->info.position.z = 0.85;
	} else if (player->info.position.z < 0.15) {
		player->info.position.z = 0.15;
	}
}

void update(GameState* gs) {
	if (intersectCorridorX(gs->ball.info)) {
		gs->ball.direction.x *= -1;
	} else if (intersectCorridorZ(gs->ball.info)) {
		gs->ball.direction.z *= -1;
	}

	if (!gs->set_ball_on_player) {
		if (intersectRect(gs->ball.info, gs->player.info)) {
			if (gs->set_ball_on_player_on_the_next_collision) {
				gs->set_ball_on_player = true;
				gs->set_ball_on_player_on_the_next_collision = false;
				gs->ball.is_in_collision = false;
				gs->ball.direction = (Vec){0, 1, 0};
			} else {
				gs->ball.info.position.y += fabs(gs->ball.direction.y) * gs->ball.speed;
				Vec bounce = {
					gs->ball.info.position.x - gs->player.info.position.x,
					gs->ball.info.position.y - gs->player.info.position.y,
					gs->ball.info.position.z - gs->player.info.position.z
				};
				float newMagnitude = normalizeVec(&bounce);
				if (newMagnitude <= 0) {
					bounce = (Vec){0, 1, 0};
				}

				gs->ball.direction = bounce;
			}
		} else if (gs->ball.info.position.y <= gs->player.info.position.y) {
			gs->player.hp--;
			gs->set_ball_on_player = true;
			gs->ball.is_in_collision = false;
			gs->ball.direction = (Vec){0, 1, 0};
		}
	}
	
	bool hit = false;
	for (int i = 0; i < gs->level.walls_count; i++) {
		if (intersectRect(gs->ball.info, gs->level.walls[i].info) && !gs->ball.is_in_collision) {
			hit = true;
			break;
		}
	}
	if (hit) {
		gs->ball.direction.y *= -1;
		gs->ball.is_in_collision = true;
	} else {
		gs->ball.is_in_collision = false;
	}

	if (canPlayerAdvance(gs)) {
		gs->player.info.position.y += currentDirection * PLAYER_SPEED;
	}

	int directionZ = playerDirection[0] - playerDirection[1];
	int directionX = playerDirection[3] - playerDirection[2];

	gs->player.info.position.x += directionX * 0.02;
	gs->player.info.position.z += directionZ * 0.02;
	
	keepPlayerOnCorridor(&gs->player);

	if (gs->set_ball_on_player) {
		gs->ball.info.position.x = gs->player.info.position.x;
		gs->ball.info.position.y = gs->player.info.position.y;
		gs->ball.info.position.z = gs->player.info.position.z;
	} else {
		gs->ball.info.position.x += gs->ball.direction.x * gs->ball.speed;
		gs->ball.info.position.y += gs->ball.direction.y * gs->ball.speed;
		gs->ball.info.position.z += gs->ball.direction.z * gs->ball.speed;
	}

	if (flagTransitions) {
		for (int i = 0; i < gs->level.walls_count; i++) {
			applyTransition(&gs->level.walls[i].info, &gs->level.walls[i].transition);
		}
	}

	for (int i = 0; i < gs->level.bonus_count; i++) {
		Bonus* bonus = &gs->level.bonus[i];
		bonus->angle = (bonus->angle + 1) % 360;

		if (intersectRect(gs->player.info, bonus->info)) {
			if (bonus->taken) continue;
			
			bonus->taken = true;
			if (bonus->type == HEAL && gs->player.hp < PLAYER_BASE_HP) {
				gs->player.hp++;
				printf("NEW HP");
			} else if (bonus->type == MAGNET) {
				gs->set_ball_on_player_on_the_next_collision = true;
			}
		}
	}

	for (int i = 0; i < gs->level.bonus_count; i++) {
		if (gs->level.bonus[i].taken) continue;
		drawBonus(&gs->level.bonus[i]);
	}

	printf("%d\n", gs->player.hp);
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
	glfwSetMouseButtonCallback(window, onClick);

	onWindowResized(window,WINDOW_WIDTH,WINDOW_HEIGHT);

	glPointSize(5.0);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);


	srand(time(NULL));

	gs = createGameState();
	Assets assets = loadAssets();


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
		
		update(&gs);
		draw(&gs, &assets);


		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		/* Elapsed time computation from loop begining */
		double elapsedTime = glfwGetTime() - startTime;
		/* If to few time is spend vs our wanted FPS, we wait */
		if(elapsedTime < FRAMERATE_IN_SECONDS / 2)
		{
			glfwWaitEventsTimeout(FRAMERATE_IN_SECONDS / 2 - elapsedTime);
		}

		/* Animate scenery */
	}

	free(gs.level.walls);
	free(gs.level.bonus);
	freeAssets(&assets);

	glfwTerminate();
	return 0;
}
