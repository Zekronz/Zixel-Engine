#pragma once

#include "Engine/Color.h"
#include <GLFW/glfw3.h>

namespace Zixel {

	struct Renderer;
	struct GUI;

	class ZixelApp {
	private:
		Renderer* renderer;
		GUI* gui;

		bool initialized = false;

		//Window.
		GLFWwindow* window = nullptr;
		s32 windowWidth, windowHeight;
		const char* windowTitle;
		Color3f windowClearColor;

		bool frameRateUpdateCap = true;
		f64 frameRateUpdate = 1000.0;
		f64 frameTimeStepUpdate = (1000.0 / frameRateUpdate);
		f64 frameTimeCounterUpdate = 0;
		f64 frameTimeCurUpdate = 0;
		f64 frameTimePrevUpdate = 0;

		f64 frameRateRender = 144.0;
		f64 frameTimeStepRender = (1000.0 / frameRateRender);
		f64 frameTimeCounterRender = 0;

		f64 frameTimeCur = 0;
		f64 frameTimePrev = 0;

		s32 frameCounter = 0;
		f64 frameCounterTime = 0;

		std::function<bool()> callbackOnWindowClose;
		std::function<void(s32, const char**)> callbackOnFileDrop;

	public:
		ZixelApp(s32 _width, s32 _height, const char* _title);
		~ZixelApp();

		//Window.
		GLFWwindow* getWindow();
		void setClearColor(Color3f& _clearColor);
		void setWindowIcon(GLFWimage& _image);

		//Renderer.
		Renderer* getRenderer();

		//GUI.
		GUI* getGUI();

		//Callbacks.
		void onWindowResize(s32 _width, s32 _height);
		bool onWindowClose();
		void onWindowFocus(bool _focused);
		void onKeyPress(u16 _button);
		void onKeyRelease(u16 _button);
		void onTextInput(u32 _codepoint);
		void onMousePress(u8 _button);
		void onMouseRelease(u8 _button);
		void onMouseScroll(s8 _dir);
		void onFileDrop(s32 _count, const char** _paths);

		void setOnWindowClose(std::function<bool()> _callback);
		void setOnFileDrop(std::function<void(s32, const char**)> _callback);

		//Events.
		bool init();
		void run();
		void update(f32 dt);
		void render();

		//Functions.
		void exit();

	};

}
