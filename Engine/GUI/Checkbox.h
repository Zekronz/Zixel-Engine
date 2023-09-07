/*
    Checkbox.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <functional>
#include "Engine/GUI/Widget.h"

#define GUI_CHECKBOX_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2)

#define GUI_CHECKBOX_SPR_BACKGROUND "checkbox"
#define GUI_CHECKBOX_SPR_ICON "checkboxIcon"
#define GUI_CHECKBOX_FONT "robotoRegular12"
#define GUI_CHECKBOX_TEXT_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_CHECKBOX_TEXT_COL_DISABLED { 0.353f, 0.353f, 0.439f, 1.0f }
#define GUI_CHECKBOX_TEXT_SPACING 5
#define GUI_CHECKBOX_TEXT_HOR_OFFSET 0
#define GUI_CHECKBOX_TEXT_VER_OFFSET -1
#define GUI_CHECKBOX_ICON_HOR_OFFSET 0
#define GUI_CHECKBOX_ICON_VER_OFFSET 0
#define GUI_CHECKBOX_DEFAULT_SIZE 19

namespace Zixel {

	struct GUI;
	struct Theme;

	enum class CheckboxTextSide : u8 {

		Left,
		Right,

	};

	struct Checkbox : public Widget {

		Sprite* sprCheckbox;
		Sprite* sprCheckboxIcon;

		Font* fntText;

		u8 ind = 0;

		std::string text = "";
		std::string textDisplay = "";
		CheckboxTextSide textSide = CheckboxTextSide::Right;

		bool toggled = false;

		std::function<void(Checkbox*, bool)> callback;

		void updateTextDisplay();

		bool onShortcutInteract();

		void onResize(s32 prevWidth, s32 prevHeight) override;
		void update(f32 dt) override;
		void render() override;

		Checkbox(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);

		void setCallback(std::function<void(Checkbox*, bool)> _callback);

		bool isToggled();
		void setToggled(bool _toggled, bool _callCallback = true);

		void setText(std::string _text);
		void setTextSide(CheckboxTextSide _textSide);

	};

}