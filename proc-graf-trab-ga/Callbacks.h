#pragma once
#include <cstdio>
#include "ProjectStruct.h"
#include "HeaderMenu.h"

static void glfw_error_callback(int error, const char* description) {
	printf("Error: %s", description);
	throw("glfw error");
}

void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int windowHeight;
        glfwGetWindowSize(window, nullptr, &windowHeight);

        Project* project = static_cast<Project*>(glfwGetWindowUserPointer(window));

        mouseY = windowHeight - mouseY; // Inverte a coordenada Y
        checkButtonClick(buttons, mouseX, mouseY, project);
        checkClickOnLateralMenu(mouseX, mouseY);
    }
}