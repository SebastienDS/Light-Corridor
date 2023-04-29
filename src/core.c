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

void setTemplateWallTopRightBottomLeft(Template* template) {
    template->count = 2;
    template->walls = malloc(template->count * sizeof(Wall));
	REQUIRE_NON_NULL(template->walls);

    template->walls[0].info = (Info){{0.75, 0, 0.75}, {0.5, 0, 0.5}};
    template->walls[0].transition = createMovementTransition((Vec){0, 0, -0.01}, -25, 25, -25);
    
    template->walls[1].info = (Info){{-0.75, 0, 0.25}, {0.5, 0, 0.5}};
    template->walls[1].transition = createMovementTransition((Vec){0, 0, 0.01}, -25, 25, -25);
}

void setTemplateWallLeftRight(Template* template) {
    template->count = 2;
    template->walls = malloc(template->count * sizeof(Wall));
	REQUIRE_NON_NULL(template->walls);

    template->walls[0].info = (Info){{-0.75, 0, 0.5}, {0.5, 0, 1}};
    template->walls[0].transition = createMovementTransition((Vec){0.01, 0, 0}, -25, 25, 25);
    
    template->walls[1].info = (Info){{0.75, 0, 0.5}, {0.5, 0, 1}};
    template->walls[1].transition = createMovementTransition((Vec){0.01, 0, 0}, -25, 25, -25);
}

void setTemplateWallMiddleH(Template* template) {
    template->count = 1;
    template->walls = malloc(template->count * sizeof(Wall));
	REQUIRE_NON_NULL(template->walls);

    template->walls[0].info = (Info){{0, 0, 0.5}, {0.5, 0, 1}};
    template->walls[0].transition = createMovementTransition((Vec){0.01, 0, 0}, -75, 75, 0);
}

void setTemplateWallTopBottom(Template* template) {
    template->count = 2;
    template->walls = malloc(template->count * sizeof(Wall));
	REQUIRE_NON_NULL(template->walls);

    template->walls[0].info = (Info){{0, 0, 0.9}, {2, 0, 0.25}};
    template->walls[0].transition = createMovementTransition((Vec){0, 0, 0.01}, -25, 25, 25);
    
    template->walls[1].info = (Info){{0, 0, 0.1}, {2, 0, 0.25}};
    template->walls[1].transition = createMovementTransition((Vec){0, 0, 0.01}, -25, 25, -25);
}

void setTemplateWallMiddleW(Template* template) {
    template->count = 1;
    template->walls = malloc(template->count * sizeof(Wall));
	REQUIRE_NON_NULL(template->walls);

    template->walls[0].info = (Info){{0, 0, 0.5}, {2, 0, 0.35}};
    template->walls[0].transition = createMovementTransition((Vec){0, 0, 0.01}, -35, 35, 0);
}

Templates getTemplates() {
    const int bonusMiddleWalls = 6;
    Templates templates;
    templates.count = 12 + bonusMiddleWalls;
    templates.templates = malloc(templates.count * sizeof(Template));
	REQUIRE_NON_NULL(templates.templates);

    setTemplateWallTopLeftBottomRight(&templates.templates[0]);
    copyTemplateWithoutTransition(&templates.templates[1], &templates.templates[0]);

    setTemplateWallTopRightBottomLeft(&templates.templates[2]);
    copyTemplateWithoutTransition(&templates.templates[3], &templates.templates[2]);

    setTemplateWallLeftRight(&templates.templates[4]);
    copyTemplateWithoutTransition(&templates.templates[5], &templates.templates[4]);

    setTemplateWallMiddleH(&templates.templates[6]);
    copyTemplateWithoutTransition(&templates.templates[7], &templates.templates[6]);

    setTemplateWallTopBottom(&templates.templates[8]);
    copyTemplateWithoutTransition(&templates.templates[9], &templates.templates[8]);

    setTemplateWallMiddleW(&templates.templates[10]);
    copyTemplateWithoutTransition(&templates.templates[11], &templates.templates[10]);

    for (int i = 0; i < bonusMiddleWalls; i += 2) {
        setTemplateWallMiddleH(&templates.templates[12 + i]);
        copyTemplateWithoutTransition(&templates.templates[13 + i], &templates.templates[12 + i]);

        setTemplateWallMiddleW(&templates.templates[14 + i]);
        copyTemplateWithoutTransition(&templates.templates[15 + i], &templates.templates[14 + i]);
    }

    return templates;
}

void freeTemplates(Templates* templates) {
    for (int i = 0; i < templates->count; i++) {
        free(templates->templates[i].walls);
    }
    free(templates->templates);
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

        Transition* transition = &template->walls[0].transition;
        int transition_index = 2 * randint(0, 1 + transition->to - transition->from);

        for (int j = 0; j < template->count; j++) {
            if (level.walls_count >= capacity) {
                capacity *= 2;
                level.walls = realloc(level.walls, capacity * sizeof(Wall));
	            REQUIRE_NON_NULL(level.walls);
            }
            Wall* wall = &level.walls[level.walls_count];

            *wall = template->walls[j];
            wall->color = color;
            wall->info.position.y = i;

            for (int step = 0; step < transition_index; step++) {
                applyTransition(&wall->info, &wall->transition);
            }

            level.walls_count++;
        }
    }

    freeTemplates(&templates);

    return level;
}