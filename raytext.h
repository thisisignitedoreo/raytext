#ifndef RAYTEXT_H
#define RAYTEXT_H

#include <stddef.h>
#include <stdlib.h>
#include <raylib.h>
#include <stb_rect_pack.h>
#include <stb_truetype.h>

#define RAYTEXT_GLYPH_DEFAULT_CAP 8

typedef struct Glyph {
  int codepoint, atlas_id;
  float advance;
  Vector2 offset;
  Rectangle rect;
} Glyph;

typedef struct Glyphs {
  Glyph *glyphs;
  size_t count;
  size_t capacity;
} Glyphs;

#define RAYTEXT_ATLAS_DEFAULT_CAP 2
#define RAYTEXT_ATLAS_DEFAULT_SIZE 256
#define RAYTEXT_ATLAS_NODE_COUNT 256

typedef struct Atlas {
  Texture2D texture;
  stbrp_context pack_context;
  stbrp_node *nodes;
} Atlas;

typedef struct Atlases {
  Atlas **atlases;
  size_t count;
  size_t capacity;
} Atlases;

typedef struct GcFont {
  Glyphs glyphs;
  Atlases atlases;
  stbtt_fontinfo font;
  float fontsize, scale, baseline;
  int ascent;
  
  const unsigned char *font_buffer;
  size_t buffer_length;
  bool is_buffer_owned;
} GcFont;

void SetGcTextLineSpacing(int spacing);

GcFont LoadGcFont(const char *filename, float fontsize);
GcFont LoadGcFontFromMemory(const unsigned char *buffer, size_t length, float fontsize);
void UnloadGcFont(GcFont *font);

void DrawGcTextEx(GcFont *font, const char *text, Vector2 pos, float fontsize, float spacing, Color tint);
Vector2 MeasureGcTextEx(GcFont *font, const char *text, float fontsize, float spacing);

#endif // RAYTEXT_H
