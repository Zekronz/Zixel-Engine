#pragma once

#include <functional>
#include "Engine/GUI/Widget.h"

#define GUI_SLIDER_VALUE_CHANGE_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2)
#define GUI_SLIDER_VALUE_SET_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

#define GUI_SLIDER_HOR_SPR "sliderHor"
#define GUI_SLIDER_HOR_FONT "robotoRegular12"
#define GUI_SLIDER_HOR_TEXT_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_SLIDER_HOR_TEXT_COL_DISABLED { 0.353f, 0.353f, 0.439f, 1.0f }
#define GUI_SLIDER_HOR_TEXT_HOR_OFFSET -1
#define GUI_SLIDER_HOR_TEXT_VER_OFFSET 0
#define GUI_SLIDER_HOR_TEXT_SPACING 4

namespace Zixel {

	struct Font;
	struct GUI;
	struct Theme;

	struct SliderHor : public Widget {

		Sprite* sprSliderHor;

		Font* fntText;

		u8 ind = 0;

		f32 percentage = 1.0f;

		f32 valCur;
		f32 valStart;
		f32 valMin = 0.0f;
		f32 valMax = 100.0f;
		bool valRound = true;

		std::string valText = "";
		std::string valTextDisplay = "";

		bool displayAsPercentage = false;
		u8 decimalPlaces = 2;

		std::function<void(SliderHor*, f32)> onValueChange;
		std::function<void(SliderHor*, f32, f32)> onValueSet;

		bool onStartShortcutDecrease();
		bool onShortcutDecrease();

		bool onStartShortcutIncrease();
		bool onShortcutIncrease();

		void onEndShortcut();

		void updateValueText();
		void checkValueTextBounds();

		void onResize(s32 prevWidth, s32 prevHeight) override;
		void update(f32 dt) override;
		void render() override;

		SliderHor(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);

		f32 valueToPercentage(f32 _value);

		void setValue(f32 _value, bool _callOnValueChange = true);
		void setRange(f32 _minValue, f32 _maxValue);

		void setPercentage(f32 _percentage, bool _callOnValueChange = true);
		f32 getPercentage();

		void setOnValueChange(std::function<void(SliderHor*, f32)> _callback);
		void setOnValueSet(std::function<void(SliderHor*, f32, f32)> _callback);
	};

}