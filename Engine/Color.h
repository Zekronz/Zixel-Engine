/*
    Color.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <cmath>
#include <string>

#include "Engine/Types.h"

namespace Zixel {

#define COLOR_RED { 1.0f, 0.0f, 0.0f, 1.0f }
#define COLOR_GREEN { 0.0f, 1.0f, 0.0f, 1.0f }
#define COLOR_BLUE { 0.0f, 0.0f, 1.0f, 1.0f }
#define COLOR_BLACK { 0.0f, 0.0f, 0.0f, 1.0f }
#define COLOR_WHITE { 1.0f, 1.0f, 1.0f, 1.0f }

	enum class BlendMode : u8 {

		Normal,
		Multiply,
		Additive,
		Subtractive,
		Overwrite,
		__Count,

	};

	struct Color3 {

		unsigned char r = 0;
		unsigned char g = 0;
		unsigned char b = 0;

	};

	struct Color4 {

		unsigned char r = 0;
		unsigned char g = 0;
		unsigned char b = 0;
		unsigned char a = 0;

	};

	struct Color3f {

		f32 r = 0.0f;
		f32 g = 0.0f;
		f32 b = 0.0f;

	};

	struct Color4f {

		f32 r = 0.0f;
		f32 g = 0.0f;
		f32 b = 0.0f;
		f32 a = 1.0f;

	};

	struct Color {

		static Color3 toColor3(Color3f _color);
		static Color3f toColor3f(Color3 _color);
		static Color4 toColor4(Color4f _color);
		static Color4f toColor4f(Color4 _color);
		static bool match(Color4 _color1, Color4 _color2);
		static Color3 HSVToRGB(Color3 _hsv);
		static Color3 RGBToHSV(Color3 _rgb);
		static std::string RGBToHex(Color3 _rgb);
		static Color3 hexToRGB(std::string& _hex);
		static u8 RGBToLum(Color3 _rgb);
		static Color4 blendColor(Color4 _source, Color4 _destination, BlendMode _blendMode);
		static u8 subtractAlpha(u8 _source, u8 _destination);

	};

}