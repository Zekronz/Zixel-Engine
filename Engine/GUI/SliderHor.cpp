/*
    SliderHor.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/GUI/SliderHor.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/GUIMacros.h"

namespace Zixel {

	SliderHor::SliderHor(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::SliderHor;

		sprSliderHor = renderer->getTextureAtlasSprite(GUI_SLIDER_HOR_SPR);
		fntText = renderer->getTextureAtlasFont(GUI_SLIDER_HOR_FONT);

		height = sprSliderHor->sizeY;

		valCur = ((valMax - valMin) * percentage) + valMin;
		if (valRound) valCur = roundf(valCur);

		valStart = valCur;

		updateValueText();

		LocalShortcut shortcutDecrease;
		shortcutDecrease.keyCode = KEY_LEFT;
		shortcutDecrease.onStart = GUI_SHORTCUT_CALLBACK(SliderHor::onStartShortcutDecrease);
		shortcutDecrease.onCallback = GUI_SHORTCUT_CALLBACK(SliderHor::onShortcutDecrease);
		shortcutDecrease.onEnd = GUI_SHORTCUT_CALLBACK(SliderHor::onEndShortcut);
		shortcutDecrease.repeatable = true;

		LocalShortcut shortcutIncrease;
		shortcutIncrease.keyCode = KEY_RIGHT;
		shortcutIncrease.onStart = GUI_SHORTCUT_CALLBACK(SliderHor::onStartShortcutIncrease);
		shortcutIncrease.onCallback = GUI_SHORTCUT_CALLBACK(SliderHor::onShortcutIncrease);
		shortcutIncrease.onEnd = GUI_SHORTCUT_CALLBACK(SliderHor::onEndShortcut);
		shortcutIncrease.repeatable = true;

		addLocalShortcut(shortcutDecrease);
		addLocalShortcut(shortcutIncrease);

	}

	void SliderHor::updateValueText() {

		if (!valRound) StringHelper::toString(valText, valCur, decimalPlaces); else valText = std::to_string((s32)valCur);
		if (displayAsPercentage) valText += "%";

		checkValueTextBounds();

	}

	void SliderHor::checkValueTextBounds() {

		valTextDisplay = valText;

		s32 i = (s32)valTextDisplay.length() - 1 - displayAsPercentage;
		bool addDots = false;

		while (renderer->getStringWidth(fntText, valTextDisplay) > (widthDraw - (GUI_SLIDER_HOR_TEXT_SPACING * 2)) && i > 0) {

			if (!addDots) {

				addDots = true;
				valTextDisplay.insert((size_t)i + 1, "...");

			}

			valTextDisplay.erase(i, 1);
			--i;

		}

	}

	void SliderHor::onResize(s32 prevWidth, s32 prevHeight) {
		checkValueTextBounds();
	}

	void SliderHor::update(f32 dt) {

		if (canInteract()) {

			if (gui->widgetSelected != this) {

				if (!inShortcutMode()) {

					if (gui->getWidgetMouseOver() == this) {

						ind = 1;

						if (gui->isMousePressed(MOUSE_LEFT)) {

							gui->setWidgetSelected(this);

							percentage = Math::clampFloat(Math::maxFloat(0.0f, (f32)(gui->mouseX - x)) / (f32)widthDraw, 0.0f, 1.0f);

							valStart = valCur;
							valCur = ((valMax - valMin) * percentage) + valMin;

							if (valRound) {
								valCur = roundf(valCur);
							}

							if (valCur != valStart) {

								updateValueText();

								if (onValueChange) {
									onValueChange(this, valCur);
								}

							}

						}

					}
					else {
						ind = 0;
					}

				}

			}
			else {

				ind = 1;

				percentage = Math::clampFloat(Math::maxFloat(0.0f, (f32)(gui->mouseX - x)) / (f32)widthDraw, 0.0f, 1.0f);

				f32 prevVal = valCur;

				valCur = ((valMax - valMin) * percentage) + valMin;
				if (valRound) valCur = roundf(valCur);

				if (valCur != prevVal) {

					updateValueText();

					if (onValueChange) {
						onValueChange(this, valCur);
					}

				}

				if (!gui->isMouseDown(MOUSE_LEFT)) {

					gui->deselectWidget(this);

					ind = Math::pointInRect(gui->mouseX, gui->mouseY, x, y, widthDraw, height);

					if (valStart != valCur && onValueSet) {
						onValueSet(this, valStart, valCur);
					}

				}

			}

		}
		else {

			gui->deselectWidget(this);

			ind = 0;

		}

	}

	void SliderHor::render() {
		
		renderer->render3PHor(sprSliderHor, !enabledGlobal, x, y, widthDraw);

		Color4f textCol;

		if (enabledGlobal) {

			renderer->render3PHor(sprSliderHor, 2 + ind, x, y, (s32)roundf((f32)widthDraw * percentage));
			textCol = GUI_SLIDER_HOR_TEXT_COL;

		}
		else {
			textCol = GUI_SLIDER_HOR_TEXT_COL_DISABLED;
		}

		renderer->cutStart(x, y, widthDraw, height);
		renderer->renderText(fntText, valTextDisplay, x + (widthDraw / 2) + GUI_SLIDER_HOR_TEXT_HOR_OFFSET, y + (height / 2) + GUI_SLIDER_HOR_TEXT_VER_OFFSET, TextAlign::Center, TextAlign::Middle, textCol);
		renderer->cutEnd();

	}

	bool SliderHor::onStartShortcutDecrease() {

		if (percentage > 0.0f) {

			valStart = valCur;
			return true;

		}

		return false;

	}

	bool SliderHor::onShortcutDecrease() {

		if (percentage > 0.0f) {

			percentage = Math::maxFloat(0.0f, percentage - 0.1f);

			f32 prevVal = valCur;
			valCur = ((valMax - valMin) * percentage) + valMin;

			if (valRound) {
				valCur = roundf(valCur);
			}

			if (valCur != prevVal) {

				updateValueText();

				if (onValueChange) {
					onValueChange(this, valCur);
				}

			}

		}
		
		return (percentage > 0.0f);

	}

	bool SliderHor::onStartShortcutIncrease() {

		if (percentage < 1.0f) {

			valStart = valCur;
			return true;

		}

		return false;

	}

	bool SliderHor::onShortcutIncrease() {

		if (percentage < 1.0f) {

			percentage = Math::minFloat(1.0f, percentage + 0.1f);

			f32 prevVal = valCur;
			valCur = ((valMax - valMin) * percentage) + valMin;

			if (valRound) {
				valCur = roundf(valCur);
			}

			if (valCur != prevVal) {

				updateValueText();

				if (onValueChange) {
					onValueChange(this, valCur);
				}

			}

		}
		
		return (percentage < 1.0f);

	}

	void SliderHor::onEndShortcut() {

		if (valStart != valCur) {

			if (onValueSet) {
				onValueSet(this, valStart, valCur);
			}

		}

	}

	f32 SliderHor::valueToPercentage(f32 _value) {
		return (Math::clampFloat(_value, valMin, valMax) - valMin) / (valMax - valMin);
	}

	void SliderHor::setValue(f32 _value, bool _callOnValueChange) {

		f32 prevVal = valCur;

		if (valRound) _value = roundf(_value);
		valCur = Math::clampFloat(_value, valMin, valMax);
		
		percentage = (valCur - valMin) / (valMax - valMin);
		
		if (valCur != prevVal) {
			
			updateValueText();

			if (_callOnValueChange && onValueChange) {
				onValueChange(this, valCur);
			}

		}

	}

	void SliderHor::setRange(f32 _minValue, f32 _maxValue) {

		valMin = _minValue;
		valMax = _maxValue;

		if (valMin > valMax) {

			valMin = _maxValue;
			valMax = _minValue;

		}

		setValue(Math::clampFloat(valCur, valMin, valMax));

	}

	void SliderHor::setPercentage(f32 _percentage, bool _callOnValueChange) {

		percentage = Math::clampFloat(_percentage, 0.0f, 1.0f);

		f32 prevVal = valCur;

		valCur = ((valMax - valMin) * percentage) + valMin;

		if (valRound) {
			valCur = roundf(valCur);
		}

		if (valCur != prevVal) {

			updateValueText();

			if (onValueChange && _callOnValueChange) {
				onValueChange(this, valCur);
			}

		}

	}

	f32 SliderHor::getPercentage() {
		return percentage;
	}

	void SliderHor::setOnValueChange(std::function<void(SliderHor*, f32)> _callback) {
		onValueChange = _callback;
	}

	void SliderHor::setOnValueSet(std::function<void(SliderHor*, f32, f32)> _callback) {
		onValueSet = _callback;
	}

}