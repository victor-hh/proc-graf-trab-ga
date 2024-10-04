#pragma once
#include <stdio.h>
#include <jpeglib.h>
#include <stdlib.h>
#include <GLFW/glfw3.h> // Ou outro header OpenGL de sua escolha (GLFW, SDL, etc.)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int pixel_size = cinfo.output_components;

    // Aloca memória para a imagem decodificada
    unsigned long bmp_size = width * height * pixel_size;
    unsigned char* bmp_buffer = (unsigned char*)malloc(bmp_size);

    // Decodifica a imagem JPEG
    while (cinfo.output_scanline < height) {
        unsigned char* buffer_array[1];
        buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * width * pixel_size;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    // Finaliza a decompressão
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);


    printf("69.width: %i", width);
    printf("70.height: %i", height);

    // Converte e salva como PPM
    writePPM(outputFile, bmp_buffer, width, height);

    // Libera a memória e fecha o arquivo
    free(bmp_buffer);
    fclose(infile);
}


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
