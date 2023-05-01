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
static unsigned int WINDOW_WIDTH = 1000;
static unsigned int WINDOW_HEIGHT = 500;
static const char WINDOW_TITLE[] = "Light Corridor";
static float aspectRatio = 1.0;

/* Minimal time wanted between two images */
static const double FRAMERATE_IN_SECONDS = 1. / 30.;

/* IHM flag */
static bool flagTransitions = true;

static int currentDirection = 0;
static bool playerDirection[4] = {false, false, false, false}; // top bottom left right

static GameState gs;
static State state;
static bool gameAlreadyRunning = false;


static const float BALL_SPEED = 0.05;
static const float PLAYER_SPEED = 0.025;
static const int PLAYER_BASE_HP = 5;
static const int CLAIRVOYANCE_DURATION = 10;


void setState(State* state, State newState, GLFWwindow* window);

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

	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
}

double convertXPosition(double x) {
	return x / (WINDOW_WIDTH / 2) - 1;
}

double convertYPosition(double y) {
	return 1 - y / WINDOW_HEIGHT;
}

GameState createGameState() {
	return (GameState){
		0,
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
		createLevel(1000 + 1, 5),
		true,
		false,
		false,
		0
	};
}

void onKeyMenu(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch(key) {
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
		}
	}
}

void onClickMenu(GLFWwindow* window, int button, int action, int mods) {
	if (action != GLFW_PRESS) return;

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	float a = convertXPosition(x);	
	float b = convertYPosition(y);

	if (a < -0.13 && a > -0.40 && b > 0.35 && b < 0.63) {
		if (!gameAlreadyRunning) {
			gs = createGameState();
			gameAlreadyRunning = true;
		}
		setState(&state, IN_GAME, window);
	} else if (a > 0.13 && a < 0.40 && b > 0.35 && b < 0.63) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void onKeyInGame(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		switch(key) {
			case GLFW_KEY_ESCAPE :
				setState(&state, MENU, window);
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

void onClickInGame(GLFWwindow* window, int button, int action, int mods)
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

// void drawDigit(Texture* texture) {
// 	glEnable(GL_TEXTURE_2D);
// 	glBindTexture(GL_TEXTURE_2D, texture->texture);

// 	glBegin(GL_QUADS);
// 		glTexCoord2f(0, 0);
// 		glVertex2f(-0.5, 0.5);
		
// 		glTexCoord2f(1, 0);
// 		glVertex2f(0.5, 0.5);
		
// 		glTexCoord2f(1, 1);
// 		glVertex2f(0.5, -0.5);
		
// 		glTexCoord2f(0, 1);
// 		glVertex2f(-0.5, -0.5);
// 	glEnd();
// 	glBindTexture(GL_TEXTURE_2D, 0);
// 	glDisable(GL_TEXTURE_2D);
// }

void draw(GameState* gs, Digit digits[10]) {
	Color corridorColor = {0.4, 0.4, 0.4};

	glPushMatrix();
		float position = gs->player.info.position.y - (int)gs->player.info.position.y;

		glTranslatef(0, 0.5 - position, 0);

		for (int i = 0; i < 50; i++) {
			drawCorridorPart(corridorColor);
			drawSeparator();

			glTranslatef(0, 1, 0);
		}
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, -gs->player.info.position.y, 0);

		drawBall(&gs->ball);

		for (int i = gs->level.walls_count - 1; i >= 0; i--) {
			if (gs->level.walls[i].info.position.y < gs->player.info.position.y) continue;
			drawWall(&gs->level.walls[i]);
		}

		for (int i = 0; i < gs->level.bonus_count; i++) {
			if (gs->level.bonus[i].taken) continue;
			drawBonus(&gs->level.bonus[i]);
		}

		drawPlayer(&gs->player);
	glPopMatrix();

	if (gs->clairvoyance_active) {
		glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			glTranslatef(0, 0.5, 0);
			glScalef(5, 5, 5);
			glColor4f(0.3, 0.3, 0.3, 0.2);
			drawFilledSquare();
		glPopMatrix();
	}

	drawShadow();

	// glPushMatrix();
	// 	glTranslatef(0, 0.2, 0.2);
	// 	drawDigit(&assets->digits[1]);
	// glPopMatrix();

	glPushMatrix();
		glColor3f(1, 1, 1);
		glTranslatef(0.9, 0, 0.85);
		glScalef(0.15, 0.15, 0.15);

		int score = gs->score;
		if (score == 0) {
			drawDigit(&digits[0]);
		}
		while (score) {
			drawDigit(&digits[score % 10]);
			glTranslatef(-0.2, 0, 0);
			score /= 10;
		}
	glPopMatrix();

	glPushMatrix();
		glColor3f(1, 1, 1);
		glTranslatef(-0.9, 0, 0.85);
		glScalef(0.15, 0.15, 0.15);

	
		drawDigit(&digits[gs->player.hp]);
	glPopMatrix();
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

void update(GameState* gs, GLFWwindow* window) {
	gs->score = gs->player.info.position.y * 100;
	if (gs->player.info.position.y >= gs->level.distance) {
		printf("GAGNE\n");
		gameAlreadyRunning = false;
		setState(&state, MENU, window);
	}

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

			if (gs->player.hp <= 0) {
				printf("PERDU\n");
				gameAlreadyRunning = false;
				setState(&state, MENU, window);
			}
		}
	}
	
	bool hit = false;
	for (int i = 0; i < gs->level.walls_count; i++) {
		if (intersectRect(gs->ball.info, gs->level.walls[i].info)) {
			hit = true;
			break;
		}
	}
	if (hit) {
		if (!gs->ball.is_in_collision) {
			gs->ball.direction.y *= -1;
			gs->ball.is_in_collision = true;
		}
	} else {
		gs->ball.is_in_collision = false;
	}

	if (canPlayerAdvance(gs)) {
		gs->player.info.position.y += currentDirection * PLAYER_SPEED;
	}

	// int directionZ = playerDirection[0] - playerDirection[1];
	// int directionX = playerDirection[3] - playerDirection[2];

	// gs->player.info.position.x += directionX * 0.02;
	// gs->player.info.position.z += directionZ * 0.02;

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	gs->player.info.position.x = convertXPosition(x);	
	gs->player.info.position.z = convertYPosition(y);	
	
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
			} else if (bonus->type == MAGNET) {
				gs->set_ball_on_player_on_the_next_collision = true;
			} else if (bonus->type == CLAIRVOYANCE) {
				gs->clairvoyance_active = true;
				gs->clairvoyance_start = glfwGetTime();
			}
		}
	}

	for (int i = 0; i < gs->level.bonus_count; i++) {
		if (gs->level.bonus[i].taken) continue;
		drawBonus(&gs->level.bonus[i]);
	}

	if (gs->clairvoyance_active && glfwGetTime() - gs->clairvoyance_start > CLAIRVOYANCE_DURATION) {
		gs->clairvoyance_active = false;
	}
}

void drawMenu() {
	glPushMatrix();
		glColor3f(0, 1, 0);
		glTranslatef(-0.25, 0, 0.5);
		glScalef(0.25, 0.25, 0.25);
		glRotatef(-90, 1, 0, 0);
		drawFilledSquare();
	glPopMatrix();

	glPushMatrix();
		glColor3f(1, 0, 0);
		glTranslatef(0.25, 0, 0.5);
		glScalef(0.25, 0.25, 0.25);
		glRotatef(-90, 1, 0, 0);
		drawFilledSquare();
	glPopMatrix();
}

void setState(State* state, State newState, GLFWwindow* window) {
	if (newState == MENU) {
		glfwSetKeyCallback(window, onKeyMenu);
		glfwSetMouseButtonCallback(window, onClickMenu);
	} else if (newState == IN_GAME) {
		glfwSetKeyCallback(window, onKeyInGame);
		glfwSetMouseButtonCallback(window, onClickInGame);
	}
	*state = newState;
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

	glfwSetWindowSizeCallback(window, onWindowResized);


	onWindowResized(window,WINDOW_WIDTH,WINDOW_HEIGHT);

	glPointSize(5.0);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	Digit digits[10] = {
		(Digit){{true, true, true, true, true, true, false}},		// 0
		(Digit){{false, false, true, false, true, false, false}},   // 1
		(Digit){{true, false, true, true, false, true, true}},	    // 2
		(Digit){{true, false, true, false, true, true, true}},      // 3
		(Digit){{false, true, true, false, true, false, true}},     // 4
		(Digit){{true, true, false, false, true, true, true}},      // 5
		(Digit){{true, true, false, true, true, true, true}},       // 6
		(Digit){{true, false, true, false, true, false, false}},    // 7
		(Digit){{true, true, true, true, true, true, true}},        // 8
		(Digit){{true, true, true, false, true, true, true}},       // 9
	};

	srand(time(NULL));

	// Assets assets = loadAssets();

	setState(&state, MENU, window);


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


		/* Scene rendering */

		if (state == IN_GAME) {
			update(&gs, window);
			draw(&gs, digits);
		} else if (state == MENU) {
			drawMenu();
		}

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
	// freeAssets(&assets);

	glfwTerminate();
	return 0;
}
