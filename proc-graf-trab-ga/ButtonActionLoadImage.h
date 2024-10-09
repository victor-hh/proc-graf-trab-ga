#pragma once
#include "tinyfiledialogs.h"
#include <iostream>
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
        
        if (project->images.size() == 1) {
            newImage.offsetX = 150;
            newImage.offsetY = 300;
        }

        if (strcmp(fileExtension, "jpg") == 0) {
            loadJPGAndConvertToPPM(newImage, fileNameWithPath);
            if (!loadTexture(newImage)) {
                printf("Erro ao carregar a textura PPM\n");
                glfwTerminate();
            }
        }
        project->images.push_back(newImage);
    }
}