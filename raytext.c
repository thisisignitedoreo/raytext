#include <raytext.h>

static int hspacing = 0;

void SetGcTextLineSpacing(int spacing) {
  hspacing = spacing;
}

GcFont LoadGcFont(const char *filename, float fontsize) {
  int buffer_length = 0;
  const unsigned char *font_buffer = LoadFileData(filename, &buffer_length);

  if (font_buffer) {
    GcFont font = LoadGcFontFromMemory(font_buffer, buffer_length, fontsize);
    font.is_buffer_owned = true;
    TraceLog(LOG_INFO, "GCFONT: [%s] Font loaded successfully", GetFileName(filename));
    return font;
  }

  return (GcFont) {0};
}

void InitAtlas(Atlas *atlas) {
  Image cpu_atlas = GenImageColor(RAYTEXT_ATLAS_DEFAULT_SIZE, RAYTEXT_ATLAS_DEFAULT_SIZE, BLANK);
  ImageFormat(&cpu_atlas, PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA);
  atlas->texture = LoadTextureFromImage(cpu_atlas);
  UnloadImage(cpu_atlas);

  atlas->nodes = RL_CALLOC(RAYTEXT_ATLAS_NODE_COUNT, sizeof(stbrp_node));
  stbrp_init_target(&atlas->pack_context, atlas->texture.width, atlas->texture.height, atlas->nodes, RAYTEXT_ATLAS_NODE_COUNT);
}

GcFont LoadGcFontFromMemory(const unsigned char *buffer, size_t length, float fontsize) {
  GcFont font = {0};

  if (buffer == NULL) return font;

  font.glyphs.capacity = RAYTEXT_GLYPH_DEFAULT_CAP;
  font.glyphs.count = 0;
  font.glyphs.glyphs = RL_CALLOC(font.glyphs.capacity, sizeof(Glyph));
  
  font.atlases.capacity = RAYTEXT_ATLAS_DEFAULT_CAP;
  font.atlases.count = 1;
  font.atlases.atlases = RL_CALLOC(font.atlases.capacity, sizeof(Atlas*));
  font.atlases.atlases[0] = RL_CALLOC(font.atlases.capacity, sizeof(Atlas));
  InitAtlas(font.atlases.atlases[0]);
  
  stbtt_InitFont(&font.font, buffer, 0);
  font.fontsize = fontsize;
  font.scale = stbtt_ScaleForPixelHeight(&font.font, fontsize);
  stbtt_GetFontVMetrics(&font.font, &font.ascent, NULL, NULL);

  font.baseline = font.ascent * font.scale;

  font.font_buffer = buffer;
  font.buffer_length = length;
  font.is_buffer_owned = false;

  return font;
}

void UnloadGcFont(GcFont *font) {
  for (size_t i = 0; i < font->atlases.count; i++) {
    UnloadTexture(font->atlases.atlases[i]->texture);
    RL_FREE(font->atlases.atlases[i]->nodes);
    RL_FREE(font->atlases.atlases[i]);
  }
  
  RL_FREE(font->atlases.atlases);
  
  RL_FREE(font->glyphs.glyphs);

  if (font->is_buffer_owned) {
    UnloadFileData((unsigned char*) font->font_buffer);
  }
}

void DrawGcTextEx(GcFont *font, const char *text, Vector2 pos, float fontsize, float spacing, Color tint) {
  int previous_codepoint = -1, codepoint = 0, size = 0;
  float original_x = pos.x;
  while ((codepoint = GetCodepoint(text, &size))) {
    text += size;

    if (stbtt_FindGlyphIndex(&font->font, codepoint) == 0) codepoint = '?';

    if (codepoint == '\n') {
      pos.x = original_x;
      pos.y += hspacing + fontsize;
      continue;
    }
    
    Glyph *glyph = NULL;
    for (size_t i = 0; i < font->glyphs.count; i++) {
      if (font->glyphs.glyphs[i].codepoint == codepoint) {
	glyph = &font->glyphs.glyphs[i];
	break;
      }
    }

    if (glyph == NULL) {
      int width, height, xoff, yoff;
      unsigned char *bitmap = stbtt_GetCodepointBitmap(&font->font, font->scale, font->scale, codepoint, &width, &height, &xoff, &yoff);
      
      Image image = GenImageColor(width, height, BLANK);
      ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA);

      for (int y = 0; y < height; y++) {
	for (int x = 0; x < width; x++) {
	  unsigned char *pixel = &((unsigned char*) image.data)[2 * (y * width + x)];
	  pixel[0] = 255; pixel[1] = ((unsigned char*) bitmap)[y * width + x];
	}
      }
      stbtt_FreeBitmap(bitmap, NULL);
      
      stbrp_rect rect = {
	.w = width, .h = height,
      };

      Atlas *current_atlas = font->atlases.atlases[font->atlases.count - 1];

      if (!stbrp_pack_rects(&current_atlas->pack_context, &rect, 1)) {
	if (font->atlases.count == font->atlases.capacity) {
	  font->atlases.capacity *= 2;
	  font->atlases.atlases = RL_REALLOC(font->atlases.atlases, font->atlases.capacity * sizeof(Atlas*));
	}
	current_atlas = font->atlases.atlases[font->atlases.count++];
	InitAtlas(current_atlas);
	if (!stbrp_pack_rects(&current_atlas->pack_context, &rect, 1)) {
	  TraceLog(LOG_WARNING, "Can't pack bitmap into an empty atlas");
	  continue;
	}
      }

      Rectangle glyph_rect = { rect.x, rect.y, rect.w, rect.h };
      UpdateTextureRec(current_atlas->texture, glyph_rect, image.data);
      UnloadImage(image);

      int advance, lsb;
      stbtt_GetCodepointHMetrics(&font->font, codepoint, &advance, &lsb);
      
      Glyph this_glyph = {
	.codepoint = codepoint,
	.advance = advance * font->scale,
	.atlas_id = font->atlases.count - 1,
	.rect = glyph_rect,
	.offset = (Vector2) { xoff, yoff },
      };

      if (font->glyphs.count == font->glyphs.capacity) {
	font->glyphs.capacity *= 2;
	font->glyphs.glyphs = RL_REALLOC(font->glyphs.glyphs, font->glyphs.capacity * sizeof(Glyph));
      }
      font->glyphs.glyphs[font->glyphs.count++] = this_glyph;
      glyph = &font->glyphs.glyphs[font->glyphs.count - 1];
    }

    float scale = fontsize/font->fontsize;
    
    if (previous_codepoint >= 0) {
      pos.x += stbtt_GetCodepointKernAdvance(&font->font, previous_codepoint, codepoint) * font->scale * scale;
    }
    
    DrawTexturePro(font->atlases.atlases[glyph->atlas_id]->texture, glyph->rect, (Rectangle) { pos.x + glyph->offset.x * scale, pos.y + (font->baseline + glyph->offset.y) * scale, glyph->rect.width * scale, glyph->rect.height * scale }, (Vector2) { 0, 0 }, 0.0f, tint);
    
    pos.x += glyph->advance * scale + spacing;

    previous_codepoint = codepoint;
  }
}

Vector2 MeasureGcTextEx(GcFont *font, const char *text, float fontsize, float spacing) {
  int previous_codepoint = -1, codepoint = 0, size = 0;
  Vector2 total_size = { 0.0f, hspacing + fontsize };
  float this_x = 0.0f;
  while ((codepoint = GetCodepoint(text, &size))) {
    text += size;

    if (stbtt_FindGlyphIndex(&font->font, codepoint) == 0) codepoint = '?';

    if (codepoint == '\n') {
      total_size.x = total_size.x > this_x ? total_size.x : this_x;
      this_x = 0.0f;
      total_size.y += hspacing + fontsize;
      continue;
    }
    
    Glyph *glyph = NULL;
    for (size_t i = 0; i < font->glyphs.count; i++) {
      if (font->glyphs.glyphs[i].codepoint == codepoint) {
	glyph = &font->glyphs.glyphs[i];
	break;
      }
    }

    if (glyph == NULL) {
      int width, height, xoff, yoff;
      unsigned char *bitmap = stbtt_GetCodepointBitmap(&font->font, font->scale, font->scale, codepoint, &width, &height, &xoff, &yoff);
      
      Image image = GenImageColor(width, height, BLANK);
      ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA);

      for (int y = 0; y < height; y++) {
	for (int x = 0; x < width; x++) {
	  unsigned char *pixel = &((unsigned char*) image.data)[2 * (y * width + x)];
	  pixel[0] = 255; pixel[1] = ((unsigned char*) bitmap)[y * width + x];
	}
      }
      stbtt_FreeBitmap(bitmap, NULL);
      
      stbrp_rect rect = {
	.w = width, .h = height,
      };

      Atlas *current_atlas = font->atlases.atlases[font->atlases.count - 1];

      if (!stbrp_pack_rects(&current_atlas->pack_context, &rect, 1)) {
	if (font->atlases.count == font->atlases.capacity) {
	  font->atlases.capacity *= 2;
	  font->atlases.atlases = RL_REALLOC(font->atlases.atlases, font->atlases.capacity * sizeof(Atlas*));
	}
	current_atlas = font->atlases.atlases[font->atlases.count++];
	InitAtlas(current_atlas);
	if (!stbrp_pack_rects(&current_atlas->pack_context, &rect, 1)) {
	  TraceLog(LOG_WARNING, "Can't pack bitmap into an empty atlas");
	  continue;
	}
      }

      Rectangle glyph_rect = { rect.x, rect.y, rect.w, rect.h };
      UpdateTextureRec(current_atlas->texture, glyph_rect, image.data);
      UnloadImage(image);

      int advance, lsb;
      stbtt_GetCodepointHMetrics(&font->font, codepoint, &advance, &lsb);
      
      Glyph this_glyph = {
	.codepoint = codepoint,
	.advance = advance * font->scale,
	.atlas_id = font->atlases.count - 1,
	.rect = glyph_rect,
	.offset = (Vector2) { xoff, yoff },
      };

      if (font->glyphs.count == font->glyphs.capacity) {
	font->glyphs.capacity *= 2;
	font->glyphs.glyphs = RL_REALLOC(font->glyphs.glyphs, font->glyphs.capacity * sizeof(Glyph));
      }
      font->glyphs.glyphs[font->glyphs.count++] = this_glyph;
      glyph = &font->glyphs.glyphs[font->glyphs.count - 1];
    }

    float scale = fontsize/font->fontsize;
    
    if (previous_codepoint >= 0) {
      this_x += stbtt_GetCodepointKernAdvance(&font->font, previous_codepoint, codepoint) * font->scale * scale;
    }
    
    this_x += glyph->advance * scale + spacing;

    previous_codepoint = codepoint;
  }
  total_size.x = total_size.x > this_x ? total_size.x : this_x;
  return total_size;
}
