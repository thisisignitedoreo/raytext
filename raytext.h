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
  float advance, fontsize;
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

typedef struct RtFont {
  Glyphs glyphs;
  Atlases atlases;
  stbtt_fontinfo font;
  int ascent, descent;
  
  const unsigned char *font_buffer;
  size_t buffer_length;
  bool is_buffer_owned;
} RtFont;

void SetRtTextLineSpacing(int spacing);

RtFont LoadRtFont(const char *filename);
RtFont LoadRtFontFromMemory(const unsigned char *buffer, size_t length);
void UnloadRtFont(RtFont *font);

void DrawRtTextEx(RtFont *font, const char *text, Vector2 pos, float fontsize, float spacing, Color tint);
Vector2 MeasureRtTextEx(RtFont *font, const char *text, float fontsize, float spacing);

#endif // RAYTEXT_H
