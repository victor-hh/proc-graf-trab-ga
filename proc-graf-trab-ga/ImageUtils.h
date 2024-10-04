#pragma once
#include <stdio.h>
#include <jpeglib.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "ImageStruct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//GLuint textureID;
//int imageWidth, imageHeight;

// Função para salvar a imagem no formato PPM
void writePPM(const char* filename, unsigned char* image, int width, int height) {
    FILE* f;
    errno_t err = fopen_s(&f, filename, "wb");
    if (err != 0) {
        printf("Erro ao abrir o arquivo para escrita\n");
        return;
    }

    // Cabeçalho do formato PPM (P6)
    printf("writePPM.width: %i", width);
    printf("writePPM.height: %i", height);

    fprintf(f, "P6\n%d %d\n255\n", width, height);

    // Escreva os dados da imagem (RGB)
    fwrite(image, 1, width * height * 3, f);
    fclose(f);
}

// Função para carregar o JPEG e convertê-lo para PPM
void loadJPEGAndConvertToPPM(Image& imageStruct) {
    FILE* infile;
    errno_t err = fopen_s(&infile, imageStruct.inputFile, "rb");
    if (err != 0) {
        printf("Erro ao abrir o arquivo JPEG\n");
        return;
    }

    // Estruturas de libjpeg
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    
    imageStruct.imageWidth = cinfo.output_width;
    imageStruct.imageHeight = cinfo.output_height;
    int pixel_size = cinfo.output_components;

    // Aloca memória para a imagem decodificada
    unsigned long bmp_size = imageStruct.imageWidth * imageStruct.imageHeight * pixel_size;
    unsigned char* bmp_buffer = (unsigned char*)malloc(bmp_size);

    // Decodifica a imagem JPEG
    while (cinfo.output_scanline < imageStruct.imageHeight) {
        unsigned char* buffer_array[1];
        buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * imageStruct.imageWidth * pixel_size;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    // Finaliza a decompressão
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);


    printf("69.width: %i", imageStruct.imageWidth);
    printf("70.height: %i", imageStruct.imageHeight);

    // Converte e salva como PPM
    writePPM(imageStruct.outputFile, bmp_buffer, imageStruct.imageWidth, imageStruct.imageHeight);

    // Libera a memória e fecha o arquivo
    free(bmp_buffer);
    fclose(infile);
}

/*
// Função para carregar o JPEG e convertê-lo para PPM
void loadJPEGAndConvertToPPM(const char* inputFile, const char* outputFile) {
    FILE* infile;
    errno_t err = fopen_s(&infile, inputFile, "rb");
    if (err != 0) {
        printf("Erro ao abrir o arquivo JPEG\n");
        return;
    }

    // Estruturas de libjpeg
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    imageWidth = cinfo.output_width;
    imageHeight = cinfo.output_height;
    int pixel_size = cinfo.output_components;

    // Aloca memória para a imagem decodificada
    unsigned long bmp_size = imageWidth * imageHeight * pixel_size;
    unsigned char* bmp_buffer = (unsigned char*)malloc(bmp_size);

    // Decodifica a imagem JPEG
    while (cinfo.output_scanline < imageHeight) {
        unsigned char* buffer_array[1];
        buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * imageWidth * pixel_size;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    // Finaliza a decompressão
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);


    printf("69.width: %i", imageWidth);
    printf("70.height: %i", imageHeight);

    // Converte e salva como PPM
    writePPM(outputFile, bmp_buffer, imageWidth, imageWidth);

    // Libera a memória e fecha o arquivo
    free(bmp_buffer);
    fclose(infile);
}*/


unsigned char* loadPPM(const char* filename, int* width, int* height) {
    FILE* fp;
    errno_t err = fopen_s(&fp, filename, "rb");
    if (err != 0) {
        printf("Erro ao abrir o arquivo PPM\n");
        return NULL;
    }

    char format[3];
    fscanf_s(fp, "%2s", format, (unsigned)_countof(format));  // Lê o formato ("P6")

    if (strcmp(format, "P6") != 0) {
        printf("Formato PPM não suportado\n");
        fclose(fp);  // Fechar o arquivo se o formato estiver incorreto
        return NULL;
    }

    // Ignorar comentários (se houver)
    char c;
    while ((c = fgetc(fp)) == '#') {
        while (fgetc(fp) != '\n');  // Ignora a linha
    }
    ungetc(c, fp);  // Retorna o último caractere lido

    // Lê a largura, altura e o valor máximo da cor
    int maxval;
    fscanf_s(fp, "%d %d %d", width, height, &maxval);

    if (maxval != 255) {
        printf("O valor máximo da cor não é 255\n");
        fclose(fp);  // Fechar o arquivo se o valor máximo estiver incorreto
        return NULL;
    }

    // Ignorar o último caractere antes dos dados
    fgetc(fp);

    // Aloca memória para os dados da imagem
    int imageSize = (*width) * (*height) * 3;  // 3 componentes (R, G, B)
    unsigned char* data = (unsigned char*)malloc(imageSize);

    if (data == NULL) {
        printf("Falha ao alocar memória para a imagem\n");
        fclose(fp);  // Fechar o arquivo se a alocação falhar
        return NULL;
    }

    // Lê os dados da imagem
    fread(data, 1, imageSize, fp);
    fclose(fp);

    return data;
}

float convertX(int pixelX, int windowWidth) {

    float result = (2.0f * pixelX / windowWidth);
    printf("Resultado da expressão - (2.0f * pixelX / windowWidth) - 1.0f: %f\n", result);
    return (2.0f * pixelX / windowWidth) - 1.0f; // Converte para o intervalo [-1, 1]
}

float convertY(int pixelY, int windowHeight) {
    float result = 1.0f - (2.0f * pixelY / windowHeight);
    printf("Resultado da expressão - 1.0f - (2.0f * pixelY / windowHeight): %f\n", result);
    return 1.0f - (2.0f * pixelY / windowHeight); // Inverte Y e converte para o intervalo [-1, 1]
}

void renderTexture(Image& imageStruct) {
    int posX = 0;
    int posY = 0;
    // Limpa o buffer de cor
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Cor de fundo (preto)
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, imageStruct.textureID);

    // Obtenha as dimensões da janela
    int windowWidth, windowHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

    // Calcule a proporção da textura
    float aspectRatio = static_cast<float>(imageStruct.imageWidth) / static_cast<float>(imageStruct.imageHeight);

    // Determine a largura e altura que respeitam a proporção
    float quadWidth, quadHeight;

    // Calcule a largura e a altura em função da posição fornecida
    quadWidth = static_cast<float>(imageStruct.imageWidth);
    quadHeight = static_cast<float>(imageStruct.imageHeight);

    // Verifique se a imagem ultrapassa a janela e ajuste a posição se necessário
    if (posX < 0) posX = 0;
    if (posY < 0) posY = 0;

    // Renderiza a imagem na posição especificada
    glBegin(GL_QUADS);
    // Inferior esquerdo
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(convertX(posX, windowWidth), convertY(posY + quadHeight, windowHeight)); // Ajuste para a base
    // Inferior direito
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(convertX(posX + quadWidth, windowWidth), convertY(posY + quadHeight, windowHeight)); // Ajuste para a base
    // Superior direito
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(convertX(posX + quadWidth, windowWidth), convertY(posY, windowHeight));
    // Superior esquerdo
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(convertX(posX, windowWidth), convertY(posY, windowHeight));
    glEnd();

    glDisable(GL_TEXTURE_2D);
}


/*
void renderTexture(Image& imageStruct) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Cor de fundo (preto)
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, imageStruct.textureID);

    // Calcule a proporção da textura
    float aspectRatio = static_cast<float>(imageStruct.imageWidth) / static_cast<float>(imageStruct.imageHeight);

    int windowWidth, windowHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

    printf("windowWidth: %i", windowWidth);
    printf("windowHeight: %i", windowHeight);

    float maxHeight = 1.8f;
    float maxWidth = 1.8f;

    float width = (maxHeight * aspectRatio);

    float xInferiorEsquerdo = -1.0f;
    float yInferiorEsquerdo = -1.0f;
    
    float xSuperiorEsquerdo = -1.0f;
    float ySuperiorEsquerdo = 0.8f;

    float xInferiorDireito = xInferiorEsquerdo + width;
    float yInferiorDireito = yInferiorEsquerdo;

    float xSuperiorDireito = xInferiorEsquerdo + width;
    float ySuperiorDireito = ySuperiorEsquerdo;


    printf("AspectRation: %f\n", aspectRatio);
    printf("Width: %f\n", width);
    glBegin(GL_QUADS);
    // Superior esquerdo
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(xSuperiorEsquerdo, ySuperiorEsquerdo);
    // Superior direito
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(xSuperiorDireito, ySuperiorDireito);
    // Inferior Direito
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(xInferiorDireito, yInferiorDireito);
    // Inferior Esquerdo
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(xInferiorEsquerdo, yInferiorEsquerdo);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}*/

bool loadTexture(Image& imageStruct) {
    unsigned char* data = loadPPM(imageStruct.outputFile, &imageStruct.imageWidth, &imageStruct.imageHeight);
    if (!data) {
        printf("Erro ao carregar a textura\n");
        return false;
    }

    // Gera uma textura OpenGL
    glGenTextures(1, &imageStruct.textureID);
    glBindTexture(GL_TEXTURE_2D, imageStruct.textureID);

    // Carrega os dados da imagem como textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageStruct.imageWidth, imageStruct.imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // Configura os parâmetros de filtro da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Libera os dados da imagem carregada
    free(data);
    return true;
}

