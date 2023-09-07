/*
    DropDownMenu.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/Math.h"
#include "Engine/GUI/DropDownMenu.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/GUIMacros.h"
#include "Engine/GUI/MenuBar.h"
#include "Engine/TextureAtlas.h"

namespace Zixel {

	DropDownMenu::DropDownMenu(GUI* _gui) {

		gui = _gui;
		renderer = _gui->renderer;

		gui->dropDownMenuList.push_back(this);

		fntText = renderer->getTextureAtlasFont(GUI_DROP_DOWN_MENU_FONT);

		sprDropDownMenu = renderer->getTextureAtlasSprite(GUI_DROP_DOWN_MENU_SPR_MENU);
		sprDropDownMenuArrow = renderer->getTextureAtlasSprite(GUI_DROP_DOWN_MENU_SPR_ARROW);
		sprDropDownMenuSeparator = renderer->getTextureAtlasSprite(GUI_DROP_DOWN_MENU_SPR_SEPARATOR);
		sprDropDownMenuCheckbox = renderer->getTextureAtlasSprite(GUI_DROP_DOWN_MENU_SPR_CHECKBOX);
		sprDropDownMenuCheckboxIcon = renderer->getTextureAtlasSprite(GUI_DROP_DOWN_MENU_SPR_CHECKBOX_ICON);

	}

	DropDownMenu::~DropDownMenu() {
		if (page != nullptr) deletePage(page);
	}

	void DropDownMenuItem::setName(std::string _name) {

		if (_name == name) return;

		name = _name;
		parentPage->menu->updatePagePos(parentPage);


	}

	void DropDownMenuItem::setCheckable(bool _checkable) {

		bool update = (checkable != _checkable);

		checkable = _checkable;

		if (update) {
			parentPage->menu->updatePagePos(parentPage);
		}

	}

	void DropDownMenuItem::setChecked(bool _checked) {
		checked = _checked;
	}

	bool DropDownMenuItem::isChecked() {

		if (!checkable) return false;
		return checked;

	}

	void DropDownMenuItem::setEnabled(bool _enabled) {
		enabled = _enabled;
	}

	void DropDownMenuItem::setCallback(std::function<void(DropDownMenu*, DropDownMenuItem*)> _callback) {

		callback = _callback;
		hasCallback = true;

	}

	void DropDownMenuItem::close(bool _updateChildren) {

		if (type == DropDownMenuItemType::Item) {

			bool parentItemOpened = (parentItem == nullptr || (parentItem != nullptr && parentItem->opened));

			if (parentPage->menu->itemHover == this && !parentItemOpened) {
				parentPage->menu->itemHover = nullptr;
			}

			if (parentPage->menu->itemLastHovered == this && !parentItemOpened) {
				parentPage->menu->itemLastHovered = nullptr;
			}

			opened = false;

			if (_updateChildren) {

				if (childPage != nullptr) {

					for (DropDownMenuItem* item : childPage->items) {
						item->close();
					}

				}

			}

		}

	}

	s32 DropDownMenuItem::getIndex() {

		return (s32)(std::find(parentPage->items.begin(), parentPage->items.end(), this) - parentPage->items.begin());

	}

	void DropDownMenu::open(s32 _x, s32 _y, MenuBar* _menuBar) {

		/*if (gui->widgetFocused != nullptr) {
			gui->unfocusWidget(gui->widgetFocused);
		}*/

		gui->stopTabbing();
		gui->stopLocalShortcut();
		gui->stopGlobalShortcut();

		gui->dropDownMenuOpen = this;

		if (gui->menuBarOpen != nullptr && gui->menuBarOpen != _menuBar) {
			gui->menuBarOpen->close();
		}

		/*if (menuBar != _menuBar && menuBar != nullptr) {
			menuBar->opened = false;
		}*/

		menuBar = _menuBar;

		mouseXStart = gui->mouseX;
		mouseYStart = gui->mouseY;
		mouseStartMoved = false;

		timer = 0.0f;
		timerTriggered = false;

		interactTimer = GUI_DROP_DOWN_MENU_INTERACT_TIME;

		itemLastHovered = nullptr;
		
		if (onOpen) onOpen(this); //This gets called before we update the page pos in case an item's width gets changed in the callback. 

		x = _x;
		y = _y;

		if (page != nullptr) {

			resetPage(page);
			updatePagePos(page);

		}

	}

	void DropDownMenu::close(bool _closeMenuBar) {

		if (page != nullptr) {

			for (DropDownMenuItem* item : page->items) {
				item->close();
			}

		}

		timer = 0.0f;
		timerTriggered = false;

		lastHoveredPage = nullptr;

		itemHover = nullptr;
		itemLastHovered = nullptr;
		itemShouldClose = nullptr;
		itemPressed = nullptr;

		keyboardMode = false;
		openedFromKeyboard = false;
		itemHoveredKeyboard = nullptr;

		keyStart = false;
		keyPress = false;
		keyShift = false;

		if (gui->dropDownMenuOpen == this) {
			gui->dropDownMenuOpen = nullptr;
		}

		if (onClose) {
			onClose(this);
		}

		if (_closeMenuBar && menuBar != nullptr) {

			if (gui->menuBarOpen == menuBar) gui->menuBarOpen->close();
			//menuBar->opened = false;
			menuBar->buttonHoveredKeyboard = nullptr;
			menuBar = nullptr;

		}

	}

	void DropDownMenu::setPosition(s32 _x, s32 _y) {

		x = _x;
		y = _y;

		if (page != nullptr) {
			updatePagePos(page);
		}

	}

	DropDownMenuItem* DropDownMenu::addItem(std::string _name, DropDownMenuItem* _parent, std::string _shortcut, Sprite* _icon, s32 _iconSub, bool _enabled) {

		DropDownMenuPage* newPage;

		if (_parent == nullptr) {

			if (page == nullptr) {
				page = addPage();
			}

			newPage = page;

		}
		else {

			if (_parent->type != DropDownMenuItemType::Item) return nullptr;

			if (_parent->childPage == nullptr) _parent->childPage = addPage(_parent->parentPage);
			newPage = _parent->childPage;

		}

		DropDownMenuItem* item = new DropDownMenuItem();
		item->type = DropDownMenuItemType::Item;
		item->name = _name;
		item->shortcut = _shortcut;
		item->parentPage = newPage;
		item->parentItem = _parent;
		item->icon = _icon;
		item->iconSub = _iconSub;
		item->enabled = _enabled;

		newPage->items.push_back(item);

		updatePagePos(newPage);

		return item;

	}

	DropDownMenuItem* DropDownMenu::addSeparator(DropDownMenuItem* _parent) {

		DropDownMenuPage* newPage;

		if (_parent == nullptr) {

			if (page == nullptr) {
				page = addPage();
			}

			newPage = page;

		}
		else {

			if (_parent->type != DropDownMenuItemType::Item) return nullptr;

			if (_parent->childPage == nullptr) _parent->childPage = addPage(_parent->parentPage);
			newPage = _parent->childPage;

		}

		DropDownMenuItem* item = new DropDownMenuItem();
		item->type = DropDownMenuItemType::Separator;
		item->parentPage = newPage;

		newPage->items.push_back(item);

		updatePagePos(newPage);

		return item;

	}

	DropDownMenuItem* DropDownMenu::getLeafItem(DropDownMenuPage* _page) {

		if (_page == nullptr) {
			_page = page;
		}

		if (_page == nullptr) return nullptr;

		for (DropDownMenuItem* item : _page->items) {

			if (item->opened && item->childPage != nullptr) {

				DropDownMenuItem* temp = getLeafItem(item->childPage);

				if (temp == nullptr) return item;
				return temp;

			}

		}

		return nullptr;

	}

	void DropDownMenu::deleteItem(DropDownMenuItem* _item, bool _updatePagePos) {

		auto it = std::find(_item->parentPage->items.begin(), _item->parentPage->items.end(), _item);
		if (it != _item->parentPage->items.end()) _item->parentPage->items.erase(it);

		if (_item->type == DropDownMenuItemType::Item && _item->childPage != nullptr) {
			deletePage(_item->childPage);
		}

		if (itemHover == _item) {
			itemHover = nullptr;
		}

		if (itemLastHovered == _item) {
			itemLastHovered = nullptr;
		}

		DropDownMenuPage* parent = _item->parentPage;

		delete _item;

		if (_updatePagePos) {
			updatePagePos(parent);
		}

	}

	DropDownMenuPage* DropDownMenu::addPage(DropDownMenuPage* _parent) {

		DropDownMenuPage* newPage = new DropDownMenuPage();
		newPage->menu = this;
		newPage->parent = _parent;
		
		if (_parent != nullptr) {
			_parent->children.push_back(newPage);
		}

		return newPage;

	}

	DropDownMenuPage* DropDownMenu::getLeafPage(DropDownMenuPage* _parent) {

		if (_parent == nullptr) {
			_parent = page;
		}

		if (_parent != nullptr) {

			for (DropDownMenuItem* item : _parent->items) {

				if (item->opened && item->childPage != nullptr) {
					return getLeafPage(item->childPage);
				}

			}

		}

		return _parent;

	}

	void DropDownMenu::deletePage(DropDownMenuPage* _page) {

		if (_page->parent != nullptr) {

			auto it = std::find(_page->parent->children.begin(), _page->parent->children.end(), _page);
			if (it != _page->parent->children.end()) _page->parent->children.erase(it);

		}
		else {
			page = nullptr;
		}

		for (s32 i = (s32)_page->items.size() - 1; i >= 0; --i) {
			deleteItem(_page->items[i], false);
		}

		delete _page;

	}

	void DropDownMenu::updatePagePos(DropDownMenuPage* _page, bool _hasSetX, bool _hasSetY, s32 _x, s32 _y) {

		s32 displayWidth, displayHeight;
		renderer->getWindowSize(displayWidth, displayHeight);

		_page->width = 0;
		_page->maxHeight = 0;

		_page->height.clear();
		_page->height.push_back(0);

		_page->maxIconWidth = 0;
		_page->hasCheckbox = false;
		_page->hasArrow = false;

		s32 maxTextWidth = 0;
		s32 maxShortcutWidth = 0;

		s32 ind = 0;
		for (DropDownMenuItem* item : _page->items) {

			s32 textWidth = 0, shortcutWidth = 0, itemHeight = 0;

			if (item->type == DropDownMenuItemType::Item) {

				textWidth = (GUI_DROP_DOWN_MENU_TEXT_SPACING * 2);
				if (item->name != "") textWidth += renderer->getStringWidth(fntText, item->name);

				if (item->shortcut != "") shortcutWidth = (renderer->getStringWidth(fntText, item->shortcut) + GUI_DROP_DOWN_MENU_TEXT_SPACING);

				if (item->childPage != nullptr) _page->hasArrow = true;
				if (item->checkable) _page->hasCheckbox = true;

				if (item->icon != nullptr) {
					if (item->icon->sizeX > _page->maxIconWidth) _page->maxIconWidth = item->icon->sizeX;
				}

				itemHeight = GUI_DROP_DOWN_MENU_ITEM_HEIGHT;

			}
			else if (item->type == DropDownMenuItemType::Separator) {

				textWidth = 0;
				shortcutWidth = 0;

				itemHeight = sprDropDownMenuSeparator->sizeY + (GUI_DROP_DOWN_MENU_SEPARATOR_VER_SPACING * 2);

			}

			if (_page->height[ind] + itemHeight > displayHeight) {

				++ind;
				_page->height.push_back(0);

			}

			item->pageInd = ind;
			item->yOffset = _page->height[ind];

			if (textWidth > maxTextWidth) maxTextWidth = textWidth;
			if (shortcutWidth > maxShortcutWidth) maxShortcutWidth = shortcutWidth;
			_page->height[ind] += itemHeight;

			if (_page->height[ind] > _page->maxHeight) _page->maxHeight = _page->height[ind];

		}

		_page->width = maxTextWidth + maxShortcutWidth;
		if(_page->maxIconWidth > 0) _page->width += _page->maxIconWidth + GUI_DROP_DOWN_MENU_ICON_SPACING;
		if (_page->hasCheckbox) _page->width += GUI_DROP_DOWN_MENU_CHECKBOX_SIZE + GUI_DROP_DOWN_MENU_CHECKBOX_SPACING;
		if (_page->hasArrow) _page->width += sprDropDownMenuArrow->sizeX + GUI_DROP_DOWN_MENU_ARROW_SPACING;

		DropDownMenuPage* parent = _page->parent;

		if (parent == nullptr) {

			_page->x = x;
			_page->y = y;

		}
		else {

			if (!_hasSetX) _page->x = parent->x;
			else _page->x = _x;

			if (!_hasSetY) _page->y = parent->y;
			else _page->y = _y;

		}

		_page->x = Math::minInt(_page->x, displayWidth - (_page->width * (s32)_page->height.size()));
		if (_page->x < 0) _page->x = 0;

		_page->y = Math::minInt(_page->y, displayHeight - _page->maxHeight);
		if (_page->y < 0) _page->y = 0;

		if (parent == nullptr) {

			x = _page->x;
			y = _page->y;

		}

		for (DropDownMenuItem* item : _page->items) {

			if (item->childPage != nullptr) {
				updatePagePos(item->childPage, true, true, _page->x + ((item->pageInd + 1) * (_page->width + GUI_DROP_DOWN_MENU_PAGE_SPACING)), _page->y + item->yOffset);
			}

		}

	}

	void DropDownMenu::updatePage(DropDownMenuPage* _page, f32 dt) {

		for (DropDownMenuItem* item : _page->items) {

			if (item->childPage != nullptr && item->opened) {

				updatePage(item->childPage, dt);
				break;

			}

		}

		for (size_t i = 0; i < _page->height.size(); ++i) {

			if (Math::pointInRect(gui->mouseX, gui->mouseY, _page->x + ((s32)i * (_page->width + GUI_DROP_DOWN_MENU_PAGE_SPACING)), _page->y, _page->width, _page->height[i])) {

				mouseOverPage = true;
				break;

			}

		}

		if ((itemHover == nullptr || keyboardMode) && mouseStartMoved) {

			s32 displayWidth, displayHeight;
			renderer->getWindowSize(displayWidth, displayHeight);

			s32 itemX = _page->x, itemY = _page->y;
			for (DropDownMenuItem* item : _page->items) {

				if (item->type == DropDownMenuItemType::Item) {

					if (itemY + GUI_DROP_DOWN_MENU_ITEM_HEIGHT - 1 >= displayHeight) {

						itemY = _page->y;
						itemX += (_page->width + GUI_DROP_DOWN_MENU_PAGE_SPACING);

					}

					if (Math::pointInRect(gui->mouseX, gui->mouseY, itemX, itemY, _page->width, GUI_DROP_DOWN_MENU_ITEM_HEIGHT)) {

						if (openedFromKeyboard) {

							openedFromKeyboard = false;
							itemHoveredKeyboard = item;

						}

						if ((itemHover == nullptr || keyboardMode) && (itemHoveredKeyboard == nullptr || itemHoveredKeyboard != item)) {

							if (keyboardMode) {

								keyboardMode = false;
								itemHoveredKeyboard = nullptr;

								keyStart = false;
								keyPress = false;
								keyShift = false;

							}

							itemHover = item;

							if (itemLastHovered != item) {

								timer = 0.0f;
								timerTriggered = false;

								itemLastHovered = item;

							}

							if (!timerTriggered) {
							
								timer += dt;

								if (timer >= GUI_DROP_DOWN_MENU_OPEN_TIME) {

									timerTriggered = true;

									for (DropDownMenuItem* item : _page->items) {
										item->close();
									}

									if (!item->opened && item->enabled && item->childPage != nullptr) {

										item->opened = true;

									}

								}

							}

						}

					}

					itemY += GUI_DROP_DOWN_MENU_ITEM_HEIGHT;

				}
				else if (item->type == DropDownMenuItemType::Separator) {

					s32 height = sprDropDownMenuSeparator->sizeY + (GUI_DROP_DOWN_MENU_SEPARATOR_VER_SPACING * 2);

					if (itemY + height - 1 >= displayHeight) {

						itemY = _page->y;
						itemX += (_page->width + GUI_DROP_DOWN_MENU_PAGE_SPACING);

					}

					if (Math::pointInRect(gui->mouseX, gui->mouseY, itemX, itemY, _page->width, height)) {

						if ((itemHover == nullptr || keyboardMode) && (itemHoveredKeyboard == nullptr || itemHoveredKeyboard != item)) {

							if (keyboardMode) {

								keyboardMode = false;
								itemHoveredKeyboard = nullptr;

								keyStart = false;
								keyPress = false;
								keyShift = false;

							}

							itemHover = item;

							if (itemLastHovered != item) {

								timer = 0.0f;
								timerTriggered = false;

								itemLastHovered = item;

							}

						}

					}

					itemY += height;

				}

			}

		}

	}

	void DropDownMenu::renderPage(DropDownMenuPage* _page) {

		s32 pageLen = (s32)_page->height.size();
		s32 itemLen = (s32)_page->items.size();

		for (s32 i = 0; i < pageLen; ++i) {
			renderer->render9P(sprDropDownMenu, 0, _page->x + (i * (_page->width + GUI_DROP_DOWN_MENU_PAGE_SPACING)), _page->y, _page->width, _page->height[i]);
		}

		if (itemLen > 0) {

			s32 displayWidth, displayHeight;
			renderer->getWindowSize(displayWidth, displayHeight);

			s32 itemX = _page->x, itemY = _page->y;
			for (DropDownMenuItem* item : _page->items) {

				if (item->type == DropDownMenuItemType::Item) {

					Color4f colText;
					if (item->enabled) colText = GUI_DROP_DOWN_MENU_TEXT_COL; else colText = GUI_DROP_DOWN_MENU_TEXT_COL_DISABLED;

					if (itemY + GUI_DROP_DOWN_MENU_ITEM_HEIGHT - 1 >= displayHeight) {

						itemY = _page->y;
						itemX += (_page->width + GUI_DROP_DOWN_MENU_PAGE_SPACING);

					}

					if ((itemLastHovered == item && item->enabled) || (item->opened && (itemLastHovered == nullptr || (itemLastHovered != nullptr && itemLastHovered->parentPage != item->parentPage)))) {
						renderer->render9P(sprDropDownMenu, 2, itemX, itemY, _page->width, GUI_DROP_DOWN_MENU_ITEM_HEIGHT);
					}

					s32 checkboxSize = GUI_DROP_DOWN_MENU_CHECKBOX_SIZE;

					s32 textX = itemX + GUI_DROP_DOWN_MENU_TEXT_SPACING;
					if (_page->maxIconWidth > 0) textX += (_page->maxIconWidth + GUI_DROP_DOWN_MENU_ICON_SPACING);
					if (_page->hasCheckbox) textX += (checkboxSize + GUI_DROP_DOWN_MENU_CHECKBOX_SPACING);

					renderer->renderText(fntText, item->name, textX, itemY + (GUI_DROP_DOWN_MENU_ITEM_HEIGHT / 2) - 1, TextAlign::Left, TextAlign::Middle, colText);

					s32 iconX = itemX + GUI_DROP_DOWN_MENU_ICON_SPACING;
					if (item->checkable) {

						renderer->render9P(sprDropDownMenuCheckbox, item->enabled ? 0 : 2, itemX + GUI_DROP_DOWN_MENU_CHECKBOX_SPACING, itemY + (GUI_DROP_DOWN_MENU_ITEM_HEIGHT / 2) - (checkboxSize / 2) - 1, checkboxSize, checkboxSize);
						if (item->checked) renderer->renderSprite(sprDropDownMenuCheckboxIcon, !item->enabled, itemX + GUI_DROP_DOWN_MENU_CHECKBOX_SPACING + (checkboxSize / 2) - (sprDropDownMenuCheckboxIcon->sizeX / 2) + GUI_DROP_DOWN_MENU_CHECKBOX_ICON_HOR_OFFSET, itemY + (GUI_DROP_DOWN_MENU_ITEM_HEIGHT / 2) - (sprDropDownMenuCheckboxIcon->sizeY / 2) + GUI_DROP_DOWN_MENU_CHECKBOX_ICON_VER_OFFSET - 1);

						iconX += (checkboxSize + GUI_DROP_DOWN_MENU_CHECKBOX_SPACING);

					}

					if (item->icon != nullptr) {
						renderer->renderSprite(item->icon, item->iconSub, iconX, itemY + (GUI_DROP_DOWN_MENU_ITEM_HEIGHT / 2) - (item->icon->sizeY / 2), (item->enabled) ? GUI_DROP_DOWN_MENU_ICON_ALPHA_ENABLED : GUI_DROP_DOWN_MENU_ICON_ALPHA_DISABLED);
					}

					if (item->shortcut != "") {

						textX = itemX + _page->width - GUI_DROP_DOWN_MENU_TEXT_SPACING;

						if (_page->hasArrow) {
							textX -= (sprDropDownMenuArrow->sizeX + GUI_DROP_DOWN_MENU_ARROW_SPACING);
						}

						renderer->renderText(fntText, item->shortcut, textX, itemY + (GUI_DROP_DOWN_MENU_ITEM_HEIGHT / 2) - 1, TextAlign::Right, TextAlign::Middle, colText);

					}

					if (item->childPage != nullptr) {
						renderer->renderSprite(sprDropDownMenuArrow, !item->enabled, itemX + _page->width - sprDropDownMenuArrow->sizeX - GUI_DROP_DOWN_MENU_ARROW_SPACING, itemY + (GUI_DROP_DOWN_MENU_ITEM_HEIGHT / 2) - (sprDropDownMenuArrow->sizeY / 2));
					}

					itemY += GUI_DROP_DOWN_MENU_ITEM_HEIGHT;

				}
				else if (item->type == DropDownMenuItemType::Separator) {

					s32 height = sprDropDownMenuSeparator->sizeY + (GUI_DROP_DOWN_MENU_SEPARATOR_VER_SPACING * 2);

					if (itemY + height - 1 >= displayHeight) {

						itemY = _page->y;
						itemX += (_page->width + GUI_DROP_DOWN_MENU_PAGE_SPACING);

					}

					s32 sepX = itemX + GUI_DROP_DOWN_MENU_SEPARATOR_HOR_SPACING;
					if (_page->hasCheckbox) sepX += (GUI_DROP_DOWN_MENU_CHECKBOX_SIZE + GUI_DROP_DOWN_MENU_CHECKBOX_SPACING);
					if (_page->maxIconWidth > 0) sepX += (_page->maxIconWidth + GUI_DROP_DOWN_MENU_ICON_SPACING);

					s32 sepWidth = _page->width - GUI_DROP_DOWN_MENU_SEPARATOR_HOR_SPACING - (sepX - itemX);

					renderer->render3PHor(sprDropDownMenuSeparator, 0, sepX, itemY + GUI_DROP_DOWN_MENU_SEPARATOR_VER_SPACING, sepWidth);

					itemY += height;

				}

			}

		}

		for (s32 i = 0; i < pageLen; ++i) {
			renderer->render9P(sprDropDownMenu, 1, _page->x + (i * (_page->width + GUI_DROP_DOWN_MENU_PAGE_SPACING)), _page->y, _page->width, _page->height[i]);
		}

		for (DropDownMenuItem* item : _page->items) {

			if (item->childPage != nullptr && item->opened) {
				renderPage(item->childPage);
				break;
			}

		}

	}

	void DropDownMenu::resetPage(DropDownMenuPage* _page) {

		for (DropDownMenuItem* item : _page->items) {

			item->opened = false;

			if (item->type == DropDownMenuItemType::Item && item->childPage != nullptr) {
				resetPage(item->childPage);
			}

		}

	}

	void DropDownMenu::setOnOpen(std::function<void(DropDownMenu*)> _callback) {
		onOpen = _callback;
	}

	void DropDownMenu::setOnClose(std::function<void(DropDownMenu*)> _callback) {
		onClose = _callback;
	}

	void DropDownMenu::update(f32 dt) {

		if (!mouseStartMoved && (mouseXStart != gui->mouseX || mouseYStart != gui->mouseY)) {
			mouseStartMoved = true;
		}

		if (!keyboardMode) {
			itemHover = nullptr;
		}
		
		mouseOverPage = false;

		if (interactTimer > 0.0f) {

			interactTimer -= dt;
			if (interactTimer < 0.0f) interactTimer = 0.0f;

		}

		if (page != nullptr) {
			updatePage(page, dt);
		}

		if (!mouseOverPage) {

			itemHoveredKeyboard = nullptr;
			openedFromKeyboard = false;

		}

		if (itemHover == nullptr) {

			if (itemLastHovered != nullptr) {

				itemShouldClose = itemLastHovered;
				itemLastHovered = nullptr;

				timerTriggered = false;
				timer = 0.0f;

			}

			if (!timerTriggered && itemShouldClose != nullptr) {

				timer += dt;
				if (timer >= GUI_DROP_DOWN_MENU_OPEN_TIME) {

					timerTriggered = true;

					for (DropDownMenuItem* item : itemShouldClose->parentPage->items) {

						if (item->opened) {
							item->close();
						}

					}

				}

			}

		}
		
		if (gui->isMousePressed(MOUSE_LEFT_RIGHT_MIDDLE)) {

			if (!mouseOverPage) {

				if (menuBar != nullptr) {
					menuBar->buttonHoveredKeyboard = nullptr;
				}

				if (!(menuBar != nullptr && menuBar->buttonHovered != nullptr)) {
					close();
				}

			}

			keyboardMode = false;
			itemHoveredKeyboard = nullptr;
			keyStart = false;
			keyPress = false;
			keyShift = false;

		}

		if (gui->isMouseReleased(MOUSE_LEFT) || gui->isMouseReleased(MOUSE_RIGHT)) {

			if (itemHover != nullptr && !keyboardMode) {

				if (gui->isMouseReleased(MOUSE_LEFT) && itemHover->enabled && itemHover->type == DropDownMenuItemType::Item && interactTimer <= 0.0f) {

					bool shouldClose = (itemHover->childPage == nullptr || itemHover->hasCallback || itemHover->checkable);

					if (itemHover->checkable) itemHover->checked = !itemHover->checked;

					if (itemHover->hasCallback) {
						itemHover->callback(this, itemHover);
					}

					if (shouldClose) {
						close();
					}

				}

			}

		}
		else if (gui->isKeyPressed(KEY_UP)) {

			keyStart = false;
			keyPress = false;
			keyShift = false;
			keyTimerStart = 0.0f;

			if (doKeyUp()) {

				key = KEY_UP;
				keyStart = true;

			}

		}
		else if (gui->isKeyPressed(KEY_DOWN)) {

			keyStart = false;
			keyPress = false;
			keyShift = false;
			keyTimerStart = 0.0f;

			if (doKeyDown()) {

				key = KEY_DOWN;
				keyStart = true;

			}

		}
		else if (gui->isKeyPressed(KEY_LEFT)) {

			key = KEY_LEFT;
			keyStart = true;
			keyPress = false;
			keyShift = false;
			keyTimerStart = 0.0f;

			if (!doKeyLeft()) {
				keyStart = false;
			}

		}
		else if (gui->isKeyPressed(KEY_RIGHT)) {

			key = KEY_RIGHT;
			keyStart = true;
			keyPress = false;
			keyShift = false;
			keyTimerStart = 0.0f;

			if (!doKeyRight()) {
				keyStart = false;
			}
			
		}
		else if (gui->isKeyPressed(KEY_TAB)) {

			key = KEY_TAB;
			keyStart = true;
			keyPress = false;
			keyShift = gui->isKeyDown(KEY_SHIFT);
			keyTimerStart = 0.0f;

			if (!doKeyTab()) {
				keyStart = false;
			}

		}
		else if (gui->isKeyPressed(KEY_ENTER)) {

			keyStart = false;
			keyPress = false;
			keyShift = false;
			keyTimerStart = 0.0f;

			if (itemHover != nullptr && itemHover->enabled) {

				bool shouldClose = (itemHover->childPage == nullptr || itemHover->hasCallback || itemHover->checkable);

				if (itemHover->checkable) itemHover->checked = !itemHover->checked;

				if (itemHover->hasCallback) {
					itemHover->callback(this, itemHover);
				}

				if (shouldClose) {

					close();
					gui->clearKey(KEY_ENTER);

				}
				else {

					if (itemHover->childPage != nullptr && !itemHover->opened) {

						itemHover->opened = true;

						for (DropDownMenuItem* item : itemHover->childPage->items) {

							if (item->type == DropDownMenuItemType::Item && item->enabled) {

								if (!keyboardMode) {

									keyboardMode = true;
									itemHoveredKeyboard = itemHover;

								}

								itemHover = item;
								itemLastHovered = itemHover;

								timer = 0.0f;
								timerTriggered = false;

								break;

							}

						}

					}

				}

			}

		}
		else if (gui->isKeyPressed(KEY_ESCAPE)) {

			close();
			gui->clearKey(KEY_ESCAPE);

		}
		else if (gui->isKeyPressed(KEY_SHIFT)) {

			keyStart = false;
			keyPress = false;
			keyShift = false;
			keyTimerStart = 0.0f;

		}

		if (keyStart) {
			
			if (!gui->isKeyDown(key) || (keyShift && !gui->isKeyDown(KEY_SHIFT))) {

				keyStart = false;
				keyShift = false;

			}
			else {

				keyTimerStart += dt;
				if (keyTimerStart >= GUI_DROP_DOWN_MENU_KEY_PRESS_START_TIME) {

					keyStart = false;
					keyPress = true;

					keyTimer = GUI_DROP_DOWN_MENU_KEY_PRESS_TIME;

				}

			}

		}
		
		if (keyPress) {

			if (!gui->isKeyDown(key) || (keyShift && !gui->isKeyDown(KEY_SHIFT))) {

				keyPress = false;
				keyShift = false;

			}
			else {

				keyTimer += dt;
				if (keyTimer >= GUI_DROP_DOWN_MENU_KEY_PRESS_TIME) {

					keyTimer = 0.0f;

					if (key == KEY_UP) {
						doKeyUp();
					}
					else if (key == KEY_DOWN) {
						doKeyDown();
					}
					else if (key == KEY_LEFT) {
						doKeyLeft();
					}
					else if (key == KEY_RIGHT) {
						doKeyRight();
					}
					else if (key == KEY_TAB) {
						doKeyTab();
					}

				}

			}

		}

	}

	void DropDownMenu::render() {
		
		if (page != nullptr) {
			renderPage(page);
		}

	}

	DropDownMenuItem* DropDownMenu::findFirstInteractableItem(DropDownMenuPage* _page) {

		for (DropDownMenuItem* item : _page->items) {

			if (item->type == DropDownMenuItemType::Item && item->enabled) {
				return item;
			}

		}

		return nullptr;

	}

	bool DropDownMenu::doKeyUp() {

		if (itemHover != nullptr && itemHover->type == DropDownMenuItemType::Item) {

			DropDownMenuPage* currentPage;
			s32 ind = -1;
			s32 indNew = -1;

			bool foundChild = false;
			DropDownMenuPage* childPage = nullptr;

			for (DropDownMenuItem* item : itemHover->parentPage->items) {

				if (item->opened && item->enabled && item->childPage != nullptr) {

					childPage = item->childPage;
					foundChild = true;
					break;

				}

			}

			if (foundChild) {

				currentPage = childPage;

				for (s32 i = 0; i < currentPage->items.size(); ++i) {

					DropDownMenuItem* item = currentPage->items[i];
					if (item->type == DropDownMenuItemType::Item && item->enabled) {

						indNew = i;
						break;

					}

				}

			}
			else {

				currentPage = itemHover->parentPage;

				ind = itemHover->getIndex();
				indNew = ind;

				for (s32 i = ind - 1; i >= 0; --i) {

					DropDownMenuItem* item = currentPage->items[i];
					if (item->type == DropDownMenuItemType::Item && item->enabled) {

						indNew = i;
						break;

					}

				}

				if (ind == indNew) {

					for (s32 i = (s32)currentPage->items.size() - 1; i > ind; --i) {

						DropDownMenuItem* item = currentPage->items[i];
						if (item->type == DropDownMenuItemType::Item && item->enabled) {

							indNew = i;
							break;

						}

					}

				}

			}

			if (indNew != ind && indNew >= 0) {

				if (!keyboardMode) {

					keyboardMode = true;
					itemHoveredKeyboard = itemHover;

				}

				itemHover = currentPage->items[indNew];
				itemLastHovered = itemHover;

				timer = 0.0f;
				timerTriggered = false;

				return true;

			}

		}
		else if (itemHover == nullptr) {

			DropDownMenuPage* leaf = getLeafPage();

			if (leaf != nullptr) {

				s32 ind = -1;
				for (s32 i = 0; i < leaf->items.size(); ++i) {

					DropDownMenuItem* item = leaf->items[i];
					if (item->type == DropDownMenuItemType::Item && item->enabled) {

						ind = i;
						break;

					}

				}

				if (ind >= 0) {

					keyboardMode = true;
					itemHoveredKeyboard = itemHover;

					itemHover = leaf->items[ind];
					itemLastHovered = itemHover;

					timer = 0.0f;
					timerTriggered = false;

					return true;

				}

			}

		}

		return false;

	}

	bool DropDownMenu::doKeyDown() {

		if (itemHover != nullptr && itemHover->type == DropDownMenuItemType::Item) {

			DropDownMenuPage* currentPage;
			s32 ind = -1;
			s32 indNew = -1;

			bool foundChild = false;
			DropDownMenuPage* childPage = nullptr;

			for (DropDownMenuItem* item : itemHover->parentPage->items) {

				if (item->opened && item->childPage != nullptr && item->enabled) {

					childPage = item->childPage;
					foundChild = true;
					break;

				}

			}

			if (foundChild) {

				currentPage = childPage;

				for (s32 i = 0; i < currentPage->items.size(); ++i) {

					DropDownMenuItem* item = currentPage->items[i];
					if (item->type == DropDownMenuItemType::Item && item->enabled) {

						indNew = i;
						break;

					}

				}

			}
			else {

				currentPage = itemHover->parentPage;

				ind = itemHover->getIndex();
				indNew = ind;

				for (s32 i = ind + 1; i < currentPage->items.size(); ++i) {

					DropDownMenuItem* item = currentPage->items[i];
					if (item->type == DropDownMenuItemType::Item && item->enabled) {

						indNew = i;
						break;

					}

				}

				if (ind == indNew) {

					for (s32 i = 0; i < ind; ++i) {

						DropDownMenuItem* item = currentPage->items[i];
						if (item->type == DropDownMenuItemType::Item && item->enabled) {

							indNew = i;
							break;

						}

					}

				}

			}

			if (indNew != ind && indNew >= 0) {

				if (!keyboardMode) {

					keyboardMode = true;
					itemHoveredKeyboard = itemHover;

				}

				itemHover = currentPage->items[indNew];
				itemLastHovered = itemHover;

				timer = 0.0f;
				timerTriggered = false;

				return true;

			}

		}
		else if (itemHover == nullptr) {

			DropDownMenuPage* leaf = getLeafPage();

			if (leaf != nullptr) {

				s32 ind = -1;
				for (s32 i = 0; i < leaf->items.size(); ++i) {

					DropDownMenuItem* item = leaf->items[i];
					if (item->type == DropDownMenuItemType::Item && item->enabled) {

						ind = i;
						break;

					}

				}

				if (ind >= 0) {

					keyboardMode = true;
					itemHoveredKeyboard = itemHover;

					itemHover = leaf->items[ind];
					itemLastHovered = itemHover;

					timer = 0.0f;
					timerTriggered = false;

					return true;

				}

			}

		}

		return false;

	}

	bool DropDownMenu::doKeyLeft() {

		if (itemHover != nullptr && itemHover->type == DropDownMenuItemType::Item && itemHover->parentItem != nullptr) {

			if (!keyboardMode) {

				keyboardMode = true;
				itemHoveredKeyboard = itemHover;

			}

			itemHover = itemHover->parentItem;
			itemHover->opened = false;
			itemLastHovered = itemHover;

			timer = 0.0f;
			timerTriggered = false;

			return true;

		}
		
		if (menuBar != nullptr) {
			goToPreviousMenuBarButton();
		}

		return false;

	}
	
	bool DropDownMenu::doKeyRight() {
		
		if (itemHover != nullptr && itemHover->type == DropDownMenuItemType::Item && itemHover->enabled && itemHover->childPage != nullptr) {

			bool foundChild = false;

			itemHover->opened = true;

			for (DropDownMenuItem* item : itemHover->childPage->items) {

				if (item->type == DropDownMenuItemType::Item && item->enabled) {

					if (!keyboardMode) {

						keyboardMode = true;
						itemHoveredKeyboard = itemHover;

					}

					itemHover = item;
					itemLastHovered = itemHover;

					timer = 0.0f;
					timerTriggered = false;

					foundChild = true;

					break;

				}

			}
			
			if (foundChild) {
				return true;
			}

		}else if (itemHover == nullptr) {

			DropDownMenuPage* leaf = getLeafPage();
			if (leaf != nullptr && leaf != page) {

				DropDownMenuItem* item = findFirstInteractableItem(leaf);
				
				if (!keyboardMode) {

					keyboardMode = true;
					itemHoveredKeyboard = itemHover;

				}

				itemHover = item;
				itemLastHovered = itemHover;

				timer = 0.0f;
				timerTriggered = false;

				return true;

				
			}

		}
		
		if(menuBar != nullptr) {
			goToNextMenuBarButton();
		}

		return false;

	}

	bool DropDownMenu::doKeyTab() {

		if (itemHover != nullptr && itemHover->type == DropDownMenuItemType::Item && itemHover->enabled) {

			if (keyShift) {

				if (!doKeyUp()) {
					return false;
				}

			}
			else {

				if (!doKeyDown()) {
					return false;
				}

			}

			return true;

		}else if (itemHover == nullptr) {

			DropDownMenuPage* leaf = getLeafPage();
			if (leaf != nullptr && leaf != page) {
				
				DropDownMenuItem* item = findFirstInteractableItem(leaf);
				if (item != nullptr) {

					if (!keyboardMode) {

						keyboardMode = true;
						itemHoveredKeyboard = itemHover;

					}

					itemHover = item;
					itemLastHovered = itemHover;

					timer = 0.0f;
					timerTriggered = false;

					return true;

				}

			}

		}

		if (menuBar != nullptr) {

			if (keyShift) {
				goToPreviousMenuBarButton(false);
			}
			else {
				goToNextMenuBarButton(false);
			}

		}

		return false;

	}

	void DropDownMenu::goToNextMenuBarButton(bool _selectFirstItemOnPage) {

		if (menuBar == nullptr) {
			return;
		}

		const auto& it = std::find(menuBar->buttonList.begin(), menuBar->buttonList.end(), menuBar->buttonSelected);
		if (it != menuBar->buttonList.end()) {

			s32 ind = (s32)(it - menuBar->buttonList.begin());
			s32 indNew = ind;

			for (s32 i = ind + 1; i < menuBar->buttonList.size(); ++i) {

				MenuBarButton* button = menuBar->buttonList[i];
				if (button->enabled && button->menu != nullptr) {

					indNew = i;
					break;

				}

			}

			if (ind == indNew) {

				for (s32 i = 0; i < ind; ++i) {

					MenuBarButton* button = menuBar->buttonList[i];
					if (button->enabled && button->menu != nullptr) {

						indNew = i;
						break;

					}

				}

			}

			if (ind != indNew) {

				bool keyStartTemp = keyStart;
				bool keyPressTemp = keyPress;
				bool keyShiftTemp = keyShift;
				f32 keyTimerStartTemp = keyTimerStart;
				f32 keyTimerTemp = keyTimer;

				close(false);

				if (menuBar->buttonHovered != nullptr) {
					menuBar->buttonHoveredKeyboard = menuBar->buttonHovered;
				}

				menuBar->buttonSelected = menuBar->buttonList[indNew];

				DropDownMenu* nextMenu = menuBar->buttonSelected->menu;
				nextMenu->open(menuBar->x + menuBar->buttonSelected->x + GUI_MENU_BAR_DROP_DOWN_HOR_OFFSET, menuBar->y + menuBar->height + GUI_MENU_BAR_DROP_DOWN_VER_OFFSET, menuBar);

				nextMenu->keyStart = keyStartTemp;
				nextMenu->keyPress = keyPressTemp;
				nextMenu->keyShift = keyShiftTemp;
				nextMenu->keyTimerStart = keyTimerStartTemp;
				nextMenu->keyTimer = keyTimerTemp;
				nextMenu->key = key;
				nextMenu->keyboardMode = true;
				nextMenu->openedFromKeyboard = true;

				if (nextMenu->page != nullptr && _selectFirstItemOnPage) {

					for (DropDownMenuItem* item : nextMenu->page->items) {

						if (item->type == DropDownMenuItemType::Item && item->enabled) {

							nextMenu->itemHover = item;
							nextMenu->itemLastHovered = nextMenu->itemHover;

							nextMenu->timer = 0.0f;
							nextMenu->timerTriggered = false;

							break;

						}

					}

				}

			}

		}

	}

	void DropDownMenu::goToPreviousMenuBarButton(bool _selectFirstItemOnPage) {

		if (menuBar == nullptr) {
			return;
		}

		const auto& it = std::find(menuBar->buttonList.begin(), menuBar->buttonList.end(), menuBar->buttonSelected);
		if (it != menuBar->buttonList.end()) {

			s32 ind = (s32)(it - menuBar->buttonList.begin());
			s32 indNew = ind;

			for (s32 i = ind - 1; i >= 0; --i) {

				MenuBarButton* button = menuBar->buttonList[i];
				if (button->enabled && button->menu != nullptr) {

					indNew = i;
					break;

				}

			}

			if (ind == indNew) {

				for (s32 i = (s32)menuBar->buttonList.size() - 1; i > ind; --i) {

					MenuBarButton* button = menuBar->buttonList[i];
					if (button->enabled && button->menu != nullptr) {

						indNew = i;
						break;

					}

				}

			}

			if (ind != indNew) {

				bool keyStartTemp = keyStart;
				bool keyPressTemp = keyPress;
				bool keyShiftTemp = keyShift;
				f32 keyTimerStartTemp = keyTimerStart;
				f32 keyTimerTemp = keyTimer;

				close(false);

				if (menuBar->buttonHovered != nullptr) {
					menuBar->buttonHoveredKeyboard = menuBar->buttonHovered;
				}

				menuBar->buttonSelected = menuBar->buttonList[indNew];

				DropDownMenu* nextMenu = menuBar->buttonSelected->menu;
				nextMenu->open(menuBar->x + menuBar->buttonSelected->x + GUI_MENU_BAR_DROP_DOWN_HOR_OFFSET, menuBar->y + menuBar->height + GUI_MENU_BAR_DROP_DOWN_VER_OFFSET, menuBar);

				nextMenu->keyStart = keyStartTemp;
				nextMenu->keyPress = keyPressTemp;
				nextMenu->keyShift = keyShiftTemp;
				nextMenu->keyTimerStart = keyTimerStartTemp;
				nextMenu->keyTimer = keyTimerTemp;
				nextMenu->key = key;
				nextMenu->keyboardMode = true;
				nextMenu->openedFromKeyboard = true;

				if (nextMenu->page != nullptr && _selectFirstItemOnPage) {

					for (DropDownMenuItem* item : nextMenu->page->items) {

						if (item->type == DropDownMenuItemType::Item && item->enabled) {

							nextMenu->itemHover = item;
							nextMenu->itemLastHovered = nextMenu->itemHover;

							nextMenu->timer = 0.0f;
							nextMenu->timerTriggered = false;

							break;

						}

					}

				}

			}

		}

	}

}