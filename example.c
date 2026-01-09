#include <raylib.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "raytext.h"

int main(void) {
  InitWindow(1200, 200, "Hello, RayText!");

  RtFont font = LoadRtFont("font.ttf");
  RtFont fontjp = LoadRtFont("fontjp.ttf");
  
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    DrawFPS(10, 10);
    Vector2 size = MeasureRtTextEx(&font, "Hello, world! Привет, мир!", 16, 0);
    DrawRectangle(10, 50, size.x, size.y, GRAY);
    DrawRtTextEx(&font, "Hello, world! Привет, мир!", (Vector2) { 10, 50 }, 16, 0, BLACK);
    size = MeasureRtTextEx(&font, "Ég get etið gler án þess að meiða mig.", 32, 0);
    DrawRectangle(10, 50 + 5 + 16, size.x, size.y, GRAY);
    DrawRtTextEx(&font, "Ég get etið gler án þess að meiða mig.", (Vector2) { 10, 50 + 5 + 16 }, 32, 0, BLACK);
    size = MeasureRtTextEx(&fontjp, "私はガラスを食べられます。それは私を傷つけません。", 64, 0);
    DrawRectangle(10, 50 + 10 + 16 + 32, size.x, size.y, GRAY);
    DrawRtTextEx(&fontjp, "私はガラスを食べられます。それは私を傷つけません。", (Vector2) { 10, 50 + 10 + 16 + 32 }, 64, 0, BLACK);
    EndDrawing();
  }

  UnloadRtFont(&fontjp);
  UnloadRtFont(&font);
  CloseWindow();
  return 0;
}
