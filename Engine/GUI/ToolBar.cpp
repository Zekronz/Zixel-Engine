#include "Engine/ZixelPCH.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/GUI/ToolBar.h"
#include "Engine/GUI/GUI.h"

namespace Zixel {

	ToolBar::ToolBar(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::ToolBar;

		sprToolBar = renderer->getTextureAtlasSprite(GUI_TOOL_BAR_SPR);
		sprToolBarSeparator = renderer->getTextureAtlasSprite(GUI_TOOL_BAR_SEPARATOR_SPR);

		renderFocus = false;

		cutContent = true;
		scrollHorVisible = false;
		scrollVerVisible = false;
		canScrollHor = false;
		canScrollVer = false;

		fillX = true;
		height = sprToolBar->sizeY;

		lineHeight = height;
		separatorY = (lineHeight / 2) - (GUI_TOOL_BAR_SEPARATOR_HEIGHT / 2);

	}

	void ToolBar::onResize(s32 prevWidth, s32 prevHeight) {

		//@Reminber: If we add line breaking the "active frame label" might cause flickering when the animation is playing because the width of the label will change.

	}

	void ToolBar::render() {

		renderer->render3PHor(sprToolBar, 0, x, y, widthDraw);

		for (ToolBarItem& item : itemList) {

			if (item.type != ToolBarItemType::Separator) continue;

			if (item.showSeparator && item.visible) {
				renderer->render9P(sprToolBarSeparator, 0, x + item.x, y + separatorY, GUI_TOOL_BAR_SEPARATOR_WIDTH, GUI_TOOL_BAR_SEPARATOR_HEIGHT);
			}
			
		}

	}

	void ToolBar::addWidget(Widget* _widget, s32 _yOffset, s32 _group) {

		//@TODO: Remove widget from widget list when destroying widget.

		if (_widget->parent != (Widget*)this) {

			ZIXEL_WARN("Error in ToolBar::addWiget: _widget must be a child of the ToolBar widget.");
			return;

		}

		for (ToolBarItem& item : itemList) {

			if (item.type != ToolBarItemType::Widget) continue;

			if (item.widget == _widget) {

				ZIXEL_WARN("Error in ToolBar::addWiget: _widget has already been added.");
				return;

			}

		}

		ToolBarItem item;
		item.type = ToolBarItemType::Widget;
		item.group = _group;
		item.widget = _widget;
		item.yOffset = _yOffset;
		
		itemList.push_back(item);

		updateWidgets();

	}

	void ToolBar::addSeparator(s32 _group) {

		ToolBarItem item;
		item.type = ToolBarItemType::Separator;
		item.group = _group;

		itemList.push_back(item);

		updateWidgets();

	}

	void ToolBar::addSpacing(s32 _spacing, s32 _group) {

		ToolBarItem item;
		item.type = ToolBarItemType::Separator;
		item.group = _group;
		item.showSeparator = false;
		item.spacing = _spacing < 0 ? GUI_TOOL_BAR_SEPARATOR_SPACING : _spacing;

		itemList.push_back(item);

		updateWidgets();

	}

	void ToolBar::setGroupVisible(s32 _group, bool _visible) {

		bool update = false;

		for (ToolBarItem& item : itemList) {

			if (item.group != _group) continue;
			
			if (item.type == ToolBarItemType::Widget) {

				if (item.widget->visible != _visible) {

					update = true;
					item.widget->setVisible(_visible);

				}

				continue;

			}
			else if (item.type == ToolBarItemType::Separator) {

				if (item.visible != _visible) {

					update = true;
					item.visible = _visible;

				}

			}

		}

		if (update) updateWidgets();

	}

	ToolBarItem ToolBar::getPrevItem(size_t _index) {

		if (_index <= 0 || _index >= itemList.size()) {

			ToolBarItem item;
			item.type = ToolBarItemType::None;

			return item;

		}

		--_index;

		while (_index >= 0) {

			ToolBarItem& item = itemList[_index];

			if (item.type == ToolBarItemType::Widget) {

				if (item.widget->visible) {
					return item;
				}

			}
			else if (item.type == ToolBarItemType::Separator) {
				return item;
			}

			if (_index <= 0) break;
			--_index;

		}

		ToolBarItem item;
		item.type = ToolBarItemType::None;

		return item;

	}

	void ToolBar::updateWidgets() {

		s32 itemX = 0;
		bool firstItem = true;

		for (size_t i = 0; i < itemList.size(); ++i) {

			ToolBarItem& item = itemList[i];

			bool isWidget = (item.type == ToolBarItemType::Widget);
			bool isSeparator = (item.type == ToolBarItemType::Separator);

			if (isWidget) {
				if (!item.widget->visible) continue;
			}
			else if (isSeparator) {
				if (!item.visible) continue;
			}

			s32 xOff = 0;
			if (firstItem) {

				firstItem = false;
				xOff = GUI_TOOL_BAR_START_SPACING;

			}
			else if (isSeparator) {
				xOff = item.spacing;
			}
			else if(isWidget) {

				ToolBarItem prevItem = getPrevItem(i);
				if (prevItem.type == ToolBarItemType::None) xOff = GUI_TOOL_BAR_START_SPACING; //This shouldn't ever happen.

				if (prevItem.type == ToolBarItemType::Separator) {
					xOff = prevItem.showSeparator ? prevItem.spacing : 0;
				}
				else {
					xOff = GUI_TOOL_BAR_WIDGET_SPACING;
				}

			}

			if (isWidget) {

				if (!item.widget->visible) continue;
				item.widget->setPadding(itemX + xOff, (lineHeight / 2) - (item.widget->height / 2) - (1 - (item.widget->height % 2)) + item.yOffset, 0, 0);
				
				itemX += xOff + item.widget->width;
				
			}
			else if (isSeparator) {

				item.x = itemX + xOff;

				itemX += xOff;
				if (item.showSeparator) itemX += GUI_TOOL_BAR_SEPARATOR_WIDTH;

			}

		}

	}

}