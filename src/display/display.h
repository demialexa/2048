#pragma once

#include <memory>

enum class ETileType {
    TILE_1,
    TILE_2,
    TILE_4,
    TILE_8,
    TILE_16,
    TILE_32,
    TILE_64,
    TILE_128,
    TILE_256,
    TILE_512,
    TILE_1024,
    TILE_2048,
};

enum class EKey {
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
};

class TDisplay {
public:
    TDisplay();
    virtual ~TDisplay();

    void DrawTile(float x, float y, ETileType type);
    void DrawWinMessage();

    double GetTime() const;
    bool IsKeyPressed(EKey key) const;
    bool Closed() const;
    void ProcessEvents();
    void Render();

private:
    class TImpl;
    std::unique_ptr<TImpl> Impl;
};
