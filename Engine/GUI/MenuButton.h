/*
    MenuButton.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include "Engine/GUI/Widget.h"

#define GUI_MENU_BUTTON_SPR "button"
#define GUI_MENU_BUTTON_FONT "robotoRegular12"
#define GUI_MENU_BUTTON_TEXT_HOR_SPACING 2
#define GUI_MENU_BUTTON_TEXT_VER_SPACING 0
#define GUI_MENU_BUTTON_TEXT_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_MENU_BUTTON_TEXT_COL_DISABLED { 0.353f, 0.353f, 0.439f, 1.0f }
#define GUI_MENU_BUTTON_ICON_HOR_SPACING 6
#define GUI_MENU_BUTTON_ICON_DISABLED_ALPHA 0.29f

namespace Zixel {

	struct Theme;
	struct Sprite;
	struct Font;
	struct DropDownMenu;

	enum class TextAlign : u8;

	struct MenuButton : public Widget {

		Sprite* sprButton;

		Font* fntText;

		u8 ind = 0;

		std::string text = "";
		std::string textDraw = "";

		TextAlign textHAlign;
		TextAlign textVAlign;

		s32 textX = 0;
		s32 textY = 0;
		s32 iconX = 0;
		s32 iconY = 0;

		Sprite* sprIcon = nullptr;
		u8 iconSub = 0;

		DropDownMenu* menu = nullptr;
		bool menuOpened = false;
		bool menuJustClosed = false;

		bool showTooltip = true;

		bool renderIdleBackground = true;
		bool renderDisabledBackground = true;

		void updateTextDisplay();

		void onResize(s32 prevWidth, s32 prevHeight) override;
		void update(f32 dt) override;
		void render() override;

		void menuCloseCallback(DropDownMenu* _menu);

		bool onShortcutInteract();
		void onShortcutInteractEnd();

		MenuButton(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);

		void setText(std::string _text);
		void setIcon(Sprite* _sprIcon, u8 _iconSub);

		std::string getText();

		void attachDropDownMenu(DropDownMenu* _menu);

	};

}