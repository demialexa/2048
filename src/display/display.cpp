#include "display.h"

#include <display/lodepng.h>

#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>

class Display::Impl {
 public:
  Impl();
  ~Impl();

  void DrawTile(float x, float y, Tiles type, float alpha);
  void DrawWinMessage();

  bool IsKeyPressed(Keys key) const;
  bool Closed() const;
  void ProcessEvents();
  void Render();

 private:
  struct Tile {
    float x;
    float y;
    Tiles type;
    float alpha;

    Tile(float x, float y, Tiles type, float alpha)
        : x(x)
        , y(y)
        , type(type)
        , alpha(alpha) {}
  };

  void InitWindow();
  void InitOpenGL();
  void InitTextures();

  unsigned LoadTexture(const std::string& filename);

  GLFWwindow* window_;

  unsigned next_texture_index_;
  std::unordered_map<Tiles, unsigned> tile_textures_;
  unsigned win_texture_;

  std::vector<Tile> tiles_;
  bool win_message_;
};

Display::Impl::Impl()
    : next_texture_index_(0)
    , tiles_()
    , win_message_(false) {
  InitWindow();
  InitOpenGL();
  InitTextures();
}

Display::Impl::~Impl() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

void Display::Impl::InitWindow() {
  if (!glfwInit()) {
      throw std::runtime_error("GLFW: Initialization failed!");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_SAMPLES, 4);
  window_ = glfwCreateWindow(700, 700, "2048", NULL, NULL);
  if (!window_) {
      glfwTerminate();
      throw std::runtime_error("GLFW: window_ creation failed!");
  }

  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1);
}

void Display::Impl::InitOpenGL() {
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

void Display::Impl::InitTextures() {
  tile_textures_[Tiles::kTile_1] = LoadTexture("data/1.png");
  tile_textures_[Tiles::kTile_2] = LoadTexture("data/2.png");
  tile_textures_[Tiles::kTile_4] = LoadTexture("data/4.png");
  tile_textures_[Tiles::kTile_8] = LoadTexture("data/8.png");
  tile_textures_[Tiles::kTile_16] = LoadTexture("data/16.png");
  tile_textures_[Tiles::kTile_32] = LoadTexture("data/32.png");
  tile_textures_[Tiles::kTile_64] = LoadTexture("data/64.png");
  tile_textures_[Tiles::kTile_128] = LoadTexture("data/128.png");
  tile_textures_[Tiles::kTile_256] = LoadTexture("data/256.png");
  tile_textures_[Tiles::kTile_512] = LoadTexture("data/512.png");
  tile_textures_[Tiles::kTile_1024] = LoadTexture("data/1024.png");
  tile_textures_[Tiles::kTile_2048] = LoadTexture("data/2048.png");

  win_texture_ = LoadTexture("data/win.png");
}

unsigned Display::Impl::LoadTexture(const std::string& filename) {
  std::vector<unsigned char> image;
  unsigned width, height;
  unsigned error = lodepng::decode(image, width, height, filename);
  if (error) {
      throw std::runtime_error(std::string("LODEPNG: ") + lodepng_error_text(error));
  }

  unsigned textureId = next_texture_index_++;
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
  return textureId;
}

void Display::Impl::DrawTile(float x, float y, Tiles type, float alpha) {
  tiles_.emplace_back(x, y, type, alpha);
}

void Display::Impl::DrawWinMessage() {
  win_message_ = true;
}

bool Display::Impl::IsKeyPressed(Keys key) const {
  switch (key) {
    case Keys::kKeyUp:
      return glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS ||
             glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS;
    case Keys::kKeyDown:
      return glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS ||
             glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS;
    case Keys::kKeyLeft:
      return glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS ||
             glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS;
    case Keys::kKeyRight:
      return glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS ||
             glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS;
    default:
      return false;
  }
}

bool Display::Impl::Closed() const {
  return glfwWindowShouldClose(window_);
}

void Display::Impl::ProcessEvents() {
  glfwPollEvents();
}

void Display::Impl::Render() {
  int width, height;
  glfwGetFramebufferSize(window_, &width, &height);
  
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.5f, 1.0f, 0.83f, 1.0f);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-0.5f, 3.5f, 3.5f, -0.5f, -1.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  for (const auto& tile : tiles_) {
    glBindTexture(GL_TEXTURE_2D, tile_textures_[tile.type]);
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

  if (win_message_) {
    glBindTexture(GL_TEXTURE_2D, win_texture_);
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

  glfwSwapBuffers(window_);

  tiles_.clear();
  win_message_ = false;
}

Display::Display()
    : impl_(new Impl()) {}

Display::~Display() {}

void Display::DrawTile(float x, float y, Tiles type, float alpha) {
  impl_->DrawTile(x, y, type, alpha);
}

void Display::DrawWinMessage() {
  impl_->DrawWinMessage();
}

double Display::GetTime() const {
  return glfwGetTime();
}

bool Display::IsKeyPressed(Keys key) const {
  return impl_->IsKeyPressed(key);
}

bool Display::Closed() const {
  return impl_->Closed();
}

void Display::ProcessEvents() {
  impl_->ProcessEvents();
}

void Display::Render() {
  impl_->Render();
}
