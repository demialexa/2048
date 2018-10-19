#include <display/display.h>

#include <cmath>

int main() {
    TDisplay display;

    double y = 0;
    double oldTime = display.GetTime();
    while (!display.Closed()) {
        display.ProcessEvents();

        // TODO: Insert your code here

        // Example: draw tiles
        display.DrawTile(0, 0, ETileType::TILE_1);
        display.DrawTile(0, 1, ETileType::TILE_2);
        display.DrawTile(0, 2, ETileType::TILE_4);
        display.DrawTile(0, 3, ETileType::TILE_8);
        display.DrawTile(1, 0, ETileType::TILE_16, 0.4);
        display.DrawTile(1, 1, ETileType::TILE_32);
        display.DrawTile(1, 2, ETileType::TILE_64);
        display.DrawTile(1, 3, ETileType::TILE_128, 0.8);
        display.DrawTile(2, 0, ETileType::TILE_256);
        display.DrawTile(2, 1, ETileType::TILE_512);
        display.DrawTile(2, 2, ETileType::TILE_1024);
        display.DrawTile(2, 3, ETileType::TILE_2048);

        // Example: you can draw tile at intermediate point to make animation
        double elapsedTime = display.GetTime() - oldTime;
        oldTime = display.GetTime();
        if (display.IsKeyPressed(EKey::KEY_LEFT)) {
            y -= 2 * elapsedTime;
            if (y < -1.5) {
                y = -1.5;
            }
        } else if (display.IsKeyPressed(EKey::KEY_RIGHT)) {
            y += 2 * elapsedTime;
            if (y > 1.5) {
                y = 1.5;
            }
        }
        display.DrawTile(3, 1.5 + y, ETileType::TILE_64);

        // Example: win message
        if (y < -1 || y > 1) {
            display.DrawWinMessage();
        }

        display.Render();
    }
    return 0;
}
