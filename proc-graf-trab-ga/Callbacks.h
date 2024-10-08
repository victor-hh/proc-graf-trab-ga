#pragma once
#include <cstdio>

static void glfw_error_callback(int error, const char* description) {
	printf("Error: %s", description);
	throw("glfw error");
}

void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}