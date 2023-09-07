/*
    ResourceManager.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/ResourceManager.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Shader.h"

namespace Zixel {

	struct ShaderLoadInfo {
		const char* name;
		const char* vertPath;
		const char* fragPath;
	};

	struct FontLoadInfo {
		const char* name;
		const char* path;
		s32 size;
	};

	static bool coreInitialized = false;
	static TextureAtlas* textureAtlas = nullptr;
	static std::unordered_map<std::string, TextureAtlasSpriteLoadInfo> textureLoadList;
	static std::unordered_map<std::string, FontLoadInfo> fontLoadList;
	static std::unordered_map<std::string, ShaderLoadInfo> shaderLoadList;
	static std::unordered_map<std::string, Shader*> shaderList;

	bool ResourceManager::init() {
		if (coreInitialized) {
			ZIXEL_WARN("Resource manager already initialized.");
			return true;
		}

		textureAtlas = new TextureAtlas();
		
		addFont("Zixel-Engine/Engine/Data/Fonts/Roboto-Regular.ttf", "robotoRegular11", 11);
		addFont("Zixel-Engine/Engine/Data/Fonts/Roboto-Regular.ttf", "robotoRegular12", 12);
		addFont("Zixel-Engine/Engine/Data/Fonts/Roboto-Regular.ttf", "robotoRegular13", 13);

		addTexture("Zixel-Engine/Engine/Data/Textures/tooltip.png", "tooltip");
		addTexture("Zixel-Engine/Engine/Data/Textures/widgetFocus.png", "widgetFocus");
		addTexture("Zixel-Engine/Engine/Data/Textures/scrollVer.png", "scrollVer", 4, 4, 8, 9);
		addTexture("Zixel-Engine/Engine/Data/Textures/scrollHor.png", "scrollHor", 4, 4, 9, 8);
		addTexture("Zixel-Engine/Engine/Data/Textures/scrollCorner.png", "scrollCorner");
		addTexture("Zixel-Engine/Engine/Data/Textures/windowTitleBar.png", "windowTitleBar", 2, 2, 30, 26);
		addTexture("Zixel-Engine/Engine/Data/Textures/windowContainer.png", "windowContainer", 2, 2, 6, 6);
		addTexture("Zixel-Engine/Engine/Data/Textures/windowClose.png", "windowClose", 4, 4, 17, 17);
		addTexture("Zixel-Engine/Engine/Data/Textures/windowMaximize.png", "windowMaximize", 10, 10, 17, 17);
		addTexture("Zixel-Engine/Engine/Data/Textures/dockArea.png", "dockArea");
		addTexture("Zixel-Engine/Engine/Data/Textures/dockContainer.png", "dockContainer", 2, 2, 3, 3);
		addTexture("Zixel-Engine/Engine/Data/Textures/dockPreview.png", "dockPreview");
		addTexture("Zixel-Engine/Engine/Data/Textures/dockSplitVer.png", "dockSplitVer", 2, 2, 2, 3);
		addTexture("Zixel-Engine/Engine/Data/Textures/dockSplitHor.png", "dockSplitHor", 2, 2, 3, 2);
		addTexture("Zixel-Engine/Engine/Data/Textures/dockTab.png", "dockTab", 3, 3, 15, 21);
		addTexture("Zixel-Engine/Engine/Data/Textures/dockTabClose.png", "dockTabClose", 2, 2, 9, 9);
		addTexture("Zixel-Engine/Engine/Data/Textures/dockTabLine.png", "dockTabLine");
		addTexture("Zixel-Engine/Engine/Data/Textures/dropDownMenu.png", "dropDownMenu", 3, 3, 9, 9);
		addTexture("Zixel-Engine/Engine/Data/Textures/dropDownMenuArrow.png", "dropDownMenuArrow", 2, 2, 7, 11);
		addTexture("Zixel-Engine/Engine/Data/Textures/dropDownMenuSeparator.png", "dropDownMenuSeparator");
		addTexture("Zixel-Engine/Engine/Data/Textures/menuBar.png", "menuBar");
		addTexture("Zixel-Engine/Engine/Data/Textures/menuBarButton.png", "menuBarButton", 2, 2, 1, 26);
		addTexture("Zixel-Engine/Engine/Data/Textures/toolBar.png", "toolBar");
		addTexture("Zixel-Engine/Engine/Data/Textures/toolBarSeparator.png", "toolBarSeparator");
		addTexture("Zixel-Engine/Engine/Data/Textures/panel.png", "panel");
		addTexture("Zixel-Engine/Engine/Data/Textures/button.png", "button", 4, 4, 17, 17);
		addTexture("Zixel-Engine/Engine/Data/Textures/toggleButton.png", "toggleButton", 4, 4, 17, 17);
		addTexture("Zixel-Engine/Engine/Data/Textures/sliderHor.png", "sliderHor", 4, 4, 15, 17);
		addTexture("Zixel-Engine/Engine/Data/Textures/comboBox.png", "comboBox", 4, 4, 17, 17);
		addTexture("Zixel-Engine/Engine/Data/Textures/comboBoxArrow.png", "comboBoxArrow", 2, 2, 11, 7);
		addTexture("Zixel-Engine/Engine/Data/Textures/checkbox.png", "checkbox", 3, 3, 9, 9);
		addTexture("Zixel-Engine/Engine/Data/Textures/checkboxIcon.png", "checkboxIcon", 2, 2, 12, 9);
		addTexture("Zixel-Engine/Engine/Data/Textures/radioButton.png", "radioButton", 3, 3, 19, 19);
		addTexture("Zixel-Engine/Engine/Data/Textures/radioButtonIcon.png", "radioButtonIcon", 2, 2, 5, 5);
		addTexture("Zixel-Engine/Engine/Data/Textures/radioButtonFocus.png", "radioButtonFocus");
		addTexture("Zixel-Engine/Engine/Data/Textures/treeViewBackground.png", "treeViewBackground");
		addTexture("Zixel-Engine/Engine/Data/Textures/treeViewArrow.png", "treeViewArrow", 4, 4, 11, 11);
		addTexture("Zixel-Engine/Engine/Data/Textures/treeViewDragDir.png", "treeViewDragDir");
		addTexture("Zixel-Engine/Engine/Data/Textures/textEdit.png", "textEdit", 3, 3, 3, 3);
		addTexture("Zixel-Engine/Engine/Data/Textures/colorPickerAnchorCircle.png", "colorPickerAnchorCircle");
		addTexture("Zixel-Engine/Engine/Data/Textures/colorPickerAnchorTriangle.png", "colorPickerAnchorTriangle");
		addTexture("Zixel-Engine/Engine/Data/Textures/colorPickerSwap.png", "colorPickerSwap", 2, 2, 5, 11);
		addTexture("Zixel-Engine/Engine/Data/Textures/colorPickerColorMode.png", "colorPickerColorMode", 2, 2, 6, 6);
		addTexture("Zixel-Engine/Engine/Data/Textures/colorPickerDisplayWheel.png", "colorPickerDisplayWheel");
		addTexture("Zixel-Engine/Engine/Data/Textures/colorPickerDisplayChannels.png", "colorPickerDisplayChannels");

		addShader("Zixel-Engine/Engine/Data/Shaders/colorWheelVert.glsl", "Zixel-Engine/Engine/Data/Shaders/colorWheelFrag.glsl", "colorWheel");
		addShader("Zixel-Engine/Engine/Data/Shaders/colorSquareVert.glsl", "Zixel-Engine/Engine/Data/Shaders/colorSquareFrag.glsl", "colorSquare");
		addShader("Zixel-Engine/Engine/Data/Shaders/colorBarVert.glsl", "Zixel-Engine/Engine/Data/Shaders/colorBarFrag.glsl", "colorBar");
		addShader("Zixel-Engine/Engine/Data/Shaders/colorPreviewVert.glsl", "Zixel-Engine/Engine/Data/Shaders/colorPreviewFrag.glsl", "colorPreview");
		addShader("Zixel-Engine/Engine/Data/Shaders/checkerVert.glsl", "Zixel-Engine/Engine/Data/Shaders/checkerFrag.glsl", "checker");

		for (const auto& it : shaderLoadList) {
			const ShaderLoadInfo& info = it.second;

			Shader* shader = new Shader();
			if (!shader->loadFromFile(info.vertPath, info.fragPath)) {
				delete shader;
				return false;
			}

			shaderList[info.name] = shader;
		}

		for (const auto& it : textureLoadList) {
			const TextureAtlasSpriteLoadInfo& info = it.second;
			textureAtlas->addTexture(info.filePath, info.name, info.numImages, info.numImagesPerColumn, info.subWidth, info.subHeight);
		}

		for (const auto& it : fontLoadList) {
			const FontLoadInfo& info = it.second;
			textureAtlas->addFont(info.path, info.name, info.size);
		}

		//Generate atlas.
		if (!textureAtlas->generateTextureAtlas()) {
			return false;
		}

		coreInitialized = true;

		ZIXEL_INFO("Initialized resource manager.");

		return true;
	}

	void ResourceManager::free() {
		if (textureAtlas != nullptr) delete textureAtlas;

		for (const auto& it : shaderList) {
			delete it.second;
		}

		ZIXEL_INFO("Destroyed resource manager.");
	}

	void ResourceManager::addTexture(const char* _filePath, const char* _spriteName, s32 _numImages, s32 _numImagesPerColumn, s32 _subWidth, s32 _subHeight) {
		if (textureLoadList.find(_spriteName) != textureLoadList.end()) {
			ZIXEL_WARN("Error in ResourceManager::addTexture. A texture with the name '{}' already exists.", _spriteName);
			return;
		}

		TextureAtlasSpriteLoadInfo info;
		info.filePath = _filePath;
		info.name = _spriteName;
		info.numImages = _numImages;
		info.numImagesPerColumn = _numImagesPerColumn;
		info.subWidth = _subWidth;
		info.subHeight = _subHeight;
		info.type = "sprite";

		textureLoadList[_spriteName] = info;
	}

	void ResourceManager::addFont(const char* _filePath, const char* _name, s32 _size) {
		if (fontLoadList.find(_name) != fontLoadList.end()) {
			ZIXEL_WARN("Error in ResourceManager::addFont. A font with the name '{}' already exists.", _name);
			return;
		}

		FontLoadInfo info;
		info.path = _filePath;
		info.name = _name;
		info.size = _size;

		fontLoadList[_name] = info;
	}

	TextureAtlas* ResourceManager::getTextureAtlas() {
		if (!coreInitialized) return nullptr;
		return textureAtlas;
	}

	void ResourceManager::addShader(const char* _vertexPath, const char* _fragmentPath, const char* _name) {
		if (shaderLoadList.find(_name) != shaderLoadList.end()) {
			ZIXEL_WARN("Error in ResourceManager::addShader. A shader with the name '{}' already exists.", _name);
			return;
		}

		ShaderLoadInfo info;
		info.name = _name;
		info.vertPath = _vertexPath;
		info.fragPath = _fragmentPath;

		shaderLoadList[_name] = info;
	}

	Shader* ResourceManager::getShader(const char* _name) {
		const auto& it = shaderList.find(_name);

		if (it == shaderList.end()) {
			ZIXEL_WARN("Error in ResourceManager::getShader. Shader '{}' does not exist.", _name);
			return nullptr;
		}

		return it->second;
	}

	Shader* ResourceManager::getShader(std::string& _name) {
		return getShader(_name.c_str());
	}
}