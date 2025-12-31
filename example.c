#include <raylib.h>
#include "raytext.h"

int main(void) {
  InitWindow(800, 200, "Hello, Dynamic Glyph Caching!");

  GcFont gcfont = LoadGcFont("font.ttf", 40);
  GcFont gcfontjp = LoadGcFont("fontjp.ttf", 40);
  
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    DrawFPS(10, 10);
    Vector2 size = MeasureGcTextEx(&gcfont, "Hello, world! Привет, мир!", 40, 0);
    DrawRectangle(10, 50, size.x, size.y, GRAY);
    DrawGcTextEx(&gcfont, "Hello, world! Привет, мир!", (Vector2) { 10, 50 }, 40, 0, BLACK);
    DrawGcTextEx(&gcfont, "Ég get etið gler án þess að meiða mig.", (Vector2) { 10, 90 }, 40, 0, BLACK);
    DrawGcTextEx(&gcfontjp, "私はガラスを食べられます。それは私を傷つけません。", (Vector2) { 10, 130 }, 40, 0, BLACK);
    EndDrawing();
  }

  UnloadGcFont(&gcfontjp);
  UnloadGcFont(&gcfont);
  CloseWindow();
  return 0;
}
