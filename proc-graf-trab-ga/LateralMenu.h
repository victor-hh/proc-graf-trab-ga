#pragma once
#include <GLFW/glfw3.h>
#include "ButtonUtils.h"

//icones disponíveis em icons8.com
struct LateralButton {
    float x;
    float y;
    float size;
    Image* image;
    GLuint iconTextureID;
    std::function<void(Image* image)> onClick;

    LateralButton(float x, float y, float size, Image* image, GLuint iconTextureID, std::function<void(Image* image)> onClick)
        : x(x), y(y), size(size), image(image), iconTextureID(iconTextureID), onClick(onClick) {}
};

std::vector<LateralButton> lateralButtons = {};

// site pra converter de rgb: https://www.colorhexa.com/abdbe3
void renderBackgroundBanner(int width, int topBannerHeight) {
    glColor3f(0.7f, 0.7f, 0.7f);

    int windowWidth, windowHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

    glBegin(GL_QUADS);
    glVertex2f(windowWidth - width, windowHeight - topBannerHeight); // Canto superior esquerdo
    glVertex2f(windowWidth, windowHeight - topBannerHeight);         // Canto superior direito
    glVertex2f(windowWidth, 0);                                      // Canto inferior direito
    glVertex2f(windowWidth - width, 0);                              // Canto inferior esquerdo
    glEnd();
}

void renderImageWithProportions(const Image& img, float x, float y, float size) {
    float scaleX = size / img.imageWidth;
    float scaleY = size / img.imageHeight;
    float scaleFactor = std::min(scaleX, scaleY);

    // Calcula a largura e altura em escala
    float scaledWidth = img.imageWidth * scaleFactor;
    float scaledHeight = img.imageHeight * scaleFactor;

    float posX = x + (size - scaledWidth) / 2.0f; // Centraliza horizontalmente
    float posY = y - (scaledHeight + (size - scaledHeight) / 2.0f); // Centraliza verticalmente

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, img.textureID);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(posX, posY);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(posX + scaledWidth, posY);
    glTexCoord2f(1.0f, 1.0f); 
    glVertex2f(posX + scaledWidth, posY - scaledHeight);
    glTexCoord2f(0.0f, 1.0f); 
    glVertex2f(posX, posY - scaledHeight);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void renderButton(const LateralButton& button) {
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


void subirImagem(Image* image) {
    image->offsetY++;
}

void descerImagem(Image* image) {
    image->offsetY--;
}

void moverImagemParaDireita(Image* image) {
    image->offsetX++;
}

void moverImagemParaEsquerda(Image* image) {
    image->offsetX--;
}

void zoomIn(Image* image) {
    image->zoomFactor *= 1.1f;
}

void zoomOut(Image* image) {
    image->zoomFactor *= 0.9f;
}
void createButtons(float buttonX, float buttonY, Image* image) {
    const float buttonSize = 25; // Tamanho dos botões

    GLuint subirIcon = loadPNGTexture("icons\\icons8-arrow-up.png");      // Exemplo de ícone para subir
    GLuint descerIcon = loadPNGTexture("icons\\icons8-arrow-down.png");   // Exemplo de ícone para descer
    GLuint direitaIcon = loadPNGTexture("icons\\icons8-arrow-right.png"); // Exemplo de ícone para mover para direita
    GLuint esquerdaIcon = loadPNGTexture("icons\\icons8-arrow-left.png");
    GLuint zoomInIcon = loadPNGTexture("icons\\icons8-zoom-in-50.png");
    GLuint zoomMinusIcon = loadPNGTexture("icons\\icons8-zoom-out-50.png");

    lateralButtons.push_back({buttonX, buttonY, buttonSize, image, subirIcon, subirImagem });
    lateralButtons.push_back({buttonX + buttonSize, buttonY, buttonSize, image, descerIcon, descerImagem });
    lateralButtons.push_back({buttonX, buttonY - buttonSize, buttonSize, image, esquerdaIcon, moverImagemParaEsquerda});
    lateralButtons.push_back({buttonX + buttonSize, buttonY - buttonSize, buttonSize, image, direitaIcon, moverImagemParaDireita});
    lateralButtons.push_back({buttonX + 2 * buttonSize + 5, buttonY, buttonSize, image, zoomInIcon, zoomIn });
    lateralButtons.push_back({ buttonX + 2 * buttonSize + 5, buttonY - buttonSize, buttonSize, image, zoomMinusIcon, zoomOut });
}

boolean isClickOverLateralButton(const LateralButton button, double mouseX, double mouseY) {
    return (mouseX >= button.x && mouseX <= button.x + button.size &&
        mouseY <= button.y && mouseY >= button.y - button.size);
}

void checkClickOnLateralMenu(double mouseX, double mouseY) {
    for (const LateralButton& button : lateralButtons) {
        if (isClickOverLateralButton(button, mouseX, mouseY)) {
            button.onClick(button.image);
        }
    }
}

void renderLateralMenu(std::vector<Image>& images) {
    lateralButtons.clear();
    renderBackgroundBanner(200, 66); 
    int windowWidth, windowHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

    const int imageSize = 50; // 50x50 pixels
    float startY = windowHeight - 66; // Começa logo abaixo do banner

    for (auto& img : images) {
        renderImageWithProportions(img, windowWidth - 200, startY + imageSize, imageSize);

        // Calcula a posição dos botões
        float buttonX = windowWidth - 200 + imageSize + 10;
        float buttonY = startY;

        createButtons(buttonX, buttonY, &img);

        startY -= imageSize;
    }

    for (const auto& button : lateralButtons) {
        renderButton(button);
    }
}

