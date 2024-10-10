#pragma once
#include <GLFW/glfw3.h>
#include <string>

struct Image {
    GLuint textureID;
    int imageWidth, imageHeight;
    float offsetX, offsetY;
    std::string fileName;
    std::string fileFormat;
    std::string ppmFileNameWithPath;
    float zoomFactor;

    Image(std::string& fileName, std::string& fileFormat)
        : textureID(0), imageWidth(0), imageHeight(0), offsetX(0), offsetY(0),
        fileName(fileName), fileFormat(fileFormat), zoomFactor(1.0f) {}

    Image(std::string& fileName, std::string& fileFormat, std::string ppmFileNameWithPath)
        : textureID(0), imageWidth(0), imageHeight(0), offsetX(0), offsetY(0),
        fileName(fileName), fileFormat(fileFormat), ppmFileNameWithPath(ppmFileNameWithPath),
        zoomFactor(1.0f) {}
};
