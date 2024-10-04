#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <functional>

// Estrutura para armazenar informa��es sobre o bot�o
struct Button {
    double x, y;            // Posi��o do bot�o
    double width, height;  // Tamanho do bot�o
    const char* label;      // Texto do bot�o
    std::function<void()> onClick; // Fun��o a ser chamada ao clicar
};

// Fun��es de callback espec�ficas para cada bot�o
void buttonOneClick() {
    printf("Bot�o 1 clicado!\n");
}

void buttonTwoClick() {
    printf("Bot�o 2 clicado!\n");
}

void buttonThreeClick() {
    printf("Bot�o 3 clicado!\n");
}

// Criar uma lista de bot�es
std::vector<Button> buttons = {
    {-1.0f, 1.0f, 0.2f, 0.2f, "Bot�o 1", buttonOneClick},
    {-0.8f, 1.0f, 0.2f, 0.2f, "Bot�o 2", buttonTwoClick},
    {-0.6f, 1.0f, 0.2f, 0.2f, "Bot�o 3", buttonThreeClick},
};

// Fun��o para renderizar um bot�o branco com texto
void renderButton(const Button& button) {
    // Cor do bot�o (branco)
    glColor3f(1.0f, 1.0f, 1.0f);

    // Renderizar o bot�o como um quadrado a partir do canto superior esquerdo
    glBegin(GL_QUADS);
    glVertex2f(button.x, button.y);                           // Canto superior esquerdo
    glVertex2f(button.x + button.width, button.y);            // Canto superior direito
    glVertex2f(button.x + button.width, button.y - button.height); // Canto inferior direito
    glVertex2f(button.x, button.y - button.height);           // Canto inferior esquerdo
    glEnd();
}

// Fun��o para verificar se um clique est� dentro de um bot�o
bool isClickOverButton(const Button& button, double mouseX, double mouseY) {
    return (mouseX >= button.x && mouseX <= button.x + button.width &&
        mouseY <= button.y && mouseY >= button.y - button.height);
}

// Fun��o que verifica em qual bot�o o clique ocorreu e chama a fun��o `onClick`
void checkButtonClick(const std::vector<Button>& buttons, double mouseX, double mouseY) {
    for (const Button& button : buttons) {
        if (isClickOverButton(button, mouseX, mouseY)) {
            button.onClick();
            return; 
        }
    }
    std::cout << "Nenhum botao clicado." << std::endl;
}

// Callback para cliques de mouse
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY); // Pega a posi��o do mouse

        // Converter coordenadas da tela para coordenadas do OpenGL (-1.0 a 1.0)
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        mouseX = (mouseX / windowWidth) * 2 - 1; // Converte de pixels para [-1, 1]
        mouseY = 1 - (mouseY / windowHeight) * 2; // Converte de pixels para [-1, 1]

        // Verificar em qual bot�o o clique ocorreu
        checkButtonClick(buttons, mouseX, mouseY);
    }
}

void renderHeader() {
    for (const auto& btn : buttons) {
        renderButton(btn);
    }
}