/*
    MenuBar.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include "Engine/GUI/Widget.h"

#define GUI_MENU_BAR_SPR_BAR "menuBar"
#define GUI_MENU_BAR_SPR_BUTTON "menuBarButton"
#define GUI_MENU_BAR_FONT "robotoRegular13"
#define GUI_MENU_BAR_TEXT_SPACING 14
#define GUI_MENU_BAR_TEXT_HOR_OFFSET 0
#define GUI_MENU_BAR_TEXT_VER_OFFSET 0
#define GUI_MENU_BAR_TEXT_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_MENU_BAR_TEXT_COL_DISABLED { 0.353f, 0.353f, 0.439f, 1.0f }
#define GUI_MENU_BAR_DROP_DOWN_HOR_OFFSET 0
#define GUI_MENU_BAR_DROP_DOWN_VER_OFFSET -1

namespace Zixel {

	struct GUI;
	struct Theme;
	struct DropDownMenu;

	struct MenuBarButton {

		DropDownMenu* menu = nullptr;

		bool enabled = true;

		std::string name;

		s32 x = 0;
		s32 width = 0;

	};

	struct MenuBar : public Widget {

		Sprite* sprMenuBar;
		Sprite* sprMenuBarButton;

		Font* fntText;

		std::vector<MenuBarButton*> buttonList;
		MenuBarButton* buttonSelected = nullptr;
		MenuBarButton* buttonHovered = nullptr;
		MenuBarButton* buttonHoveredKeyboard = nullptr;

		//bool opened = false;
		bool justOpened = false;

		void updateButtonPos();

		void onResize(s32 prevWidth, s32 prevHeight) override;
		void update(f32 dt) override;
		void render() override;

		MenuBar(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);
		~MenuBar();

		void close();

		MenuBarButton* addButton(std::string _name, DropDownMenu* _menu = nullptr, bool _enabled = true);

	};

}