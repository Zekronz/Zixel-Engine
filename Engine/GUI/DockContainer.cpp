/*
    DockContainer.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include "Engine/ZixelPCH.h"
#include "Engine/Math.h"
#include "Engine/GUI/Window.h"
#include "Engine/GUI/DockContainer.h"
#include "Engine/GUI/DockArea.h"
#include "Engine/GUI/DockTab.h"

namespace Zixel {

	DockContainer::DockContainer(DockArea* _area) {
		dockArea = _area;
	}

	DockTab* DockContainer::addTab(Window* _window) {

		DockTab* tab = new DockTab(_window->gui, this, _window);

		tabList.push_back(tab);

		if (tabList.size() == 1) {
			selectTab(tab);
		}

		return tab;

	}

	void DockContainer::appendTab(DockTab* _tab, bool _selectedTab) {

		_tab->container = this;

		tabList.push_back(_tab);

		_tab->window->opened = true;
		_tab->window->maximized = false;
		_tab->window->docked = true;
		_tab->window->dockTab = _tab;

		for (Widget* child : _tab->window->widgetList) {
			child->setVisibleGlobal(true);
		}

		updateTabs();
		_tab->updateWindowRect();

		if (_selectedTab || tabList.size() == 1 && tabSelected != _tab) {
			selectTab(_tab);
		}

	}

	void DockContainer::updateTabs() {

		if (tabList.size() > 0) {

			s32 tabX = 0;

			for (DockTab* tab : tabList) {

				s32 tabWidth = tab->getDefaultWidth();

				tab->x = tabX;
				tab->width = tabWidth;

				tab->updateTitleShow();

				tabX += (tabWidth + GUI_DOCK_TAB_SPACING);

			}

			s32 totalWidth = Math::maxInt(0, tabX - GUI_DOCK_TAB_SPACING);

			if (totalWidth > 0 && totalWidth > width) {

				f32 ratio = ((f32)width / (f32)totalWidth);
				f32 tabX = 0.0f;

				f32 tabSpacing = (f32)GUI_DOCK_TAB_SPACING * ratio;

				for (DockTab* tab : tabList) {

					f32 tabWidth = (f32)tab->width * ratio;

					tab->x = (s32)roundf(tabX);
					tab->width = (s32)roundf(tabWidth);
					tab->width = Math::maxInt(tab->width, GUI_DOCK_TAB_MIN_WIDTH);

					tab->updateTitleShow();

					tabX += (tabWidth + tabSpacing);

				}

			}

		}

	}

	void DockContainer::selectTab(DockTab* _tab, bool _callOnWindowFocus) {

		if (std::find(tabList.begin(), tabList.end(), _tab) != tabList.end()) {

			tabSelected = _tab;
			addTabToOrder(_tab);

			if (_callOnWindowFocus && _tab->window->onFocus) {
				_tab->window->onFocus(_tab->window);
			}

		}

	}

	void DockContainer::addTabToOrder(DockTab* _tab) {

		const auto& it = std::find(tabOrderList.begin(), tabOrderList.end(), _tab);
		if (it != tabOrderList.end()) {
			tabOrderList.erase(it);
		}

		tabOrderList.push_back(_tab);

	}

	bool DockContainer::removeTabFromOrder(DockTab* _tab) {

		const auto& it = std::find(tabOrderList.begin(), tabOrderList.end(), _tab);
		if (it != tabOrderList.end()) {

			tabOrderList.erase(it);
			return true;

		}

		return false;

	}

}