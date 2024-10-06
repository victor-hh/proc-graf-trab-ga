#pragma once
#include "tinyfiledialogs.h"
#include <iostream>

// Função para abrir a caixa de diálogo e carregar o arquivo
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

