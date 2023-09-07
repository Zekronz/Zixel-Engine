#include "Engine/ZixelPCH.h"
#include "Engine/Zixel.h"
#include "Engine/ResourceManager.h"
#include "Engine/Texture.h"
#include "Engine/Renderer.h"
#include "Engine/GUI/GUI.h"

extern "C" {
	//Prioritizes high performance GPU.
	__declspec(dllexport) u32 NvOptimusEnablement = 1;
	__declspec(dllexport) s32 AmdPowerXpressRequestHighPerformance = 1;
}

namespace Zixel {

	static ZixelApp* __Zixel_App = nullptr;

	static void Zixel_onWindowResize(GLFWwindow* _window, s32 _width, s32 _height) {

		if (__Zixel_App == nullptr) return;
		__Zixel_App->onWindowResize(_width, _height);

	}

	static void Zixel_onWindowRefresh(GLFWwindow* window) {
		
		if (__Zixel_App == nullptr) return;
		__Zixel_App->render();

	}

	static void Zixel_onWindowClose(GLFWwindow* _window) {

		if (__Zixel_App == nullptr) return;
		if (!__Zixel_App->onWindowClose()) glfwSetWindowShouldClose(_window, GLFW_FALSE);

	}

	static void Zixel_onWindowFocus(GLFWwindow* _window, s32 _focused) {
		
		if (__Zixel_App == nullptr) return;
		__Zixel_App->onWindowFocus((bool)_focused);

	}

	static void Zixel_onKeyButton(GLFWwindow* _window, s32 _key, s32 _scancode, s32 _action, s32 _mods) {

		if (__Zixel_App == nullptr) return;
		if (_action == GLFW_PRESS) __Zixel_App->onKeyPress((u16)_key);
		else if (_action == GLFW_RELEASE) __Zixel_App->onKeyRelease((u16)_key);

	}

	static void Zixel_onTextInput(GLFWwindow* _window, u32 _codepoint) {

		if (__Zixel_App == nullptr) return;
		__Zixel_App->onTextInput(_codepoint);

	}

	static void Zixel_onMouseButton(GLFWwindow* _window, s32 _button, s32 _action, s32 _mods) {

		if (__Zixel_App == nullptr) return;
		if (_action == GLFW_PRESS) __Zixel_App->onMousePress((u8)_button);
		else if (_action == GLFW_RELEASE) __Zixel_App->onMouseRelease((u8)_button);

	}

	static void Zixel_onMouseScroll(GLFWwindow* _window, f64 _xOff, f64 _yOff) {

		//@Consider: Should we take into account the actual distance that the mouse wheel has scrolled?

		if (__Zixel_App == nullptr) return;
		if (_yOff < 0) __Zixel_App->onMouseScroll(1);
		else if (_yOff > 0) __Zixel_App->onMouseScroll(-1);

	}

	static void Zixel_onFileDrop(GLFWwindow* _window, s32 _count, const char** _paths) {

		if (__Zixel_App == nullptr) return;
		__Zixel_App->onFileDrop(_count, _paths);
		
	}

	ZixelApp::ZixelApp(s32 _width, s32 _height, const char* _title) {

		#ifndef NDEBUG
		Log::Init();
		#endif

		windowWidth = _width;
		windowHeight = _height;
		windowTitle = _title;

		renderer = new Renderer();
		gui = new GUI();

		__Zixel_App = this;
		
	}

	ZixelApp::~ZixelApp() {

		delete gui;
		delete renderer;

		ResourceManager::free();

		if (initialized) {
			glfwTerminate();
		}

		ZIXEL_INFO("Destroyed Zixel.");

	}

	//Window.
	GLFWwindow* ZixelApp::getWindow() {
		return window;
	}

	void ZixelApp::setClearColor(Color3f& clearColor) {
		windowClearColor = clearColor;
	}

	void ZixelApp::setWindowIcon(GLFWimage& _image) {
		glfwSetWindowIcon(window, 1, &_image);
	}

	//Renderer.
	Renderer* ZixelApp::getRenderer() {
		return renderer;
	}

	//GUI.
	GUI* ZixelApp::getGUI() {
		return gui;
	}

	//Events.
	bool ZixelApp::init() {

		//Initialize GLFW.
		if (glfwInit() == GLFW_FALSE) {

			ZIXEL_CRITICAL("Unable to initialize GLFW.");
			return false;

		}

		ZIXEL_INFO("Initialized GLFW.");

		//Specify OpenGL version.
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		//Create GLFW window.
		window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
		if (window == NULL) {

			ZIXEL_CRITICAL("Failed to create GLFW window.");
			glfwTerminate();

			return false;

		}

		glfwMakeContextCurrent(window);
		glfwSwapInterval(0);
		timeBeginPeriod(1);

		ZIXEL_INFO("Created GLFW window.");

		//Initialize GLAD.
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {

			ZIXEL_CRITICAL("Unable to initialize GLAD.");
			glfwTerminate();

			return false;

		}

		ZIXEL_INFO("Initialized GLAD.");

		//Initialize renderer.
		if (!renderer->init(window)) {
			glfwTerminate();
			return false;
		}

		ZIXEL_INFO("Initializing Zixel...");

		Color3f clearCol = { 32.0f / 255.0f, 33.0f / 255.0f, 40.0f / 255.0f };
		setClearColor(clearCol);

		//Initialize resource mananger.
		if (!ResourceManager::init()) {
			glfwTerminate();
			return false;
		}

		//Initialize GUI.
		if (!gui->init(renderer)) {
			glfwTerminate();
			return false;
		}
		
		//Set GLFW callbacks.
		glfwSetFramebufferSizeCallback(window, Zixel_onWindowResize);
		glfwSetWindowRefreshCallback(window, Zixel_onWindowRefresh);
		glfwSetWindowCloseCallback(window, Zixel_onWindowClose);
		glfwSetWindowFocusCallback(window, Zixel_onWindowFocus);
		glfwSetKeyCallback(window, Zixel_onKeyButton);
		glfwSetCharCallback(window, Zixel_onTextInput);
		glfwSetMouseButtonCallback(window, Zixel_onMouseButton);
		glfwSetScrollCallback(window, Zixel_onMouseScroll);
		glfwSetDropCallback(window, Zixel_onFileDrop);

		initialized = true;

		return true;
	}

	void ZixelApp::run() {
		ZIXEL_INFO("Zixel loop begin.");

		while (!glfwWindowShouldClose(window)) {

			frameTimePrev = frameTimeCur;
			frameTimeCur = glfwGetTime() * 1000.0;

			++frameCounter;
			frameCounterTime += (frameTimeCur - frameTimePrev) / 1000.0;
			if (frameCounterTime >= 1) {

				frameCounterTime = 0;
				ZIXEL_TRACE("FPS: {}", frameCounter);
				frameCounter = 0;

			}

			if (frameRateUpdateCap) {

				frameTimeCounterUpdate += (frameTimeCur - frameTimePrev);
				if (frameTimeCounterUpdate >= frameTimeStepUpdate) {

					glfwPollEvents();

					frameTimePrevUpdate = frameTimeCurUpdate;
					frameTimeCurUpdate = frameTimeCur;

					update((f32)((frameTimeCurUpdate - frameTimePrevUpdate) / 1000));
					frameTimeCounterUpdate = 0;

				}

			}
			else {

				glfwPollEvents();
				update((f32)((frameTimeCur - frameTimePrev) / 1000));

			}

			frameTimeCounterRender += (frameTimeCur - frameTimePrev);
			if (frameTimeCounterRender >= frameTimeStepRender) {

				render();
				frameTimeCounterRender = 0;

			}

			if (frameRateUpdateCap) Sleep(1); //@TODO: Platform specific.
		
		}

		ZIXEL_INFO("Zixel loop end.");
	}

	void ZixelApp::update(f32 dt) {
		gui->update(dt);
	}

	void ZixelApp::render() {
		
		renderer->renderBegin(windowClearColor);
		gui->render();
		renderer->renderEnd();

	}

	//Functions.
	void ZixelApp::exit() {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	//Callbacks.
	void ZixelApp::onWindowResize(s32 _width, s32 _height) {

		if (_width == 0 && _height == 0) return; //This happens when main window is minimized.

		s32 prevWidth = windowWidth;
		s32 prevHeight = windowHeight;

		windowWidth = _width;
		windowHeight = _height;

		renderer->onWindowResize(_width, _height);
		gui->onWindowResize(_width, _height, prevWidth, prevHeight);

	}

	bool ZixelApp::onWindowClose() {

		if (callbackOnWindowClose) return callbackOnWindowClose();
		return false;

	}

	void ZixelApp::onWindowFocus(bool _focused) {
		gui->onWindowFocus(_focused);
	}

	void ZixelApp::onKeyPress(u16 _button) {
		gui->onKeyPress(_button);
	}

	void ZixelApp::onKeyRelease(u16 _button) {
		gui->onKeyRelease(_button);
	}

	void ZixelApp::onTextInput(u32 _codepoint) {
		gui->onTextInput(_codepoint);
	}

	void ZixelApp::onMousePress(u8 _button) {
		gui->onMousePress(_button);
	}

	void ZixelApp::onMouseRelease(u8 _button) {
		gui->onMouseRelease(_button);
	}

	void ZixelApp::onMouseScroll(s8 _dir) {
		gui->onMouseScroll(_dir);
	}

	void ZixelApp::onFileDrop(s32 _count, const char** _paths) {
		if (callbackOnFileDrop) callbackOnFileDrop(_count, _paths);
	}

	void ZixelApp::setOnWindowClose(std::function<bool()> _callback) {
		callbackOnWindowClose = _callback;
	}

	void ZixelApp::setOnFileDrop(std::function<void(s32, const char**)> _callback) {
		callbackOnFileDrop = _callback;
	}

}