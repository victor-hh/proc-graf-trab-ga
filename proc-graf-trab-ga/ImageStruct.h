#pragma once
#include <GLFW/glfw3.h>
#include <string>

struct Image {
	GLuint textureID;
	int imageWidth, imageHeight;
	float offsetX, offsetY;
	const char* fileName;
	const char* fileFormat;
	std::string ppmFileNameWithPath;

	Image(const char* fileName, const char* fileFormat)
		: textureID(0), imageWidth(0), imageHeight(0), fileName(fileName), fileFormat(fileFormat) {}

	Image(const char* fileName, const char* fileFormat, std::string ppmFileNameWithPath)
		: textureID(0), imageWidth(0), imageHeight(0), offsetX(0), offsetY(0), fileName(fileName), fileFormat(fileFormat), ppmFileNameWithPath(ppmFileNameWithPath) {}
};
