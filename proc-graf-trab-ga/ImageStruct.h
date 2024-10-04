#pragma once
#include <GLFW/glfw3.h>

struct Image {
	GLuint textureID;
	int imageWidth, imageHeight;
	// tornar dinâmico para utilizar em diversos casos 
	const char* inputFile;
	const char* outputFile;

	Image(const char* input, const char* output)
		: textureID(0), imageWidth(0), imageHeight(0), inputFile(input), outputFile(output) {}

};
