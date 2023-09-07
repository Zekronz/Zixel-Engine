/*
    ToggleButton.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/Renderer.h"
#include "Engine/KeyCodes.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/GUI/ToggleButton.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/GUIMacros.h"

namespace Zixel {

	ToggleButton::ToggleButton(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::ToggleButton;

		sprToggleButton = renderer->getTextureAtlasSprite(GUI_TOGGLE_BUTTON_SPR);
		fntText = renderer->getTextureAtlasFont(GUI_TOGGLE_BUTTON_FONT);

		LocalShortcut shortcut;
		shortcut.keyCode = KEY_ENTER;
		shortcut.callOnEndImplicitly = false;
		shortcut.onCallback = GUI_SHORTCUT_CALLBACK(ToggleButton::onShortcutInteract);
		shortcut.onEnd = GUI_SHORTCUT_CALLBACK(ToggleButton::onShortcutInteractEnd);

		addLocalShortcut(shortcut);

	}

	void ToggleButton::setCallback(std::function<void(ToggleButton*, bool)> _callback) {
		callback = _callback;
	}

	void ToggleButton::setText(std::string _text) {

		text = _text;
		updateTextDisplay();

	}

	void ToggleButton::setIcon(Sprite* _sprIcon, u8 _iconSub) {

		sprIcon = _sprIcon;
		iconSub = _iconSub;

		updateTextDisplay();

	}

	std::string ToggleButton::getText() {
		return text;
	}

	void ToggleButton::updateTextDisplay() {

		textDraw = text;

		if (textDraw != "") {

			s32 maxW = widthDraw - (GUI_TOGGLE_BUTTON_TEXT_HOR_SPACING * 2);
			if (sprIcon != nullptr) maxW -= (sprIcon->sizeX + GUI_TOGGLE_BUTTON_ICON_HOR_SPACING);
			maxW = Math::maxInt(0, maxW);

			if (renderer->getStringWidth(fntText, textDraw) > maxW) {

				textDraw += "...";

				while (textDraw.length() > 3 && renderer->getStringWidth(fntText, textDraw) > maxW)
					textDraw.erase(textDraw.begin() + textDraw.size() - 4);

			}

			textX = 0;
			textY = 0;

			if (textHAlign == TextAlign::Right) {
				textX = widthDraw - 1 - GUI_TOGGLE_BUTTON_TEXT_HOR_SPACING;
			}
			else if (textHAlign == TextAlign::Center) {
				textX = (widthDraw / 2);
				if (sprIcon != nullptr) textX += (sprIcon->sizeX / 2) + (GUI_TOGGLE_BUTTON_ICON_HOR_SPACING / 2);
			}
			else {
				textX = GUI_TOGGLE_BUTTON_TEXT_HOR_SPACING;
				if (sprIcon != nullptr) textX += (sprIcon->sizeX + GUI_TOGGLE_BUTTON_ICON_HOR_SPACING);
			}

			if (textVAlign == TextAlign::Bottom) {

				s32 textHeight = renderer->getStringHeight(fntText, textDraw);

				if (sprIcon != nullptr && sprIcon->sizeY > textHeight)
					textY = heightDraw - 1 - GUI_TOGGLE_BUTTON_TEXT_VER_SPACING - (sprIcon->sizeY / 2) + (textHeight / 2);
				else
					textY = heightDraw - 1 - GUI_TOGGLE_BUTTON_TEXT_VER_SPACING;
			}
			else if (textVAlign == TextAlign::Middle) {
				textY = (heightDraw / 2);
			}
			else {

				s32 textHeight = renderer->getStringHeight(fntText, textDraw);

				if (sprIcon != nullptr && sprIcon->sizeY > textHeight)
					textY = GUI_TOGGLE_BUTTON_TEXT_VER_SPACING + (sprIcon->sizeY / 2) - (textHeight / 2);
				else
					textY = GUI_TOGGLE_BUTTON_TEXT_VER_SPACING;
			}

		}

		if (sprIcon != nullptr) {

			iconX = 0;
			iconY = 0;

			if (textHAlign == TextAlign::Right) {

				iconX += (widthDraw - sprIcon->sizeX - GUI_TOGGLE_BUTTON_TEXT_HOR_SPACING);
				if (textDraw != "") iconX -= (renderer->getStringWidth(fntText, textDraw) + GUI_TOGGLE_BUTTON_ICON_HOR_SPACING);

			}
			else if (textHAlign == TextAlign::Center) {

				iconX += (widthDraw / 2) - (sprIcon->sizeX / 2);
				if (textDraw != "") iconX -= (renderer->getStringWidth(fntText, textDraw) / 2) + (GUI_TOGGLE_BUTTON_ICON_HOR_SPACING / 2);

			}
			else iconX += GUI_TOGGLE_BUTTON_TEXT_HOR_SPACING;

			if (textVAlign == TextAlign::Bottom) {

				s32 textHeight = renderer->getStringHeight(fntText, textDraw);

				iconY += heightDraw - sprIcon->sizeY - GUI_TOGGLE_BUTTON_TEXT_VER_SPACING;
				if (textHeight > sprIcon->sizeY) iconY -= (textHeight / 2) - (sprIcon->sizeY / 2);

			}
			else if (textVAlign == TextAlign::Middle) iconY += (heightDraw / 2) - (sprIcon->sizeY / 2);
			else {

				s32 textHeight = renderer->getStringHeight(fntText, textDraw);

				iconY = GUI_TOGGLE_BUTTON_TEXT_VER_SPACING;
				if (textHeight > sprIcon->sizeY) iconY += (textHeight / 2) - (sprIcon->sizeY / 2);

			}

		}

	}

	void ToggleButton::onResize(s32 prevWidth, s32 prevHeight) {
		updateTextDisplay();
	}

	void ToggleButton::update(f32 dt) {

		if (canInteract()) {

			if (gui->widgetSelected != this) {

				if (!inShortcutMode()) {

					if (gui->getWidgetMouseOver() == this) {

						if(showTooltip) gui->setTooltip(this, tooltip);

						ind = 1;

						if (gui->isMousePressed(MOUSE_LEFT)) {

							gui->setWidgetSelected(this);
							pressed = true;

							ind = 2;
							showTooltip = false;

							toggled = !toggled;

							if (callback) {
								callback(this, toggled);
							}

						}

					}
					else {
						ind = toggled ? 2 : 0;
						showTooltip = true;
					}

				}

			}
			else {

				if (!gui->isMouseDown(MOUSE_LEFT)) {

					gui->deselectWidget(this);
					pressed = false;

					if (gui->getWidgetMouseOver() == this) {
						ind = 1;
					}
					else {
						ind = toggled ? 2 : 0;
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

			ind = (!enabledGlobal ? 3 : (toggled ? 2 : 0));
			showTooltip = true;

			pressed = false;

		}

	}

	void ToggleButton::render() {

		if(ind != 1 || !toggled) renderer->render9P(sprToggleButton, ind, x, y, widthDraw, heightDraw);
		else if(toggled) renderer->render9P(sprToggleButton, 2, x, y, widthDraw, heightDraw);

		renderer->cutStart(x + GUI_TOGGLE_BUTTON_TEXT_HOR_SPACING, y + GUI_TOGGLE_BUTTON_TEXT_VER_SPACING, widthDraw - (GUI_TOGGLE_BUTTON_TEXT_HOR_SPACING * 2), heightDraw - (GUI_TOGGLE_BUTTON_TEXT_VER_SPACING * 2));

		if(textDraw != ""){

			Color4f textCol;
			if (enabledGlobal) textCol = GUI_TOGGLE_BUTTON_TEXT_COL; else textCol = GUI_TOGGLE_BUTTON_TEXT_COL_DISABLED;
			
			renderer->renderText(fntText, textDraw, x + textX, y + textY + (s32)pressed - 1, textHAlign, textVAlign, textCol);

		}

		if (sprIcon != nullptr) {
			renderer->renderSprite(sprIcon, iconSub, x + iconX, y + iconY + (s32)pressed, enabledGlobal ? 1.0f : GUI_TOGGLE_BUTTON_ICON_DISABLED_ALPHA);
		}

		renderer->cutEnd();

	}

	bool ToggleButton::onShortcutInteract() {

		pressed = true;

		ind = 2;
		showTooltip = false;

		toggled = !toggled;

		if (callback) {
			callback(this, toggled);
		}

		return true;

	}

	void ToggleButton::onShortcutInteractEnd() {

		pressed = false;
		
		if (gui->getWidgetMouseOver() == this) {
			ind = 1;
		}
		else {
			ind = toggled ? 2 : 0;
		}

	}

	bool ToggleButton::isToggled() {
		return toggled;
	}

	void ToggleButton::setToggled(bool _toggled, bool _callCallback) {

		if (toggled == _toggled) return;

		toggled = _toggled;

		if (_callCallback && callback) {
			callback(this, toggled);
		}

	}

	void ToggleButton::toggle() {

		toggled = !toggled;

		if (callback) {
			callback(this, toggled);
		}

	}

}