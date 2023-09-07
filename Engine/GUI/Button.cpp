#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/GUI/Button.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/GUIMacros.h"

namespace Zixel {

	Button::Button(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::Button;

		sprButton = renderer->getTextureAtlasSprite(GUI_BUTTON_SPR);
		fntText = renderer->getTextureAtlasFont(GUI_BUTTON_FONT);

		textHAlign = TextAlign::Center;
		textVAlign = TextAlign::Middle;

		LocalShortcut shortcut;
		shortcut.keyCode = KEY_ENTER;
		shortcut.callOnEndImplicitly = false;
		shortcut.onCallback = GUI_SHORTCUT_CALLBACK(Button::onShortcutInteract);
		shortcut.onEnd = GUI_SHORTCUT_CALLBACK(Button::onShortcutInteractEnd);
		
		addLocalShortcut(shortcut);

	}

	void Button::setCallback(std::function<void(Button*)> _callback) {
		callback = _callback;
	}

	void Button::setText(std::string _text) {

		text = _text;
		updateTextDisplay();

	}

	void Button::setIcon(Sprite* _sprIcon, u8 _iconSub) {

		sprIcon = _sprIcon;
		iconSub = _iconSub;

		updateTextDisplay();

	}

	std::string Button::getText() {
		return text;
	}

	void Button::updateTextDisplay() {

		textDraw = text;

		if (textDraw != "") {

			s32 maxW = widthDraw - (GUI_BUTTON_TEXT_HOR_SPACING * 2);
			if (sprIcon != nullptr) maxW -= (sprIcon->sizeX + GUI_BUTTON_ICON_HOR_SPACING);
			maxW = Math::maxInt(0, maxW);

			if (renderer->getStringWidth(fntText, textDraw) > maxW) {

				textDraw += "...";

				while (textDraw.length() > 3 && renderer->getStringWidth(fntText, textDraw) > maxW)
					textDraw.erase(textDraw.begin() + textDraw.size() - 4);

			}

			textX = 0;
			textY = 0;

			if (textHAlign == TextAlign::Right) {
				textX = widthDraw - 1 - GUI_BUTTON_TEXT_HOR_SPACING;
			}
			else if (textHAlign == TextAlign::Center) {
				textX = (widthDraw / 2);
				if (sprIcon != nullptr) textX += (sprIcon->sizeX / 2) + (GUI_BUTTON_ICON_HOR_SPACING / 2);
			}
			else {
				textX = GUI_BUTTON_TEXT_HOR_SPACING;
				if (sprIcon != nullptr) textX += (sprIcon->sizeX + GUI_BUTTON_ICON_HOR_SPACING);
			}

			if (textVAlign == TextAlign::Bottom) {

				s32 textHeight = renderer->getStringHeight(fntText, textDraw);

				if (sprIcon != nullptr && sprIcon->sizeY > textHeight)
					textY = heightDraw - 1 - GUI_BUTTON_TEXT_VER_SPACING - (sprIcon->sizeY / 2) + (textHeight / 2);
				else
					textY = heightDraw - 1 - GUI_BUTTON_TEXT_VER_SPACING;
			}
			else if (textVAlign == TextAlign::Middle) {
				textY = (heightDraw / 2);
			}
			else {

				s32 textHeight = renderer->getStringHeight(fntText, textDraw);

				if (sprIcon != nullptr && sprIcon->sizeY > textHeight)
					textY = GUI_BUTTON_TEXT_VER_SPACING + (sprIcon->sizeY / 2) - (textHeight / 2);
				else
					textY = GUI_BUTTON_TEXT_VER_SPACING;
			}

		}

		if (sprIcon != nullptr) {

			iconX = 0;
			iconY = 0;

			if (textHAlign == TextAlign::Right) {

				iconX += (widthDraw - sprIcon->sizeX - GUI_BUTTON_TEXT_HOR_SPACING);
				if (textDraw != "") iconX -= (renderer->getStringWidth(fntText, textDraw) + GUI_BUTTON_ICON_HOR_SPACING);

			}
			else if (textHAlign == TextAlign::Center) {

				iconX += (widthDraw / 2) - (sprIcon->sizeX / 2);
				if (textDraw != "") iconX -= (renderer->getStringWidth(fntText, textDraw) / 2) + (GUI_BUTTON_ICON_HOR_SPACING / 2);

			}
			else iconX += GUI_BUTTON_TEXT_HOR_SPACING;

			if (textVAlign == TextAlign::Bottom) {

				s32 textHeight = renderer->getStringHeight(fntText, textDraw);

				iconY += heightDraw - sprIcon->sizeY - GUI_BUTTON_TEXT_VER_SPACING;
				if (textHeight > sprIcon->sizeY) iconY -= (textHeight / 2) - (sprIcon->sizeY / 2);

			}
			else if (textVAlign == TextAlign::Middle) iconY += (heightDraw / 2) - (sprIcon->sizeY / 2);
			else {

				s32 textHeight = renderer->getStringHeight(fntText, textDraw);

				iconY = GUI_BUTTON_TEXT_VER_SPACING;
				if (textHeight > sprIcon->sizeY) iconY += (textHeight / 2) - (sprIcon->sizeY / 2);

			}

		}

	}

	void Button::onResize(s32 prevWidth, s32 prevHeight) {
		updateTextDisplay();
	}

	void Button::update(f32 dt) {

		if (canInteract()) {

			if (gui->widgetSelected != this) {

				if (!inShortcutMode()) {

					if (gui->getWidgetMouseOver() == this) {

						if(showTooltip) gui->setTooltip(this, tooltip);

						ind = 1;

						if (gui->isMousePressed(MOUSE_LEFT)) {

							gui->setWidgetSelected(this);
							ind = 2;
							showTooltip = false;

						}

					}
					else {
						ind = 0;
						showTooltip = true;
					}

				}

			}
			else {

				if (!gui->isMouseDown(MOUSE_LEFT)) {

					gui->deselectWidget(this);

					if (gui->getWidgetMouseOver() == this) {

						ind = 1;

						if (callback) {
							callback(this);
						}

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

			ind = (!enabledGlobal ? 3 : 0);
			showTooltip = true;

		}

	}

	void Button::render() {

		if((ind != 0 || renderIdleBackground) && (ind != 3 || renderDisabledBackground)) renderer->render9P(sprButton, ind, x, y, widthDraw, heightDraw);

		renderer->cutStart(x + GUI_BUTTON_TEXT_HOR_SPACING, y + GUI_BUTTON_TEXT_VER_SPACING, widthDraw - (GUI_BUTTON_TEXT_HOR_SPACING * 2), heightDraw - (GUI_BUTTON_TEXT_VER_SPACING * 2));

		if(textDraw != ""){

			Color4f textCol;
			if (enabledGlobal) textCol = GUI_BUTTON_TEXT_COL; else textCol = GUI_BUTTON_TEXT_COL_DISABLED;
			
			renderer->renderText(fntText, textDraw, x + textX, y + textY + (ind == 2) - 1, textHAlign, textVAlign, textCol);

		}

		if (sprIcon != nullptr) {
			renderer->renderSprite(sprIcon, iconSub, x + iconX, y + iconY + (ind == 2), enabledGlobal ? 1.0f : GUI_BUTTON_ICON_DISABLED_ALPHA);
		}

		renderer->cutEnd();

	}

	bool Button::onShortcutInteract() {

		ind = 2;
		showTooltip = false;

		return true;

	}

	void Button::onShortcutInteractEnd() {
		
		if (ind == 2 && callback) {
			callback(this);
		}

		ind = (gui->getWidgetMouseOver() == this);

	}

}