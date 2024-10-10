#pragma once
#include <glad/glad.h>
#include "ImageStruct.h"
#include "ProjectStruct.h"
#include <vector>


unsigned char* getTextureData(GLuint textureID, int& width, int& height) {
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Obter a largura e a altura da textura
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	// Criar um buffer para os dados da textura
	unsigned char* data = new unsigned char[width * height * 3]; // 3 bytes por pixel (RGB)
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	return data;
}



void applyNegativeFilter(Project* project) {
	for (auto& image : project->images) {
		// Carregar os dados da textura
		int width, height;
		unsigned char* data = getTextureData(image.textureID, width, height);
		printf("Botão 4 clicado com a imagem, entao ta tentando passar o filtro!\n");
		// Aplicar o filtro negativo
		for (int i = 0; i < width * height * 3; i += 3) {

			//aplicar o filtro
			data[i] = 255 - data[i];     // R
			data[i + 1] = 255 - data[i + 1]; // G
			data[i + 2] = 255 - data[i + 2]; // B

		}

		// Criar uma nova textura com os dados modificados
		GLuint newTextureID;
		glGenTextures(1, &newTextureID);
		glBindTexture(GL_TEXTURE_2D, newTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Atualiza o ID da textura da imagem
		image.textureID = newTextureID;

		delete[] data; // Libera os dados da textura antiga
	}

}
void applyGrayscaleFilter(Project* project) {
	for (auto& image : project->images) {
		// Carregar os dados da textura
		int width, height;
		unsigned char* data = getTextureData(image.textureID, width, height);
		printf("Botão 5 clicado com a imagem!\n");

		// Aplicar o filtro de preto e branco
		for (int i = 0; i < width * height * 3; i += 3) {
			// Calcular a média dos valores RGB
			unsigned char gray = static_cast<unsigned char>((data[i] + data[i + 1] + data[i + 2]) / 3);

			// Aplicar a média a R, G e B
			data[i] = gray;       // R
			data[i + 1] = gray;   // G
			data[i + 2] = gray;   // B
		}

		// Criar uma nova textura com os dados modificados
		GLuint newTextureID;
		glGenTextures(1, &newTextureID);
		glBindTexture(GL_TEXTURE_2D, newTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Atualiza o ID da textura da imagem
		image.textureID = newTextureID;

		delete[] data; // Libera os dados da textura antiga
	}



}
void reverseImageOrder(Project* project) {
	std::reverse(project->images.begin(), project->images.end());
	printf("A ordem das imagens foi invertida!\n");
}