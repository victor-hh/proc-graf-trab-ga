#pragma once
#include <GLFW/glfw3.h>
#include <string>

struct Image {
	GLuint textureID;
	int imageWidth, imageHeight;
	// tornar dinâmico para utilizar em diversos casos 
	const char* fileName;
	const char* fileFormat;
	std::string completeFileName; 
	std::string completeOutputFile;

	Image(const char* fileName, const char* fileFormat)
		: textureID(0), imageWidth(0), imageHeight(0), fileName(fileName), fileFormat(fileFormat) {}
};
