/*
    TextureAtlas.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <string>
#include <unordered_map>

namespace Zixel {

	class Texture;

	struct TextureAtlasLoadInfo {

		const char* filePath;
		const char* name;
		const char* type;

	};

	struct TextureAtlasSpriteLoadInfo : TextureAtlasLoadInfo {

		s32 numImages;
		s32 numImagesPerColumn;
		s32 subWidth;
		s32 subHeight;

	};

	struct TextureAtlasFontLoadInfo : TextureAtlasLoadInfo {
		s32 size;
	};

	struct TextureAtlasTexture {

		Texture* texture;
		s32 numImages;
		s32 numImagesPerColumn;
		s32 subWidth;
		s32 subHeight;

	};

	struct SubSprite {

		s32 textureAtlasPosX, textureAtlasPosY;
		f32 textureAtlasUVX, textureAtlasUVY;

	};

	struct Sprite {

		std::string name;

		s32 sizeX, sizeY;
		f32 uvSizeX, uvSizeY;

		std::vector<SubSprite*> subSpriteList;

	};

	struct FontGlyph {

		bool drawable;
		s32 textureAtlasPosX, textureAtlasPosY;
		f32 textureAtlasUVX, textureAtlasUVY;
		s32 sizeX, sizeY;
		f32 uvSizeX, uvSizeY;
		s32 bearingX, bearingY;
		s32 advanceX;

	};

	struct Font {

		std::string name;

		s32 height;
		s32 advanceY;

		std::vector<FontGlyph*> glyphs;

	};
	
	class TextureAtlas {

	private:
		std::unordered_map<const char*, TextureAtlasLoadInfo*> textureAtlasLoadList;
		std::unordered_map<std::string, Sprite*> textureAtlasSprites;
		std::unordered_map<std::string, Font*> textureAtlasFonts;
		bool textureAtlasGenerated = false;
		s32 textureAtlasWidth = 512;
		s32 textureAtlasHeight = 512;
		Texture* textureAtlas = nullptr;

	public:
		TextureAtlas();
		~TextureAtlas();

		void addTexture(const char* filePath, const char* spriteName, s32 numImages = -1, s32 numImagesPerColumn = -1, s32 subWidth = -1, s32 subHeight = -1);
		void addFont(const char* filePath, const char* fontName, s32 size);
		bool generateTextureAtlas();
		Texture* getTexture();
		Sprite* getTextureAtlasSprite(std::string spriteName);
		Font* getTextureAtlasFont(std::string fontName);
		
	};

}