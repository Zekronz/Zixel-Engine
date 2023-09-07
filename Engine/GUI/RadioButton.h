/*
    RadioButton.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <functional>
#include "Engine/GUI/Widget.h"

#define GUI_RADIO_BUTTON_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2)

#define GUI_RADIO_BUTTON_SPR_BACKGROUND "radioButton"
#define GUI_RADIO_BUTTON_SPR_ICON "radioButtonIcon"
#define GUI_RADIO_BUTTON_SPR_FOCUS "radioButtonFocus"
#define GUI_RADIO_BUTTON_FONT "robotoRegular12"
#define GUI_RADIO_BUTTON_TEXT_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_RADIO_BUTTON_TEXT_COL_DISABLED { 0.353f, 0.353f, 0.439f, 1.0f }
#define GUI_RADIO_BUTTON_TEXT_SPACING 5
#define GUI_RADIO_BUTTON_TEXT_HOR_OFFSET 0
#define GUI_RADIO_BUTTON_TEXT_VER_OFFSET -1
#define GUI_RADIO_BUTTON_ICON_HOR_OFFSET 0
#define GUI_RADIO_BUTTON_ICON_VER_OFFSET 0
#define GUI_RADIO_BUTTON_DEFAULT_SIZE 19

namespace Zixel {

	struct GUI;
	struct Theme;

	enum class RadioButtonTextSide : u8 {

		Left,
		Right,

	};

	struct RadioButton : public Widget {

		Sprite* sprRadioButton;
		Sprite* sprRadioButtonIcon;
		Sprite* sprRadioButtonFocus;

		Font* fntText;

		u8 ind = 0;

		std::string text = "";
		std::string textDisplay = "";
		RadioButtonTextSide textSide = RadioButtonTextSide::Right;

		s32 groupId = -1;
		bool toggled = false;

		std::function<void(RadioButton*, s32)> callback;

		void updateTextDisplay();

		bool onShortcutInteract();

		void onResize(s32 prevWidth, s32 prevHeight) override;
		void update(f32 dt) override;
		void render() override;

		RadioButton(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);

		void setCallback(std::function<void(RadioButton*, s32)> _callback);

		bool isToggled();
		void toggle(bool _callCallback = true);

		void setGroupId(s32 _groupId);

		void setText(std::string _text);
		void setTextSide(RadioButtonTextSide _textSide);

	};

}