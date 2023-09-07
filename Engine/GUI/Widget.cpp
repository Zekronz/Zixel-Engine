#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/GUI/Widget.h"
#include "Engine/GUI/WidgetIncludes.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/Window.h"

namespace Zixel {

	Widget::Widget(GUI* _gui, Widget* _parent, Window* _window) {

		gui = _gui;
		renderer = _gui->renderer;
		
		sprFocus = renderer->getTextureAtlasSprite("widgetFocus");
		sprScrollVer = renderer->getTextureAtlasSprite("scrollVer");
		sprScrollHor = renderer->getTextureAtlasSprite("scrollHor");
		sprScrollCorner = renderer->getTextureAtlasSprite("scrollCorner");

		gui->initWidget(this, _parent, _window);
		
	}

	Widget::~Widget() {

		for (s32 i = (s32)widgetList.size() - 1; i >= 0; --i) {
			destroyChild(widgetList[i]);
		}

	}

	void Widget::init(std::string& parameters) {

		setParameters(parameters);

		updateEnabledGlobal();
		updateVisibleGlobal();

		if (window != nullptr) {
			window->updateTransform(true, false);
		}
		else {

			Widget* root = getRootWidget(this);
			root->updateTransform(true, false);

		}

	}

	void Widget::destroyChild(Widget* child) {

		for (s32 i = (s32)widgetList.size() - 1; i >= 0; --i) {

			if (widgetList[i] == child) {

				delete child;
				widgetList.erase(widgetList.begin() + i);
				return;

			}

		}

	}

	Widget* Widget::getRootWidget(Widget* widget) {

		if (widget->parent != nullptr) {
			return getRootWidget(widget->parent);
		}

		return widget;

	}

	bool Widget::checkEnabledGlobal() {

		if (!enabled) return false;

		if (parent != nullptr) {
			return parent->checkEnabledGlobal();
		}
		else if (window != nullptr) {
			
			if (window->docked) {
				Widget* dockAreaWidget = (Widget*)window->dockTab->container->dockArea;
				return dockAreaWidget->checkEnabledGlobal();
			}

			return true;
		}

		return true;

	}

	bool Widget::checkVisibleGlobal() {

		if (!visible) return false;

		if (parent != nullptr) {
			return parent->checkVisibleGlobal();
		}
		else if (window != nullptr) {

			if (!window->isOpened()) return false;
			
			if (window->docked) {

				Widget* dockAreaWidget = (Widget*)window->dockTab->container->dockArea;
				return dockAreaWidget->checkVisibleGlobal();

			}

			return true;
		}

		return true;

	}

	void Widget::updateEnabledGlobal() {

		bool prev = enabledGlobal;
		enabledGlobal = checkEnabledGlobal();

		if (prev && !enabledGlobal) {

			gui->deselectWidget(this);
			gui->unfocusWidget(this);

		}

	}

	void Widget::updateVisibleGlobal() {

		bool prev = visibleGlobal;
		visibleGlobal = checkVisibleGlobal();

		if (prev && !visibleGlobal) {

			gui->deselectWidget(this);
			gui->unfocusWidget(this);

		}

	}

	void Widget::setEnabledGlobal(bool _enabledGlobal) {

		if (!_enabledGlobal) {

			gui->deselectWidget(this);
			gui->unfocusWidget(this);

			if (!enabledGlobal) {
				return;
			}

			enabledGlobal = false;

		}
		else {

			bool prev = enabledGlobal;

			bool windowEnabled = true;
			if (window != nullptr) {

				if (window->docked) {

					Widget* dockAreaWidget = (Widget*)window->dockTab->container->dockArea;
					if (!dockAreaWidget->enabledGlobal) {
						windowEnabled = false;
					}

				}

			}

			enabledGlobal = !(!enabled || (parent != nullptr && !parent->enabledGlobal) || !windowEnabled);

			if (!prev && !enabledGlobal) {
				return;
			}

		}

		for (Widget* child : widgetList) {
			child->setEnabledGlobal(enabledGlobal);
		}

	}

	void Widget::setVisibleGlobal(bool _visibleGlobal) {
		
		if (!_visibleGlobal) {

			gui->deselectWidget(this);
			gui->unfocusWidget(this);

			if (!visibleGlobal) {
				return;
			}

			visibleGlobal = false;

		}
		else {

			bool prev = visibleGlobal;

			bool windowVisible = true;
			if (window != nullptr) {

				if (!window->isOpened()) {
					windowVisible = false;
				}
				else if(window->docked) {

					Widget* dockAreaWidget = (Widget*)window->dockTab->container->dockArea;
					if (!dockAreaWidget->visibleGlobal) {
						windowVisible = false;
					}

				}

			}

			visibleGlobal = !(!visible || (parent != nullptr && !parent->visibleGlobal) || !windowVisible);

			if (!prev && !visibleGlobal) {
				return;
			}

		}

		for (Widget* child : widgetList) {
			child->setVisibleGlobal(visibleGlobal);
		}

	}

	void Widget::setEnabled(bool _enabled) {

		if (enabled != _enabled) {

			enabled = _enabled;

			if (!enabled) {

				enabledGlobal = false;

				gui->deselectWidget(this);
				gui->unfocusWidget(this);

			}
			else {
				updateEnabledGlobal();
			}

			for (Widget* child : widgetList) {
				child->setEnabledGlobal(enabledGlobal);
			}

		}

	}

	void Widget::setVisible(bool _visible) {

		if (visible != _visible) {

			visible = _visible;

			if (!visible) {

				visibleGlobal = false;

				gui->deselectWidget(this);
				gui->unfocusWidget(this);

			}
			else {
				updateVisibleGlobal();
			}

			for (Widget* child : widgetList) {
				child->setVisibleGlobal(visibleGlobal);
			}

			if (window != nullptr) {
				window->updateTransform(true, false);
			}
			else {

				Widget* root = getRootWidget(this);
				root->updateTransform(true, false);

			}

		}

	}

	void Widget::setGridColumn(s32 _column, bool _updateTransform) {
		gridColumn = _column;
		if(_updateTransform) updateTransform(true);
	}

	void Widget::setGridRow(s32 _row, bool _updateTransform) {
		gridRow = _row;
		if (_updateTransform) updateTransform(true);
	}

	void Widget::setGridColumnCount(s32 _count) {

		if (Math::maxInt(1, _count) != gridColumnCount) {

			gridColumnCount = Math::maxInt(1, _count);

			for (Widget* child : widgetList) {
				if (child->gridColumn >= _count) child->setGridColumn(0, false);
				child->updateTransform(true);
			}

		}

	}

	void Widget::setGridRowCount(s32 _count) {

		if (Math::maxInt(1, _count) != gridRowCount) {

			gridRowCount = Math::maxInt(1, _count);

			for (Widget* child : widgetList) {
				if (child->gridRow >= _count) child->setGridRow(0, false);
				child->updateTransform(true);
			}

		}
	}

	void Widget::setPadding(s32 _paddingLeft, s32 _paddingTop, s32 _paddingRight, s32 _paddingBottom) {

		if (_paddingLeft == paddingLeft && _paddingTop == paddingTop && _paddingRight == paddingRight && _paddingBottom == paddingBottom) return;

		paddingLeft = _paddingLeft;
		paddingTop = _paddingTop;
		paddingRight = _paddingRight;
		paddingBottom = _paddingBottom;

		if (window != nullptr) {
			window->updateTransform(true, false);
		}
		else {

			Widget* root = getRootWidget(this);
			root->updateTransform(true, false);

		}

	}

	void Widget::setWidth(s32 _width) {

		if (_width == width) return;

		width = _width;

		if (window != nullptr) {
			window->updateTransform(true, false);
		}
		else {

			Widget* root = getRootWidget(this);
			root->updateTransform(true, false);

		}

	}

	void Widget::setHeight(s32 _height) {

		if (_height == height) return;

		height = _height;

		if (window != nullptr) {
			window->updateTransform(true, false);
		}
		else {

			Widget* root = getRootWidget(this);
			root->updateTransform(true, false);

		}

	}

	f32 Widget::getMaxVerScrollOffset() {
		return Math::maxFloat(0.0f, (f32)(contentHeight - viewportHeight));
	}

	f32 Widget::getMaxHorScrollOffset() {
		return Math::maxFloat(0.0f, (f32)(contentWidth - viewportWidth));
	}

	void Widget::setInsideSize(s32 _width, s32 _height) {

		_width = Math::maxInt(0, _width);
		_height = Math::maxInt(0, _height);

		if (_width != insideWidth || _height != insideHeight) {

			insideWidth = _width;
			insideHeight = _height;

			calculateContentSize();

			bool showV = scrollVerShow;
			bool showH = scrollHorShow;

			updateScrollbars();

			if (showV != scrollVerShow || showH != scrollHorShow) {

				for (Widget* child : widgetList) {
					child->updateTransform(true);
				}

				calculateContentSize();
				updateScrollbars();
			}


		}

	}

	void Widget::setScrollOffset(f32 _horScroll, f32 _verScroll) {

		f32 verPrev = scrollVerOffset;
		f32 horPrev = scrollHorOffset;

		f32 maxVer = getMaxVerScrollOffset();
		f32 maxHor = getMaxHorScrollOffset();

		scrollVerOffset = Math::clampFloat(_verScroll, 0.0f, maxVer);
		scrollHorOffset = Math::clampFloat(_horScroll, 0.0f, maxHor);

		if (verPrev != scrollVerOffset || horPrev != scrollHorOffset) {

			scrollVerBarPos = (s32)((f32)scrollVerBarMaxPos * (scrollVerOffset / maxVer));
			scrollHorBarPos = (s32)((f32)scrollHorBarMaxPos * (scrollHorOffset / maxHor));

			for (Widget* child : widgetList) {
				child->updateTransform(true);
			}

		}

	}

	void Widget::setParameters(std::string& parameters) {

		bool updateAllGlobalWidgets = false;
		bool updateSliderHor = false;

		std::vector<std::string> pairs;
		StringHelper::split(pairs, parameters, ';');

		for (std::string& pair : pairs) {
			
			if (pair == "") continue;

			std::vector<std::string> keyValues;
			StringHelper::split(keyValues, pair, '=');

			if (keyValues.size() < 2) continue;

			std::string& key = keyValues[0];
			std::string& value = keyValues[1];

			StringHelper::trim(key);
			StringHelper::trim(value);

			if (key == "" || value == "") continue;

			if (value == "true") value = "1";
			else if (value == "false") value = "0";
			else if ((value.front() == '"' && value.back() == '"') || (value.front() == '\'' && value.back() == '\'')) StringHelper::crop(value, 1, value.size() - 2);

			if (key == "width") {

				StringHelper::toInt(value, width);

				if (width < 0) {

					width = 0;
					ZIXEL_WARN("Error in Widget::setParameters. \"width\" can't be less than 0.");

				}

				viewportWidth = width;

			}
			else if (key == "height") {

				StringHelper::toInt(value, height);

				if (height < 0) {

					height = 0;
					ZIXEL_WARN("Error in Widget::setParameters. \"height\" can't be less than 0.");

				}

				viewportHeight = height;

			}
			else if (key == "hAlign") {

				if (value == "left") hAlign = WidgetAlign::Left;
				else if (value == "center") hAlign = WidgetAlign::Center;
				else if (value == "right") hAlign = WidgetAlign::Right;
				else ZIXEL_WARN("Error in Widget::setParameters. Invalid \"hAlign\" value found: \"{}\".", value);

			}
			else if (key == "vAlign") {

				if (value == "top") vAlign = WidgetAlign::Top;
				else if (value == "middle") vAlign = WidgetAlign::Middle;
				else if (value == "bottom") vAlign  = WidgetAlign::Bottom;
				else ZIXEL_WARN("Error in Widget::setParameters. Invalid \"vAlign\" value found: \"{}\".", value);

			}
			else if (key == "fillX") {
				StringHelper::toBool(value, fillX);
			}
			else if (key == "fillY") {
				StringHelper::toBool(value, fillY);
			}
			else if (key == "marginTop") {
				StringHelper::toInt(value, marginTop);
			}
			else if (key == "marginBottom") {
				StringHelper::toInt(value, marginBottom);
			}
			else if (key == "marginLeft") {
				StringHelper::toInt(value, marginLeft);
			}
			else if (key == "marginRight") {
				StringHelper::toInt(value, marginRight);
			}
			else if (key == "paddingTop") {
				StringHelper::toInt(value, paddingTop);
			}
			else if (key == "paddingBottom") {
				StringHelper::toInt(value, paddingBottom);
			}
			else if (key == "paddingLeft") {
				StringHelper::toInt(value, paddingLeft);
			}
			else if (key == "paddingRight") {
				StringHelper::toInt(value, paddingRight);
			}
			else if (key == "cutContent") {
				StringHelper::toBool(value, cutContent);
			}
			else if (key == "scrollVerVisible") {
				StringHelper::toBool(value, scrollVerVisible);
			}
			else if (key == "scrollHorVisible") {
				StringHelper::toBool(value, scrollHorVisible);
			}
			else if (key == "canScrollVer") {
				StringHelper::toBool(value, canScrollVer);
			}
			else if (key == "canScrollHor") {
				StringHelper::toBool(value, canScrollHor);
			}
			else if (key == "canScrollWhileSelected") {
				StringHelper::toBool(value, canScrollWhileSelected);
			}
			else if (key == "enabled") {

				StringHelper::toBool(value, enabled);
				updateEnabledGlobal();

			}
			else if (key == "visible") {

				StringHelper::toBool(value, visible);
				updateVisibleGlobal();

			}
			else if (key == "gridLayout") {
				StringHelper::toBool(value, gridLayout);
			}
			else if (key == "gridColumnCount") {

				StringHelper::toInt(value, gridColumnCount);

				if (gridColumnCount <= 0) {

					gridColumnCount = 1;
					ZIXEL_WARN("Error in Widget::setParameters. \"gridColumnCount\" can't be less than 1.");

				}

			}
			else if (key == "gridRowCount") {

				StringHelper::toInt(value, gridRowCount);

				if (gridRowCount <= 0) {

					gridRowCount = 1;
					ZIXEL_WARN("Error in Widget::setParameters. \"gridRowCount\" can't be less than 1.");

				}

			}
			else if (key == "gridColumn") {

				StringHelper::toInt(value, gridColumn);
				
				if (gridColumn < 0) {

					gridColumn = 0;
					ZIXEL_WARN("Error in Widget::setParameters. \"gridColumn\" can't be less than 0.");

				}

				if (parent != nullptr) {
					if (gridColumn >= parent->gridColumnCount) parent->gridColumnCount = gridColumn + 1;
				}
				else if (window != nullptr) {
					if (gridColumn >= window->gridColumnCount) window->gridColumnCount = gridColumn + 1;
				}
				else {

					if (gridColumn >= gui->gridColumnCount) {

						gui->gridColumnCount = gridColumn + 1;
						updateAllGlobalWidgets = true;

					}

				}

			}
			else if (key == "gridRow") {

				StringHelper::toInt(value, gridRow);
				
				if (gridRow < 0) {

					gridRow = 0;
					ZIXEL_WARN("Error in Widget::setParameters. \"gridRow\" can't be less than 0.");

				}

				if (parent != nullptr) {
					if (gridRow >= parent->gridRowCount) parent->gridRowCount = gridRow + 1;
				}
				else if (window != nullptr) {
					if (gridRow >= window->gridRowCount) window->gridRowCount = gridRow + 1;
				}
				else {

					if (gridRow >= gui->gridRowCount) {

						gui->gridRowCount = gridRow + 1;
						updateAllGlobalWidgets = true;

					}

				}

			}
			else if (key == "tooltip") {
				tooltip = value;
			}
			else if (key == "includeWhenCalculatingParentContentWidth") {
				StringHelper::toBool(value, includeWhenCalculatingParentContentWidth);
			}
			else if (key == "includeWhenCalculatingParentContentHeight") {
				StringHelper::toBool(value, includeWhenCalculatingParentContentHeight);
			}

			if (type == WidgetType::Label) {

				Label* label = (Label*)this;

				if (key == "text") {
					label->setText(value);
				}
				else if (key == "textHAlign") {

					if (value == "right") label->textHAlign = TextAlign::Right;
					else if (value == "center") label->textHAlign = TextAlign::Center;
					else label->textHAlign = TextAlign::Left;

				}
				else if (key == "textVAlign") {

					if (value == "bottom") label->textVAlign = TextAlign::Bottom;
					else if (value == "middle") label->textVAlign = TextAlign::Middle;
					else label->textVAlign = TextAlign::Top;

				}
				else if (key == "font") {
					label->setFont(value);
				}
				else if (key == "extendLineBreakToParent") {

					bool extend;
					StringHelper::toBool(value, extend);

					label->setExtendLineBreakToParent(extend);

				}
				else if (key == "lineBreakWidth") {

					if (!label->extendLineBreakToParent) {

						s32 lineBreakWidth;
						StringHelper::toInt(value, lineBreakWidth);

						label->setLineBreakWidth(lineBreakWidth);

					}

				}
				else if (key == "lineBreakMargin") {

					s32 margin;
					StringHelper::toInt(value, margin);

					label->setLineBreakMargin(margin);

				}

			}
			else if (type == WidgetType::Button) {

				Button* button = (Button*)this;

				if (key == "text") {
					button->setText(value);
				}
				else if (key == "iconSprite") {

					Sprite* icon = renderer->getTextureAtlasSprite(value);
					button->setIcon(icon, button->iconSub);

				}
				else if (key == "iconSub") {

					s32 sub;
					StringHelper::toInt(value, sub);

					if (sub < 0) {

						ZIXEL_WARN("Error in Widget::setParameters. \"iconSub\" can't be less than 0.");
						sub = 0;

					}

					button->setIcon(button->sprIcon, sub);

				}
				else if (key == "renderIdleBackground") {
					StringHelper::toBool(value, button->renderIdleBackground);
				}
				else if (key == "renderDisabledBackground") {
					StringHelper::toBool(value, button->renderDisabledBackground);
				}

			}
			else if (type == WidgetType::MenuButton) {

				MenuButton* button = (MenuButton*)this;

				if (key == "text") {
					button->setText(value);
				}
				else if (key == "iconSprite") {

					Sprite* icon = renderer->getTextureAtlasSprite(value);
					button->setIcon(icon, button->iconSub);

				}
				else if (key == "iconSub") {

					s32 sub;
					StringHelper::toInt(value, sub);

					if (sub < 0) {

						ZIXEL_WARN("Error in Widget::setParameters. \"iconSub\" can't be less than 0.");
						sub = 0;

					}

					button->setIcon(button->sprIcon, sub);

				}
				else if (key == "renderIdleBackground") {
					StringHelper::toBool(value, button->renderIdleBackground);
				}
				else if (key == "renderDisabledBackground") {
					StringHelper::toBool(value, button->renderDisabledBackground);
				}

			}
			else if (type == WidgetType::ComboBox) {

				ComboBox* comboBox = (ComboBox*)this;

				if (key == "arrowAlignment") {

					if (value == "left") comboBox->setArrowAlignment(TextAlign::Left);
					else if (value == "center") comboBox->setArrowAlignment(TextAlign::Center);
					else comboBox->setArrowAlignment(TextAlign::Right);

				}
				else if (key == "renderText") {

					bool render;
					StringHelper::toBool(value, render);

					comboBox->setRenderText(render);

				}
				else if (key == "iconSprite") {

					Sprite* icon = renderer->getTextureAtlasSprite(value);
					comboBox->setIcon(icon, comboBox->iconSub);

				}
				else if (key == "iconSub") {

					s32 sub;
					StringHelper::toInt(value, sub);

					if (sub < 0) {

						ZIXEL_WARN("Error in Widget::setParameters. \"iconSub\" can't be less than 0.");
						sub = 0;

					}

					comboBox->setIcon(comboBox->sprIcon, sub);

				}
				else if (key == "renderIdleBackground") {
					StringHelper::toBool(value, comboBox->renderIdleBackground);
				}
				else if (key == "renderDisabledBackground") {
					StringHelper::toBool(value, comboBox->renderDisabledBackground);
				}

			}
			else if (type == WidgetType::Checkbox) {

				Checkbox* checkbox = (Checkbox*)this;

				if (key == "toggled") {

					bool toggle;
					StringHelper::toBool(value, toggle);

					checkbox->setToggled(toggle, true);

				}
				else if (key == "text") {
					checkbox->setText(value);
				}
				else if (key == "textSide") {

					if (value == "left") checkbox->setTextSide(CheckboxTextSide::Left);
					else if (value == "right") checkbox->setTextSide(CheckboxTextSide::Right);

				}

			}
			else if (type == WidgetType::RadioButton) {

				RadioButton* radioButton = (RadioButton*)this;

				if (key == "groupId") {
					s32 id;
					StringHelper::toInt(value, id);

					radioButton->setGroupId(id);
				}
				else if (key == "text") {
					radioButton->setText(value);
				}
				else if (key == "textSide") {

					if (value == "left") radioButton->setTextSide(RadioButtonTextSide::Left);
					else if (value == "right") radioButton->setTextSide(RadioButtonTextSide::Right);

				}

			}
			else if (type == WidgetType::SliderHor) {

				SliderHor* sliderHor = (SliderHor*)this;

				if (key == "minValue") {

					StringHelper::toFloat(value, sliderHor->valMin);
					updateSliderHor = true;

				}
				else if (key == "maxValue") {

					StringHelper::toFloat(value, sliderHor->valMax);
					updateSliderHor = true;

				}
				else if (key == "value") {

					StringHelper::toFloat(value, sliderHor->valCur);
					updateSliderHor = true;

				}
				else if (key == "roundValue") {

					StringHelper::toBool(value, sliderHor->valRound);
					updateSliderHor = true;
				}
				else if (key == "displayAsPercentage") {

					StringHelper::toBool(value, sliderHor->displayAsPercentage);
					updateSliderHor = true;

				}
				else if (key == "decimalPlaces") {

					s32 decimalPlaces;
					StringHelper::toInt(value, decimalPlaces);

					sliderHor->decimalPlaces = decimalPlaces;

					updateSliderHor = true;

				}

			}
			else if (type == WidgetType::TextEdit) {

				TextEdit* textEdit = (TextEdit*)this;

				if (key == "filter") {
					textEdit->setTextFilter(value);
				}
				else if (key == "mask") {
					textEdit->setTextMask((value != "") ? value[0] : 0);
				}
				else if (key == "allowSprites") {
					StringHelper::toBool(value, textEdit->allowSprites);
				}
				else if (key == "allowPastingSprites") {
					StringHelper::toBool(value, textEdit->allowPastingSprites);
				}
				else if (key == "readOnly") {

					bool readOnly;
					StringHelper::toBool(value, readOnly);

					textEdit->setReadOnly(readOnly);

				}
				else if (key == "showLineNumbers") {

					bool showLineNumbers;
					StringHelper::toBool(value, showLineNumbers);

					textEdit->setShowLineNumbers(showLineNumbers);
					
				}
				else if (key == "showCharPosition") {

					bool showCharPosition;
					StringHelper::toBool(value, showCharPosition);

					textEdit->setShowCharPosition(showCharPosition);

				}
				else if (key == "maxLineCharCount") {
					
					s32 maxCharCount;
					StringHelper::toInt(value, maxCharCount);

					textEdit->setMaxLineCharCount(maxCharCount);

				}
				else if (key == "maxLineCount") {

					s32 maxLineCount;
					StringHelper::toInt(value, maxLineCount);
					
					textEdit->setMaxLineCount(maxLineCount);

				}
				else if (key == "centerCursorIfSingleLine") {

					bool center;
					StringHelper::toBool(value, center);

					textEdit->setCenterCursorIfSingleLine(center);
					
				}
				else if (key == "highlightCurLine") {
					StringHelper::toBool(value, textEdit->highlightCurLine);
				}
				else if (key == "deselectTextOnDeselected") {
					StringHelper::toBool(value, textEdit->deselectTextOnDeselected);
				}
				else if (key == "selectTextOnFocused") {
					StringHelper::toBool(value, textEdit->selectTextOnFocused);
				}
				else if (key == "deselectOnEnter") {
					StringHelper::toBool(value, textEdit->deselectOnEnter);
				}
				else if (key == "numberOnly") {

					bool numOnly;
					StringHelper::toBool(value, numOnly);

					textEdit->setNumberOnly(numOnly);

				}
				else if (key == "roundNumber") {
					StringHelper::toBool(value, textEdit->numberRound);
				}
				else if (key == "decimalPlaces") {

					s32 decimalPlaces;
					StringHelper::toInt(value, decimalPlaces);

					textEdit->numberDecimalPlaces = Math::maxInt(0, decimalPlaces);

				}
				else if (key == "minNumber") {

					f32 minNumber;
					StringHelper::toFloat(value, minNumber);

					textEdit->setMinNumber(minNumber);

				}
				else if (key == "maxNumber") {

					f32 maxNumber;
					StringHelper::toFloat(value, maxNumber);

					textEdit->setMaxNumber(maxNumber);

				}
				else if (key == "placeholderText") {
					textEdit->setPlaceholderText(value);
				}

			}
			
		}
		
		if (updateSliderHor) {
			
			SliderHor* sliderHor = (SliderHor*)this;

			f32 valCur = sliderHor->valCur, valMin = sliderHor->valMin, valMax = sliderHor->valMax;

			sliderHor->setRange(valMin, valMax);
			sliderHor->setValue(valCur, false);
			sliderHor->updateValueText();

		}

		//We need to do this if the global grid column/row count changes.
		if (updateAllGlobalWidgets) {

			for (Widget* child : gui->widgetList) {
				if (child != this) child->updateTransform(true);
			}

		}

	}

	void Widget::addLocalShortcut(LocalShortcut& _shortcut) {

		if (_shortcut.keyCode == KEY_TAB) return;
		
		for (LocalShortcut& shortcut : localShortcutList) {

			if (shortcut.keyCode == _shortcut.keyCode && shortcut.modifiers == _shortcut.modifiers) {
				return;
			}

		}
		
		localShortcutList.push_back(_shortcut);

	}

	void Widget::stopShortcutMode() {

		if (shortcutMode) {

			if (currentShortcut.onEnd && currentShortcut.callOnEndImplicitly) {
				currentShortcut.onEnd();
			}

			shortcutMode = false;
			shortcutStart = false;
			shortcutRepeat = false;

			gui->widgetShortcut = nullptr;

		}

	}

	bool Widget::inShortcutMode() {
		return shortcutMode;
	}

	bool Widget::isShortcutDown(LocalShortcut& _shortcut) {

		if (gui->dropDownMenuOpen != nullptr || gui->widgetSelected != nullptr || gui->widgetFocused != this) {
			return false;
		}

		bool modCtrl = gui->isKeyDown(KEY_CONTROL);
		bool modShift = gui->isKeyDown(KEY_SHIFT);
		bool modAlt = gui->isKeyDown(KEY_LEFT_ALT);

		u16 keyCode = _shortcut.keyCode;
		u8 modifiers = _shortcut.modifiers;

		if ((modifiers & 0x01) == 1 && !modCtrl) return false;
		if (((modifiers >> 1) & 0x01) == 1 && !modShift) return false;
		if (((modifiers >> 2) & 0x01) == 1 && !modAlt) return false;

		return gui->isKeyDown(keyCode);

	}

	bool Widget::isShortcutPressed(LocalShortcut& _shortcut) {

		if (gui->dropDownMenuOpen != nullptr || gui->widgetSelected != nullptr || gui->widgetFocused != this) {
			return false;
		}
		
		bool modCtrl = gui->isKeyDown(KEY_CONTROL);
		bool modShift = gui->isKeyDown(KEY_SHIFT);
		bool modAlt = gui->isKeyDown(KEY_LEFT_ALT);

		u16 keyCode = _shortcut.keyCode;
		u8 modifiers = _shortcut.modifiers;

		return (gui->isKeyPressed(keyCode) && (modifiers & 0x01) == modCtrl && ((modifiers >> 1) & 0x01) == modShift && ((modifiers >> 2) & 0x01) == modAlt);

	}

	void Widget::setOnFocus(std::function<void(Widget*)> _onFocus) {
		onFocus = _onFocus;
	}

	bool Widget::canInteract() {
		
		return (
				gui->windowSelected == nullptr && (!gui->hasPopupWindow() || gui->getPopupWindow() == window)
				&& (gui->widgetSelected == nullptr || gui->widgetSelected == this)
				&& (enabledGlobal && visibleGlobal)
				&& (gui->widgetScroll == nullptr && gui->windowScroll == nullptr)
				//&& (gui->dropDownMenuOpen == nullptr || (type == WidgetType::MenuBar && gui->dropDownMenuOpen->menuBar == (MenuBar*)this)));
				//&& (!gui->isMouseOverDropDownMenu()/* || (type == WidgetType::MenuBar && gui->dropDownMenuOpen != nullptr && gui->dropDownMenuOpen->menuBar == (MenuBar*)this)*/)
			);
	}

	bool Widget::shouldUnfocus() {

		if (gui->widgetFocused != this) {
			return false;
		}

		return (!visibleGlobal || !enabledGlobal
			 || (gui->hasPopupWindow() && gui->getPopupWindow() != window )
			 || (gui->widgetSelected != nullptr && gui->widgetSelected != this )
			 || (gui->widgetScroll != nullptr && gui->widgetScroll != this ) );

	}

	void Widget::updatePosSize() {

		//@TODO: Anchor sides to other widgets.

		x = 0;
		y = 0;

		widthDraw = width;
		heightDraw = height;

		s32 parentX = 0;
		s32 parentY = 0;
		s32 parentWidth = 0;
		s32 parentHeight = 0;

		if (parent == nullptr) {

			if (window == nullptr) {

				s32 displayWidth, displayHeight;
				renderer->getWindowSize(displayWidth, displayHeight);

				if (gui->gridLayout) {

					s32 columnWidth = (displayWidth / gui->gridColumnCount);
					s32 rowHeight = (displayHeight / gui->gridRowCount);

					parentWidth = (gridColumn == gui->gridColumnCount - 1) ? (displayWidth - (gridColumn * columnWidth)) : columnWidth;
					parentHeight = (gridRow == gui->gridRowCount - 1) ? (displayHeight - (gridRow * rowHeight)) : rowHeight;
					parentX = (gridColumn * columnWidth);
					parentY = (gridRow * rowHeight);

				}
				else {
					parentWidth = displayWidth;
					parentHeight = displayHeight;
				}

			}
			else {

				if (window->gridLayout) {

					s32 columnWidth = (window->getContainerWidth() / window->gridColumnCount);
					s32 rowHeight = (window->getContainerHeight() / window->gridRowCount);

					parentWidth = (gridColumn == window->gridColumnCount - 1) ? (window->getContainerWidth() - (gridColumn * columnWidth)) : columnWidth;
					parentHeight = (gridRow == window->gridRowCount - 1) ? (window->getContainerHeight() - (gridRow * rowHeight)) : rowHeight;
					parentX = window->getContainerX() + (gridColumn * columnWidth);
					parentY = window->getContainerY() + (gridRow * rowHeight);

				}
				else {

					parentX = window->getContainerX();
					parentY = window->getContainerY();
					parentWidth = window->getContainerWidth() - (window->sprScrollVer->sizeX * window->scrollVerShow);
					parentHeight = window->getContainerHeight() - (window->sprScrollHor->sizeY * window->scrollHorShow);

				}

			}

		}
		else {

			if (parent->gridLayout) {

				s32 columnWidth = (parent->viewportWidth / gui->gridColumnCount);
				s32 rowHeight = (parent->viewportHeight / gui->gridRowCount);

				parentWidth = (gridColumn == parent->gridColumnCount - 1) ? (parent->viewportWidth - (gridColumn * columnWidth)) : columnWidth;
				parentHeight = (gridRow == parent->gridRowCount - 1) ? (parent->viewportHeight - (gridRow * rowHeight)) : rowHeight;
				parentX = parent->x + (gridColumn * columnWidth);
				parentY = parent->y + (gridRow * rowHeight);

			}
			else {

				parentX = parent->x;
				parentY = parent->y;
				parentWidth = parent->viewportWidth;
				parentHeight = parent->viewportHeight;

			}

		}

		if (fillX) {
			x = parentX + marginLeft;
			if (canResize) widthDraw = parentWidth - marginLeft - marginRight;
		}
		else {
			if (hAlign == WidgetAlign::Right) x = parentX + parentWidth - width;
			else if (hAlign == WidgetAlign::Center) x = parentX + (parentWidth / 2) - (width / 2);
			else x = parentX;

			x += marginLeft;
			if (canResize) widthDraw -= (marginLeft + marginRight);
		}

		x += (paddingLeft - paddingRight);
		xNoScroll = x;

		if (parent != nullptr) {
			xNoScroll += (parent->xNoScroll - parent->x);
			x -= (s32)roundf(parent->scrollHorOffset);
		}
		else if (window != nullptr) {
			x -= (s32)roundf(window->scrollHorOffset);
		}

		if (fillY) {
			y = parentY + marginTop;
			if (canResize) heightDraw = parentHeight - marginTop - marginBottom;
		}
		else {
			if (vAlign == WidgetAlign::Bottom) y = parentY + parentHeight - height;
			else if (vAlign == WidgetAlign::Middle) y = parentY + (parentHeight / 2) - (height / 2);
			else y = parentY;

			y += marginTop;
			if (canResize) heightDraw -= (marginTop + marginBottom);
		}

		y += (paddingTop - paddingBottom);
		yNoScroll = y;

		if (parent != nullptr) {
			yNoScroll += (parent->yNoScroll - parent->y);
			y -= (s32)roundf(parent->scrollVerOffset);
		}
		else if (window != nullptr) {
			y -= (s32)roundf(window->scrollVerOffset);
		}

		widthDraw = Math::maxInt(minWidth, widthDraw);
		heightDraw = Math::maxInt(minHeight, heightDraw);

		viewportWidth = Math::maxInt(0, widthDraw - (sprScrollVer->sizeX * scrollVerShow));
		viewportHeight = Math::maxInt(0, heightDraw - (sprScrollHor->sizeY * scrollHorShow));

	}

	void Widget::calculateContentSize(s32& maxX, s32& maxY) {

		contentWidth = 0;
		contentHeight = 0;

		s32 childMaxX = 0, childMaxY = 0;
		for (Widget* child : widgetList) {

			if (child->visibleGlobal) {

				child->calculateContentSize(childMaxX, childMaxY);

				s32 cX = child->xNoScroll + child->widthDraw - 1;
				s32 cY = child->yNoScroll + child->heightDraw - 1;

				bool ignoreX = !child->includeWhenCalculatingParentContentWidth;
				bool ignoreY = !child->includeWhenCalculatingParentContentHeight;

				if (child->type == WidgetType::Label) {

					Label* label = (Label*)child;
					if (label->extendLineBreakToParent) ignoreX = true;

					//Labels are handled differently now and we might not need this.
					/*if (label->textHAlign == TextAlign::Center) cX -= (child->widthDraw / 2);
					else if (label->textHAlign == TextAlign::Right) cX -= child->widthDraw;

					if (label->textVAlign == TextAlign::Middle) cY -= (child->heightDraw / 2);
					else if (label->textVAlign == TextAlign::Bottom) cY -= child->heightDraw;*/

				}

				if (child->cutContent) {

					if (!ignoreX) maxX = Math::maxInt(maxX, cX);
					if (!ignoreY) maxY = Math::maxInt(maxY, cY);

				}
				else {

					if (!ignoreX) maxX = Math::maxInt(maxX, cX, childMaxX);
					if (!ignoreY) maxY = Math::maxInt(maxY, cY, childMaxY);

				}

			}

		}

		contentWidth = Math::maxInt(0, insideWidth, maxX - (xNoScroll - 1));
		contentHeight = Math::maxInt(0, insideHeight, maxY - (yNoScroll - 1));

	}

	void Widget::calculateContentSize() {

		s32 maxX = 0, maxY = 0;
		calculateContentSize(maxX, maxY);

	}

	void Widget::updateScrollbars() {

		if (cutContent) {

			scrollVerShow = false;
			scrollHorShow = false;

			if (contentHeight - heightDraw > 0) {

				if (scrollVerVisible) {

					scrollVerShow = true;

					//@TODO: Recalculate content width here in case of fill_x
						   //We also need to take into account the margin and the padding of the widget, as that could cause to still be outside the viewport.

					if (scrollHorVisible && contentWidth - (widthDraw - sprScrollVer->sizeX) > 0)
						scrollHorShow = true;

				}

			}else if (contentWidth - widthDraw > 0) {

				if (scrollHorVisible) {

					scrollHorShow = true;

					if (scrollVerVisible && contentHeight - (heightDraw - sprScrollHor->sizeY) > 0)
						scrollVerShow = true;

				}

			}

			viewportWidth = Math::maxInt(0, widthDraw - (sprScrollVer->sizeX * scrollVerShow));
			viewportHeight = Math::maxInt(0, heightDraw - (sprScrollHor->sizeY * scrollHorShow));

			s32 offsetChanged = false;

			if (contentHeight - viewportHeight > 0) {

				scrollVerLen = Math::maxInt(0, viewportHeight);
				scrollVerBarLen = Math::maxInt(GUI_SCROLLBAR_MIN_LENGTH, (s32)((f32)scrollVerLen / ((f32)contentHeight / (f32)viewportHeight)));
				scrollVerBarMaxPos = (scrollVerLen - scrollVerBarLen);

				f32 prevOffset = scrollVerOffset;

				if (scrollVerOffset > 0.0f) {

					scrollVerOffset = Math::minFloat(scrollVerOffset, (f32)(contentHeight - viewportHeight));
					scrollVerBarPos = (s32)((scrollVerOffset / (f32)(contentHeight - viewportHeight)) * (f32)scrollVerBarMaxPos);

				}

				if (prevOffset != scrollVerOffset) {
					offsetChanged = true;
				}

			}
			else {

				scrollVerBarPos = 0;
				scrollVerOffset = 0.0f;

			}

			if (contentWidth - viewportWidth > 0) {

				scrollHorLen = Math::maxInt(0, viewportWidth);
				scrollHorBarLen = Math::maxInt(GUI_SCROLLBAR_MIN_LENGTH, (s32)((f32)scrollHorLen / ((f32)contentWidth/ (f32)viewportWidth)));
				scrollHorBarMaxPos = (scrollHorLen - scrollHorBarLen);

				f32 prevOffset = scrollHorOffset;

				if (scrollHorOffset > 0.0f) {

					scrollHorOffset = Math::minFloat(scrollHorOffset, (f32)(contentWidth - viewportWidth));
					scrollHorBarPos = (s32)((scrollHorOffset / (f32)(contentWidth - viewportWidth)) * (f32)scrollHorBarMaxPos);

				}

				if (prevOffset != scrollHorOffset) {
					offsetChanged = true;
				}

			}
			else {

				scrollHorBarPos = 0;
				scrollHorOffset = 0.0f;

			}

			if (offsetChanged) {

				for (Widget* child : widgetList) {
					child->updateTransform(true);
				}

			}

		}

	}

	void Widget::updateTransform(bool updateChildren, bool checkResize) {

		updatePosSize();

		if (updateChildren) {

			for (Widget* child : widgetList) {
				child->updateTransform(true, checkResize);
			}

		}

		if (xPrev != x || yPrev != y || widthDrawPrev != widthDraw || heightDrawPrev != heightDraw || !checkResize) {

			calculateContentSize();

			bool showH = scrollHorShow;
			bool showV = scrollVerShow;

			scrollHorOffsetPrev = scrollHorOffset;
			scrollVerOffsetPrev = scrollVerOffset;

			updateScrollbars();

			if (showV != scrollVerShow || showH != scrollHorShow) {

				for (Widget* child : widgetList) {
					child->updateTransform(true, checkResize);
				}

				calculateContentSize();
				updateScrollbars();

			}

			if (widthDrawPrev != widthDraw || heightDrawPrev != heightDraw) {

				//Do this to avoid stack overflow.
				s32 tempWidth = widthDrawPrev;
				s32 tempHeight = heightDrawPrev;

				widthDrawPrev = widthDraw;
				heightDrawPrev = heightDraw;

				onResize(tempWidth, tempHeight);

			}

			xPrev = x;
			yPrev = y;

		}

	}

	void Widget::updateRootTransform(bool updateChildren, bool checkResize) {

		if (window != nullptr) {
			window->updateTransform(updateChildren, checkResize);
		}
		else {

			Widget* root = getRootWidget(this);
			root->updateTransform(updateChildren, checkResize);

		}

	}

	s32 Widget::getParentX() {

		if (parent != nullptr) return parent->x;
		if (window != nullptr) return window->getContainerX();

		return 0;

	}

	s32 Widget::getParentY() {

		if (parent != nullptr) return parent->y;
		if (window != nullptr) return window->getContainerY();

		return 0;

	}

	s32 Widget::getParentWidth() {

		if (parent != nullptr) return parent->viewportWidth;
		if (window != nullptr) return window->getContainerWidth() - ((s32)window->scrollVerShow * window->sprScrollVer->sizeX);
		
		return renderer->windowWidth;

	}
	
	s32 Widget::getParentHeight() {

		if (parent != nullptr) return parent->viewportHeight;
		if (window != nullptr) return window->getContainerHeight() - ((s32)window->scrollHorShow * window->sprScrollHor->sizeY);
		
		return renderer->windowHeight;

	}

	void Widget::onFocused() {}
	void Widget::onUnfocused() {}
	void Widget::onResize(s32 prevWidth, s32 prevHeight) {}

	void Widget::updateMain(f32 dt) {
		
		if (gui == nullptr) return;

		s32 mouseX = gui->mouseX;
		s32 mouseY = gui->mouseY;

		mouseOver = (Math::pointInRect(mouseX, mouseY, x, y, widthDraw, heightDraw) && (parent == nullptr || (!parent->cutContent || (parent->mouseOver && !parent->mouseOverScrollbars))));
		mouseOverScrollbars = (Math::pointInRect(mouseX, mouseY, x, y, widthDraw, heightDraw) && !Math::pointInRect(mouseX, mouseY, x, y, viewportWidth, viewportHeight));

		for (s32 i = (s32)widgetList.size() - 1; i >= 0; --i) {

			Widget* child = widgetList[i];

			if (child->visibleGlobal) {
				child->updateMain(dt);
			}

		}

		//Update docked windows if the widget is a dock area.
		if (type == WidgetType::DockArea) {
			
			DockArea* dockArea = (DockArea*)this;

			dockArea->tabMouseOver = nullptr;

			for (DockContainer* container : dockArea->containerList) {

				s32 containerX = x + container->x;
				s32 containerY = y + container->y;

				if (Math::pointInRect(mouseX, mouseY, containerX, containerY, container->width, container->height)) {

					for (DockTab* tab : container->tabList) {

						if (Math::pointInRect(mouseX, mouseY, containerX + tab->x, containerY, tab->width, dockArea->sprDockTab->sizeY)) {

							dockArea->tabMouseOver = tab;
							//gui->setTooltip(this, tab->window->tooltip);
							break;

						}

					}

					break;

				}

			}

			bool alreadySet = gui->dockSplitMouseOver;

			if (!gui->isMouseOverScrollbars(this) && gui->widgetSelected == nullptr) {

				for (DockSplit* split : dockArea->splitList) {

					s32 sprWidth = dockArea->sprDockSplitVer->sizeX;
					s32 sprHeight = dockArea->sprDockSplitHor->sizeY;

					if (split->dir == 0 && Math::pointInRect(mouseX, mouseY, x + split->xRound, y + split->yRound + (sprHeight / 2) - (GUI_DOCK_GRAB_RANGE / 2), split->lengthRound, GUI_DOCK_GRAB_RANGE)) {

						gui->dockSplitMouseOver = true;
						break;

					}
					else if (split->dir != 0 && Math::pointInRect(mouseX, mouseY, x + split->xRound + (sprWidth / 2) - (GUI_DOCK_GRAB_RANGE / 2), y + split->yRound, GUI_DOCK_GRAB_RANGE, split->lengthRound)) {

						gui->dockSplitMouseOver = true;
						break;

					}

				}

			}

			for (DockContainer* container : dockArea->containerList) {

				if (container->tabSelected != nullptr) {

					Window* tempWindow = container->tabSelected->window;

					for (s32 i = (s32)tempWindow->widgetList.size() - 1; i >= 0; --i) {

						Widget* child = tempWindow->widgetList[i];

						if (child->visibleGlobal) {
							child->updateMain(dt);
						}
					}

				}

			}

			if (!alreadySet) gui->dockSplitMouseOver = false;

		}

		s32 cW = (contentWidth - viewportWidth) * cutContent;
		s32 cH = (contentHeight - viewportHeight) * cutContent;

		if (
			( 
				(gui->windowMouseOver == nullptr && window == nullptr)
			 || (window != nullptr && gui->windowMouseOver == window->getRootWindow() && gui->windowMouseOverContainer)
			 || (gui->windowMouseOver == nullptr && window != nullptr && window->isDocked() && Math::pointInRect(mouseX, mouseY, window->getX(), window->getY(), window->getWidth(), window->getHeight()))
			)
			&& !gui->dockSplitMouseOver
			&& !gui->isMouseOverDropDownMenu()
		   ) {
			
			if (mouseOver && gui->getWidgetMouseOver() == nullptr) {
				gui->setWidgetMouseOver(this);
			}

			if (mouseOver && gui->widgetMouseOverScroll == nullptr && ((cW > 0 && canScrollHor && (scrollHorVisible || gui->widgetFocused == this)) || (cH > 0 && canScrollVer && (scrollVerVisible || gui->widgetFocused == this)))) {
				gui->widgetMouseOverScroll = this;
			}
		}

		if (gui->widgetScroll == this) {

			if (gui->widgetScrollDir == 0) {

				if (gui->mouseY != gui->widgetScrollMousePosPrev) {

					gui->widgetScrollMousePosPrev = gui->mouseY;

					s32 pos = scrollVerBarPos;

					scrollVerBarPos = Math::clampInt(gui->widgetScrollBarPos + (mouseY - gui->widgetScrollMousePos), 0, scrollVerBarMaxPos);
					scrollVerOffset = (((f32)scrollVerBarPos / (f32)scrollVerBarMaxPos) * (f32)(contentHeight - viewportHeight));

					if (pos != scrollVerBarPos) {

						for (Widget* child : widgetList) {
							child->updateTransform(true, false);
						}

					}

				}

			}
			else {

				if (gui->mouseX != gui->widgetScrollMousePosPrev) {

					gui->widgetScrollMousePosPrev = gui->mouseX;

					s32 pos = scrollHorBarPos;

					scrollHorBarPos = Math::clampInt(gui->widgetScrollBarPos + (mouseX - gui->widgetScrollMousePos), 0, scrollHorBarMaxPos);
					scrollHorOffset = (((f32)scrollHorBarPos / (f32)scrollHorBarMaxPos) * (f32)(contentWidth - viewportWidth));

					if (pos != scrollHorBarPos) {

						for (Widget* child : widgetList) {
							child->updateTransform(true, false);
						}

					}

				}

			}

			if (!gui->isMouseDown(MOUSE_LEFT)) {

				gui->setWidgetScroll(nullptr);

				if (gui->widgetScrollDir == 0) scrollVerInd = Math::pointInRect(mouseX, mouseY, x + widthDraw - sprScrollVer->sizeX, y + scrollVerBarPos, sprScrollVer->sizeX, scrollVerBarLen);
				else scrollHorInd = Math::pointInRect(mouseX, mouseY, x + scrollHorBarPos, y + heightDraw - sprScrollHor->sizeY, scrollHorBarLen, sprScrollHor->sizeY);

			}

		}
		else {
			
			if ((cW > 0 || cH > 0) && gui->widgetMouseOverScroll == this) {
				
				if (canInteract()) {

					bool update = false;
					
					if (gui->widgetSelected != this) {
						
						if (scrollVerShow && canScrollVer) {
							
							if (Math::pointInRect(mouseX, mouseY, x + widthDraw - sprScrollVer->sizeX, y + scrollVerBarPos, sprScrollVer->sizeX, scrollVerBarLen)) {

								scrollVerInd = 1;

								if (gui->isMousePressed(MOUSE_LEFT)) {

									gui->setWidgetScroll(this);
									gui->widgetScrollDir = 0;
									gui->widgetScrollMousePos = mouseY;
									gui->widgetScrollMousePosPrev = mouseY;
									gui->widgetScrollBarPos = scrollVerBarPos;

									scrollVerInd = 2;

								}

							}
							else scrollVerInd = 0;

						}

						if (scrollHorShow && canScrollHor) {

							if (Math::pointInRect(mouseX, mouseY, x + scrollHorBarPos, y + heightDraw - sprScrollHor->sizeY, scrollHorBarLen, sprScrollHor->sizeY)) {

								scrollHorInd = 1;

								if (gui->isMousePressed(MOUSE_LEFT)) {

									gui->setWidgetScroll(this);
									gui->widgetScrollDir = 1;
									gui->widgetScrollMousePos = mouseX;
									gui->widgetScrollMousePosPrev = mouseX;
									gui->widgetScrollBarPos = scrollHorBarPos;

									scrollHorInd = 2;

								}

							}
							else scrollHorInd = 0;

						}	

					}

					if ((gui->widgetSelected != this || canScrollWhileSelected) && canScrollVer && cH > 0 && !gui->isKeyDown(KEY_CONTROL)) {

						if (!gui->isKeyDown(KEY_SHIFT) || cW <= 0 || !canScrollHor) {

							if (gui->isMouseScrollDown()) {

								s32 pos = scrollVerBarPos;
								scrollVerBarPos = Math::minInt(scrollVerBarMaxPos, scrollVerBarPos + 16);

								if (pos != scrollVerBarPos) {

									update = true;
									scrollVerOffset = (((f32)scrollVerBarPos / (f32)scrollVerBarMaxPos) * (f32)cH);

								}

							}
							else if (gui->isMouseScrollUp()) {

								s32 pos = scrollVerBarPos;
								scrollVerBarPos = Math::maxInt(0, scrollVerBarPos - 16);

								if (pos != scrollVerBarPos) {

									update = true;
									scrollVerOffset = (((f32)scrollVerBarPos / (f32)scrollVerBarMaxPos) * (f32)cH);

								}

							}

						}

					}

					if ((gui->widgetSelected != this || canScrollWhileSelected) && canScrollHor && cW > 0 && !gui->isKeyDown(KEY_CONTROL)) {

						if (gui->isKeyDown(KEY_SHIFT) || cH <= 0 || !canScrollVer) {

							if (gui->isMouseScrollDown()) {

								s32 pos = scrollHorBarPos;
								scrollHorBarPos = Math::minInt(scrollHorBarMaxPos, scrollHorBarPos + 16);

								if (pos != scrollHorBarPos) {

									update = true;
									scrollHorOffset = (((f32)scrollHorBarPos / (f32)scrollHorBarMaxPos) * (f32)cW);

								}

							}
							else if (gui->isMouseScrollUp()) {

								s32 pos = scrollHorBarPos;
								scrollHorBarPos = Math::maxInt(0, scrollHorBarPos - 16);

								if (pos != scrollHorBarPos) {

									update = true;
									scrollHorOffset = (((f32)scrollHorBarPos / (f32)scrollHorBarMaxPos) * (f32)cW);

								}

							}

						}

					}

					if (update) {

						for (Widget* child : widgetList) {
							child->updateTransform(true);
						}

					}

				}
				else {

					scrollHorInd = 0;
					scrollVerInd = 0;

				}

			}
			else {

				scrollHorInd = 0;
				scrollVerInd = 0;

			}

		}

		bool interactable = canInteract();

		//Handle local shortcuts.
		if (localShortcutList.size() > 0) {

			if (gui->widgetFocused == this && interactable) {

				for (LocalShortcut& shortcut : localShortcutList) {

					if (!shortcut.onCallback) continue;
					
					if (isShortcutPressed(shortcut)) {
						
						gui->stopTabbing();
						gui->stopGlobalShortcut();
						gui->stopLocalShortcut();

						gui->widgetShortcut = this;
						shortcutMode = true;
						currentShortcut = shortcut;

						if (shortcut.onStart) {

							if (shortcut.onStart()) {

								if (shortcut.onCallback()) {

									if (shortcut.repeatable) {

										shortcutStart = true;
										shortcutRepeat = false;
										shortcutTimer = 0.0f;

									}
									else if (!shortcut.onEnd) {

										shortcutMode = false;
										gui->widgetShortcut = nullptr;

									}

								}
								else {

									if (shortcut.onEnd/* && shortcut.callOnEndImplicitly*/) shortcut.onEnd(); //Don't think we wanna check implicitly here.

									shortcutMode = false;
									gui->widgetShortcut = nullptr;

								}

							}
							else {

								shortcutMode = false;
								gui->widgetShortcut = nullptr;

							}

						}
						else {

							if (shortcut.onCallback()) {

								if (shortcut.repeatable) {

									shortcutStart = true;
									shortcutRepeat = false;
									shortcutTimer = 0.0f;

								}
								else if (!shortcut.onEnd) {

									shortcutMode = false;
									gui->widgetShortcut = nullptr;

								}

							}
							else {

								if (shortcut.onEnd/* && shortcut.callOnEndImplicitly*/) shortcut.onEnd(); //Don't think we wanna check implicitly here.

								shortcutMode = false;
								gui->widgetShortcut = nullptr;

							}

						}

						break;

					}

				}

				if (shortcutMode) {

					if (!currentShortcut.repeatable) {

						if (!isShortcutDown(currentShortcut)) {

							shortcutMode = false;
							gui->widgetShortcut = nullptr;

							currentShortcut.onEnd();

						}

					}
					else {

						if (shortcutStart) {

							if (!isShortcutDown(currentShortcut)) {

								shortcutStart = false;
								shortcutMode = false;
								gui->widgetShortcut = nullptr;

								if (currentShortcut.onEnd) {
									currentShortcut.onEnd();
								}

							}
							else {

								shortcutTimer += dt;
								if (shortcutTimer >= GUI_GLOBAL_SHORTCUT_START_TIME) {

									shortcutStart = false;
									shortcutRepeat = true;
									shortcutTimer = GUI_GLOBAL_SHORTCUT_REPEAT_TIME;

								}

							}

						}

						if (shortcutRepeat) {

							if (!isShortcutDown(currentShortcut)) {

								shortcutRepeat = false;
								shortcutMode = false;
								gui->widgetShortcut = nullptr;

								if (currentShortcut.onEnd) {
									currentShortcut.onEnd();
								}

							}
							else {

								shortcutTimer += dt;
								if (shortcutTimer >= GUI_GLOBAL_SHORTCUT_REPEAT_TIME) {

									shortcutTimer = 0.0f;

									if (!currentShortcut.onCallback()) {

										shortcutRepeat = false;
										shortcutMode = false;
										gui->widgetShortcut = nullptr;

										if (currentShortcut.onEnd && currentShortcut.callOnEndImplicitly) {
											currentShortcut.onEnd();
										}

									}

								}

							}

						}

					}

				}

			}
			else {
				stopShortcutMode();
			}

		}
		else {

			shortcutMode = false;
			shortcutStart = false;
			shortcutRepeat = false;
			if (gui->widgetShortcut == this) gui->widgetShortcut = nullptr;

		}

		if (!interactable && shouldUnfocus()) {
			gui->unfocusWidget(this);
		}

		update(dt);

	}

	void Widget::update(f32 dt) {};

	void Widget::renderMain() {
		
		if (gui == nullptr) return;

		render(); //@Consider: Should cutContent apply to this?

		if (cutContent) {
			renderer->cutStart(x, y, viewportWidth, viewportHeight);
		}

		for (Widget* child : widgetList) {

			if (child->visibleGlobal) {
				child->renderMain();
			}

		}

		if (cutContent) {

			renderer->cutEnd();

			if (scrollVerShow || scrollHorShow) {

				renderer->cutStart(x, y, widthDraw, heightDraw);

				if (scrollVerShow) {

					s32 scrollX = Math::maxInt(x, x + widthDraw - sprScrollVer->sizeX);

					renderer->render3PVer(sprScrollVer, 0, scrollX, y, scrollVerLen);

					renderer->cutStart(scrollX, y, sprScrollVer->sizeX, scrollVerLen);
					renderer->render3PVer(sprScrollVer, 1 + scrollVerInd, scrollX, y + scrollVerBarPos, scrollVerBarLen);
					renderer->cutEnd();

				}

				if (scrollHorShow) {

					s32 scrollY = Math::maxInt(y, y + heightDraw - sprScrollHor->sizeY);
					
					renderer->render3PHor(sprScrollHor, 0, x, scrollY, scrollHorLen);

					renderer->cutStart(x, scrollY, widthDraw, sprScrollHor->sizeY);
					renderer->render3PHor(sprScrollHor, 1 + scrollHorInd, x + scrollHorBarPos, scrollY, scrollHorBarLen);
					renderer->cutEnd();

				}

				if (scrollVerShow && scrollHorShow) {
					renderer->render9P(sprScrollCorner, 0, x + widthDraw - sprScrollVer->sizeX, y + heightDraw - sprScrollHor->sizeY, sprScrollVer->sizeX, sprScrollHor->sizeY);
				}

				renderer->cutEnd();

			}

		}

		if (renderFocus && gui->widgetFocused == this) {
			renderer->render9P(sprFocus, 0, x, y, viewportWidth, viewportHeight);
		}

	}

	void Widget::render() {
		//renderer->render9P("test", 0, x, y, viewportWidth, viewportHeight);
	};

}