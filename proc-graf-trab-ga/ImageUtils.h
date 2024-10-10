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
    FILE* ppmFile;
    errno_t err = fopen_s(&ppmFile, filename, "wb");
    if (err != 0) {
        printf("Erro ao abrir o arquivo para escrita\n");
        return;
    }
    fprintf(ppmFile, "P6\n%d %d\n255\n", width, height);
    fwrite(image, 1, width * height * 3, ppmFile);
    fclose(ppmFile);
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

    glGenTextures(1, &imageStruct.textureID);
    if (imageStruct.textureID == 0) {
        std::cerr << "glGenTextures - Falha ao gerar a textura (ID = 0)!" << std::endl;
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, imageStruct.textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageStruct.imageWidth, imageStruct.imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    if (imageStruct.textureID == 0) {
        std::cerr << "glTexImage2D - Falha ao gerar a textura (ID = 0)!" << std::endl;
        return false;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(data);
    return true;
}

void renderOverlappingTextures(const std::vector<Image>& images) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

    // Define as margens para os menus
    const float bottomMargin = 50.0f;
    const float topMargin = 100.0f;
    const float leftMargin = 50.0f;
    const float rightMargin = 50.0f;

    // Calcular a área disponível para renderização
    const float availableHeight = windowHeight - (topMargin + bottomMargin);
    const float availableWidth = windowWidth - (leftMargin + rightMargin);

    // Determinar o maior tamanho de imagem para calcular o fator global de escala
    int maxImageWidth = 0;
    int maxImageHeight = 0;

    // Encontrar o tamanho máximo das imagens
    for (const auto& img : images) {
        if (img.imageWidth > maxImageWidth) {
            maxImageWidth = img.imageWidth;
        }
        if (img.imageHeight > maxImageHeight) {
            maxImageHeight = img.imageHeight;
        }
    }

    // Garantir que o globalScaleFactor seja calculado corretamente com base na área disponível
    float globalScaleX = availableWidth / static_cast<float>(maxImageWidth);
    float globalScaleY = availableHeight / static_cast<float>(maxImageHeight);
    float globalScaleFactor = std::min(globalScaleX, globalScaleY); // Garantir que caiba na tela

    // Verificação do globalScaleFactor
    if (globalScaleFactor <= 0.0f) {
        printf("O fator de escala global é inválido! Verifique o tamanho da janela e das imagens.\n");
        return;
    }

    for (const auto& img : images) {
        float effectiveScaleFactor = globalScaleFactor; 

        if (img.zoomFactor != 1.0f) {
            // Multiplicar pelo zoom, se houver - está gerando erro quando vem carregado dos arquivos
            effectiveScaleFactor *= img.zoomFactor; 
        }

        float scaledWidth = img.imageWidth * effectiveScaleFactor;
        float scaledHeight = img.imageHeight * effectiveScaleFactor;

        float posX = leftMargin + (availableWidth - scaledWidth) / 2.0f + (img.offsetX * effectiveScaleFactor);
        float posY = bottomMargin + (availableHeight - scaledHeight) / 2.0f + (img.offsetY * effectiveScaleFactor);

        // Ativar e vincular a textura
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, img.textureID);
        if (img.textureID == 0) {
            printf("A textura não foi gerada corretamente!\n");
        }

        // Renderizar a textura (imagem)
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(posX, posY);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(posX + scaledWidth, posY);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(posX + scaledWidth, posY + scaledHeight);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(posX, posY + scaledHeight);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
}