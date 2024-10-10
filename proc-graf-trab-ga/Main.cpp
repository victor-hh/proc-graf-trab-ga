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
	fscanf_s(ppmFile, "P6\n%d %d\n%d\n", &width, &height, &maxVal);

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

void saveTexturesAsPPM(const std::vector<Image>& images, const char* outputFile) {
	printf("outputFile: %s\n", outputFile);

	int maxImageWidth = 0;
	int maxImageHeight = 0;

	for (const auto& img : images) {
		if (img.imageWidth > maxImageWidth) {
			maxImageWidth = img.imageWidth;
		}
		if (img.imageHeight > maxImageHeight) {
			maxImageHeight = img.imageHeight;
		}
	}

	// NÃO PRECISA, MAS SE TIRAR QUEBRA(???)
	const float topMargin = 0.0f;
	const float bottomMargin = 0.0f;
	const float leftMargin = 0.0f;
	const float rightMargin = 0.0f;
	const float combinedWidth = (maxImageWidth + leftMargin + rightMargin);
	const float combinedHeight = (maxImageHeight + topMargin + bottomMargin);

	unsigned char* combinedData = new unsigned char[combinedWidth * combinedHeight * 3];
	std::memset(combinedData, 0, combinedWidth * combinedHeight * 3);

	// Calcula o fator de escala para caber na imagem
	float scaleX = combinedWidth / static_cast<float>(maxImageWidth);
	float scaleY = combinedHeight / static_cast<float>(maxImageHeight);
	float scaleFactor = std::min(scaleX, scaleY);

	for (const auto& img : images) {
		float scaledWidth = img.imageWidth * scaleFactor;
		float scaledHeight = img.imageHeight * scaleFactor;

		float posX = (combinedWidth - scaledWidth) / 2.0f + (img.offsetX * scaleFactor);
		float posY = (combinedHeight - scaledHeight) / 2.0f - (img.offsetY * scaleFactor);

		int width, height;
		unsigned char* data = getTextureData(img.textureID, width, height);

		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				int combinedIndex = ((static_cast<int>(posY) + y) * combinedWidth * 3) + ((static_cast<int>(posX) + x) * 3);
				int originalIndex = (y * width + x) * 3;

				if (combinedIndex < combinedWidth * combinedHeight * 3) {
					combinedData[combinedIndex] = data[originalIndex];     // R
					combinedData[combinedIndex + 1] = data[originalIndex + 1]; // G
					combinedData[combinedIndex + 2] = data[originalIndex + 2]; // B
				}
			}
		}

		delete[] data;
	}

	std::ofstream file(outputFile, std::ios::binary);
	file << "P6\n" << combinedWidth << " " << combinedHeight << "\n255\n";
	file.write(reinterpret_cast<char*>(combinedData), combinedWidth * combinedHeight * 3);
	file.close();
	printf("Imagem Salva Com Sucesso");
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		setOrthoProjection(windowWidth, windowHeight);

		renderHeaderAndButtons();
		renderLateralMenu(project.images);
		renderOverlappingTextures(project.images);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//saveFramebufferToPPM("C:\\Users\\victo\\processamento-grafico-test\\output.ppm", windowWidth, windowHeight);
	saveTexturesAsPPM(project.images, "C:\\Users\\victo\\processamento-grafico-test\\output.ppm");
	convertPPMtoJPG("C:\\Users\\victo\\processamento-grafico-test\\output.ppm", "C:\\Users\\victo\\processamento-grafico-test\\output.jpg");
	glfwTerminate();
	return 0;

}
