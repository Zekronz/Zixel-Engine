/*
    Texture.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

namespace Zixel {

	struct Renderer;

	class Texture {

	private:

		Renderer* renderer;

		s32 width = 0;
		s32 height = 0;
		s32 nChannels = 0;
		u8* data = nullptr;
		u32 texId = 0;

		bool loaded = false;

	public:

		Texture(Renderer* _renderer);
		~Texture();

		bool load(const char* filePath);
		bool createFromData(s32 width, s32 height, u8* textureData);

		s32 getWidth();
		s32 getHeight();
		u8* getData();

		u32 getId();

	};

}