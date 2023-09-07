/*
    DockTab.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include "Engine/StringHelper.h"

#define GUI_DOCK_TAB_SPR_AREA "dockArea"
#define GUI_DOCK_TAB_SPR_SPLIT_VER "dockSplitVer"
#define GUI_DOCK_TAB_SPR_SPLIT_HOR "dockSplitHor"
#define GUI_DOCK_TAB_SPR_CONTAINER "dockContainer"
#define GUI_DOCK_TAB_SPR_TAB "dockTab"
#define GUI_DOCK_TAB_SPR_TAB_CLOSE "dockTabClose"
#define GUI_DOCK_TAB_SPR_LINE "dockTabLine"
#define GUI_DOCK_TAB_SPR_PREVIEW "dockPreview"
#define GUI_DOCK_TAB_FONT "robotoRegular12"
#define GUI_DOCK_TAB_TITLE_COL_FOCUSED { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_DOCK_TAB_TITLE_COL_UNFOCUSED { 0.584f, 0.596f, 0.678f, 1.0f }
#define GUI_DOCK_TAB_MIN_WIDTH 11
#define GUI_DOCK_TAB_MAX_WIDTH 246
#define GUI_DOCK_TAB_TEXT_HOR_SPACING 10
#define GUI_DOCK_TAB_TEXT_VER_SPACING 0
#define GUI_DOCK_TAB_SPACING 1
#define GUI_DOCK_TAB_CLOSE_HOR_SPACING 8
#define GUI_DOCK_TAB_UNDOCK_THRESHOLD 9

namespace Zixel {

	struct GUI;
	struct DockArea;
	struct DockContainer;
	struct Window;

	struct DockTab {

		GUI* gui;
		DockArea* dockArea;
		DockContainer* container;
		Window* window;

		s32 x = 0;
		s32 width = 0;

		UTF8String titleShow = "";

		DockTab(GUI* _gui, DockContainer* _container, Window* _window);

		s32 getDefaultWidth();
		UTF8String getTitleShowFromWidth(s32 _width);
		void updateTitleShow();
		void updateWindowRect();

	};

}