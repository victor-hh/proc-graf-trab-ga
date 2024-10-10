#pragma once
#include "ProjectStruct.h"
#include <jpeglib.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "ProjectStruct.h"
#include "ButtonActionLoadDirectory.h"
#include <iostream>
#include <fstream>
#include <vector>

// MUITO CUIDADO AO MEXER NESTE MÉTODO
unsigned char* createCombinedPPMData(const std::vector<Image>& images, int& combinedWidth, int& combinedHeight) {
	int maxImageWidth = 0;
	int maxImageHeight = 0;

	// Calcula as dimensões máximas
	for (const auto& img : images) {
		if (img.imageWidth > maxImageWidth) {
			maxImageWidth = img.imageWidth;
		}
		if (img.imageHeight > maxImageHeight) {
			maxImageHeight = img.imageHeight;
		}
	}

	// Margens
	const float topMargin = 0.0f;
	const float bottomMargin = 0.0f;
	const float leftMargin = 0.0f;
	const float rightMargin = 0.0f;
	combinedWidth = static_cast<int>(maxImageWidth + leftMargin + rightMargin);
	combinedHeight = static_cast<int>(maxImageHeight + topMargin + bottomMargin);

	// Aloca os dados combinados
	unsigned char* combinedData = new unsigned char[combinedWidth * combinedHeight * 3];
	std::memset(combinedData, 0, combinedWidth * combinedHeight * 3);

	// Calcula o fator de escala
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

		// Copia os dados da textura para a imagem combinada
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

		delete[] data; // Libera os dados da textura
	}

	return combinedData; // Retorna os dados combinados
}

void convertPPMtoJPG(unsigned char* imageData, int width, int height, const char* jpgFilename) {
	// Configura a estrutura de compressão JPEG
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	FILE* jpgFile = nullptr;
	errno_t err = fopen_s(&jpgFile, jpgFilename, "wb");
	if (err != 0 || !jpgFile) {
		std::cerr << "Erro ao criar o arquivo JPEG." << std::endl;
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
	jpeg_destroy_compress(&cinfo);

	std::cout << "Conversão de PPM para JPEG concluída: " << jpgFilename << std::endl;
}

void savePPMData(const unsigned char* combinedData, int combinedWidth, int combinedHeight, const char* outputFile) {
	std::ofstream file(outputFile, std::ios::binary);
	if (!file) {
		printf("Erro ao abrir o arquivo: %s\n", outputFile);
		return;
	}
	file << "P6\n" << combinedWidth << " " << combinedHeight << "\n255\n";
	file.write(reinterpret_cast<const char*>(combinedData), combinedWidth * combinedHeight * 3);
	file.close();
	printf("Imagem Salva Com Sucesso\n");
}

void saveTexturesAsJPG(const std::vector<Image>& images, const char* outputFile) {
	printf("outputFile: %s\n", outputFile);

	int combinedWidth, combinedHeight;
	unsigned char* combinedData = createCombinedPPMData(images, combinedWidth, combinedHeight);

	convertPPMtoJPG(combinedData, combinedWidth, combinedHeight, outputFile);

	delete[] combinedData;
}

void saveTexturesAsPPM(const std::vector<Image>& images, const char* outputFile) {
	printf("outputFile: %s\n", outputFile);

	int combinedWidth, combinedHeight;
	unsigned char* combinedData = createCombinedPPMData(images, combinedWidth, combinedHeight);

	savePPMData(combinedData, combinedWidth, combinedHeight, outputFile);

	delete[] combinedData; // Libera o buffer combinado
}

// MÉTODO REPETIDO, CONVÉM MOVER PARA CLASSE UTILITARIA
const char* getFileExtension2(const char* fileName) {
	const char* lastDot = strrchr(fileName, '.');

	if (lastDot) {
		return lastDot + 1;
	}

	return nullptr;
}

void saveImage(Project* project) {
	const char* filterPatterns[] = { "*.jpg", "*.png", "*.ppm", "*.jpeg" };
	const char* savePath = tinyfd_saveFileDialog("Salvar imagem como...", "", 1, filterPatterns, NULL);
	const char* fileExtension = getFileExtension2(savePath);

	printf("savePath: %s", savePath);

	if (savePath) {
		if (strcmp(fileExtension, "jpg") == 0 || strcmp(fileExtension, "jpeg") == 0) {
			saveTexturesAsJPG(project->images, savePath);
		} else if (strcmp(fileExtension, "ppm") == 0) {
			saveTexturesAsPPM(project->images, savePath);
		} 
	}
	else {
		std::cout << "Nenhum arquivo de destino escolhido para salvar." << std::endl;
		return;
	}
}

