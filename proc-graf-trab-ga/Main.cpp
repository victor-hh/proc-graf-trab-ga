#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Callbacks.h"
#include "ImageUtils.h"

GLuint textureID;
int width, height;

// Função para renderizar a textura na tela
void renderTexture() {
	// Limpa o buffer de cor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Cor de fundo (preto)
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Calcule a proporção da textura
	float aspectRatio = (float)width / (float)height;

	// Obtenha o tamanho da janela
	int windowWidth, windowHeight;
	glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);
	float windowAspectRatio = (float)windowWidth / (float)windowHeight;

	float quadWidth, quadHeight;

	// Calcule as dimensões do quadrado a ser desenhado, mantendo o aspecto da textura

	if (windowAspectRatio > aspectRatio) {
	quadHeight = 1.0f;         // A altura será 1.0f
	quadWidth = aspectRatio;   // A largura será proporcional
	} else {
		quadWidth = 1.0f;          // A largura será 1.0f
		quadHeight = 1.0f / aspectRatio; // A altura será proporcional
	}

	// Renderiza o quadrado centralizado na tela
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-quadWidth / 2, -quadHeight / 2);  // Inferior esquerdo
	glTexCoord2f(1.0f, 1.0f); glVertex2f(quadWidth / 2, -quadHeight / 2);  // Inferior direito
	glTexCoord2f(1.0f, 0.0f); glVertex2f(quadWidth / 2, quadHeight / 2);  // Superior direito
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-quadWidth / 2, quadHeight / 2);  // Superior esquerdo
	glEnd();

	glDisable(GL_TEXTURE_2D);
}


bool loadTexture(const char* filename) {
	unsigned char* data = loadPPM(filename, &width, &height);
	if (!data) {
		printf("Erro ao carregar a textura\n");
		return false;
	}

	// Gera uma textura OpenGL
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Carrega os dados da imagem como textura
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// Configura os parâmetros de filtro da textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Libera os dados da imagem carregada
	free(data);
	return true;
}

int main(int argc, char** argv) {

	glfwSetErrorCallback(glfw_error_callback);
	glfwInit();


	//criando uma janela
	GLFWwindow* window = glfwCreateWindow(1020, 700, "Nome da Aplicacao", nullptr, nullptr);

	// criando o contexto
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

	const char* inputFile = "stockImage\\rg.jpg";
	const char* outputFile = "stockImage\\output2.ppm";

	loadJPEGAndConvertToPPM(inputFile, outputFile);

	printf("Imagem convertida para PPM com sucesso!\n");

	// Carregar a textura PPM gerada
	if (!loadTexture(outputFile)) {
		printf("Erro ao carregar a textura PPM\n");
		glfwTerminate();
		return -1;
	}

	while (!glfwWindowShouldClose(window)) {
		// Limpa o buffer de cor
		glClear(GL_COLOR_BUFFER_BIT);

		// Renderiza a textura no quadrado
		renderTexture();
		
		// função que renderiza
		glfwSwapBuffers(window);
		//função que pega clicks de mouse
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;

}
