#include "Engine/ZixelPCH.h"
#include "Engine/Texture.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Zixel {
	
	Texture::Texture(Renderer* _renderer) {
		renderer = _renderer;
	}

	Texture::~Texture() {

		if (loaded) {

			if (data != nullptr) free(data);
			glDeleteTextures(1, &texId);

		}

	}

	bool Texture::load(const char* filePath) {

		if (loaded) {
			ZIXEL_CRITICAL("Texture already generated.");
			return false;
		}

		u8* tempData = stbi_load(filePath, &width, &height, &nChannels, 0);
		
		if (!tempData) {
			ZIXEL_CRITICAL("Unable to load texture: \"{}\"", filePath);
			tempData = nullptr;
			return false;
		}

		size_t dataSize = (size_t)width * (size_t)height * 4;
		data = (u8*)malloc(dataSize);

		if(data != NULL) memcpy(data, tempData, dataSize);
		else {
			ZIXEL_CRITICAL("Unable to allocate memory for texture: \"{}\"", filePath);
			stbi_image_free(tempData);
			return false;
		}	

		glGenTextures(1, &texId);
		if (texId == 0) {
			ZIXEL_CRITICAL("Unable to generate OpenGL texture for: \"{}\"", filePath);
			stbi_image_free(tempData);
			free(data);
			return false;
		}

		glBindTexture(GL_TEXTURE_2D, texId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D, (renderer != nullptr && renderer->getTextureAtlas() != nullptr) ? renderer->getTextureAtlas()->getTexture()->getId() : 0);

		stbi_image_free(tempData);

		loaded = true;

		return true;
	}

	bool Texture::createFromData(s32 width, s32 height, u8* textureData) {

		if (loaded) {
			ZIXEL_CRITICAL("Texture already generated.");
			return false;
		}

		size_t dataSize = (size_t)width * (size_t)height * 4;
		data = (u8*)malloc(dataSize);

		if (data != NULL) memcpy(data, textureData, dataSize);
		else {
			ZIXEL_CRITICAL("Unable to allocate memory for texture. (createFromData)");
			return false;
		}

		glGenTextures(1, &texId);

		glBindTexture(GL_TEXTURE_2D, texId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D, (renderer != nullptr && renderer->getTextureAtlas() != nullptr) ? renderer->getTextureAtlas()->getTexture()->getId() : 0);

		loaded = true;

		return true;

	}

	s32 Texture::getWidth() {
		return width;
	}

	s32 Texture::getHeight() {
		return height;
	}

	u8* Texture::getData() {
		return data;
	}

	u32 Texture::getId() {
		return texId;
	}

}