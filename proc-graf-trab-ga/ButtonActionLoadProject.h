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

//void createNewProject(Project** project, const char* newWorkingDirectory) {
//    printf("%s\n", newWorkingDirectory);
//    if (*project != nullptr) {
//        printf("passou no if\n");
//        delete *project;
//        printf("passou no if\n");
//    }
//    *project << Project project(newWorkingDirectory);
//
//    //std::cout << "Novo projeto criado com o diretório de trabalho: " << newWorkingDirectory << std::endl;
//}


void loadProject(Project* project) {
    const char* newWorkingDirectory = selectWorkingDirectory();
    printf("%s\n", newWorkingDirectory);

    project->workingDirectory = newWorkingDirectory;
    project->images.clear();
    //createNewProject(&project, newWorkingDirectory);
    //std::cout << "Novo projeto criado com o diretório de trabalho: " << project->workingDirectory << std::endl;
}