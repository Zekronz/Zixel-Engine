/*
    Surface.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/Surface.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Texture.h"
#include "Engine/PixelBuffer.h"

namespace Zixel {

	Surface::Surface(Renderer* _renderer, u32 _width, u32 _height, PixelBuffer* _pixelBuffer) {

		renderer = _renderer;
		width = _width;
		height = _height;

		if (width <= 0 || height <= 0) {

			ZIXEL_WARN("Unable to create Surface with size of {}, {}. Width and height must be greater than 0.", width, height);
			return;

		}
		

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (_pixelBuffer != nullptr) ? _pixelBuffer->buffer : NULL);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

		if (_pixelBuffer == nullptr) {

			if(renderer->atRenderStage) renderer->cutPause();

			glViewport(0, 0, width, height);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glViewport(0, 0, renderer->windowWidth, renderer->windowHeight);

			if (renderer->atRenderStage) renderer->cutResume();

		}

		glBindTexture(GL_TEXTURE_2D, (renderer->getTextureAtlas() != nullptr) ? renderer->getTextureAtlas()->getTexture()->getId() : 0);
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
			created = true;
		}
		else {
			ZIXEL_WARN("Error creating Surface.");
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, (renderer->targetSurface != nullptr) ? renderer->targetSurface->fbo : 0);
	}

	Surface::~Surface() {

		if (created) {

			glDeleteFramebuffers(1, &fbo);
			glDeleteTextures(1, &tex);

		}

	}

	void Surface::clear(Color4f _color) {

		if (created) {

			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			glViewport(0, 0, width, height);

			renderer->cutPause();

			glClearColor(_color.r, _color.g, _color.b, _color.a);
			glClear(GL_COLOR_BUFFER_BIT);
			
			renderer->cutResume();

			glViewport(0, 0, renderer->windowWidth, renderer->windowHeight);

			glBindFramebuffer(GL_FRAMEBUFFER, (renderer->targetSurface != nullptr) ? renderer->targetSurface->fbo : 0);

		}

	}

	void Surface::setData(PixelBuffer* _pixelBuffer) {

		if (created) {

			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixelBuffer->buffer);
			glBindTexture(GL_TEXTURE_2D, (renderer->getTextureAtlas() != nullptr) ? renderer->getTextureAtlas()->getTexture()->getId() : 0);

		}

	}

}