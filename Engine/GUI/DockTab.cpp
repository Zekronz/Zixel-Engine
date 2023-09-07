#pragma once

#include "Engine/ZixelPCH.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/Window.h"
#include "Engine/GUI/DockTab.h"
#include "Engine/GUI/DockArea.h"
#include "Engine/GUI/DockContainer.h"

namespace Zixel {

	DockTab::DockTab(GUI* _gui, DockContainer* _container, Window* _window) {

		gui = _gui;
		container = _container;
		dockArea = (_container != nullptr) ? _container->dockArea : nullptr;
		window = _window;

	}

	s32 DockTab::getDefaultWidth() {

		UTF8String title;
		if (window->saveState) title.append("*");
		title += window->getTitle();

		s32 tabWidth = gui->renderer->getStringWidth(dockArea->fntDockTab, title) + (GUI_DOCK_TAB_TEXT_HOR_SPACING * 2) + ((dockArea->sprDockTabClose->sizeX + GUI_DOCK_TAB_CLOSE_HOR_SPACING) * window->showButtonClose);
		tabWidth = Math::clampInt(tabWidth, GUI_DOCK_TAB_MIN_WIDTH, GUI_DOCK_TAB_MAX_WIDTH);

		return tabWidth;

	}

	UTF8String DockTab::getTitleShowFromWidth(s32 _width) {
		
		UTF8String fullTitle;
		if (window->saveState) fullTitle.append("*");
		fullTitle += window->title;

		s32 closeWidth = ((dockArea->sprDockTabClose->sizeX + GUI_DOCK_TAB_CLOSE_HOR_SPACING) * window->showButtonClose);

		s32 titleX = GUI_DOCK_TAB_TEXT_HOR_SPACING;
		s32 titleMinWidth = titleX + gui->renderer->getStringWidth(dockArea->fntDockTab, fullTitle) + GUI_DOCK_TAB_TEXT_HOR_SPACING + closeWidth;
		s32 titleSpace = _width;

		if (titleSpace - titleMinWidth < 0) {

			fullTitle += "...";
			titleMinWidth = titleX + gui->renderer->getStringWidth(dockArea->fntDockTab, fullTitle) + GUI_DOCK_TAB_TEXT_HOR_SPACING + closeWidth;

			if (!window->saveState || fullTitle.size() > 4) {

				while (titleSpace - titleMinWidth < 0) {

					fullTitle.erase(fullTitle.begin() + fullTitle.size() - 4);
					titleMinWidth = titleX + gui->renderer->getStringWidth(dockArea->fntDockTab, fullTitle) + GUI_DOCK_TAB_TEXT_HOR_SPACING + closeWidth;

					if (fullTitle.length() <= 3 + (size_t)window->saveState) break;

				}

			}

		}

		return fullTitle;

	}

	void DockTab::updateTitleShow() {
		titleShow = getTitleShowFromWidth(width);
	}

	void DockTab::updateWindowRect() {

		Widget* dockAreaWidget = (Widget*)dockArea;
		window->setRect(dockAreaWidget->x + container->x, dockAreaWidget->y + container->y + dockArea->sprDockTab->sizeY, container->width, Math::maxInt(1, container->height - dockArea->sprDockTab->sizeY));

	}

}