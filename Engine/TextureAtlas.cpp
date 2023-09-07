/*
    TextureAtlas.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/Texture.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/Texture.h"
#include "Engine/FontData.h"

namespace Zixel {

	TextureAtlas::TextureAtlas() {
		
	}

	TextureAtlas::~TextureAtlas() {

		if (textureAtlasGenerated) {

			for (auto& it : textureAtlasSprites) {
				
				Sprite* sprite = it.second;
				for (SubSprite* subSprite : sprite->subSpriteList) {
					delete subSprite;
				}

				delete sprite;

			}

			for (auto& it : textureAtlasFonts) {

				Font* font = it.second;
				for (FontGlyph* glyph : font->glyphs) {
					delete glyph;
				}

				delete font;

			}

			delete textureAtlas;
		}

		ZIXEL_INFO("Destroyed texture atlas.");
	}

	void TextureAtlas::addTexture(const char* filePath, const char* spriteName, s32 numImages, s32 numImagesPerColumn, s32 subWidth, s32 subHeight) {

		if (textureAtlasLoadList.find(spriteName) != textureAtlasLoadList.end()) {
			ZIXEL_WARN("Unable to add texture \"{}\" to texture atlas. A texture or font with that name already exists.", spriteName);
			return;
		}

		TextureAtlasSpriteLoadInfo* loadInfo = new TextureAtlasSpriteLoadInfo();
		loadInfo->filePath = filePath;
		loadInfo->name = spriteName;
		loadInfo->type = "sprite";
		loadInfo->numImages = numImages;
		loadInfo->numImagesPerColumn = numImagesPerColumn;
		loadInfo->subWidth = subWidth;
		loadInfo->subHeight = subHeight;

		textureAtlasLoadList[spriteName] = loadInfo;
	}

	void TextureAtlas::addFont(const char* filePath, const char* fontName, s32 size) {

		if (textureAtlasLoadList.find(fontName) != textureAtlasLoadList.end()) {
			ZIXEL_WARN("Unable to add font \"{}\" to texture atlas. A texture or font with that name already exists.", fontName);
			return;
		}

		TextureAtlasFontLoadInfo* loadInfo = new TextureAtlasFontLoadInfo();
		loadInfo->filePath = filePath;
		loadInfo->name = fontName;
		loadInfo->type = "font";
		loadInfo->size = size;

		textureAtlasLoadList[fontName] = loadInfo;
	}

	bool TextureAtlas::generateTextureAtlas() {
		if (textureAtlasGenerated) {
			ZIXEL_WARN("Trying to generate a texture atlas that already exists. Ignoing...");
			return true;
		}

		if (textureAtlasLoadList.size() <= 0) {
			ZIXEL_WARN("Skipping texture atlas generation. No input textures specified.");
		}

		std::vector<TextureAtlasTexture*> textures;
		std::vector<const char*> names;

		//Add and sort textures from largest to smallest.
		for (auto& it : textureAtlasLoadList) {

			TextureAtlasLoadInfo* info = it.second;

			if (info->type == "sprite") {

				Texture* texture = new Texture(nullptr);

				TextureAtlasSpriteLoadInfo* spriteInfo = (TextureAtlasSpriteLoadInfo*)info;

				if (!texture->load(spriteInfo->filePath)) {

					for (TextureAtlasTexture* temp : textures) {
						delete temp->texture;
						delete temp;
					}

					delete texture;

					for (auto& it2 : textureAtlasLoadList) delete it2.second;

					ZIXEL_CRITICAL("Failed to generate texture atlas (0).");

					return false;
				}

				if (spriteInfo->numImages > 0 && spriteInfo->numImagesPerColumn > 0 && spriteInfo->subWidth > 0 && spriteInfo->subHeight > 0) {

					s32 width = Math::minInt(spriteInfo->numImagesPerColumn, spriteInfo->numImages) * spriteInfo->subWidth;
					s32 height = (((spriteInfo->numImages - 1) / spriteInfo->numImagesPerColumn) + 1) * spriteInfo->subHeight;

					if (width > texture->getWidth() || height > texture->getHeight()) {
						ZIXEL_WARN("Texture sheet specification for \"{}\" exceeds texture size. Ignoring texture.", it.first);

						delete texture;
						continue;
					}

				}

				TextureAtlasTexture* tex = new TextureAtlasTexture({ texture, spriteInfo->numImages, spriteInfo->numImagesPerColumn, spriteInfo->subWidth, spriteInfo->subHeight });

				s32 size = texture->getWidth() * texture->getHeight();

				s32 ind = 0;
				for (TextureAtlasTexture* temp : textures) {
					if ((temp->texture->getWidth() * temp->texture->getHeight()) < size) break;
					++ind;
				}

				textures.insert(textures.begin() + ind, tex);
				names.insert(names.begin() + ind, it.first);

			}

		}
		
		//Generate texture atlas.
		std::vector<GLubyte> atlasData((size_t)textureAtlasWidth * (size_t)textureAtlasHeight * 4);

		for (s32 i = 0; i < textures.size(); ++i) {
			
			TextureAtlasTexture* tex = textures[i];
			
			bool isSheet = !(tex->numImages < 0 || tex->numImagesPerColumn < 0 || tex->subWidth < 0 || tex->subHeight < 0);

			s32 width, height;
			if (!isSheet) {
				width = tex->texture->getWidth();
				height = tex->texture->getHeight();
			}
			else {
				width = tex->subWidth;
				height = tex->subHeight;
			}
			
			bool found = false;

			s32 placeX = 0;
			s32 placeY = 0;

			if (textureAtlasSprites.size() <= 0) found = true;
			
			while (!found) {
				
				s32 maxHeight = 0;
				bool place = true;

				for (auto& it : textureAtlasSprites) {
					Sprite* sprite = it.second;
					
					if (sprite->sizeY > maxHeight) maxHeight = sprite->sizeY;

					for (SubSprite* subSprite : sprite->subSpriteList) {

						if (placeX + tex->texture->getWidth() - 1 >= subSprite->textureAtlasPosX && placeY + tex->texture->getHeight() - 1 >= subSprite->textureAtlasPosY && placeX <= subSprite->textureAtlasPosX + sprite->sizeX - 1 && placeY <= subSprite->textureAtlasPosY + sprite->sizeY - 1) {

							placeX += sprite->sizeX;
							if (placeX + tex->texture->getWidth() - 1 >= textureAtlasWidth) {
								placeX = 0;
								placeY += 32; //@TODO: Hardcoded.
								maxHeight = 0;
							}

							place = false;
						}

					}
				}

				if (placeX + tex->texture->getWidth() - 1 >= textureAtlasWidth || placeY + tex->texture->getHeight() - 1 >= textureAtlasHeight) break;
				if (place) found = true;

			}
			
			if (!found) {
				ZIXEL_WARN("Unable to fit texture \"{}\" in texture atlas.", names[i]);
				continue;
			}

			//ZIXEL_TRACE("{} {} {}", names[i], placeX, placeY);

			Sprite* sprite = new Sprite();
			sprite->name = names[i];

			textureAtlasSprites[names[i]] = sprite;

			s32 numImages = (isSheet) ? Math::maxInt(1, tex->numImages) : 1;
			for (s32 j = 0; j < numImages; ++j) {

				s32 offX = 0, offY = 0;
				s32 texWidth, texHeight;
				s32 column = 0, row = 0;

				SubSprite* subSprite;

				if (!isSheet) {

					texWidth = tex->texture->getWidth();
					texHeight = tex->texture->getHeight();

					f32 u = (f32)placeX / (f32)textureAtlasWidth;
					f32 v = (f32)placeY / (f32)textureAtlasHeight;
					f32 uvW = (f32)texWidth / (f32)textureAtlasWidth;
					f32 uvH = (f32)texHeight / (f32)textureAtlasHeight;

					sprite->sizeX = texWidth;
					sprite->sizeY = texHeight;
					sprite->uvSizeX = uvW;
					sprite->uvSizeY = uvH;

					subSprite = new SubSprite();
					subSprite->textureAtlasPosX = placeX;
					subSprite->textureAtlasPosY = placeY;
					subSprite->textureAtlasUVX = u;
					subSprite->textureAtlasUVY = v;
				}
				else {

					texWidth = tex->subWidth;
					texHeight = tex->subHeight;

					column = (j % tex->numImagesPerColumn);
					row = (j / tex->numImagesPerColumn);
					
					offX = (column * texWidth);
					offY = (row * texHeight);
					
					f32 u = (f32)(placeX + offX) / (f32)textureAtlasWidth;
					f32 v = (f32)(placeY + offY) / (f32)textureAtlasHeight;
					f32 uvW = (f32)texWidth / (f32)textureAtlasWidth;
					f32 uvH = (f32)texHeight / (f32)textureAtlasHeight;

					if (j == 0) {
						sprite->sizeX = texWidth;
						sprite->sizeY = texHeight;
						sprite->uvSizeX = uvW;
						sprite->uvSizeY = uvH;
					}

					subSprite = new SubSprite();
					subSprite->textureAtlasPosX = placeX + offX;
					subSprite->textureAtlasPosY = placeY + offY;
					subSprite->textureAtlasUVX = u;
					subSprite->textureAtlasUVY = v;
				}
				
				sprite->subSpriteList.push_back(subSprite);

				for (s32 px = texWidth - 1; px >= 0; --px) {
					for (s32 py = texHeight - 1; py >= 0; --py) {
						s32 peek = (((py + (row * texHeight)) * tex->texture->getWidth()) + px + (column * texWidth)) * 4;
						s32 poke = (((placeY + offY + py) * textureAtlasWidth) + (placeX + offX + px)) * 4;

						GLubyte* texData = tex->texture->getData();

						atlasData[(size_t)(poke)] = texData[peek];
						atlasData[(size_t)(poke) + 1] = texData[peek + 1];
						atlasData[(size_t)(poke) + 2] = texData[peek + 2];
						atlasData[(size_t)(poke) + 3] = texData[peek + 3];
					}
				}

			}
		}

		//Load fonts.
		for (auto& it : textureAtlasLoadList) {
			
			TextureAtlasLoadInfo* info = it.second;
			if (info->type == "font") {

				TextureAtlasFontLoadInfo* fontInfo = (TextureAtlasFontLoadInfo*)info;

				FontData* fontData = new FontData();

				if (!fontData->load(fontInfo->filePath, fontInfo->size)) {

					ZIXEL_CRITICAL("Failed to generate texture atlas. Font \"{}\" doesn't exist.", fontInfo->filePath);

					for (TextureAtlasTexture* temp : textures) {
						delete temp->texture;
						delete temp;
					}

					for (auto& it2 : textureAtlasLoadList) delete it2.second;

					for (auto& it2 : textureAtlasSprites) {

						Sprite* sprite = it2.second;
						for (SubSprite* subSprite : sprite->subSpriteList) {
							delete subSprite;
						}

						delete sprite;

					}

					for (auto& it2 : textureAtlasFonts) {

						Font* font = it2.second;
						for (FontGlyph* glyph : font->glyphs) {
							delete glyph;
						}

						delete font;

					}

					return false;
				
				}

				Font* font = new Font();
				font->name = fontInfo->name;
				font->height = fontData->height;
				font->advanceY = fontData->advanceY;

				textureAtlasFonts[fontInfo->name] = font;

				for (FontDataGlyph& glyphData : fontData->glyphs) {

					if (glyphData.sizeX <= 0 || glyphData.sizeY <= 0) {

						FontGlyph* glyph = new FontGlyph();
						glyph->drawable = false;
						glyph->sizeX = glyphData.sizeX;
						glyph->sizeY = glyphData.sizeY;
						glyph->bearingX = glyphData.bearingX;
						glyph->bearingY = glyphData.bearingY;
						glyph->advanceX = glyphData.advanceX;

						font->glyphs.push_back(glyph);

						continue;
					}

					bool found = false;

					s32 placeX = 0;
					s32 placeY = 0;

					if (textureAtlasSprites.size() <= 0 && textureAtlasFonts.size() <= 0) found = true;

					while (!found) {

						s32 maxHeight = 0;
						bool place = true;

						for (auto& it : textureAtlasSprites) {
							Sprite* sprite = it.second;

							if (sprite->sizeY > maxHeight) maxHeight = sprite->sizeY;

							for (SubSprite* subSprite : sprite->subSpriteList) {

								if (placeX + glyphData.sizeX - 1 >= subSprite->textureAtlasPosX && placeY + glyphData.sizeY - 1 >= subSprite->textureAtlasPosY && placeX <= subSprite->textureAtlasPosX + sprite->sizeX - 1 && placeY <= subSprite->textureAtlasPosY + sprite->sizeY - 1) {

									placeX += sprite->sizeX;
									if (placeX + glyphData.sizeX - 1 >= textureAtlasWidth) {
										placeX = 0;
										placeY += 32; //@TODO: Hardcoded.
										maxHeight = 0;
									}

									place = false;
								}

							}
						}

						for (auto& it : textureAtlasFonts) {
							Font* font = it.second;

							for (FontGlyph* glyph: font->glyphs) {

								if (!glyph->drawable) continue;

								if (glyph->sizeY > maxHeight) maxHeight = glyph->sizeY;

								if (placeX + glyphData.sizeX - 1 >= glyph->textureAtlasPosX && placeY + glyphData.sizeY - 1 >= glyph->textureAtlasPosY && placeX <= glyph->textureAtlasPosX + glyph->sizeX - 1 && placeY <= glyph->textureAtlasPosY + glyph->sizeY - 1) {

									placeX += glyph->sizeX;
									if (placeX + glyphData.sizeX - 1 >= textureAtlasWidth) {
										placeX = 0;
										placeY += 32; //@TODO: Hardcoded.
										maxHeight = 0;
									}

									place = false;
								}

							}
						}

						if (placeX + glyphData.sizeX - 1 >= textureAtlasWidth || placeY + glyphData.sizeY - 1 >= textureAtlasHeight) break;
						if (place) found = true;

					}

					if (!found) {
						ZIXEL_WARN("Unable to fit font texture \"{}\" in texture atlas.", fontInfo->name);
						continue;
					}

					FontGlyph* glyph = new FontGlyph();
					glyph->drawable = true;
					glyph->textureAtlasPosX = placeX;
					glyph->textureAtlasPosY = placeY;
					glyph->textureAtlasUVX = (f32)placeX / (f32)textureAtlasWidth;
					glyph->textureAtlasUVY = (f32)placeY / (f32)textureAtlasHeight;
					glyph->sizeX = glyphData.sizeX;
					glyph->sizeY = glyphData.sizeY;
					glyph->uvSizeX = (f32)glyphData.sizeX / (f32)textureAtlasWidth;
					glyph->uvSizeY = (f32)glyphData.sizeY / (f32)textureAtlasHeight;
					glyph->bearingX = glyphData.bearingX;
					glyph->bearingY = glyphData.bearingY;
					glyph->advanceX = glyphData.advanceX;

					font->glyphs.push_back(glyph);

					for (s32 px = glyphData.sizeX - 1; px >= 0; --px) {
						for (s32 py = glyphData.sizeY - 1; py >= 0; --py) {
							s32 peek = (py * glyphData.sizeX) + px;
							s32 poke = (((placeY + py) * textureAtlasWidth) + (placeX + px)) * 4;

							atlasData[(size_t)(poke)] = 255;
							atlasData[(size_t)(poke) + 1] = 255;
							atlasData[(size_t)(poke) + 2] = 255;
							atlasData[(size_t)(poke) + 3] = glyphData.buffer[peek];
						}
					}

				}

				delete fontData;

			}

		}
		
		textureAtlas = new Texture(nullptr);
		textureAtlas->createFromData(textureAtlasWidth, textureAtlasHeight, atlasData.data());

		for (TextureAtlasTexture* temp : textures) {
			delete temp->texture;
			delete temp;
		}

		textureAtlasGenerated = true;
		ZIXEL_INFO("Generated texture atlas.");

		for (auto& it : textureAtlasLoadList) delete it.second;
		textureAtlasLoadList.clear();

		return true;
	}

	Texture* TextureAtlas::getTexture() {
		return textureAtlas;
	}

	Sprite* TextureAtlas::getTextureAtlasSprite(std::string spriteName) {
		/*if (index < 0 || index >= subSprites->size()) {

			if (subSprites->size() <= 0) {
				ZIXEL_WARN("Error in TextureAtlas::getTextureAtlasSprite for sprite \"{}\", no sub-sprites exists.", spriteName);
			}
			else {
				ZIXEL_WARN("Error in TextureAtlas::getTextureAtlasSprite for sprite \"{}\", invalid index {} given. Range = 0-{}.", spriteName, index, subSprites->size() - 1);
			}

			return nullptr;
		}*/

		auto it = textureAtlasSprites.find(spriteName);

		if (it == textureAtlasSprites.end()) {
			ZIXEL_WARN("Error in TextureAtlas::getTextureAtlasSprite. No sprite with name \"{}\".", spriteName);
			return nullptr;
		}

		return it->second;
	}

	Font* TextureAtlas::getTextureAtlasFont(std::string fontName) {
		auto it = textureAtlasFonts.find(fontName);

		if (it == textureAtlasFonts.end()) {
			ZIXEL_WARN("Error in TextureAtlas::getTextureAtlasFont. No font with name \"{}\".", fontName);
			return nullptr;
		}

		return it->second;
	}

}