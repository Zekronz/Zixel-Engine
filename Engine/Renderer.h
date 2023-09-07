#pragma once

#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>

#include "Engine/Color.h"
#include "Engine/Cursor.h"

namespace Zixel {

	enum class Blend : GLuint {

		Zero = GL_ZERO,
		One = GL_ONE,
		SrcCol = GL_SRC_COLOR,
		InvSrcCol = GL_ONE_MINUS_SRC_COLOR,
		DestCol = GL_DST_COLOR,
		InvDestCol = GL_ONE_MINUS_DST_COLOR,
		SrcAlpha = GL_SRC_ALPHA,
		InvSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
		DestAlpha = GL_DST_ALPHA,
		InvDestAlpha = GL_ONE_MINUS_DST_ALPHA,

	};

	enum class TextAlign : u8 {

		Left,
		Center,
		Right,

		Top,
		Middle,
		Bottom

	};

	struct Cut {
		s32 x, y, width, height;
	};

	struct Surface;
	struct Sprite;
	struct Shader;
	struct Font;
	class Texture;
	class TextureAtlas;
	class UTF8String;

	struct Renderer {

		//@TODO: We should add support for renderer states.

		//Private.
		bool initialized = false;

		GLFWwindow* window = nullptr;
		s32 windowWidth = 0;
		s32 windowHeight = 0;

		bool atRenderStage = false;

		//Shader.
		Shader* currentShader = nullptr;
		u8 shaderSamplerIndex = 0;
		std::unordered_map<GLint, u8> shaderSamplers;

		//Blend mode.
		Blend blendSourceColor = Blend::SrcAlpha;
		Blend blendSourceAlpha = Blend::SrcAlpha;
		Blend blendDestColor = Blend::InvSrcAlpha;
		Blend blendDestAlpha = Blend::InvSrcAlpha;

		Blend blendSourceColorSaved = blendSourceColor;
		Blend blendSourceAlphaSaved = blendSourceAlpha;
		Blend blendDestColorSaved = blendDestColor;
		Blend blendDestAlphaSaved = blendDestAlpha;

		//Cut.
		//@Consider: For now cuts don't work when rendering to surfaces, should we add support for this in the future?
		//			 Also, would we want to allow rendering to surfaces outside of the render stage? Sounds like a bad idea to me.
		std::vector<Cut> cutStack;
		u16 cutPausedCounter = 0;

		//Quad shader.
		glm::mat4 quadMat;

		GLuint quadVBO = 0;
		GLuint quadVAO = 0;
		GLuint quadEBO = 0;

		Shader* quadShader = nullptr;

		//Surface.
		Surface* targetSurface = nullptr;

		//Standard cursors.
		GLFWcursor* cursorIBeam = nullptr;
		GLFWcursor* cursorCrosshair = nullptr;
		GLFWcursor* cursorHand = nullptr;
		GLFWcursor* cursorInvalid = nullptr;
		GLFWcursor* cursorSizeWE = nullptr;
		GLFWcursor* cursorSizeNS = nullptr;
		GLFWcursor* cursorSizeNWSE = nullptr;
		GLFWcursor* cursorSizeNESW = nullptr;
		GLFWcursor* cursorSizeAll = nullptr;
		GLFWcursor* cursorRotateTopLeft = nullptr;
		GLFWcursor* cursorRotateTopRight = nullptr;
		GLFWcursor* cursorRotateBottomRight = nullptr;
		GLFWcursor* cursorRotateBottomLeft = nullptr;
		GLFWcursor* cursorRotateTopMiddle = nullptr;
		GLFWcursor* cursorRotateRightMiddle = nullptr;
		GLFWcursor* cursorRotateBottomMiddle = nullptr;
		GLFWcursor* cursorRotateLeftMiddle = nullptr;

		//Public.
		Renderer();
		~Renderer();

		bool init(GLFWwindow* _window);

		f64 getCurrentTime();

		void getWindowSize(s32& width, s32& height);
		void onWindowResize(s32 _width, s32 _height);

		void getMousePos(s32& x, s32& y);
		void showMouseCursor();
		void hideMouseCursor();
		void setMouseCursor(u8 _cursor);

		void setBlendMode(Blend _sourceFactor, Blend _destFactor);
		void setBlendMode(Blend _sourceColorFactor, Blend _destColorFactor, Blend _sourceAlphaFactor, Blend _destAlphaFactor);
		void defaultBlendMode();
		void saveBlendModeState();
		void restoreBlendModeState();

		void cutStart(s32 x, s32 y, s32 width, s32 height);
		void cutEnd();
		void cutPause();
		void cutResume();

		//void setResourceManager(ResourceManager* _resourceManager);
		TextureAtlas* getTextureAtlas();
		Sprite* getTextureAtlasSprite(std::string spriteName);
		Font* getTextureAtlasFont(std::string fontName);
		Shader* getShader(std::string _shaderName);

		s32 getStringWidth(Font* font, std::string& text);
		s32 getStringWidth(Font* font, UTF8String& text);
		s32 getStringWidth(Font* font, char text);

		s32 getStringHeight(Font* font, std::string& text);
		s32 getStringHeight(Font* font, UTF8String& text);
		s32 getStringHeight(Font* font, char text);

		void setShader(Shader* _shader);
		void resetShader();
		void setDefaultShader();
		void bindTexture(GLint _uniform, GLuint _textureHandle);
		void bindTexture(GLint _uniform, Texture* _texture);
		void bindTexture(GLint _uniform, Surface* _surface);

		void renderBegin(Color3f& clearColor);
		void renderEnd();

		void bindSurface(Surface* _surface);
		void unbindSurface();

		void renderTexture(Texture* texture, s32 x, s32 y, s32 width, s32 height, f32 alpha = 1.0f);

		void renderSprite(const char* spriteName, s32 index, s32 x, s32 y, f32 alpha = 1.0f, Color4f blend = { 1.0f, 1.0f, 1.0f, 1.0f });
		void renderSprite(Sprite* sprite, s32 index, s32 x, s32 y, f32 alpha = 1.0f, Color4f blend = { 1.0f, 1.0f, 1.0f, 1.0f });

		void renderSpriteStretched(const char* _spriteName, s32 _index, s32 _x, s32 _y, s32 _width, s32 _height, f32 _alpha = 1.0f);
		void renderSpriteStretched(Sprite* _sprite, s32 _index, s32 _x, s32 _y, s32 _width, s32 _height, f32 _alpha = 1.0f);

		void renderSpritePart(const char* spriteName, s32 index, s32 left, s32 top, s32 width, s32 height, s32 x, s32 y, f32 alpha = 1.0f);
		void renderSpritePart(Sprite* sprite, s32 index, s32 left, s32 top, s32 width, s32 height, s32 x, s32 y, f32 alpha = 1.0f);

		void renderSpritePartStretched(const char* spriteName, s32 index, s32 left, s32 top, s32 partWidth, s32 partHeight, s32 x, s32 y, s32 width, s32 height, f32 alpha = 1.0f);
		void renderSpritePartStretched(Sprite* sprite, s32 index, s32 left, s32 top, s32 partWidth, s32 partHeight, s32 x, s32 y, s32 width, s32 height, f32 alpha = 1.0f);

		void render9P(const char* spriteName, s32 index, s32 x, s32 y, s32 width, s32 height, f32 alpha = 1.0f);
		void render9P(Sprite* sprite, s32 index, s32 x, s32 y, s32 width, s32 height, f32 alpha = 1.0f);

		void render3PHor(const char* spriteName, s32 index, s32 x, s32 y, s32 width, f32 alpha = 1.0f);
		void render3PHor(Sprite* sprite, s32 index, s32 x, s32 y, s32 width, f32 alpha = 1.0f);

		void render3PVer(const char* spriteName, s32 index, s32 x, s32 y, s32 height, f32 alpha = 1.0f);
		void render3PVer(Sprite* sprite, s32 index, s32 x, s32 y, s32 height, f32 alpha = 1.0f);

		void render9PRepeat(const char* _spriteName, s32 _index, s32 _x, s32 _y, s32 _width, s32 _height, f32 _alpha = 1.0f);
		void render9PRepeat(Sprite* _sprite, s32 _index, s32 _x, s32 _y, s32 _width, s32 _height, f32 _alpha = 1.0f);

		void renderText(const char* fontName, std::string& text, s32 x, s32 y, TextAlign hAlign = TextAlign::Left, TextAlign vAlign = TextAlign::Top, Color4f color = { 1.0f, 1.0f, 1.0f, 1.0f });
		void renderText(Font* font, std::string& text, s32 x, s32 y, TextAlign hAlign = TextAlign::Left, TextAlign vAlign = TextAlign::Top, Color4f color = { 1.0f, 1.0f, 1.0f, 1.0f });
		void renderText(Font* font, UTF8String& text, s32 x, s32 y, TextAlign hAlign = TextAlign::Left, TextAlign vAlign = TextAlign::Top, Color4f color = { 1.0f, 1.0f, 1.0f, 1.0f });

		void renderSurface(Surface* _surface, s32 _x, s32 _y, f32 _alpha = 1.0f);
		void renderSurfaceStretched(Surface* _surface, s32 _x, s32 _y, s32 _width, s32 _height, f32 _alpha = 1.0f);

		void renderRect(s32 _x, s32 _y, s32 _width, s32 _height, Color4f color = { 1.0f, 1.0f, 1.0f, 1.0f });
		void renderRectOutline(s32 _x, s32 _y, s32 _width, s32 _height, s32 _outlineWidth = 1, Color4f color = { 1.0f, 1.0f, 1.0f, 1.0f });

	};

}