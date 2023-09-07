#pragma once

#include <functional>
#include "Engine/GUI/Widget.h"

#define GUI_TOGGLE_BUTTON_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2)

#define GUI_TOGGLE_BUTTON_SPR "toggleButton"
#define GUI_TOGGLE_BUTTON_FONT "robotoRegular12"
#define GUI_TOGGLE_BUTTON_TEXT_HOR_SPACING 2
#define GUI_TOGGLE_BUTTON_TEXT_VER_SPACING 0
#define GUI_TOGGLE_BUTTON_TEXT_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_TOGGLE_BUTTON_TEXT_COL_DISABLED { 0.353f, 0.353f, 0.439f, 1.0f }
#define GUI_TOGGLE_BUTTON_ICON_HOR_SPACING 6
#define GUI_TOGGLE_BUTTON_ICON_DISABLED_ALPHA 0.29f

namespace Zixel {

	struct GUI;
	struct Theme;

	struct ToggleButton : public Widget {

		Sprite* sprToggleButton;

		Font* fntText;

		u8 ind = 0;

		std::string text = "";
		std::string textDraw = "";

		TextAlign textHAlign = TextAlign::Center;
		TextAlign textVAlign = TextAlign::Middle;

		s32 textX = 0;
		s32 textY = 0;
		s32 iconX = 0;
		s32 iconY = 0;

		Sprite* sprIcon = nullptr;
		u8 iconSub = 0;

		bool pressed = false;
		bool toggled = false;

		bool showTooltip = true;

		std::function<void(ToggleButton*, bool)> callback;

		void updateTextDisplay();

		bool onShortcutInteract();
		void onShortcutInteractEnd();

		void onResize(s32 prevWidth, s32 prevHeight) override;
		void update(f32 dt) override;
		void render() override;

		ToggleButton(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);

		void setCallback(std::function<void(ToggleButton*, bool)> _callback);
		
		void setIcon(Sprite* _sprIcon, u8 _iconSub);

		void setText(std::string _text);
		std::string getText();

		bool isToggled();
		void setToggled(bool _toggled, bool _callCallback = true);
		void toggle();

	};

}