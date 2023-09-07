/*
    MaskBuffer.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

namespace Zixel {

	struct MaskBuffer {

		s32 width = 0, height = 0, columns = 0, size = 0;
		u8* buffer = nullptr;

		MaskBuffer(s32 _width, s32 _height, bool _fill = false);
		~MaskBuffer();

		void fill();
		void clear();
		void write(s32 _x, s32 _y, bool _value);
		bool read(s32 _x, s32 _y);
		MaskBuffer* clone();

	};

}