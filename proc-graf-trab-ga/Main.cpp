#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Callbacks.h"
#include "ImageUtils.h"
#include "HeaderMenu.h"
#include "ProjectStruct.h"
#include "ButtonActionLoadDirectory.h"
#include <iostream>
#include <fstream>
#include <vector>

void setOrthoProjection(int windowWidth, int windowHeight) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv) {
	// adicionado para facilitar testes
	Project project("C:\\Users\\victo\\processamento-grafico-test");
 	//Project project(selectDirectory());
	if (!project.workingDirectory) {
		printf("Nenhum diretorio selecionado");
		return -1;
	}

	glfwSetErrorCallback(glfw_error_callback);
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1200, 600, "Nome da Aplicacao", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
	// associa um ponteiro ao window para utilizar nos callbacks
	glfwSetWindowUserPointer(window, &project);

	// editor.meta
	std::vector<Image> images = {};
	project.images = images;

	// Registrar o callback para cliques de mouse
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	setOrthoProjection(windowWidth, windowHeight);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	generateButtons();
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		setOrthoProjection(windowWidth, windowHeight);

		renderHeaderAndButtons();
		renderLateralMenu(project.images);
		renderOverlappingTextures(project.images);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
