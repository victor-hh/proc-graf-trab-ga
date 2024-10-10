#pragma once
#include <glad/glad.h>
#include "ImageStruct.h"
#include "ProjectStruct.h"
#include <vector>

unsigned char* getTextureData(GLuint textureID, int& width, int& height) {
	glBindTexture(GL_TEXTURE_2D, textureID);

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	unsigned char* data = new unsigned char[width * height * 3];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	return data;
}

void applyNegativeFilter(Project* project) {
	printf("Aplicando filtro em preto e branco!\n");
	for (auto& image : project->images) {
		int width, height;
		unsigned char* data = getTextureData(image.textureID, width, height);
		printf("Botão 4 clicado com a imagem, entao ta tentando passar o filtro!\n");
		for (int i = 0; i < width * height * 3; i += 3) {
			//aplicar o filtro
			data[i] = 255 - data[i];
			data[i + 1] = 255 - data[i + 1];
			data[i + 2] = 255 - data[i + 2];
		}
		GLuint newTextureID;
		glGenTextures(1, &newTextureID);
		glBindTexture(GL_TEXTURE_2D, newTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		image.textureID = newTextureID;

		delete[] data;
	}
}

void applyGrayscaleFilter(Project* project) {
	printf("Aplicando filtro em preto e branco!\n");
	for (auto& image : project->images) {
		int width, height;
		unsigned char* data = getTextureData(image.textureID, width, height);

		for (int i = 0; i < width * height * 3; i += 3) {
			unsigned char gray = static_cast<unsigned char>((data[i] + data[i + 1] + data[i + 2]) / 3);
			data[i] = gray;
			data[i + 1] = gray;
			data[i + 2] = gray;
		}

		GLuint newTextureID;
		glGenTextures(1, &newTextureID);
		glBindTexture(GL_TEXTURE_2D, newTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		image.textureID = newTextureID;

		delete[] data;
	}
}

void reverseImageOrder(Project* project) {
	std::reverse(project->images.begin(), project->images.end());
	printf("A ordem das imagens foi invertida!\n");
}

void applySepiaFilter(Project* project) {
	printf("Aplicando filtro sepia!\n");
	for (auto& image : project->images) {
		int width, height;
		unsigned char* data = getTextureData(image.textureID, width, height);

		for (int i = 0; i < width * height * 3; i += 3) {
			unsigned char originalR = data[i];     // Vermelho
			unsigned char originalG = data[i + 1]; // Verde
			unsigned char originalB = data[i + 2]; // Azul

			// Aplicar a transformação sepia
			unsigned char tr = static_cast<unsigned char>(std::min((0.393 * originalR) + (0.769 * originalG) + (0.189 * originalB), 255.0));
			unsigned char tg = static_cast<unsigned char>(std::min((0.349 * originalR) + (0.686 * originalG) + (0.168 * originalB), 255.0));
			unsigned char tb = static_cast<unsigned char>(std::min((0.272 * originalR) + (0.534 * originalG) + (0.131 * originalB), 255.0));

			// Atribuir os novos valores aos pixels
			data[i] = tr;     // Novo Vermelho
			data[i + 1] = tg; // Novo Verde
			data[i + 2] = tb; // Novo Azul
		}

		GLuint newTextureID;
		glGenTextures(1, &newTextureID);
		glBindTexture(GL_TEXTURE_2D, newTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		image.textureID = newTextureID;

		delete[] data;
	}
}
void applyAntiSepiaFilter(Project* project) {
	printf("Aplicando filtro anti-sepia!\n");
	for (auto& image : project->images) {
		int width, height;
		unsigned char* data = getTextureData(image.textureID, width, height);

		for (int i = 0; i < width * height * 3; i += 3) {
			unsigned char originalR = data[i];     // Vermelho
			unsigned char originalG = data[i + 1]; // Verde
			unsigned char originalB = data[i + 2]; // Azul

			// Aplicar uma transformação para neutralizar o sepia
			// Estes valores são exemplos, você pode ajustar conforme necessário
			unsigned char newR = static_cast<unsigned char>(std::min(originalR + 30, 255)); // Aumentar vermelho
			unsigned char newG = static_cast<unsigned char>(std::min(originalG - 15, 255)); // Reduzir verde
			unsigned char newB = static_cast<unsigned char>(std::min(originalB - 30, 255)); // Reduzir azul

			// Atribuir os novos valores aos pixels
			data[i] = newR;     // Novo Vermelho
			data[i + 1] = newG; // Novo Verde
			data[i + 2] = newB; // Novo Azul
		}

		GLuint newTextureID;
		glGenTextures(1, &newTextureID);
		glBindTexture(GL_TEXTURE_2D, newTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		image.textureID = newTextureID;

		delete[] data;
	}
}

