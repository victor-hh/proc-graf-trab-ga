#pragma once
#include <GLFW/glfw3.h>

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

GLuint loadPNGTexture(const char* pngFilePath) {
    int width, height, channels;
    unsigned char* data = stbi_load(pngFilePath, &width, &height, &channels, 0);

    if (!data) {
        printf("Erro ao carregar imagem PNG: %s\n", pngFilePath);
        return 0; // Retorna 0 em caso de erro
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Se a imagem tem um canal alfa, usamos GL_RGBA, senão GL_RGB
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    for (int i = 0; i < height / 2; ++i) {
        int swapIndex = height - i - 1;
        for (int j = 0; j < width * channels; ++j) {
            std::swap(data[i * width * channels + j], data[swapIndex * width * channels + j]);
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data); // Libera a memória da imagem
    return textureID;
}

GLuint loadIconTexture(const char* ppmFilePath) {
    int width, height;
    unsigned char* data = loadPPM(ppmFilePath, &width, &height);  // Assumindo que você já tem a função `loadPPM` implementada.

    if (!data) {
        printf("Erro ao carregar ícone PPM: %s\n", ppmFilePath);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(data);
    return textureID;
}

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

//void renderButton(float x, float y, float size) {
//    glColor3f(1.0f, 1.0f, 1.0f);
//
//    glBegin(GL_QUADS);
//    glVertex2f(x, y);
//    glVertex2f(x + size, y);
//    glVertex2f(x + size, y - size);
//    glVertex2f(x, y - size);
//    glEnd();
//}

void renderButton(const LateralButton& button) {
    if (button.iconTextureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, button.iconTextureID);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(button.x, button.y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(button.x + button.size, button.y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(button.x + button.size, button.y - button.size);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(button.x, button.y - button.size);
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
    image->offsetY--;
}

void descerImagem(Image* image) {
    image->offsetY++;
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
    GLuint plusIcon = loadPNGTexture("icons\\icons8-plus-50.png");
    GLuint minusIcon = loadPNGTexture("icons\\icons8-minus-50.png");

    lateralButtons.push_back({buttonX, buttonY, buttonSize, image, subirIcon, subirImagem });
    lateralButtons.push_back({buttonX + buttonSize, buttonY, buttonSize, image, descerIcon, descerImagem });
    lateralButtons.push_back({buttonX, buttonY - buttonSize, buttonSize, image, esquerdaIcon, moverImagemParaEsquerda});
    lateralButtons.push_back({buttonX + buttonSize, buttonY - buttonSize, buttonSize, image, direitaIcon, moverImagemParaDireita});
    lateralButtons.push_back({ buttonX + buttonSize + buttonSize, buttonY, buttonSize, image, plusIcon, zoomIn });
    lateralButtons.push_back({ buttonX + buttonSize + buttonSize + buttonSize, buttonY, buttonSize, image, minusIcon, zoomOut });
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

