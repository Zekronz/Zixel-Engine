#pragma once

#include "Engine/Color.h"

namespace Zixel {

	struct Renderer;
	struct PixelBuffer;

	struct Surface {

		Renderer* renderer;

		u32 fbo = 0;
		u32 tex = 0;

		u32 width;
		u32 height;

		bool created = false;

		Surface(Renderer* _renderer, u32 _width, u32 _height, PixelBuffer* _pixelBuffer = nullptr);
		~Surface();

		void clear(Color4f _color);
		void setData(PixelBuffer* _pixelBuffer);

	};

}