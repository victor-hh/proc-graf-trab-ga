#pragma once
#include <string>
#include <sstream>

std::string escapeJSONString(const std::string& input) {
    std::ostringstream escaped;
    for (char c : input) {
        switch (c) {
        case '"':  escaped << "\\\""; break;
        case '\\': escaped << "\\\\"; break;
        case '\b': escaped << "\\b"; break;
        case '\f': escaped << "\\f"; break;
        case '\n': escaped << "\\n"; break;
        case '\r': escaped << "\\r"; break;
        case '\t': escaped << "\\t"; break;
        default:   escaped << c; break;
        }
    }
    return escaped.str();
}

std::string generateProjectJson(Project* project) {
    std::ostringstream jsonStream;
    jsonStream << "{\n";
    jsonStream << "  \"workingDirectory\": \"" << escapeJSONString(project->workingDirectory) << "\",\n";
    jsonStream << "  \"images\": [\n";

    for (size_t i = 0; i < project->images.size(); ++i) {
        const Image& img = project->images[i];
        jsonStream << "    {\n";
        jsonStream << "      \"imageWidth\": " << img.imageWidth << ",\n";
        jsonStream << "      \"imageHeight\": " << img.imageHeight << ",\n";
        jsonStream << "      \"offsetX\": " << img.offsetX << ",\n";
        jsonStream << "      \"offsetY\": " << img.offsetY << ",\n";
        jsonStream << "      \"fileName\": \"" << escapeJSONString(img.fileName) << "\",\n";
        jsonStream << "      \"fileFormat\": \"" << escapeJSONString(img.fileFormat) << "\",\n";
        jsonStream << "      \"ppmFileNameWithPath\": \"" << escapeJSONString(img.ppmFileNameWithPath) << "\",\n";
        jsonStream << "      \"zoomFactor\": " << img.zoomFactor << "\n";
        jsonStream << "    }";
        if (i < project->images.size() - 1) {
            jsonStream << ",";
        }
        jsonStream << "\n";
    }

    jsonStream << "  ]\n";
    jsonStream << "}";

    return jsonStream.str();
}

void saveProjectJsonToFile(const std::string& content, const std::string& filePath) {
    std::ofstream outFile(filePath);

    if (outFile.is_open()) {
        outFile << content;
        outFile.close();
        std::cout << "Arquivo salvo com sucesso: " << filePath << std::endl;
    }
    else {
        std::cerr << "Erro ao abrir o arquivo: " << filePath << std::endl;
    }
}

void saveProject(Project* project) {
    const char* fileName = "\\project-meta.json";
    saveProjectJsonToFile(generateProjectJson(project), std::string(project->workingDirectory).append(fileName));
}