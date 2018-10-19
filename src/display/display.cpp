#include "display.h"

#include <display/lodepng.h>

#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>

class TDisplay::TImpl {
public:
    TImpl();
    ~TImpl();

    void DrawTile(float x, float y, ETileType type, float alpha);
    void DrawWinMessage();

    bool IsKeyPressed(EKey key) const;
    bool Closed() const;
    void ProcessEvents();
    void Render();

private:
    void InitWindow();
    void InitOpenGL();
    void InitTextures();

    unsigned LoadTexture(const std::string& filename);

private:
    struct TTile {
        float x;
        float y;
        ETileType type;
        float alpha;

        TTile(float x, float y, ETileType type, float alpha)
            : x(x)
            , y(y)
            , type(type)
            , alpha(alpha) {}
    };

    GLFWwindow* Window;

    unsigned NextTextureIndex;
    std::unordered_map<ETileType, unsigned> TileTextures;
    unsigned WinTexture;

    std::vector<TTile> Tiles;
    bool WinMessage;
};

TDisplay::TImpl::TImpl()
    : NextTextureIndex(0)
    , Tiles()
    , WinMessage(false)
{
    InitWindow();
    InitOpenGL();
    InitTextures();
}

TDisplay::TImpl::~TImpl() {
    glfwDestroyWindow(Window);
    glfwTerminate();
}

void TDisplay::TImpl::InitWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("GLFW: Initialization failed!");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);
    Window = glfwCreateWindow(700, 700, "2048", NULL, NULL);
    if (!Window) {
        glfwTerminate();
        throw std::runtime_error("GLFW: Window creation failed!");
    }

    glfwMakeContextCurrent(Window);
    glfwSwapInterval(1);
}

void TDisplay::TImpl::InitOpenGL() {
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_TEXTURE_2D);
#ifndef WIN32
    glEnable(GL_MULTISAMPLE);
#endif
}

void TDisplay::TImpl::InitTextures() {
    TileTextures[ETileType::TILE_1] = LoadTexture("data/1.png");
    TileTextures[ETileType::TILE_2] = LoadTexture("data/2.png");
    TileTextures[ETileType::TILE_4] = LoadTexture("data/4.png");
    TileTextures[ETileType::TILE_8] = LoadTexture("data/8.png");
    TileTextures[ETileType::TILE_16] = LoadTexture("data/16.png");
    TileTextures[ETileType::TILE_32] = LoadTexture("data/32.png");
    TileTextures[ETileType::TILE_64] = LoadTexture("data/64.png");
    TileTextures[ETileType::TILE_128] = LoadTexture("data/128.png");
    TileTextures[ETileType::TILE_256] = LoadTexture("data/256.png");
    TileTextures[ETileType::TILE_512] = LoadTexture("data/512.png");
    TileTextures[ETileType::TILE_1024] = LoadTexture("data/1024.png");
    TileTextures[ETileType::TILE_2048] = LoadTexture("data/2048.png");

    WinTexture = LoadTexture("data/win.png");
}

unsigned TDisplay::TImpl::LoadTexture(const std::string& filename) {
    std::vector<unsigned char> image;
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, filename);
    if (error) {
        throw std::runtime_error(std::string("LODEPNG: ") + lodepng_error_text(error));
    }

    unsigned textureId = NextTextureIndex++;
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
    return textureId;
}

void TDisplay::TImpl::DrawTile(float x, float y, ETileType type, float alpha) {
    Tiles.emplace_back(x, y, type, alpha);
}

void TDisplay::TImpl::DrawWinMessage() {
    WinMessage = true;
}

bool TDisplay::TImpl::IsKeyPressed(EKey key) const {
    switch (key) {
        case EKey::KEY_UP:
            return glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS ||
                   glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS;
        case EKey::KEY_DOWN:
            return glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS ||
                   glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS;
        case EKey::KEY_LEFT:
            return glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS ||
                   glfwGetKey(Window, GLFW_KEY_LEFT) == GLFW_PRESS;
        case EKey::KEY_RIGHT:
            return glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS ||
                   glfwGetKey(Window, GLFW_KEY_RIGHT) == GLFW_PRESS;
        default:
            return false;
    }
}

bool TDisplay::TImpl::Closed() const {
    return glfwWindowShouldClose(Window);
}

void TDisplay::TImpl::ProcessEvents() {
    glfwPollEvents();
}

void TDisplay::TImpl::Render() {
    int width, height;
    glfwGetFramebufferSize(Window, &width, &height);
    
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5f, 1.0f, 0.83f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5f, 3.5f, 3.5f, -0.5f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (const auto& tile : Tiles) {
        glBindTexture(GL_TEXTURE_2D, TileTextures[tile.type]);
        glColor4f(1.0f, 1.0f, 1.0f, tile.alpha);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f,  1.0f);
        glVertex2f(tile.y - 0.5f, tile.x + 0.5f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(tile.y - 0.5f, tile.x - 0.5f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(tile.y + 0.5f, tile.x - 0.5f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(tile.y + 0.5f, tile.x + 0.5f);
        glEnd(); 
    }

    if (WinMessage) {
        glBindTexture(GL_TEXTURE_2D, WinTexture);
        glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f,  1.0f);
        glVertex2f(-0.5f, 3.5f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-0.5f, -0.5f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(3.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(3.5f, 3.5f);
        glEnd();
    }

    glfwSwapBuffers(Window);

    Tiles.clear();
    WinMessage = false;
}

TDisplay::TDisplay()
    : Impl(new TImpl()) {}

TDisplay::~TDisplay() {}

void TDisplay::DrawTile(float x, float y, ETileType type, float alpha) {
    Impl->DrawTile(x, y, type, alpha);
}

void TDisplay::DrawWinMessage() {
    Impl->DrawWinMessage();
}

double TDisplay::GetTime() const {
    return glfwGetTime();
}

bool TDisplay::IsKeyPressed(EKey key) const {
    return Impl->IsKeyPressed(key);
}

bool TDisplay::Closed() const {
    return Impl->Closed();
}

void TDisplay::ProcessEvents() {
    Impl->ProcessEvents();
}

void TDisplay::Render() {
    Impl->Render();
}
