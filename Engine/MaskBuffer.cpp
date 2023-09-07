/*
    MaskBuffer.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/MaskBuffer.h"

namespace Zixel {

	MaskBuffer::MaskBuffer(s32 _width, s32 _height, bool _fill) {

		if (_width < 1 || _height < 1) {

			ZIXEL_WARN("Error in MaskBuffer::MaskBuffer. Size cannot be less than 1: {}x{}", _width, _height);
			return;

		}

		width = _width;
		height = _height;
		columns = (s32)ceil(_width / 8.0);
		
		size =  columns * _height;

		buffer = new u8[size]();

		if (_fill) {

			for (s32 i = 0; i < size; ++i) {
				buffer[i] = 0xFF;
			}

		}

	}

	MaskBuffer::~MaskBuffer() {

		if (buffer != nullptr) {
			delete[] buffer;
		}

	}

	void MaskBuffer::fill() {

		for (s32 i = 0; i < size; ++i) {
			buffer[i] = 0xFF;
		}

	}

	void MaskBuffer::clear() {

		for (s32 i = 0; i < size; ++i) {
			buffer[i] = 0x00;
		}

	}

	void MaskBuffer::write(s32 _x, s32 _y, bool _value) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in MaskBuffer::write. Write position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return;

		}

		s32 column = (_x / 8);
		s32 bit = 7 - (_x - (column * 8));
		
		if (_value) {
			buffer[(_y * columns) + column] |= (0x01 << bit);
		}
		else {
			buffer[(_y * columns) + column] &= ((0x01 << bit) ^ 0xFF);
		}

	}

	bool MaskBuffer::read(s32 _x, s32 _y) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in MaskBuffer::read. Read position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return false;

		}

		s32 column = (_x / 8);
		s32 bit = 7 - (_x - (column * 8));

		return (((buffer[(_y * columns) + column] >> bit) & 0x01) == 0x01);

	}

	MaskBuffer* MaskBuffer::clone() {

		MaskBuffer* mask = new MaskBuffer(width, height);
		for (size_t i = 0; i < (size_t)size; ++i) mask->buffer[i] = buffer[i];

		return mask;

	}

}