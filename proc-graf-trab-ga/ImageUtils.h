#pragma once
#include <stdio.h>
#include <jpeglib.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "ProjectStruct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <cstring>

void writePPM(const char* filename, unsigned char* image, int width, int height) {
    FILE* f;
    errno_t err = fopen_s(&f, filename, "wb");
    if (err != 0) {
        printf("Erro ao abrir o arquivo para escrita\n");
        return;
    }
    fprintf(f, "P6\n%d %d\n255\n", width, height);
    fwrite(image, 1, width * height * 3, f);
    fclose(f);
}

void renderTexture(const Image& imageStruct) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, imageStruct.textureID);

    int windowWidth, windowHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

    float imageWidth = static_cast<float>(imageStruct.imageWidth);
    float imageHeight = static_cast<float>(imageStruct.imageHeight);

    float aspectRatio = imageWidth / imageHeight;

    float viewHeight = ( windowHeight - 100 ) * 0.9; 
    float viewWidth = ( windowWidth - 100 ) * 0.9; 

    float textureWidth;
    float textureHeight;

    if (aspectRatio > (viewWidth / viewHeight)) {
        textureWidth = viewWidth;
        textureHeight = viewWidth / aspectRatio;
    }
    else {
        textureHeight = viewHeight;
        textureWidth = viewHeight * aspectRatio;
    }

    float posX = (windowWidth - textureWidth) / 2.0f;
    float posY = (windowHeight - textureHeight) / 2.0f;

    glBegin(GL_QUADS);
    // Inferior esquerdo
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(posX, posY + textureHeight);
    // Inferior direito
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(posX + textureWidth, posY + textureHeight);
    // Superior direito
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(posX + textureWidth, posY);
    // Superior esquerdo
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(posX, posY);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

unsigned char* loadPPM(const char* filename, int* width, int* height) {
    FILE* fp;
    errno_t err = fopen_s(&fp, filename, "rb");
    if (err != 0) {
        printf("Erro ao abrir o arquivo PPM\n");
        return NULL;
    }

    char format[3];
    fscanf_s(fp, "%2s", format, (unsigned)_countof(format));

    if (strcmp(format, "P6") != 0) {
        printf("Formato PPM não suportado\n");
        fclose(fp);
        return NULL;
    }
    char c;
    while ((c = fgetc(fp)) == '#') {
        while (fgetc(fp) != '\n');
    }
    ungetc(c, fp);

    int maxval;
    fscanf_s(fp, "%d %d %d", width, height, &maxval);

    if (maxval != 255) {
        printf("O valor máximo da cor não é 255\n");
        fclose(fp);
        return NULL;
    }

    // Ignorar o último caractere antes dos dados
    fgetc(fp);

    int imageSize = (*width) * (*height) * 3;
    unsigned char* data = (unsigned char*)malloc(imageSize);

    if (data == NULL) {
        printf("Falha ao alocar memória para a imagem\n");
        fclose(fp);
        return NULL;
    }
    fread(data, 1, imageSize, fp);
    fclose(fp);

    return data;
}

bool loadTexture(Image& imageStruct) {
    unsigned char* data = loadPPM(imageStruct.ppmFileNameWithPath.c_str(), &imageStruct.imageWidth, &imageStruct.imageHeight);
    if (!data) {
        printf("Erro ao carregar a textura\n");
        return false;
    }

    // Gera uma textura OpenGL
    glGenTextures(1, &imageStruct.textureID);
    if (imageStruct.textureID == 0) {
        std::cerr << "glGenTextures - Falha ao gerar a textura (ID = 0)!" << std::endl;
        return false; // Retorna se a textura não foi gerada
    }
    glBindTexture(GL_TEXTURE_2D, imageStruct.textureID);

    // Carrega os dados da imagem como textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageStruct.imageWidth, imageStruct.imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    if (imageStruct.textureID == 0) {
        std::cerr << "glTexImage2D - Falha ao gerar a textura (ID = 0)!" << std::endl;
        return false; // Retorna se a textura não foi gerada
    }
    // Configura os parâmetros de filtro da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Libera os dados da imagem carregada
    free(data);
    return true;
}
