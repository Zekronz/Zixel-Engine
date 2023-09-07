/*
    FontData.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <cstdint>
#include <vector>

namespace Zixel {

	struct FontDataGlyph {

		u8* buffer = nullptr;
		//u8 c;
		s32 sizeX, sizeY;
		s32 bearingX, bearingY;
		s32 advanceX = 0;

	};

	struct FontData {

		bool initialized = false;

		s32 height = 0;
		s32 advanceY = 0;

		std::vector<FontDataGlyph> glyphs;

		~FontData();

		bool load(const char* filePath, s32 size);

	};

}