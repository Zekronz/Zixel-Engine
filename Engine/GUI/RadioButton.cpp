#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/GUI/RadioButton.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/GUIMacros.h"

namespace Zixel {

	struct RadioButtonGroup {
		s32 groupId = -1;
		std::vector<RadioButton*> radioButtons;
	};

	static std::vector<RadioButtonGroup> radioButtonGroups;

	RadioButtonGroup* findRadioButtonGroup(s32 _groupId) {
		for (RadioButtonGroup& group : radioButtonGroups) {
			if (group.groupId == _groupId) return &group;
		}

		return nullptr;
	}

	RadioButton::RadioButton(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::RadioButton;

		sprRadioButton = renderer->getTextureAtlasSprite(GUI_RADIO_BUTTON_SPR_BACKGROUND);
		sprRadioButtonIcon = renderer->getTextureAtlasSprite(GUI_RADIO_BUTTON_SPR_ICON);
		sprRadioButtonFocus = renderer->getTextureAtlasSprite(GUI_RADIO_BUTTON_SPR_FOCUS);
		fntText = renderer->getTextureAtlasFont(GUI_RADIO_BUTTON_FONT);

		renderFocus = false;

		width = GUI_RADIO_BUTTON_DEFAULT_SIZE;
		height = GUI_RADIO_BUTTON_DEFAULT_SIZE;

		LocalShortcut shortcut;
		shortcut.keyCode = KEY_ENTER;
		shortcut.onCallback = GUI_SHORTCUT_CALLBACK(RadioButton::onShortcutInteract);
		
		addLocalShortcut(shortcut);

	}

	void RadioButton::onResize(s32 prevWidth, s32 prevHeight) {
		updateTextDisplay();
	}

	void RadioButton::update(f32 dt) {

		if (canInteract()) {

			if (gui->widgetSelected != this) {

				if (gui->getWidgetMouseOver() == this) {

					ind = 1;

					if (gui->isMousePressed(MOUSE_LEFT)) {

						if (!toggled) toggle();
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

	void RadioButton::render() {

		renderer->cutStart(x, y, widthDraw, heightDraw);

		s32 boxSize = Math::minInt(widthDraw, heightDraw);
		s32 boxX = 0, boxY = y + (heightDraw / 2) - (boxSize / 2);
		s32 textX = 0;

		if (textSide == RadioButtonTextSide::Left) {

			boxX = x + widthDraw - boxSize;
			textX = x;

		}
		else {

			boxX = x;
			textX = Math::maxInt(x + boxSize + GUI_RADIO_BUTTON_TEXT_SPACING, x + widthDraw - renderer->getStringWidth(fntText, text));

		}

		renderer->render9P(sprRadioButton, enabledGlobal ? ind : 2, boxX, boxY, boxSize, boxSize);

		if (toggled) renderer->renderSprite(sprRadioButtonIcon, (s32)(!enabledGlobal), boxX + (boxSize / 2) - (sprRadioButtonIcon->sizeX / 2) + GUI_RADIO_BUTTON_ICON_HOR_OFFSET, boxY + (boxSize / 2) - (sprRadioButtonIcon->sizeY / 2) + GUI_RADIO_BUTTON_ICON_VER_OFFSET);

		if (!textDisplay.empty()) {

			Color4f textCol = GUI_RADIO_BUTTON_TEXT_COL;
			if (!enabledGlobal) textCol = GUI_RADIO_BUTTON_TEXT_COL_DISABLED;
			renderer->renderText(fntText, textDisplay, textX + GUI_RADIO_BUTTON_TEXT_HOR_OFFSET, boxY + (boxSize / 2) + GUI_RADIO_BUTTON_TEXT_VER_OFFSET, TextAlign::Left, TextAlign::Middle, textCol);

		}

		if (gui->widgetFocused == this) renderer->renderSprite(sprRadioButtonFocus, 0, boxX, boxY);

		renderer->cutEnd();

	}

	void RadioButton::updateTextDisplay() {

		textDisplay = text;

		if (!textDisplay.empty()) {

			s32 boxSize = Math::minInt(widthDraw, heightDraw);
			s32 textBBox = Math::maxInt(0, widthDraw - boxSize - GUI_RADIO_BUTTON_TEXT_SPACING);

			if (renderer->getStringWidth(fntText, textDisplay) > textBBox) {
				textDisplay += "...";
			}

			while (textDisplay.length() > 3 && renderer->getStringWidth(fntText, textDisplay) > textBBox) {
				textDisplay.erase(textDisplay.end() - 4);
			}

		}

	}

	bool RadioButton::onShortcutInteract() {

		if (!toggled) toggle();
		return false;

	}

	void RadioButton::setCallback(std::function<void(RadioButton*, s32)> _callback) {
		callback = _callback;
	}

	bool RadioButton::isToggled() {
		return toggled;
	}

	void RadioButton::toggle(bool _callCallback) {
		if (toggled) return;

		if (groupId != -1) {
			RadioButtonGroup* group = findRadioButtonGroup(groupId);

			if (group != nullptr) {
				for (RadioButton* radioButton : group->radioButtons) {
					if (radioButton->toggled) {
						radioButton->toggled = false;
						break;
					}
				}
			}
		}

		toggled = true;
		if (_callCallback && callback) callback(this, groupId);

	}

	void RadioButton::setGroupId(s32 _groupId) {
		if (_groupId < 0) _groupId = -1;
		if (_groupId == groupId) return;
		
		//Go through old group.
		if (groupId != -1) {
			RadioButtonGroup* group = findRadioButtonGroup(groupId);
			
			//Remove radio button from old group.
			const auto& it = std::find(group->radioButtons.begin(), group->radioButtons.end(), this);
			if (it != group->radioButtons.end()) group->radioButtons.erase(it);

			//Delete group if no radio buttons are left.
			if (group->radioButtons.size() == 0) {
				size_t ind = 0;
				for (const RadioButtonGroup& tempGroup : radioButtonGroups) {
					if (tempGroup.groupId == group->groupId) {
						radioButtonGroups.erase(radioButtonGroups.begin() + ind);
						break;
					}
					++ind;
				}
			}
			else {

				//Set new toggled radio button if we were toggled.
				if (toggled) {
					group->radioButtons[0]->toggle();
				}

			}
		}

		toggled = false;
		groupId = _groupId;

		if (groupId != -1) {
			RadioButtonGroup* group = findRadioButtonGroup(groupId);
			
			if (group == nullptr) {
				RadioButtonGroup newGroup;
				newGroup.groupId = groupId;

				radioButtonGroups.push_back(newGroup);
				group = findRadioButtonGroup(groupId);
			}

			group->radioButtons.push_back(this);

			if (group->radioButtons.size() == 1) {
				toggle();
			}
		}
	}

	void RadioButton::setText(std::string _text) {

		if (text == _text) return;

		text = _text;
		updateTextDisplay();

	}

	void RadioButton::setTextSide(RadioButtonTextSide _textSide) {
		textSide = _textSide;
	}

}