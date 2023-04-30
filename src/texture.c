#include "texture.h"
#include "core.h"
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void loadAsset(char* filename, Texture* t) {
    t->image = stbi_load(filename, &t->w, &t->h, &t->c, 0);
    REQUIRE_NON_NULL(t->image);

    glGenTextures(1, &t->texture);

    glBindTexture(GL_TEXTURE_2D, t->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->w, t->h, 0, GL_RGB, GL_UNSIGNED_BYTE, t->image);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Assets loadAssets() {
    Assets assets;
    char buffer[16];

    for (int i = 0; i < 10; i++) {
        snprintf(buffer, 16, "img/digit_%d.jpg", i);
        printf("%s\n", buffer);
        loadAsset(buffer, &assets.digits[i]);
    }

    return assets;
}

void freeAssets(Assets* assets) {
    for (int i = 0; i < 10; i++) {
        Texture* t = &assets->digits[i];
        glDeleteTextures(1, &t->texture);
        stbi_image_free(t->image);
    }
}