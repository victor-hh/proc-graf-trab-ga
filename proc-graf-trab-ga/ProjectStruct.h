#pragma once
#include <vector>
#include "ImageStruct.h"

struct Project {
	const char* workingDirectory;
	std::vector<Image> images;

	Project(const char* workingDirectory)
		: workingDirectory(workingDirectory), images() {};
};
