#include <raylib.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "raytext.h"

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(1200, 200, "Hello, RayText!");

  RtChainedFont font = LoadRtChainedFont();
  RtChainFont(&font, "font.ttf");
  RtChainFont(&font, "fontjp.ttf");
  RtChainFont(&font, "fontem.ttf");
  
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    DrawFPS(10, 10);
    Vector2 size = MeasureRtCTextEx(&font, "Hello, world! Привет, мир!", 16, 0);
    DrawRectangle(10, 50, size.x, size.y, GRAY);
    DrawRtCTextEx(&font, "Hello, world! Привет, мир!", (Vector2) { 10, 50 }, 16, 0, BLACK);
    size = MeasureRtCTextEx(&font, "Ég get etið gler án þess að meiða mig. 私はガラスを食べられます 😂💦", 32, 0);
    DrawRectangle(10, 50 + 5 + 16, size.x, size.y, GRAY);
    DrawRtCTextEx(&font, "Ég get etið gler án þess að meiða mig. 私はガラスを食べられます 😂💦", (Vector2) { 10, 50 + 5 + 16 }, 32, 0, BLACK);
    size = MeasureRtCTextEx(&font, "私はガラスを食べられます。それは私を傷つけません。", 64, 0);
    DrawRectangle(10, 50 + 10 + 16 + 32, size.x, size.y, GRAY);
    DrawRtCTextEx(&font, "私はガラスを食べられます。それは私を傷つけません。", (Vector2) { 10, 50 + 10 + 16 + 32 }, 64, 0, BLACK);
    EndDrawing();
  }

  UnloadRtChainedFont(&font);
  CloseWindow();
  return 0;
}
