/*
    ComboBox.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/GUI/ComboBox.h"
#include "Engine/GUI/DropDownMenu.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/GUIMacros.h"

namespace Zixel {

	s32 ComboBoxItem::getIndex() {
		return index;
	}

	ComboBox::ComboBox(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::ComboBox;

		sprComboBox = renderer->getTextureAtlasSprite(GUI_COMBO_BOX_SPR_BACKGROUND);
		sprComboBoxArrow = renderer->getTextureAtlasSprite(GUI_COMBO_BOX_SPR_ARROW);

		fntText = renderer->getTextureAtlasFont(GUI_COMBO_BOX_FONT);

		arrowAlignment = TextAlign::Right;

		height = GUI_COMBO_BOX_HEIGHT;

		menu = new DropDownMenu(gui);
		menu->setOnClose(GUI_DROP_DOWN_MENU_CLOSE_CALLBACK(ComboBox::menuCloseCallback));

		LocalShortcut shortcutInteract;
		shortcutInteract.keyCode = KEY_ENTER;
		shortcutInteract.callOnEndImplicitly = false;
		shortcutInteract.onCallback = GUI_SHORTCUT_CALLBACK(ComboBox::onShortcutInteract);

		addLocalShortcut(shortcutInteract);

	}

	ComboBox::~ComboBox() {

		for (ComboBoxItem* item : itemList) {
			delete item;
		}

	}

	void ComboBox::onResize(s32 prevWidth, s32 prevHeight) {
		updateTextDisplay();
	}
	
	void ComboBox::update(f32 dt) {
		
		if (canInteract()) {

			if (gui->widgetSelected != this) {
				
				if (!inShortcutMode()) {

					if (gui->getWidgetMouseOver() == this) {

						if (!menuOpened) {

							if(showTooltip) gui->setTooltip(this, tooltip);

							ind = 1;

							if (gui->isMousePressed(MOUSE_LEFT) && !menuJustClosed) {

								gui->setWidgetFocused(this);

								menu->open(x, y + height);
								menuOpened = true;

								ind = 2;
								showTooltip = false;

							}

						}
						else {

							if (gui->isMousePressed(MOUSE_LEFT)) {

								menu->close();
								menuOpened = false;

								ind = 1;

							}
							else {
								ind = 2;
							}

						}

					}
					else {
						ind = (menuOpened ? 2 : 0);
						if (!menuOpened) showTooltip = true;
					}

				}

				menuJustClosed = false;

			}

		}
		else {

			gui->deselectWidget(this);
			
			ind = (menuOpened ? 2 : 0);
			menuJustClosed = false;
			showTooltip = true;

		}

	}

	void ComboBox::render() {
		s32 drawInd = !enabledGlobal + ((ind + (ind > 0)) * enabledGlobal);
		if ((drawInd != 0 || renderIdleBackground) && (drawInd != 1 || renderDisabledBackground)) renderer->render9P(sprComboBox, drawInd, x, y, widthDraw, height);

		bool hasText = !textDisplay.empty() && renderText;

		s32 totalWidth = sprComboBoxArrow->sizeX * showArrow;
		if (hasText) totalWidth += (textDisplayWidth + GUI_COMBO_BOX_ARROW_SPACING * showArrow);
		if (sprIcon != nullptr) totalWidth += (sprIcon->sizeX + GUI_COMBO_BOX_ICON_HOR_SPACING * hasText);

		//renderer->renderRect(x + widthDraw / 2 - totalWidth / 2, y, totalWidth, height, COLOR_RED);

		if (hasText || sprIcon != nullptr) {

			Color4f textCol;
			if (enabledGlobal) textCol = GUI_COMBO_BOX_TEXT_COL; else textCol = GUI_COMBO_BOX_TEXT_COL_DISABLED;

			s32 textX = x + GUI_COMBO_BOX_TEXT_SPACING + GUI_COMBO_BOX_TEXT_HOR_OFFSET;
			if (sprIcon != nullptr) textX += sprIcon->sizeX + GUI_COMBO_BOX_ICON_HOR_SPACING;

			TextAlign textAlign = TextAlign::Left;

			s32 cutX, cutWidth;
			if (arrowAlignment == TextAlign::Left) {

				textX = x + widthDraw - 1 - GUI_COMBO_BOX_TEXT_SPACING - GUI_COMBO_BOX_TEXT_HOR_OFFSET;
				textAlign = TextAlign::Right;

				if (showArrow) {
					cutX = x + (GUI_COMBO_BOX_ARROW_SPACING * 2) + sprComboBoxArrow->sizeX;
					cutWidth = widthDraw - (GUI_COMBO_BOX_ARROW_SPACING * 2) - sprComboBoxArrow->sizeX - GUI_COMBO_BOX_TEXT_SPACING;
				}
				else {
					cutX = x + GUI_COMBO_BOX_TEXT_SPACING;
					cutWidth = widthDraw - (GUI_COMBO_BOX_TEXT_SPACING * 2);
				}

				renderer->cutStart(cutX, y, cutWidth, height);
				//renderer->renderRect(cutX, y, cutWidth, height, COLOR_RED);

			}
			else if (arrowAlignment == TextAlign::Center) {

				textX = x + (widthDraw / 2) - (totalWidth / 2) + (sprComboBoxArrow->sizeX + GUI_COMBO_BOX_ARROW_SPACING) * showArrow;
				if (sprIcon != nullptr) textX += sprIcon->sizeX + GUI_COMBO_BOX_ICON_HOR_SPACING;

				textAlign = TextAlign::Left;

				renderer->cutStart(x, y, widthDraw, height);
			}
			else {

				if (showArrow) cutWidth = widthDraw - GUI_COMBO_BOX_TEXT_SPACING - (GUI_COMBO_BOX_ARROW_SPACING * 2) - sprComboBoxArrow->sizeX;
				else cutWidth = widthDraw - (GUI_COMBO_BOX_TEXT_SPACING * 2);

				renderer->cutStart(x + GUI_COMBO_BOX_TEXT_SPACING, y, cutWidth, height);
				//renderer->renderRect(x + GUI_COMBO_BOX_TEXT_SPACING, y, cutWidth, height, COLOR_RED);
			}

			if (hasText) renderer->renderText(fntText, textDisplay, textX, y + (heightDraw / 2) + GUI_COMBO_BOX_TEXT_VER_OFFSET, textAlign, TextAlign::Middle, textCol);

			if (sprIcon != nullptr) {
				s32 iconX = x;
				if (hasText) {
					if (arrowAlignment == TextAlign::Left) {
						iconX = textX - textDisplayWidth - sprIcon->sizeX - GUI_COMBO_BOX_ICON_HOR_SPACING;
					}
					else if (arrowAlignment == TextAlign::Center) {
						iconX = textX - sprIcon->sizeX - GUI_COMBO_BOX_ICON_HOR_SPACING;
					}
					else {
						iconX = x + GUI_COMBO_BOX_TEXT_SPACING;
					}
				}
				else {
					if (arrowAlignment == TextAlign::Left) {
						iconX = x + widthDraw - sprIcon->sizeX - GUI_COMBO_BOX_TEXT_SPACING;
					}
					else if (arrowAlignment == TextAlign::Center) {
						iconX = x + (widthDraw / 2) - (totalWidth / 2) + totalWidth - sprIcon->sizeX;
					}
					else {
						iconX = x + GUI_COMBO_BOX_TEXT_SPACING;
					}
				}

				renderer->renderSprite(sprIcon, iconSub, iconX, y + (heightDraw / 2) - (sprIcon->sizeY / 2), enabledGlobal ? 1.0f : GUI_COMBO_BOX_ICON_DISABLED_ALPHA);
			}

			renderer->cutEnd();

		}

		renderer->cutStart(x, y, widthDraw, height);

		if (showArrow) {
			s32 arrowX = x + widthDraw - sprComboBoxArrow->sizeX - GUI_COMBO_BOX_ARROW_SPACING;
			if (arrowAlignment == TextAlign::Left) arrowX = x + GUI_COMBO_BOX_ARROW_SPACING;
			else if (arrowAlignment == TextAlign::Center) arrowX = x + (widthDraw / 2) - (totalWidth / 2);

			renderer->renderSprite(sprComboBoxArrow, !enabledGlobal, arrowX, y + (heightDraw / 2) - (sprComboBoxArrow->sizeY / 2));
		}

		renderer->cutEnd();

	}

	bool ComboBox::onShortcutInteract() {

		showTooltip = false;

		if (!menuOpened) {

			menu->open(x, y + height);
			menuOpened = true;

			if (itemSelected != nullptr && menu->page != nullptr) {

				menu->keyboardMode = true;
				menu->itemHoveredKeyboard = menu->page->items[itemSelected->getIndex()];
				menu->itemHover = menu->itemHoveredKeyboard;
				menu->itemLastHovered = menu->itemHoveredKeyboard;

			}

			ind = 2;

		}

		return false;

	}

	void ComboBox::updateTextDisplay() {

		if (itemSelected == nullptr) {

			textDisplayWidth = 0;
			textDisplay = "";

		}
		else {
			textDisplay = itemSelected->name;
		}

		if (textDisplay.empty()) return;

		s32 i = (s32)textDisplay.length() - 1;
		bool addedDots = false;

		s32 cutWidth = widthDraw;
		if (showArrow) cutWidth = widthDraw - (GUI_COMBO_BOX_ARROW_SPACING * 2) - sprComboBoxArrow->sizeX - GUI_COMBO_BOX_TEXT_SPACING;
		else cutWidth = widthDraw - (GUI_COMBO_BOX_TEXT_SPACING * 2);

		s32 iconWidth = 0;
		if (sprIcon != nullptr) iconWidth = sprIcon->sizeX + GUI_COMBO_BOX_ICON_HOR_SPACING;

		while (renderer->getStringWidth(fntText, textDisplay) + iconWidth > cutWidth && i >= 0) {

			if (!addedDots) {

				addedDots = true;
				textDisplay += "...";

			}

			textDisplay.erase(i, 1);
			--i;

		}

		textDisplayWidth = renderer->getStringWidth(fntText, textDisplay);

	}

	ComboBoxItem* ComboBox::addItem(std::string _name, bool _callOnItemChange) {

		ComboBoxItem* item = new ComboBoxItem();
		itemList.push_back(item);

		item->name = _name;
		item->index = (s32)itemList.size() - 1;

		if (itemSelected == nullptr) {

			itemSelected = item;

			if (onItemChange && _callOnItemChange) {
				onItemChange(this, item);
			}

		}

		DropDownMenuItem* menuItem = menu->addItem(_name);
		menuItem->setCallback(GUI_DROP_DOWN_MENU_ITEM_PRESS_CALLBACK(ComboBox::menuItemCallback));

		updateTextDisplay();

		return item;

	}

	ComboBoxItem* ComboBox::getItemFromIndex(s32 _index) {

		if (_index < 0 || _index >= (s32)itemList.size()) {
			return nullptr;
		}

		return itemList[_index];

	}

	void ComboBox::selectItemFromIndex(s32 _index, bool _callOnItemChange) {

		ComboBoxItem* newItem = nullptr;

		if (_index >= 0 && _index < (s32)itemList.size()) {
			newItem = itemList[_index];
		}
		
		if (newItem != itemSelected) {
			
			itemSelected = newItem;
			updateTextDisplay();

			if (_callOnItemChange && onItemChange) {
				onItemChange(this, newItem);
			}

		}

	}

	void ComboBox::selectItem(ComboBoxItem* _item, bool _callOnItemChange) {

		if (_item != nullptr) {

			if (_item->index < 0 || _item->index >= (s32)itemList.size()) {
				return;
			}

			if (itemList[_item->index] != _item) return;

		}

		if (_item != itemSelected) {

			itemSelected = _item;
			updateTextDisplay();

			if (_callOnItemChange && onItemChange) {
				onItemChange(this, _item);
			}

		}

	}

	void ComboBox::setArrowAlignment(TextAlign _alignment) {
		arrowAlignment = _alignment;
	}

	void ComboBox::setRenderText(bool _renderText) {
		renderText = _renderText;
	}

	void ComboBox::setIcon(Sprite* _sprIcon, u8 _iconSub) {
		sprIcon = _sprIcon;
		iconSub = _iconSub;

		updateTextDisplay();
	}

	void ComboBox::menuItemCallback(DropDownMenu* _menu, DropDownMenuItem* _item) {

		ComboBoxItem* itemPrev = itemSelected;
		itemSelected = itemList[_item->getIndex()];

		if (itemPrev != itemSelected) {

			updateTextDisplay();

			if (onItemChange) {
				onItemChange(this, itemSelected);
			}
			
			if (onItemSelect) {
				onItemSelect(this, itemPrev, itemSelected);
			}

		}

	}

	void ComboBox::menuCloseCallback(DropDownMenu* _menu) {

		menuOpened = false;
		menuJustClosed = true;

		ind = (gui->widgetMouseOver == this);

	}

	void ComboBox::setOnItemChange(std::function<void(ComboBox*, ComboBoxItem*)> _callback) {
		onItemChange = _callback;
	}

	void ComboBox::setOnItemSelect(std::function<void(ComboBox*, ComboBoxItem*, ComboBoxItem*)> _callback) {
		onItemSelect = _callback;
	}

}