/*
    DockContainer.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#define GUI_DOCK_CONTAINER_MIN_WIDTH 32
#define GUI_DOCK_CONTAINER_MIN_HEIGHT 32

namespace Zixel {

	struct DockArea;
	struct DockSplit;
	struct DockTab;

	struct DockContainer {

		s32 x = 0;
		s32 y = 0;
		s32 xStart = 0;
		s32 yStart = 0;

		s32 width = 0;
		s32 height = 0;
		s32 widthStart = 0;
		s32 heightStart = 0;
		s32 minWidth = GUI_DOCK_CONTAINER_MIN_WIDTH;
		s32 minHeight = GUI_DOCK_CONTAINER_MIN_HEIGHT;

		DockSplit* splitLeft = nullptr;
		DockSplit* splitRight = nullptr;
		DockSplit* splitUp = nullptr;
		DockSplit* splitDown = nullptr;

		std::vector<DockTab*> tabList;
		std::vector<DockTab*> tabOrderList;
		DockTab* tabSelected = nullptr;

		DockContainer* parent = nullptr;
		std::vector<DockContainer*>children;

		DockArea* dockArea;

		DockContainer(DockArea* _area);

		DockTab* addTab(Window* _window);
		void appendTab(DockTab* _tab, bool _selectedTab = true);
		void updateTabs();
		void selectTab(DockTab* _tab, bool _callOnWindowFocus = true);

		void addTabToOrder(DockTab* _tab);
		bool removeTabFromOrder(DockTab* _tab);

	};

}