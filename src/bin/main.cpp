#include "engine/engine.h"

int main() {
  Engine engine(4);
  engine.MainLoop();
  return 0;
}
    // Example: draw tiles
#if 0
    display.DrawTile(0, 0, Tiles::kTile_1);
    display.DrawTile(0, 1, Tiles::kTile_2);
    display.DrawTile(0, 2, Tiles::kTile_4);
    display.DrawTile(0, 3, Tiles::kTile_8);
    display.DrawTile(1, 0, Tiles::kTile_16, 0.4);
    display.DrawTile(1, 1, Tiles::kTile_32);
    display.DrawTile(1, 2, Tiles::kTile_64);
    display.DrawTile(1, 3, Tiles::kTile_128, 0.8);
    display.DrawTile(2, 0, Tiles::kTile_256);
    display.DrawTile(2, 1, Tiles::kTile_512);
    display.DrawTile(2, 2, Tiles::kTile_1024);
    display.DrawTile(2, 3, Tiles::kTile_2048);

    // Example: you can draw tile at intermediate point to make animation
    double elapsed_time = display.GetTime() - old_time;
    old_time = display.GetTime();
    if (display.IsKeyPressed(Keys::kKeyLeft)) {
        y -= 2 * elapsed_time;
        if (y < -1.5) {
            y = -1.5;
        }
    } else if (display.IsKeyPressed(Keys::kKeyRight)) {
        y += 2 * elapsed_time;
        if (y > 1.5) {
            y = 1.5;
        }
    }
    display.DrawTile(3, 1.5 + y, Tiles::kTile_64);

    // Example: win message
    if (y < -1 || y > 1) {
        display.DrawWinMessage();
    }
#endif
