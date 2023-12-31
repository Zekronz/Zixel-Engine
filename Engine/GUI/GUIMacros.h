/*
    GUIMacros.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <functional>

#define GUI_NO_PARENT (Widget*)nullptr

#define GUI_SHORTCUT_CALLBACK(funcPointer) std::bind(&funcPointer, this)

#define GUI_MODIFIER_CTRL 0b00000001
#define GUI_MODIFIER_SHIFT 0b00000010
#define GUI_MODIFIER_ALT 0b00000100

#define GUI_TAB_START_TIME 0.5f
#define GUI_TAB_REPEAT_TIME 0.055f

#define GUI_GLOBAL_SHORTCUT_START_TIME 0.4f
#define GUI_GLOBAL_SHORTCUT_REPEAT_TIME 0.03f

#define GUI_CANCEL_DOCKING_KEY KEY_CONTROL

#define GUI_TOOLTIP_SPR "tooltip"
#define GUI_TOOLTIP_FONT "robotoRegular12"
#define GUI_TOOLTIP_HOR_SPACING 6
#define GUI_TOOLTIP_VER_SPACING 4
#define GUI_TOOLTIP_TEXT_HOR_OFFSET 0
#define GUI_TOOLTIP_TEXT_VER_OFFSET 0
#define GUI_TOOLTIP_X_OFFSET 0
#define GUI_TOOLTIP_Y_OFFSET 20
#define GUI_TOOLTIP_TIMER 0.4f

#define GUI_SCROLL_SPR_VER "scrollVer"
#define GUI_SCROLL_SPR_HOR "scrollHor"
#define GUI_SCROLL_SPR_CORNER "scrollCorner"
#define GUI_SCROLLBAR_MIN_LENGTH 10