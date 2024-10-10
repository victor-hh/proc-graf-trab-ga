#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ImageUtils.h"

GLuint loadPNGTexture(const char* pngFilePath) {
    int width, height, channels;
    unsigned char* data = stbi_load(pngFilePath, &width, &height, &channels, 0);

    if (!data) {
        printf("Erro ao carregar imagem PNG: %s\n", pngFilePath);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Se a imagem tem um canal alfa, usamos GL_RGBA, senão GL_RGB
    // isso é pra trabalhar com PNG, mas não estamos suportando
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    for (int i = 0; i < height / 2; ++i) {
        int swapIndex = height - i - 1;
        for (int j = 0; j < width * channels; ++j) {
            std::swap(data[i * width * channels + j], data[swapIndex * width * channels + j]);
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}

GLuint loadIconTexture(const char* ppmFilePath) {
    int width, height;
    unsigned char* data = loadPPM(ppmFilePath, &width, &height);

    if (!data) {
        printf("Erro ao carregar ícone PPM: %s\n", ppmFilePath);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(data);
    return textureID;
}
