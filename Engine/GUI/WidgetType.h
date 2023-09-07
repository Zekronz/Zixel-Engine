/*
    WidgetType.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <cstdint>

namespace Zixel {

	enum class WidgetType : u8 {

		Base,
		DockArea,
		MenuBar,
		MenuButton,
		ToolBar,
		TextEdit,
		Label,
		Panel,
		Button,
		ToggleButton,
		ComboBox,
		Checkbox,
		RadioButton,
		SliderHor,
		TreeView,
		ColorWheel,
		ColorPicker,
		Custom,

	};

}