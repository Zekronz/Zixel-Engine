/*
    Panel.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/GUI/Panel.h"
#include "Engine/GUI/Theme.h"
#include "Engine/GUI/GUI.h"

namespace Zixel {

	Panel::Panel(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::Panel;
		panelTheme = (_theme != nullptr) ? (PanelTheme*)_theme : (PanelTheme*)gui->getDefaultTheme(type);

		sprPanel = renderer->getTextureAtlasSprite(panelTheme->sprPanel);
		cutContent = true;
		renderFocus = false;

	}

	void Panel::render() {
		renderer->render9P(sprPanel, 0, x, y, viewportWidth, viewportHeight);
	}

}