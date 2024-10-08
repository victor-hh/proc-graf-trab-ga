#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Callbacks.h"
#include "ImageUtils.h"
#include "HeaderMenu.h"
#include "ProjectStruct.h"
#include "ButtonActionLoadDirectory.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <jpeglib.h>

void setOrthoProjection(int windowWidth, int windowHeight) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void convertPPMtoJPG(const char* ppmFilename, const char* jpgFilename) {
	// Abre o arquivo PPM
	FILE* ppmFile = nullptr;
	errno_t err = fopen_s(&ppmFile, ppmFilename, "rb");
	if (err != 0 || !ppmFile) {
		std::cerr << "Erro ao abrir o arquivo PPM." << std::endl;
		return;
	}

	// Lê o cabeçalho PPM
	int width, height, maxVal;
	fscanf_s(ppmFile, "P6\n%d %d\n%d\n", &width, &height, &maxVal); // Usando fscanf_s

	// Aloca memória para os dados da imagem
	unsigned char* imageData = new unsigned char[width * height * 3];
	fread(imageData, sizeof(unsigned char), width * height * 3, ppmFile);
	fclose(ppmFile);

	// Configura a estrutura de compressão JPEG
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	FILE* jpgFile = nullptr;
	err = fopen_s(&jpgFile, jpgFilename, "wb");
	if (err != 0 || !jpgFile) {
		std::cerr << "Erro ao criar o arquivo JPEG." << std::endl;
		delete[] imageData;
		return;
	}

	jpeg_stdio_dest(&cinfo, jpgFile);

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3; // RGB
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_start_compress(&cinfo, TRUE);

	// Escreve os dados da imagem no arquivo JPEG
	while (cinfo.next_scanline < cinfo.image_height) {
		unsigned char* row_pointer[1];
		row_pointer[0] = &imageData[cinfo.next_scanline * width * 3];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(jpgFile);
	delete[] imageData;
	jpeg_destroy_compress(&cinfo);

	std::cout << "Conversão de PPM para JPEG concluída: " << jpgFilename << std::endl;
}

// Função para obter os dados da textura (implementação pode variar)
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

void saveTexturesAsPPM(const std::vector<Image>& images, const char* outputFile) {
	printf("outputFile: %s\n", outputFile);

	if (images.empty()) {
		printf("Nenhuma imagem para salvar.\n");
		return;
	}

	// Supondo que todas as texturas tenham a mesma largura e altura
	int combinedWidth = 0;
	int maxHeight = 0;

	//fixme ta bizzaro isso aqui, precisa fazer resize pra ficar num tamanho que faça sentido
	for (const auto& img : images) {
		combinedWidth += img.imageWidth; // Soma as larguras
		if (img.imageHeight > maxHeight) {
			maxHeight = img.imageHeight; // Mantém a maior altura
		}
	}

	// Cria um buffer para a imagem combinada
	unsigned char* combinedData = new unsigned char[combinedWidth * maxHeight * 3];
	std::memset(combinedData, 0, combinedWidth * maxHeight * 3); // Inicializa o buffer com zeros

	int offsetX = 0;

	for (const auto& img : images) {
		int width, height;
		unsigned char* data = getTextureData(img.textureID, width, height);

		//fixme melhorar tratamento
		if (!data) {
			printf("Erro ao obter dados da textura para ID %d\n", img.textureID);
			continue;
		}

		// Sobrepor a textura no buffer combinado
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				int combinedIndex = (y * combinedWidth * 3) + (offsetX + x) * 3;
				int originalIndex = (y * width + x) * 3;

				combinedData[combinedIndex] = data[originalIndex];     // R
				combinedData[combinedIndex + 1] = data[originalIndex + 1]; // G
				combinedData[combinedIndex + 2] = data[originalIndex + 2]; // B
			}
		}

		delete[] data; // Libera os dados da textura após o uso
		offsetX += width; // Move o offset para a próxima textura
	}

	// Salvar a imagem combinada como PPM
	printf("Salvando a imagem combinada como PPM\n");
	std::ofstream file(outputFile, std::ios::binary);
	if (!file) {
		printf("Erro ao abrir o arquivo %s para escrita\n", outputFile);
		delete[] combinedData; // Libera o buffer combinado em caso de erro
		return;
	}
	file << "P6\n" << combinedWidth << " " << maxHeight << "\n255\n";
	file.write(reinterpret_cast<char*>(combinedData), combinedWidth * maxHeight * 3);
	file.close();

	delete[] combinedData; // Libera o buffer combinado
}

int main(int argc, char** argv) {
	// adicionado para facilitar testes
	Project project("C:\\Users\\victo\\processamento-grafico-test");
 	//Project project(selectDirectory());
	if (!project.workingDirectory) {
		printf("Nenhum diretorio selecionado");
		return -1;
	}	

	glfwSetErrorCallback(glfw_error_callback);
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1200, 600, "Nome da Aplicacao", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
	// associa um ponteiro ao window para utilizar nos callbacks
	glfwSetWindowUserPointer(window, &project);

	// editor.meta
	std::vector<Image> images = {};
	project.images = images;

	// Registrar o callback para cliques de mouse
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	setOrthoProjection(windowWidth, windowHeight);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		setOrthoProjection(windowWidth, windowHeight);

		renderHeaderAndButtons();
		for (const auto& img : project.images) {
			renderTexture(img);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//saveFramebufferToPPM("C:\\Users\\victo\\processamento-grafico-test\\output.ppm", windowWidth, windowHeight);
	saveTexturesAsPPM(project.images, "C:\\Users\\victo\\processamento-grafico-test\\output.ppm");
	convertPPMtoJPG("C:\\Users\\victo\\processamento-grafico-test\\output.ppm", "C:\\Users\\victo\\processamento-grafico-test\\output.jpg");
	glfwTerminate();
	return 0;

}
