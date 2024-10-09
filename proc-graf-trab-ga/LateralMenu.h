#pragma once
#include <GLFW/glfw3.h>

//icones disponíveis em icons8.com
struct LateralButton {
    float x;                       // Posição X
    float y;                       // Posição Y
    float size;                    // Tamanho do botão (assumindo quadrado)
    Image* image;
    std::function<void(Image* image)> onClick; // Função de callback para o botão

    LateralButton(float x, float y, float size, Image* image, std::function<void(Image* image)> onClick)
        : x(x), y(y), size(size), image(image), onClick(onClick) {}
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

void renderButton(float x, float y, float size) {
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + size, y);
    glVertex2f(x + size, y - size);
    glVertex2f(x, y - size);
    glEnd();
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

void createButtons(float buttonX, float buttonY, Image* image) {
    const float buttonSize = 25; // Tamanho dos botões

    lateralButtons.push_back({buttonX, buttonY, buttonSize, image, subirImagem });
    lateralButtons.push_back({buttonX + buttonSize, buttonY, buttonSize, image, descerImagem });
    lateralButtons.push_back({buttonX, buttonY - buttonSize, buttonSize, image, moverImagemParaDireita });
    lateralButtons.push_back({buttonX + buttonSize, buttonY - buttonSize, buttonSize, image, moverImagemParaEsquerda });
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
        renderButton(button.x, button.y, button.size);
    }
}
