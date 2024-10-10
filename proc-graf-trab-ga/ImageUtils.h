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

//bool loadTexture(Image& imageStruct) {
//    unsigned char* data = loadPPM(imageStruct.ppmFileNameWithPath.c_str(), &imageStruct.imageWidth, &imageStruct.imageHeight);
//    if (!data) {
//        printf("Erro ao carregar a textura\n");
//        return false;
//    }
//
//    // Verifica se a imagem tem canal alpha ou não (RGB ou RGBA)
//    GLenum format = (imageStruct.hasAlphaChannel) ? GL_RGBA : GL_RGB;
//
//    // Gera uma textura OpenGL
//    glGenTextures(1, &imageStruct.textureID);
//    if (imageStruct.textureID == 0) {
//        std::cerr << "glGenTextures - Falha ao gerar a textura (ID = 0)!" << std::endl;
//        free(data);
//        return false;
//    }
//
//    glBindTexture(GL_TEXTURE_2D, imageStruct.textureID);
//
//    // Garante o alinhamento correto dos dados da textura
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//    // Carrega os dados da imagem como textura (verifica se é RGB ou RGBA)
//    glTexImage2D(GL_TEXTURE_2D, 0, format, imageStruct.imageWidth, imageStruct.imageHeight, 0, format, GL_UNSIGNED_BYTE, data);
//    if (glGetError() != GL_NO_ERROR) {
//        std::cerr << "glTexImage2D - Falha ao gerar a textura!" << std::endl;
//        free(data);
//        return false;
//    }
//
//    // Configura os parâmetros de filtro da textura
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    // Verifica se houve erro durante as configurações de filtro
//    GLenum err = glGetError();
//    if (err != GL_NO_ERROR) {
//        printf("Erro OpenGL ao definir parâmetros de textura: %d\n", err);
//        free(data);
//        return false;
//    }
//
//    // Libera os dados da imagem carregada
//    free(data);
//    return true;
//}

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

void renderOverlappingTextures(const std::vector<Image>& images) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

    // Definir margens (atualmente 0 para esquerda e direita, mas ajustáveis)

    const float bottomMargin = 50.0f;
    const float topMargin = 100.0f;
    const float leftMargin = 50.0f;
    const float rightMargin = 50.0f;
    const float availableHeight = windowHeight - (topMargin + bottomMargin);
    const float availableWidth = windowWidth - (leftMargin + rightMargin);

    // Determina a largura e altura máxima das texturas
    int maxImageWidth = 0;
    int maxImageHeight = 0;

    for (const auto& img : images) {
        if (img.imageWidth > maxImageWidth) {
            maxImageWidth = img.imageWidth;
        }
        if (img.imageHeight > maxImageHeight) {
            maxImageHeight = img.imageHeight;
        }
    }

    // Calcula o fator de escala para caber na janela, considerando as margens
    float scaleX = availableWidth / maxImageWidth;
    float scaleY = availableHeight / maxImageHeight;
    float scaleFactor = std::min(scaleX, scaleY);  // Usa o menor fator para garantir que caiba na janela
    // Ajusta o tamanho final de cada imagem
    for (const auto& img : images) {
        float scaledWidth = img.imageWidth * scaleFactor;
        float scaledHeight = img.imageHeight * scaleFactor;

        // Calcula a posição centralizada, com margens aplicadas
        float posX = leftMargin + (availableWidth - scaledWidth) / 2.0f + (img.offsetX * scaleFactor);
        float posY = bottomMargin + (availableHeight - scaledHeight) / 2.0f + (img.offsetY * scaleFactor);

        // Ativa e vincula a textura
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, img.textureID);
        if (img.textureID == 0) {
            printf("A textura não foi gerada corretamente!\n");
        }

        // Renderiza a textura com as coordenadas escaladas
        glBegin(GL_QUADS);
        // Inferior esquerdo
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(posX, posY);
        // Inferior direito
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(posX + scaledWidth, posY);
        // Superior direito
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(posX + scaledWidth, posY + scaledHeight);
        // Superior esquerdo
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(posX, posY + scaledHeight);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
}