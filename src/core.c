#include "core.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


int randint(int min, int max) {
    return rand() % (1 + max - min) + min;
}

float magnitude(Vec* vec) {
    return sqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
}

void divide(Vec* vec, float value) {
    vec->x /= value;
    vec->y /= value;
    vec->z /= value;
}

float normalizeVec(Vec* vec) {
    float m = magnitude(vec);
    if (m > 0) {
        divide(vec, m);
    }
    return m;
}

void copyTemplateWithoutTransition(Template* template, Template* templateToCopy) {
    template->count = templateToCopy->count;
    template->walls = malloc(template->count * sizeof(Wall));
    REQUIRE_NON_NULL(template->walls);

    for (int i = 0; i < template->count; i++) {
        template->walls[i].info = templateToCopy->walls[i].info;
        template->walls[i].transition = createNoTransition();
    }
}

void setTemplateWallTopLeftBottomRight(Template* template) {
    template->count = 2;
    template->walls = malloc(template->count * sizeof(Wall));
	REQUIRE_NON_NULL(template->walls);

    template->walls[0].info = (Info){{-0.75, 0, 0.75}, {0.5, 0, 0.5}};
    template->walls[0].transition = createMovementTransition((Vec){0, 0, -0.01}, -25, 25, -25);
    
    template->walls[1].info = (Info){{0.75, 0, 0.25}, {0.5, 0, 0.5}};
    template->walls[1].transition = createMovementTransition((Vec){0, 0, 0.01}, -25, 25, -25);
}

Templates getTemplates() {
    Templates templates;
    templates.count = 2;
    templates.templates = malloc(templates.count * sizeof(Template));
	REQUIRE_NON_NULL(templates.templates);

    setTemplateWallTopLeftBottomRight(&templates.templates[0]);
    copyTemplateWithoutTransition(&templates.templates[1], &templates.templates[0]);
    
    return templates;
}

Level createLevel(int distance) {
    Templates templates = getTemplates();
    int capacity = distance;

    Level level;
	level.distance = distance;
    level.walls_count = 0;
	level.walls = malloc(capacity * sizeof(Wall));
	REQUIRE_NON_NULL(level.walls);

    for (int i = 1; i < distance; i += 2) {
        Template* template = &templates.templates[randint(0, templates.count - 1)];
        Color color = (Color){randint(0, 255) / 255.f, randint(0, 255) / 255.f, randint(0, 255) / 255.f};

        for (int j = 0; j < template->count; j++) {
            if (level.walls_count >= capacity) {
                capacity *= 2;
                level.walls = realloc(level.walls, capacity * sizeof(Wall));
	            REQUIRE_NON_NULL(level.walls);
            }

            level.walls[level.walls_count] = template->walls[j];
            level.walls[level.walls_count].color = color;
            level.walls[level.walls_count].info.position.y = i;
            level.walls_count++;
        }
    }

    return level;
}

// Level createLevel1() {
// 	Level level;
// 	level.distance = 10;
// 	level.walls_count = 7;
// 	level.walls = malloc(level.walls_count * sizeof(Wall));
// 	REQUIRE_NON_NULL(level.walls);

	
// 	level.walls[0] = (Wall) {
// 		(Info){-0.75, 1, 0.75, 0.5, 0, 0.5},
// 		(Color){1, 0.4, 0.5},
// 		createMovementTransition((Vec){0, 0, -0.01}, -25, 25, -25)
// 	};
// 	level.walls[1] = (Wall) {
// 		(Info){0.75, 1, 0.25, 0.5, 0, 0.5},
// 		(Color){1, 0.4, 0.5},
// 		createMovementTransition((Vec){0, 0, 0.01}, -25, 25, -25)
// 	};
// 	level.walls[2] = (Wall) {
// 		(Info){-0.75, 3, 0.5, 0.5, 0, 1},
// 		(Color){0.5, 0.5, 0.5},
// 		createMovementTransition((Vec){0.01, 0, 0}, -25, 25, 25)
// 	};
// 	level.walls[3] = (Wall) {
// 		(Info){0.75, 3, 0.5, 0.5, 0, 1},
// 		(Color){0.5, 0.5, 0.5},
// 		createMovementTransition((Vec){0.01, 0, 0}, -25, 25, -25)
// 	};
// 	level.walls[4] = (Wall) {
// 		(Info){0, 5, 0.5, 0.5, 0, 1},
// 		(Color){1, 0, 0},
// 		createMovementTransition((Vec){0.01, 0, 0}, -75, 75, 0)
// 	};
// 	level.walls[5] = (Wall) {
// 		(Info){0, 7, 0.5, 2, 0, 1},
// 		(Color){0, 0, 1},
// 		createScalingTransition((Vec){0.025, 0, 0.01}, -30, 30, 30)
// 	};
// 	level.walls[6] = (Wall) {
// 		(Info){0, 9, 0.5, 2, 0, 1},
// 		(Color){0.5, 0.5, 1},
// 		createNoTransition()
// 	};

// 	return level;
// }