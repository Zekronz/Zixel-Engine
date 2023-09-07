/*
    ResourceManager.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <string>

namespace Zixel {

	struct Shader;
	class TextureAtlas;

	struct ResourceManager {
		static bool init();
		static void free();

		static void addTexture(const char* _filePath, const char* _spriteName, s32 _numImages = -1, s32 _numImagesPerColumn = -1, s32 _subWidth = -1, s32 _subHeight = -1);
		static void addFont(const char* _filePath, const char* _name, s32 _size);
		static TextureAtlas* getTextureAtlas();
		static void addShader(const char* _vertexPath, const char* _fragmentPath, const char* _name);
		static Shader* getShader(const char* _name);
		static Shader* getShader(std::string& _name);
	};

}