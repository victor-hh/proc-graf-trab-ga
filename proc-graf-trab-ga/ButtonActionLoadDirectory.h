#pragma once
#include "tinyfiledialogs.h"
#include <iostream>

const char* selectDirectory() {
    const char* dir = tinyfd_selectFolderDialog("Selecionar diret�rio de trabalho", nullptr);

    if (dir) {
        return dir;
    }
    else {
        return nullptr;
    }
}