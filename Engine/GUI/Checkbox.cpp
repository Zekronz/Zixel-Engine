/*
    Checkbox.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/GUI/Checkbox.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/GUIMacros.h"

namespace Zixel {

	Checkbox::Checkbox(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::Checkbox;

		sprCheckbox = renderer->getTextureAtlasSprite(GUI_CHECKBOX_SPR_BACKGROUND);
		sprCheckboxIcon = renderer->getTextureAtlasSprite(GUI_CHECKBOX_SPR_ICON);
		fntText = renderer->getTextureAtlasFont(GUI_CHECKBOX_FONT);

		renderFocus = false;

		width = GUI_CHECKBOX_DEFAULT_SIZE;
		height = GUI_CHECKBOX_DEFAULT_SIZE;

		LocalShortcut shortcut;
		shortcut.keyCode = KEY_ENTER;
		shortcut.onCallback = GUI_SHORTCUT_CALLBACK(Checkbox::onShortcutInteract);
		
		addLocalShortcut(shortcut);

	}

	void Checkbox::onResize(s32 prevWidth, s32 prevHeight) {
		updateTextDisplay();
	}

	void Checkbox::update(f32 dt) {

		if (canInteract()) {

			if (gui->widgetSelected != this) {

				if (gui->getWidgetMouseOver() == this) {

					ind = 1;

					if (gui->isMousePressed(MOUSE_LEFT)) {

						toggled = !toggled;
						if (callback) callback(this, toggled);

						gui->setWidgetFocused(this);

					}

				}
				else {
					ind = 0;
				}

			}

		}
		else {

			gui->deselectWidget(this);
			ind = 0;

		}

	}

	void Checkbox::render() {

		renderer->cutStart(x, y, widthDraw, heightDraw);

		s32 boxSize = Math::minInt(widthDraw, heightDraw);
		s32 boxX = 0, boxY = y + (heightDraw / 2) - (boxSize / 2);
		s32 textX = 0;

		if (textSide == CheckboxTextSide::Left) {

			boxX = x + widthDraw - boxSize;
			textX = x;

		}
		else {

			boxX = x;
			textX = Math::maxInt(x + boxSize + GUI_CHECKBOX_TEXT_SPACING, x + widthDraw - renderer->getStringWidth(fntText, text));

		}

		renderer->render9P(sprCheckbox, enabledGlobal ? ind : 2, boxX, boxY, boxSize, boxSize);

		if (toggled) renderer->renderSprite(sprCheckboxIcon, (s32)(!enabledGlobal), boxX + (boxSize / 2) - (sprCheckboxIcon->sizeX / 2) + GUI_CHECKBOX_ICON_HOR_OFFSET, boxY + (boxSize / 2) - (sprCheckboxIcon->sizeY / 2) + GUI_CHECKBOX_ICON_VER_OFFSET);

		if (!textDisplay.empty()) {

			Color4f textCol = GUI_CHECKBOX_TEXT_COL;
			if (!enabledGlobal) textCol = GUI_CHECKBOX_TEXT_COL_DISABLED;
			renderer->renderText(fntText, textDisplay, textX + GUI_CHECKBOX_TEXT_HOR_OFFSET, boxY + (boxSize / 2) + GUI_CHECKBOX_TEXT_VER_OFFSET, TextAlign::Left, TextAlign::Middle, textCol);

		}

		if (gui->widgetFocused == this) renderer->render9P(sprFocus, 0, boxX, boxY, boxSize, boxSize);

		renderer->cutEnd();

	}

	void Checkbox::updateTextDisplay() {

		textDisplay = text;

		if (!textDisplay.empty()) {

			s32 boxSize = Math::minInt(widthDraw, heightDraw);
			s32 textBBox = Math::maxInt(0, widthDraw - boxSize - GUI_CHECKBOX_TEXT_SPACING);

			if (renderer->getStringWidth(fntText, textDisplay) > textBBox) {
				textDisplay += "...";
			}

			while (textDisplay.length() > 3 && renderer->getStringWidth(fntText, textDisplay) > textBBox) {
				textDisplay.erase(textDisplay.end() - 4);
			}

		}

	}

	bool Checkbox::onShortcutInteract() {

		toggled = !toggled;
		if (callback) callback(this, toggled);

		return false;

	}

	void Checkbox::setCallback(std::function<void(Checkbox*, bool)> _callback) {
		callback = _callback;
	}

	bool Checkbox::isToggled() {
		return toggled;
	}

	void Checkbox::setToggled(bool _toggled, bool _callCallback) {

		if (toggled == _toggled) return;

		toggled = _toggled;
		if (_callCallback && callback) callback(this, toggled);

	}

	void Checkbox::setText(std::string _text) {

		if (text == _text) return;

		text = _text;
		updateTextDisplay();

	}

	void Checkbox::setTextSide(CheckboxTextSide _textSide) {
		textSide = _textSide;
	}

}