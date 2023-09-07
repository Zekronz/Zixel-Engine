/*
    Panel.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include "Engine/GUI/Widget.h"

#define GUI_PANEL_SPR "panel"

namespace Zixel {

	struct GUI;
	struct Theme;
	struct PanelTheme;

	struct Panel : public Widget {

		PanelTheme* panelTheme;

		Sprite* sprPanel;

		void render() override;

		Panel(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);

	};

}