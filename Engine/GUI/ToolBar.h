/*
    ToolBar.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include "Engine/GUI/Widget.h"

#define GUI_TOOL_BAR_SPR "toolBar"
#define GUI_TOOL_BAR_SEPARATOR_SPR "toolBarSeparator"
#define GUI_TOOL_BAR_WIDGET_SPACING 2
#define GUI_TOOL_BAR_SEPARATOR_SPACING 6
#define GUI_TOOL_BAR_START_SPACING 4
#define GUI_TOOL_BAR_SEPARATOR_WIDTH 1
#define GUI_TOOL_BAR_SEPARATOR_HEIGHT 15

namespace Zixel {

	struct Theme;

	enum class ToolBarItemType : u8 {

		None,
		Widget,
		Separator,

	};

	struct ToolBarItem {

		ToolBarItemType type = ToolBarItemType::Widget;
		s32 group = -1;

		//Widget.
		Widget* widget = nullptr;
		s32 yOffset = 0;

		//Separator.
		s32 x = 0;
		s32 spacing = GUI_TOOL_BAR_SEPARATOR_SPACING;
		bool visible = true;
		bool showSeparator = true;

	};

	struct ToolBar : public Widget {

		Sprite* sprToolBar;
		Sprite* sprToolBarSeparator;

		void onResize(s32 prevWidth, s32 prevHeight) override;
		void render() override;

		bool lineBreak = false;
		s32 lineHeight = 0;

		s32 separatorY = 0;

		std::vector<ToolBarItem> itemList;

		ToolBar(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);

		void addWidget(Widget* _widget, s32 _yOffset = 0, s32 _group = -1);
		void addSeparator(s32 _group = -1);
		void addSpacing(s32 _spacing, s32 _group = -1);

		void setGroupVisible(s32 _group, bool _visible);

		ToolBarItem getPrevItem(size_t _index);
		void updateWidgets();

	};

}