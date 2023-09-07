/*
    MenuButton.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/GUI/MenuButton.h"
#include "Engine/GUI/DropDownMenu.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/GUIMacros.h"

namespace Zixel {

	MenuButton::MenuButton(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::MenuButton;

		sprButton = renderer->getTextureAtlasSprite(GUI_MENU_BUTTON_SPR);
		fntText = renderer->getTextureAtlasFont(GUI_MENU_BUTTON_FONT);

		textHAlign = TextAlign::Center;
		textVAlign = TextAlign::Middle;

		LocalShortcut shortcut;
		shortcut.keyCode = KEY_ENTER;
		shortcut.callOnEndImplicitly = false;
		shortcut.onCallback = GUI_SHORTCUT_CALLBACK(MenuButton::onShortcutInteract);
		shortcut.onEnd = GUI_SHORTCUT_CALLBACK(MenuButton::onShortcutInteractEnd);
		
		addLocalShortcut(shortcut);

	}

	void MenuButton::onResize(s32 prevWidth, s32 prevHeight) {
		updateTextDisplay();
	}

	void MenuButton::update(f32 dt) {

		if (canInteract()) {

			if (gui->widgetSelected != this) {

				if (!inShortcutMode()) {

					if (gui->getWidgetMouseOver() == this) {

						if (!menuOpened || menu == nullptr) {

							if (showTooltip) gui->setTooltip(this, tooltip);

							ind = 1;

							if (gui->isMousePressed(MOUSE_LEFT) && (!menuJustClosed || menu == nullptr)) {

								if(menu == nullptr) gui->setWidgetSelected(this);

								ind = 2;
								showTooltip = false;

								if (menu != nullptr) {
									gui->setWidgetFocused(this);
									menu->open(x, y + height);
									menuOpened = true;
								}

							}

						}
						else if (menuOpened) {

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
			else {

				if (!gui->isMouseDown(MOUSE_LEFT)) {

					gui->deselectWidget(this);

					if (gui->getWidgetMouseOver() == this) {

						ind = 1;

					}
					else {
						ind = 0;
						showTooltip = true;
					}

				}
				else {
					ind = 2;
				}

			}

		}
		else {

			gui->deselectWidget(this);

			if (enabledGlobal) ind = (menuOpened ? 2 : 0);
			else ind = 3;

			menuJustClosed = false;
			showTooltip = true;

		}

	}

	void MenuButton::render() {

		if((ind != 0 || renderIdleBackground) && (ind != 3 || renderDisabledBackground)) renderer->render9P(sprButton, ind, x, y, widthDraw, heightDraw);

		renderer->cutStart(x + GUI_MENU_BUTTON_TEXT_HOR_SPACING, y + GUI_MENU_BUTTON_TEXT_VER_SPACING, widthDraw - (GUI_MENU_BUTTON_TEXT_HOR_SPACING * 2), heightDraw - (GUI_MENU_BUTTON_TEXT_VER_SPACING * 2));

		if(textDraw != ""){

			Color4f textCol;
			if (enabledGlobal) textCol = GUI_MENU_BUTTON_TEXT_COL; else textCol = GUI_MENU_BUTTON_TEXT_COL_DISABLED;
			
			renderer->renderText(fntText, textDraw, x + textX, y + textY + (ind == 2) - 1, textHAlign, textVAlign, textCol);

		}

		if (sprIcon != nullptr) {
			renderer->renderSprite(sprIcon, iconSub, x + iconX, y + iconY + (ind == 2), enabledGlobal ? 1.0f : GUI_MENU_BUTTON_ICON_DISABLED_ALPHA);
		}

		renderer->cutEnd();

	}

	void MenuButton::setText(std::string _text) {

		text = _text;
		updateTextDisplay();

	}

	void MenuButton::setIcon(Sprite* _sprIcon, u8 _iconSub) {

		sprIcon = _sprIcon;
		iconSub = _iconSub;

		updateTextDisplay();

	}

	std::string MenuButton::getText() {
		return text;
	}

	void MenuButton::updateTextDisplay() {

		textDraw = text;

		if (textDraw != "") {

			s32 maxW = widthDraw - (GUI_MENU_BUTTON_TEXT_HOR_SPACING * 2);
			if (sprIcon != nullptr) maxW -= (sprIcon->sizeX + GUI_MENU_BUTTON_ICON_HOR_SPACING);
			maxW = Math::maxInt(0, maxW);

			if (renderer->getStringWidth(fntText, textDraw) > maxW) {

				textDraw += "...";

				while (textDraw.length() > 3 && renderer->getStringWidth(fntText, textDraw) > maxW)
					textDraw.erase(textDraw.begin() + textDraw.size() - 4);

			}

			textX = 0;
			textY = 0;

			if (textHAlign == TextAlign::Right) {
				textX = widthDraw - 1 - GUI_MENU_BUTTON_TEXT_HOR_SPACING;
			}
			else if (textHAlign == TextAlign::Center) {
				textX = (widthDraw / 2);
				if (sprIcon != nullptr) textX += (sprIcon->sizeX / 2) + (GUI_MENU_BUTTON_ICON_HOR_SPACING / 2);
			}
			else {
				textX = GUI_MENU_BUTTON_TEXT_HOR_SPACING;
				if (sprIcon != nullptr) textX += (sprIcon->sizeX + GUI_MENU_BUTTON_ICON_HOR_SPACING);
			}

			if (textVAlign == TextAlign::Bottom) {

				s32 textHeight = renderer->getStringHeight(fntText, textDraw);

				if (sprIcon != nullptr && sprIcon->sizeY > textHeight)
					textY = heightDraw - 1 - GUI_MENU_BUTTON_TEXT_VER_SPACING - (sprIcon->sizeY / 2) + (textHeight / 2);
				else
					textY = heightDraw - 1 - GUI_MENU_BUTTON_TEXT_VER_SPACING;
			}
			else if (textVAlign == TextAlign::Middle) {
				textY = (heightDraw / 2);
			}
			else {

				s32 textHeight = renderer->getStringHeight(fntText, textDraw);

				if (sprIcon != nullptr && sprIcon->sizeY > textHeight)
					textY = GUI_MENU_BUTTON_TEXT_VER_SPACING + (sprIcon->sizeY / 2) - (textHeight / 2);
				else
					textY = GUI_MENU_BUTTON_TEXT_VER_SPACING;
			}

		}

		if (sprIcon != nullptr) {

			iconX = 0;
			iconY = 0;

			if (textHAlign == TextAlign::Right) {

				iconX += (widthDraw - sprIcon->sizeX - GUI_MENU_BUTTON_TEXT_HOR_SPACING);
				if (textDraw != "") iconX -= (renderer->getStringWidth(fntText, textDraw) + GUI_MENU_BUTTON_ICON_HOR_SPACING);

			}
			else if (textHAlign == TextAlign::Center) {

				iconX += (widthDraw / 2) - (sprIcon->sizeX / 2);
				if (textDraw != "") iconX -= (renderer->getStringWidth(fntText, textDraw) / 2) + (GUI_MENU_BUTTON_ICON_HOR_SPACING / 2);

			}
			else iconX += GUI_MENU_BUTTON_TEXT_HOR_SPACING;

			if (textVAlign == TextAlign::Bottom) {

				s32 textHeight = renderer->getStringHeight(fntText, textDraw);

				iconY += heightDraw - sprIcon->sizeY - GUI_MENU_BUTTON_TEXT_VER_SPACING;
				if (textHeight > sprIcon->sizeY) iconY -= (textHeight / 2) - (sprIcon->sizeY / 2);

			}
			else if (textVAlign == TextAlign::Middle) iconY += (heightDraw / 2) - (sprIcon->sizeY / 2);
			else {

				s32 textHeight = renderer->getStringHeight(fntText, textDraw);

				iconY = GUI_MENU_BUTTON_TEXT_VER_SPACING;
				if (textHeight > sprIcon->sizeY) iconY += (textHeight / 2) - (sprIcon->sizeY / 2);

			}

		}

	}

	void MenuButton::attachDropDownMenu(DropDownMenu* _menu) {
		menu = _menu;
		menu->setOnClose(GUI_DROP_DOWN_MENU_CLOSE_CALLBACK(MenuButton::menuCloseCallback));
	}

	void MenuButton::menuCloseCallback(DropDownMenu* _menu) {
		menuOpened = false;
		menuJustClosed = true;

		ind = (gui->widgetMouseOver == this);
	}

	bool MenuButton::onShortcutInteract() {

		showTooltip = false;

		if (!menuOpened && menu != nullptr) {

			menu->open(x, y + height);
			menuOpened = true;

			if (menu->page != nullptr && menu->page->items.size() > 0) {
				menu->keyboardMode = true;
				menu->itemHoveredKeyboard = menu->page->items[0];
				menu->itemHover = menu->itemHoveredKeyboard;
				menu->itemLastHovered = menu->itemHoveredKeyboard;
			}

			ind = 2;

			return false;

		}
		else if (menu == nullptr) {
			ind = 2;
		}

		return true;

	}

	void MenuButton::onShortcutInteractEnd() {
		ind = (gui->getWidgetMouseOver() == this);
	}

}