#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Callbacks.h"
#include "ImageUtils.h"
#include "HeaderMenu.h"
#include "ProjectStruct.h"
#include "ButtonActionLoadDirectory.h"

void setOrthoProjection(int windowWidth, int windowHeight) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void adjustFileNamesToAbsolutePath(Project& project) {
	std::string workingDirectory = project.workingDirectory;

	for (auto& imageStruct : project.images) {
		imageStruct.completeFileName = workingDirectory + "\\" + imageStruct.fileName + "." + imageStruct.fileFormat;

		imageStruct.completeOutputFile = workingDirectory + "\\" + imageStruct.fileName + ".ppm";

		std::cout << "completeFileName: " << imageStruct.completeFileName << std::endl;
		std::cout << "completeOutputFile: " << imageStruct.completeOutputFile << std::endl;
	}
}

int main(int argc, char** argv) {
	// adicionado para facilitar testes
	//Project project("C:\\Users\\victo\\processamento-grafico-test");
	
 	Project project(selectDirectory());
	if (!project.workingDirectory) {
		printf("Nenhum diretorio selecionado");
		return -1;
	}	
	printf("Working Directory: %s", project.workingDirectory);

	glfwSetErrorCallback(glfw_error_callback);
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1200, 600, "Nome da Aplicacao", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
	
	std::vector<Image> images = {
		{"Fotinha", "jpeg"},
		{"rg", "jpg"},
	};

	project.images = images;

	adjustFileNamesToAbsolutePath(project);

	for (auto& img : project.images) {
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

		renderHeaderAndButtons();
		for (auto& img : project.images) {
			renderTexture(img);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;

}
