#pragma once
#include "tinyfiledialogs.h"
#include <iostream>
#include <png.h>
#include <jpeglib.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include "ImageUtils.h"

const char* openFileDialog() {
    const char* filterPatterns[] = { "*.jpg", "*.png", "*.ppm", "*.jpeg" };
    const char* fileName = tinyfd_openFileDialog("Selecionar Imagem", "", 4, filterPatterns, NULL, 0);

    if (fileName) {
        return fileName;
    }
    else {
        std::cout << "Nenhum arquivo foi selecionado." << std::endl;
        return "";
    }
}

const char* getFileName(const char* filePath) {
    const char* lastSlash = strrchr(filePath, '\\');

    if (lastSlash) {
        return lastSlash + 1;
    }

    return nullptr;
}

const char* getFileExtension(const char* fileName) {
    const char* lastDot = strrchr(fileName, '.');

    if (lastDot) {
        return lastDot + 1;
    }

    return nullptr;
}

void loadJPGAndConvertToPPM(Image& imageStruct, const char* fileNameWithPath) {
    FILE* infile;
    errno_t err = fopen_s(&infile, fileNameWithPath, "rb");
    if (err != 0) {
        printf("Erro ao abrir o arquivo JPEG\n");
        return;
    }

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

    unsigned long bmp_size = imageStruct.imageWidth * imageStruct.imageHeight * pixel_size;
    unsigned char* bmp_buffer = (unsigned char*)malloc(bmp_size);

    while (cinfo.output_scanline < imageStruct.imageHeight) {
        unsigned char* buffer_array[1];
        buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * imageStruct.imageWidth * pixel_size;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    writePPM(imageStruct.ppmFileNameWithPath.c_str(), bmp_buffer, imageStruct.imageWidth, imageStruct.imageHeight);

    free(bmp_buffer);
    fclose(infile);
}

//// Função para carregar um PNG e converter para um array de dados
//unsigned char* loadPNG(const char* filename, int* width, int* height, bool* hasAlpha) {
//    FILE* file;
//    errno_t err = fopen_s(&file, filename, "rb");
//    if (err != 0) {
//        printf("Erro ao abrir o arquivo JPEG\n");
//        return nullptr;
//    }
//
//    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
//    png_infop info = png_create_info_struct(png);
//    if (setjmp(png_jmpbuf(png))) {
//        fclose(file);
//        png_destroy_read_struct(&png, &info, nullptr);
//        return nullptr;
//    }
//
//    png_init_io(png, file);
//    png_read_info(png, info);
//
//    *width = png_get_image_width(png, info);
//    *height = png_get_image_height(png, info);
//    png_byte color_type = png_get_color_type(png, info);
//    png_byte bit_depth = png_get_bit_depth(png, info);
//
//    if (bit_depth == 16) png_set_strip_16(png);
//    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
//    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
//    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
//    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
//        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
//
//    *hasAlpha = (color_type == PNG_COLOR_TYPE_RGBA || color_type == PNG_COLOR_TYPE_GRAY_ALPHA);
//
//    png_read_update_info(png, info);
//
//    png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * (*height));
//    for (int y = 0; y < (*height); y++) {
//        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
//    }
//
//    png_read_image(png, row_pointers);
//    fclose(file);
//    png_destroy_read_struct(&png, &info, nullptr);
//
//    unsigned char* data = (unsigned char*)malloc((*width) * (*height) * (*hasAlpha ? 4 : 3));
//    int index = 0;
//    for (int y = 0; y < (*height); y++) {
//        memcpy(&data[index], row_pointers[y], png_get_rowbytes(png, info));
//        index += png_get_rowbytes(png, info);
//        free(row_pointers[y]);
//    }
//    free(row_pointers);
//
//    return data;
//}
//
//// Função para gerar textura OpenGL a partir dos dados do PNG
//GLuint generateTextureFromPNG(const char* filename) {
//    int width, height;
//    bool hasAlpha;
//    unsigned char* data = loadPNG(filename, &width, &height, &hasAlpha);
//
//    if (!data) {
//        printf("Erro ao carregar PNG\n");
//        return 0;
//    }
//
//    GLuint texture;
//    glGenTextures(1, &texture);
//    glBindTexture(GL_TEXTURE_2D, texture);
//
//    glTexImage2D(GL_TEXTURE_2D, 0, hasAlpha ? GL_RGBA : GL_RGB, width, height, 0, hasAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
//
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    free(data);
//    return texture;
//}

std::string generatePPMFileNameWithPath(const char* workingDirectory, const char* fileName) {
    std::string fileNameStr(fileName);
    size_t dotPosition = fileNameStr.find_last_of('.');
    if (dotPosition != std::string::npos) {
        fileNameStr = fileNameStr.substr(0, dotPosition);
    }

    std::string completeFileName = std::string(workingDirectory) + "\\" + fileNameStr + ".ppm";

    return completeFileName;
}

void loadImageFromFiles(Project* project) {
    const char* fileNameWithPath = openFileDialog();   
    
    if (fileNameWithPath && strcmp(fileNameWithPath, "") != 0) {
        const char* fileName = getFileName(fileNameWithPath);
        const char* fileExtension = getFileExtension(fileName);

        Image newImage = Image{ fileName, fileExtension, generatePPMFileNameWithPath(project->workingDirectory, fileName) };
        
        if (strcmp(fileExtension, "jpg") == 0 || strcmp(fileExtension, "jpeg") == 0) {
            loadJPGAndConvertToPPM(newImage, fileNameWithPath);
            if (!loadTexture(newImage)) {
                printf("Erro ao carregar a textura PPM\n");
                glfwTerminate();
            }
        } 
        //else if (strcmp(fileExtension, "png") == 0) {
        //    printf("carregando arquivo PNG");
        //    newImage.textureID = generateTextureFromPNG(fileNameWithPath);
        //    if (newImage.textureID == 0) {
        //        printf("A textura não foi gerada corretamente!\n");
        //    }
        //}

        project->images.push_back(newImage);
    }
}