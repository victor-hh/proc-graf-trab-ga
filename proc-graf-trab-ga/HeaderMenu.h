#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <functional>
#include "ButtonActionLoadImage.h"
#include "LateralMenu.h"
#include "FilterFunctions.h"

// Estrutura para armazenar informações sobre o botão
struct Button {
    double x, y;
    double width, height = 50;
    std::function<void(Project*)> onClick;

    Button(std::function<void(Project*)> onClick)
        : x(0), y(0), width(50.0), height(50.0), onClick(onClick) {}
};

// Funções de callback específicas para cada botão
void buttonOneClick(Project* project) {
    printf("Botão 1 clicado!\n");
}

void buttonTwoClick(Project* project) {
    printf("Botão 2 clicado!\n");
}

void buttonThreeClick(Project* project) {
    printf("Botão 3 clicado!\n");
}

std::vector<Button> buttons = {
    {loadImageFromFiles},
    {buttonTwoClick},
    {buttonThreeClick},
    {applyNegativeFilter},
    {applyGrayscaleFilter},
    {reverseImageOrder}
};

void renderButton(const Button& button) {
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glVertex2f(button.x, button.y);                           // Canto superior esquerdo
    glVertex2f(button.x + button.width, button.y);            // Canto superior direito
    glVertex2f(button.x + button.width, button.y - button.height); // Canto inferior direito
    glVertex2f(button.x, button.y - button.height);           // Canto inferior esquerdo
    glEnd();
}

bool isClickOverButton(const Button& button, double mouseX, double mouseY) {
    return (mouseX >= button.x && mouseX <= button.x + button.width &&
        mouseY >= button.y - button.height && mouseY <= button.y);
}

// Função que verifica em qual botão o clique ocorreu e chama a função `onClick`
void checkButtonClick(const std::vector<Button>& buttons, double mouseX, double mouseY, Project* project) {
    for (const Button& button : buttons) {
        if (isClickOverButton(button, mouseX, mouseY)) {
            button.onClick(project);
            return;
        }
    }
    std::cout << "Nenhum botão clicado." << std::endl;
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

// site pra converter de rgb: https://www.colorhexa.com/abdbe3
void renderBackgroundBanner(int heigh) {
    glColor3f(0.6f, 0.6f, 0.6f);

    int windowWidth, windowHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

    glBegin(GL_QUADS);
    glVertex2f(0, windowHeight);           // Canto superior esquerdo
    glVertex2f(windowWidth, windowHeight); // Canto superior direito
    glVertex2f(windowWidth, windowHeight - heigh); // Canto inferior direito
    glVertex2f(0, windowHeight - heigh);           // Canto inferior esquerdo
    glEnd();
}

void renderHeaderAndButtons() {
    renderBackgroundBanner(66);

    int windowWidth, windowHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

    int buttonSpacing = 8;
    int buttonCount = 0;

    for (auto& btn : buttons) {
        btn.x = buttonCount * btn.width + buttonSpacing * (buttonCount + 1);
        btn.y = windowHeight - buttonSpacing;

        buttonCount++;
        renderButton(btn);
    }
}
