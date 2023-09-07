/*
    LineEdit.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/GUI/LineEdit.h"

namespace Zixel {

	LineEdit::LineEdit(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : TextEdit(_gui, _parent, _window, _theme) {
		
		height = 17;

		scrollHorVisible = false;
		scrollVerVisible = false;
		canScrollVer = false;

		deselectOnEnter = true;
		highlightCurLine = false;
		deselectTextOnDeselected = true;

		setMaxLineCount(1);
		setCenterCursorIfSingleLine(true);
		setShowLineNumbers(false);
		setShowCharPosition(false);

	}

}