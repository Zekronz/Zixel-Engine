/*
    FontData.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/FontData.h"
#include "Engine/Math.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Zixel {

	FontData::~FontData() {

		for (FontDataGlyph& glyph : glyphs) {
			free(glyph.buffer);
		}

	}

	bool FontData::load(const char* filePath, s32 size) {

		if (initialized) {
			ZIXEL_WARN("Font has already been loaded.", filePath);
			return false;
		}

		FT_Library freeType;

		if (FT_Init_FreeType(&freeType) != 0) {
			ZIXEL_CRITICAL("Unable to initialize FreeType library.");
			return false;
		}

		FT_Face face;

		if(FT_New_Face(freeType, filePath, 0, &face) != 0) {
			ZIXEL_CRITICAL("Unable to load font: \"{}\"", filePath);
			return false;

		}

		FT_Set_Pixel_Sizes(face, 0, size);
		
		s32 maxGlyphHeight = 0;

		for (u8 c = 0; c < 255; ++c) {

			if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0) {

				ZIXEL_CRITICAL("Unable to load character \"{}\" in font: \"{}\" (0).", c, filePath);

				FT_Done_Face(face);
				FT_Done_FreeType(freeType);

				return false;

			}

			size_t size = (size_t)face->glyph->bitmap.width * (size_t)face->glyph->bitmap.rows;
			u8* data = (u8*)malloc(size);

			if (data == NULL) {

				ZIXEL_CRITICAL("Unable to load character \"{}\" in font: \"{}\" (1).", c, filePath);

				FT_Done_Face(face);
				FT_Done_FreeType(freeType);

				return false;

			}

			if (memcpy_s(data, size, face->glyph->bitmap.buffer, size) != 0) {

				ZIXEL_CRITICAL("Unable to load character \"{}\" in font: \"{}\" (2).", c, filePath);

				FT_Done_Face(face);
				FT_Done_FreeType(freeType);

				return false;

			}

			FontDataGlyph glyph;
			glyph.buffer = data;
			glyph.sizeX = face->glyph->bitmap.width;
			glyph.sizeY = face->glyph->bitmap.rows;
			glyph.bearingX = face->glyph->bitmap_left;
			glyph.bearingY = face->glyph->bitmap_top;
			glyph.advanceX = (face->glyph->advance.x >> 6);

			if (glyph.bearingY > maxGlyphHeight) {
				maxGlyphHeight = glyph.bearingY;
			}

			if (glyph.sizeY > advanceY) {
				advanceY = glyph.sizeY;
			}

			glyphs.push_back(glyph);

		}

		height = (face->size->metrics.ascender >> 6);
		
		//if (maxGlyphHeight > height) {
			//height = maxGlyphHeight;
		//}

		FT_Done_Face(face);
		FT_Done_FreeType(freeType);

		initialized = true;

		return true;

	}

}