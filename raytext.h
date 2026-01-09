/*
 * RayText library, released into the public domain. (Unlicense)
 * by aciddev_
 *
 * Implements Dynamic Glyph-Cached fonts, and also allows to render in different
 * fontsizes, not just scaling the rasterized glyph but rerasterizing it for
 * every fontsize.
 *
 * This API meant to be mostly compatible with raylib's built in text drawing
 * tools, and is seen as a drop-in replacement.
 */

#ifndef RAYTEXT_H
#define RAYTEXT_H

#include <stddef.h>
#include <stdlib.h>
#include <raylib.h>
#include <stb_rect_pack.h>
#include <stb_truetype.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Internal initial capacity of the font glyph array. Grows when needed. */
#define RAYTEXT_GLYPH_DEFAULT_CAP 256

/* Internal glyph structure, used to index glyphs and their position on the
   atlases. Fontsize is checked exactly with a == when indexing, so if you don't
   pass a float literal, but rather calculate the result in some way, this can
   cache miss. */
typedef struct Glyph {
  int codepoint, atlas_id;
  float advance, fontsize;
  Vector2 offset;
  Rectangle rect;
} Glyph;

/* Internal glyph dynamic array structure. */
typedef struct Glyphs {
  Glyph *glyphs;
  size_t count;
  size_t capacity;
} Glyphs;

/* Internal initial capacity of the font atlas array. Grows when needed. */
#define RAYTEXT_ATLAS_DEFAULT_CAP 16
/* Internal size of the atlas square side. */
#define RAYTEXT_ATLAS_DEFAULT_SIZE 1024
/* Internal size of the node array for stbrp. Keep in sync with the atlas size. */
#define RAYTEXT_ATLAS_NODE_COUNT 1024

/* Internal atlas structure. */
typedef struct Atlas {
  Texture2D texture;
  stbrp_context pack_context;
  stbrp_node *nodes;
} Atlas;

/* Internal atlas dynamic array structure.
   This is an array of pointers because stbrp internally stores a pointer to the
   node array which needs to be valid for the lifetime of the context, so the
   atlases structures are allocated on the heap. */
typedef struct Atlases {
  Atlas **atlases;
  size_t count;
  size_t capacity;
} Atlases;

/* Font structure. Meant to be stored as value as returned by the initializer
   functions. */
typedef struct RtFont {
  Glyphs glyphs;
  Atlases atlases;
  stbtt_fontinfo font;
  int ascent, descent;

  /* Internal buffer for the font data. Owned and freed upon unload only if it
     was loaded from a file using LoadRtFont.
     In this case, is_buffer_owned = true. */
  const unsigned char *font_buffer;
  size_t buffer_length;
  bool is_buffer_owned;
} RtFont;

/* Sets a global line height, analagous to raylib. In there mostly to keep API
   compatibiliy. */
void SetRtTextLineSpacing(int spacing);

/* Loads a font from a file and returns a font structure that, as opposed to
   raylib, is referenced by a pointer by other API functions. */
RtFont LoadRtFont(const char *filename);

/* Loads a font from a buffer and returns a font structure that, as opposed to
   raylib, is referenced by a pointer by other API functions.
   Does NOT take ownership of a buffer: this pointer must be valid until the
   font is unloaded. */
RtFont LoadRtFontFromMemory(const unsigned char *buffer, size_t length);

/* Unloads a previously loaded font. Frees the underlying buffer if the font was
   loaded from a file. */
void UnloadRtFont(RtFont *font);

/* Draws text in the given position with the given fontsize, spacing and tint.
   All arguments are analagous to raylib. Text is zero-terminated UTF-8.
   Thread UNSAFE. Should only be called from the thread that created the font. */
void DrawRtTextEx(RtFont *font, const char *text, Vector2 pos, float fontsize, float spacing, Color tint);

/* Measures the size of a text string with given fontsize and spacing.
   All arguments are analagous to raylib. Text is zero-terminated UTF-8.
   NOTE: This function also rasterizes glyphs so it's as slow as drawing the
         text to the screen.
   Thread UNSAFE. Should only be called from the thread that created the font. */
Vector2 MeasureRtTextEx(RtFont *font, const char *text, float fontsize, float spacing);

/* Internal initial capacity of the font chain array. Grows when needed. */
#define RAYTEXT_CHAINED_FONT_DEFAULT_CAP 8

/* Font chain structure. Allows you to add multiple fonts with different
   codepoints and use it as if it's just one font. Picks the first font
   it can find a codepoint in. */
typedef struct RtChainFont {
  RtFont *fonts;
  size_t count;
  size_t capacity;
} RtChainedFont;

/* Loads an empty chained font. */
RtChainedFont LoadRtChainedFont(void);

/* Chain a font loaded from a file. */
void RtChainFont(RtChainedFont *font, const char *filename);

/* Chain a font loaded from a buffer.
   Does NOT take ownership of a buffer: this pointer must be valid until the
   whole font chain is unloaded. */
void RtChainFontFromMemory(RtChainedFont *font, const unsigned char *buffer, size_t length);

/* Unloads the whole chain of fonts. Frees the underlying buffers if there are
   fonts that were loaded from a file. */
void UnloadRtChainedFont(RtChainedFont *font);

/* Draws text in the given position with the given fontsize, spacing and tint.
   All arguments are analagous to raylib. Text is zero-terminated UTF-8.
   Thread UNSAFE. Should only be called from the thread that created the font. */
void DrawRtCTextEx(RtChainedFont *font, const char *text, Vector2 pos, float fontsize, float spacing, Color tint);

/* Measures the size of a text string with given fontsize and spacing.
   All arguments are analagous to raylib. Text is zero-terminated UTF-8.
   NOTE: This function also rasterizes glyphs so it's as slow as drawing the
         text to the screen.
   Thread UNSAFE. Should only be called from the thread that created the font. */
Vector2 MeasureRtCTextEx(RtChainedFont *font, const char *text, float fontsize, float spacing);

#ifdef __cplusplus
}
#endif

#endif /* RAYTEXT_H */
