/*
    PixelBuffer.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include "Engine/Color.h"

namespace Zixel {

	struct MaskBuffer;

	struct PixelBuffer {

		s32 width = 0, height = 0;
		s32 bBoxLeft = -1, bBoxTop = -1, bBoxRight = -1, bBoxBottom = -1;
		s32 pixelCount = 0;

		bool useBBox = true;
		bool makeEmptyPixelsBlack = false; //Replaces red, green and blue channels to 0 if alpha is 0.

		u8* buffer = nullptr;

		PixelBuffer(s32 _width, s32 _height, Color4 _fillColor = { 0, 0, 0, 0 }, bool _useBBox = true, bool _makeEmptyPixelsBlack = false);
		~PixelBuffer();

		void __setBBox(s32 _left, s32 _top, s32 _right, s32 _bottom);
		void checkBBoxIncrease(s32 _x, s32 _y);
		void calculateBBox(bool _startFromCurrentBBox = false);

		bool isEmpty();

		void fill(Color4 _color);
		bool fillCheckModified(Color4 _color);
		void writePixel(s32 _x, s32 _y, Color4 _color, BlendMode _blendMode = BlendMode::Overwrite, bool _calculateBBox = true);
		bool writePixelCheckModified(s32 _x, s32 _y, Color4 _color, BlendMode _blendMode = BlendMode::Overwrite, bool _calculateBBox = true);
		void writeLine(s32 _x1, s32 _y1, s32 _x2, s32 _y2, Color4 _color, BlendMode _blendMode = BlendMode::Overwrite, bool _writeFirstPixel = true, bool _calculateBBox = true);
		bool writeLineCheckModified(s32 _x1, s32 _y1, s32 _x2, s32 _y2, Color4 _color, BlendMode _blendMode = BlendMode::Overwrite, bool _writeFirstPixel = true, bool _calculateBBox = true);
		void writeRect(s32 _x, s32 _y, s32 _width, s32 _height, Color4 _color, BlendMode _blendMode = BlendMode::Overwrite, bool _calculateBBox = true);
		void writeRed(s32 _x, s32 _y, u8 _red);
		void writeGreen(s32 _x, s32 _y, u8 _green);
		void writeBlue(s32 _x, s32 _y, u8 _blue);
		void writeAlpha(s32 _x, s32 _y, u8 _alpha, bool _calculateBBox = true);
		bool writeAlphaCheckModified(s32 _x, s32 _y, u8 _alpha, bool _calculateBBox = true);

		Color4 readPixel(s32 _x, s32 _y);
		u8 readRed(s32 _x, s32 _y);
		u8 readGreen(s32 _x, s32 _y);
		u8 readBlue(s32 _x, s32 _y);
		u8 readAlpha(s32 _x, s32 _y);

		void copy(PixelBuffer* _sourceBuffer);
		bool merge(PixelBuffer* _sourceBuffer, BlendMode _blendMode, f32 _sourceOpacity = 1.0f);
		bool merge(PixelBuffer* _sourceBuffer, s32 _destX, s32 _destY, BlendMode _blendMode, f32 _sourceOpacity = 1.0f, MaskBuffer* _maskBuffer = nullptr);
		bool compare(PixelBuffer* _buffer);

		PixelBuffer* clone();

	};

}