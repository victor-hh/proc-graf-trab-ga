#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Callbacks.h"
#include "ImageUtils.h"
#include "HeaderMenu.h"
#include "ImageStruct.h"

void setOrthoProjection(int windowWidth, int windowHeight) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv) {
	glfwSetErrorCallback(glfw_error_callback);
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1200, 600, "Nome da Aplicacao", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
	
	std::vector<Image> images = {
		{"stockImage\\rg.jpg", "stockImage\\output.ppm"},
//		{"stockImage\\Fotinha.jpeg", "stockImage\\output2.ppm"}
	};

	for (auto& img : images) {
		loadJPEGAndConvertToPPM(img);
		if (!loadTexture(img)) {
			printf("Erro ao carregar a textura PPM\n");
			glfwTerminate();
			return -1;
		}
	}

	// Registrar o callback para cliques de mouse
	glfwSetMouseButtonCallback(window, mouse_button_callback);


	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	setOrthoProjection(windowWidth, windowHeight);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		setOrthoProjection(windowWidth, windowHeight);

		for (auto& img : images) {
			renderTexture(img);
		}
		renderHeaderAndButtons();
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;

}
