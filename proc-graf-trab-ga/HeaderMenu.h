#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <functional>
#include "LateralMenu.h"
#include "ButtonUtils.h"
#include "FilterFunctions.h"
#include "ButtonActionSaveImage.h"
#include "ButtonActionLoadImage.h"

// Estrutura para armazenar informações sobre o botão
struct Button {
    double x, y;
    double size = 50;
    double width, height = 50;
    GLuint iconTextureID;
    std::function<void(Project*)> onClick;

    Button(GLuint iconTextureID, std::function<void(Project*)> onClick)
        : x(0), y(0), width(50.0), height(50.0), iconTextureID(iconTextureID), onClick(onClick) {}
};

void renderButton(const Button& button) {
    if (button.iconTextureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, button.iconTextureID);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(button.x, button.y);                       // Canto superior esquerdo
        glTexCoord2f(1.0f, 1.0f); glVertex2f(button.x + button.size, button.y);          // Canto superior direito
        glTexCoord2f(1.0f, 0.0f); glVertex2f(button.x + button.size, button.y - button.size); // Canto inferior direito
        glTexCoord2f(0.0f, 0.0f); glVertex2f(button.x, button.y - button.size);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
    else {
        // Caso não tenha uma textura de ícone, renderiza o botão de forma simples
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2f(button.x, button.y);
        glVertex2f(button.x + button.size, button.y);
        glVertex2f(button.x + button.size, button.y - button.size);
        glVertex2f(button.x, button.y - button.size);
        glEnd();
    }
}

std::vector<Button> buttons = {};

void generateButtons() {
    GLuint addImageIcon = loadPNGTexture("icons\\icons8-add-image-50.png");
    GLuint iconSave = loadPNGTexture("icons\\icons8-save-as-50.png");
    GLuint sepiaIcon = loadPNGTexture("icons\\icons8-old-fashioned-family-photo-48.png");
    GLuint negative = loadPNGTexture("icons\\icons8-negative-100.png");
    GLuint grayscaleIcon = loadPNGTexture("icons\\icons8-grayscale-50.png");
    GLuint direitaIcon = loadPNGTexture("icons\\icons8-arrow-right.png");

    buttons.push_back({ addImageIcon, loadImageFromFiles });
    buttons.push_back({iconSave, saveImage});
    buttons.push_back({negative, applyNegativeFilter});
    buttons.push_back({grayscaleIcon, applyGrayscaleFilter});
    buttons.push_back({sepiaIcon, applySepiaFilter });
    buttons.push_back({direitaIcon, applyAntiSepiaFilter });
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
