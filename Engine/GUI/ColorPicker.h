/*
    ColorPicker.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <functional>

#include "Engine/GUI/Widget.h"
#include "Engine/Color.h"

#define GUI_COLOR_PICKER_COLOR_CHANGE_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define GUI_COLOR_PICKER_COLOR_CONFIRM_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)

#define GUI_COLOR_PICKER_SPR_ANCHOR_CIRCLE "colorPickerAnchorCircle"
#define GUI_COLOR_PICKER_SPR_ANCHOR_TRIANGLE "colorPickerAnchorTriangle"
#define GUI_COLOR_PICKER_SPR_SWAP "colorPickerSwap"
#define GUI_COLOR_PICKER_SPR_COLOR_MODE "colorPickerColorMode"
#define GUI_COLOR_PICKER_SPR_DISPLAY_WHEEL "colorPickerDisplayWheel"
#define GUI_COLOR_PICKER_SPR_DISPLAY_CHANNELS "colorPickerDisplayChannels"
#define GUI_COLOR_PICKER_FONT "robotoRegular12"
#define GUI_COLOR_PICKER_TEXT_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_COLOR_PICKER_SELECTED_COLOR_UNDERLINE_COL { 0.706f, 0.718f, 0.816f, 1.0f }
#define GUI_COLOR_PICKER_SELECTED_COLOR_UNDERLINE_COL_HOVER { 0.443f, 0.451f, 0.518f, 1.0f }
//#define GUI_COLOR_PICKER_MAX_WHEEL_SIZE 124
//#define GUI_COLOR_PICKER_MIN_WHEEL_SIZE 48
#define GUI_COLOR_PICKER_TOOLTIP_HSV "Use HSV Color Mode"
#define GUI_COLOR_PICKER_TOOLTIP_RGB "Use RGB Color Mode"
#define GUI_COLOR_PICKER_TOOLTIP_COLOR_WHEEL "Display Color Wheel"
#define GUI_COLOR_PICKER_TOOLTIP_COLOR_CHANNELS "Display Color Channels"
#define GUI_COLOR_PICKER_TOOLTIP_PRIMARY "Primary Color"
#define GUI_COLOR_PICKER_TOOLTIP_SECONDARY "Secondary Color"
#define GUI_COLOR_PICKER_TOOLTIP_SWAP "Swap Colors"

namespace Zixel {

	struct Shader;
	struct Font;
	struct GUI;
	struct Theme;
	struct TextEdit;
	struct NumberEdit;

	//@TODO: Should these enums be an app specific thing?
	enum class ColorSelected : u8 {

		Primary,
		Secondary,

	};

	enum class ColorMode : u8 {

		HSV,
		RGB,

	};

	enum class ColorPickerComponent : u8 {

		None,
		ColorModeHSV,
		ColorModeRGB,
		DisplayWheel,
		DisplayChannels,
		Wheel,
		Square,
		Channel,
		Primary,
		Secondary,
		Swap,

	};

	struct ColorPicker : public Widget {

		Sprite* sprAnchorCircle = nullptr;
		Sprite* sprAnchorTriangle = nullptr;
		Sprite* sprSwap = nullptr;
		Sprite* sprColorMode = nullptr;
		Sprite* sprDisplayWheel = nullptr;
		Sprite* sprDisplayChannels = nullptr;

		Font* fntText = nullptr;

		Shader* colorWheelShader = nullptr;
		s32 uniformWheelThickness = -1;

		Shader* colorSquareShader = nullptr;
		s32 uniformSquareHue = -1;

		Shader* colorBarShader = nullptr;
		s32 uniformBarColorMode = -1;
		s32 uniformBarChannel = -1;
		s32 uniformBarColor = -1;

		Shader* colorPreviewShader = nullptr;
		s32 uniformPreviewRGBA = -1;

		s32 minHeightBeforeResizeFull = 290;
		s32 minHeightBeforeResizeWheel = 193;
		s32 minHeightBeforeResizeChannels = 154;
		s32 minHeightBeforeResizeNone = 60;

		Color4 startColor;
		Color4 startColorRGBA;

		ColorPickerComponent mouseOverComponent = ColorPickerComponent::None;

		ColorMode colorMode = ColorMode::HSV;
		s32 colorModeWidth = 31;
		s32 colorModeHeight = 15;
		s32 colorModeXOff = 5;
		s32 colorModeYOff = 4;
		s32 colorModeHorSpacing = 2;
		u8 colorModeCount = 2;

		s32 displayWidth = 17;
		s32 displayHeight = 17;
		s32 displayXOff = 5;
		s32 displayYOff = 3;
		s32 displayHorSpacing = 2;
		u8 displayCount = 2;
		bool displayWheel = true;
		bool displayChannels = true;

		s32 maxWheelSize = 125;
		s32 minWheelSize = 90;
		s32 wheelSize = maxWheelSize;
		f32 wheelThickness = 14.0f;
		s32 wheelYOff = 25;
		s32 huePickerX = 0;
		s32 huePickerY = 0;

		s32 squareSize = 0;
		s32 squarePickerX = 0;
		s32 squarePickerY = 0;

		s32 channelX = 55;
		s32 channelYOff = 8;
		s32 channelHeight = 14;
		s32 channelHorSpacing = 7;
		s32 channelVerSpacing = 4;
		u8 channelCount = 4;
		f32 channelPickerX[4] = {};
		NumberEdit* editChannel[4] = {}; //All elements gets initialized to zero (null).
		s32 channelSelectInd = 0;

		s32 hexX = 55;
		s32 hexYOff = 4;
		s32 hexHeight = 14;
		s32 hexHorSpacing = 7;

		TextEdit* editHex = nullptr;

		s32 previewSize = 29;
		s32 previewXOff = 7;
		s32 previewYOff = 8;
		s32 previewHorSpacing = 5;
		s32 previewUnderlineYOff = 2;
		s32 previewSwapXOff = 5;

		ColorSelected selectedColor = ColorSelected::Primary;

		u8 colorHue[2] = { 0, 0 };
		u8 colorSat[2] = { 0, 0 };
		u8 colorVal[2] = { 0, 255 };
		u8 colorRed[2] = { 0, 255 };
		u8 colorGreen[2] = { 0, 255 };
		u8 colorBlue[2] = { 0, 255 };
		u8 colorAlpha[2] = { 255, 255 };

		std::string tooltipHSV = GUI_COLOR_PICKER_TOOLTIP_HSV;
		std::string tooltipRGB = GUI_COLOR_PICKER_TOOLTIP_RGB;
		std::string tooltipColorWheel = GUI_COLOR_PICKER_TOOLTIP_COLOR_WHEEL;
		std::string tooltipColorChannels = GUI_COLOR_PICKER_TOOLTIP_COLOR_CHANNELS;
		std::string tooltipPrimary = GUI_COLOR_PICKER_TOOLTIP_PRIMARY;
		std::string tooltipSecondary = GUI_COLOR_PICKER_TOOLTIP_SECONDARY;
		std::string tooltipSwap = GUI_COLOR_PICKER_TOOLTIP_SWAP;

		std::function<void(ColorPicker*, bool, bool)> onColorChange;
		std::function<void(ColorPicker*, bool, bool, Color4, Color4)> onColorConfirm;

		void onResize(s32 _prevWidth, s32 _prevHeight) override;
		void update(f32 dt) override;
		void render() override;

		void updateResize();
		void setWheelSize(s32 _size);
		void updateSquareSize();

		void hueToXY(s32& _x, s32& _y, s32 _hue);
		void satValToXY(s32& _x, s32& _y, s32 _saturation, s32 _value);
		void channelToX(f32& _x, s32 _value);

		void checkColorConfirm();

		void onEditChannel0(TextEdit* _textEdit);
		void onEditChannel1(TextEdit* _textEdit);
		void onEditChannel2(TextEdit* _textEdit);
		void onEditChannel3(TextEdit* _textEdit);

		void onEditChannelConfirm(TextEdit* _textEdit);

		void onEditHex(TextEdit* _textEdit);
		void onEditHexConfirm(TextEdit* _textEdit);

		void onEditFocus(TextEdit* _textEdit);

		ColorPicker(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);

		void setSelectedColor(ColorSelected _selectedColor);

		void setInputHSVAChannels(u8 _hue, u8 _saturation, u8 _value, u8 _alpha);
		void setInputRGBAChannels(u8 _red, u8 _green, u8 _blue, u8 _alpha);
		void setInputHex(u8 _red, u8 _green, u8 _blue);

		void setHSVA(ColorSelected _selectedColor, u8 _hue, u8 _saturation, u8 _value, u8 _alpha, bool _setChannelInput = true, bool _setHexInput = true, bool _forceCalculationOfOtherModes = false, bool _callOnColorChange = true);
		void setRGBA(ColorSelected _selectedColor, u8 _red, u8 _green, u8 _blue, u8 _alpha, bool _setChannelInput = true, bool _setHexInput = true, bool _forceCalculationOfOtherModes = false, bool _callOnColorChange = true);
		void setData(ColorSelected _selectedColor, u8 _hue, u8 _saturation, u8 _value, u8 _red, u8 _green, u8 _blue, u8 _alpha, bool _setChannelInput = true, bool _setHexInput = true, bool _callOnColorChange = true);

		Color4 getRGBA(ColorSelected _selectedColor);
		Color4 getHSVA(ColorSelected _selectedColor);
		u8 getAlpha(ColorSelected _selectedColor);

		void setOnColorChange(std::function<void(ColorPicker*, bool, bool)> _callback);
		void setOnColorConfirm(std::function<void(ColorPicker*, bool, bool, Color4, Color4)> _callback);

	};

}