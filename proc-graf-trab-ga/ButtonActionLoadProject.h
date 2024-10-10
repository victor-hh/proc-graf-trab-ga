#pragma once

const char* selectWorkingDirectory() {
    const char* dir = tinyfd_selectFolderDialog("Selecionar diretório de trabalho", nullptr);

    if (dir) {
        return dir;
    }
    else {
        return nullptr;
    }
}

std::string getValue(const std::string& json, const std::string& key, size_t startPos = 0) {
    size_t start = json.find(key, startPos);
    if (start == std::string::npos) return "";
    start = json.find(":", start);
    size_t end = json.find(",", start);
    if (end == std::string::npos) end = json.find("}", start);
    std::string value = json.substr(start + 1, end - start - 1);

    size_t first = value.find_first_not_of(" \t\n\r\"");
    size_t last = value.find_last_not_of(" \t\n\r\"");
    return value.substr(first, last - first + 1);
}

size_t findNextImage(const std::string& json, size_t startPos) {
    return json.find("{", startPos);
}

void loadProject(Project* project) {
    const char* newWorkingDirectory = selectWorkingDirectory();
    printf("%s\n", newWorkingDirectory);

    project->workingDirectory = newWorkingDirectory;
    project->images.clear();

    std::string jsonFilePath = std::string(newWorkingDirectory) + "\\project-meta.json";

    std::ifstream jsonFile(jsonFilePath.c_str());
    if (!jsonFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo JSON!" << std::endl;
        return;
    }

    std::string jsonContent((std::istreambuf_iterator<char>(jsonFile)),
        std::istreambuf_iterator<char>());

    jsonFile.close();

    size_t imagePos = jsonContent.find("\"images\"");
    imagePos = jsonContent.find("[", imagePos);
    size_t endPos = jsonContent.find("]", imagePos);

    while ((imagePos = findNextImage(jsonContent, imagePos)) != std::string::npos && imagePos < endPos) {
        int imageWidth = std::stoi(getValue(jsonContent, "imageWidth", imagePos));
        int imageHeight = std::stoi(getValue(jsonContent, "imageHeight", imagePos));
        float offsetX = std::stoi(getValue(jsonContent, "offsetX", imagePos));
        float offsetY = std::stoi(getValue(jsonContent, "offsetY", imagePos));
        std::string fileName = getValue(jsonContent, "fileName", imagePos);
        std::string fileFormat = getValue(jsonContent, "fileFormat", imagePos);
        std::string ppmFileNameWithPath = getValue(jsonContent, "ppmFileNameWithPath", imagePos);
        float zoomFactor = std::stoi(getValue(jsonContent, "zoomFactor", imagePos));

        Image loadedImage(imageWidth, imageHeight, offsetX, offsetY, fileName, fileFormat, ppmFileNameWithPath, zoomFactor);
        if (!loadTexture(loadedImage)) {
            printf("Erro ao carregar a textura PPM de um arquivo\n");
            glfwTerminate();
        }
        project->images.push_back(loadedImage);
        imagePos++;
    }

}