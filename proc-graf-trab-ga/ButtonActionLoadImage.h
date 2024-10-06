#pragma once
#include "tinyfiledialogs.h"
#include <iostream>

// Fun��o para abrir a caixa de di�logo e carregar o arquivo
std::string openFileDialog(Project& project) {
    const char* filterPatterns[] = { "*.jpg", "*.png", "*.ppm", "*.jpeg" }; // Tipos de arquivos permitidos
    const char* fileName = tinyfd_openFileDialog("Selecionar Imagem", "", 4, filterPatterns, NULL, 0);

    if (fileName) {
        std::cout << "Arquivo selecionado: " << fileName << std::endl;
        project.workingDirectory = fileName;
        return std::string(fileName);
    }
    else {
        std::cout << "Nenhum arquivo foi selecionado." << std::endl;
        return "";
    }
}

