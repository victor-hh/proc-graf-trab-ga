#pragma once
#include <cstdio>
//#include "Utilities.h"

static void glfw_error_callback(int error, const char* description) {
//	write_log(description);
	printf("Error: %s", description);
	throw("glfw error");
}

//static void glfw_window_close_callback(GLFWwindow* window) {
//	write_log("Window Closed\n");
//}

void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}