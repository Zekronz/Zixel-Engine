/*
    Label.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include "Engine/GUI/Widget.h"

#define GUI_LABEL_FONT "robotoRegular12"
#define GUI_LABEL_COLOR { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_LABEL_COLOR_DISABLED { 0.353f, 0.353f, 0.439f, 1.0f }

namespace Zixel {

	struct GUI;
	struct Theme;

	struct Label : public Widget {

		Font* fntText = nullptr;
		Color4f color = GUI_LABEL_COLOR;
		Color4f colorDisabled = GUI_LABEL_COLOR_DISABLED;
		
		std::string text = "";
		std::string textDisplay = "";

		TextAlign textHAlign = TextAlign::Left;
		TextAlign textVAlign = TextAlign::Top;

		bool extendLineBreakToParent = false;
		s32 lineBreakWidth = -1;
		s32 lineBreakMargin = 0;

		void update(f32 dt) override;
		void render() override;

		s32 getLineBreakOffset();
		void updateSize();
		void calculateLineBreak();

		Label(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);

		void setExtendLineBreakToParent(bool _extend);
		void setLineBreakWidth(s32 _width);
		void setLineBreakMargin(s32 _margin);
		void setFont(std::string _font);
		void setText(std::string _text);
		void setColor(Color4f _color, Color4f _colorDisabled);

	};

}