/*
    Label.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/GUI/Label.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/Window.h"

namespace Zixel {

	Label::Label(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::Label;

		width = 0;
		height = 0;
		canResize = false;

		fntText = renderer->getTextureAtlasFont(GUI_LABEL_FONT);

	}

	void Label::update(f32 dt) {

		if (extendLineBreakToParent) {

			s32 prevWidth = lineBreakWidth;

			s32 offset = getLineBreakOffset(); //Account for padding and margin.
			s32 newWidth = Math::maxInt(0, getParentWidth() - offset);

			if (prevWidth != newWidth) {

				bool prevScroll = (parent != nullptr) ? parent->scrollVerShow : ((window != nullptr) ? window->scrollVerShow : false);

				setLineBreakWidth(newWidth);

				bool newScroll = (parent != nullptr) ? parent->scrollVerShow : ((window != nullptr) ? window->scrollVerShow : false);

				if (prevScroll != newScroll) setLineBreakWidth(Math::maxInt(0, getParentWidth() - offset));

			}

		}

		if (canInteract() && gui->getWidgetMouseOver() == this) {
			gui->setTooltip(this, tooltip);
		}

	}

	void Label::render() {
		
		//if (lineBreakWidth >= 0) renderer->renderRect(x, y, lineBreakWidth, renderer->getStringHeight(fntText, textDisplay), COLOR_BLUE);

		if (text != "") {

			s32 drawX = x, drawY = y;

			if (textHAlign == TextAlign::Center) drawX += (widthDraw / 2);
			else if (textHAlign == TextAlign::Right) drawX += (widthDraw - 1);

			if (textVAlign == TextAlign::Middle) drawY += (heightDraw / 2);
			else if (textVAlign == TextAlign::Bottom) drawY += (heightDraw - 1);

			renderer->renderText(fntText, textDisplay, drawX, drawY, textHAlign, textVAlign, (enabledGlobal) ? color : colorDisabled);

			//if (window != nullptr) renderer->renderRect(drawX, window->getContainerY(), 1, window->getContainerHeight(), COLOR_BLUE);

		}

	}

	s32 Label::getLineBreakOffset() {

		s32 offset = paddingLeft - paddingRight + marginLeft;
		if (hAlign == WidgetAlign::Center) offset = Math::absInt(offset * 2);
		else if (hAlign == WidgetAlign::Right) offset = -offset;
		
		return offset;

	}

	void Label::updateSize() {

		s32 prevWidth = width;
		s32 prevHeight = height;

		width = renderer->getStringWidth(fntText, textDisplay);
		height = renderer->getStringHeight(fntText, textDisplay);

		if (prevWidth != width || prevHeight != height) {
			updateRootTransform(true, false);
		}

	}

	void Label::calculateLineBreak() {

		if (lineBreakWidth < 0 && textDisplay != text) {

			textDisplay = text;
			return;

		}

		if (lineBreakWidth >= 0) {

			textDisplay = text;

			s32 breakWidth = Math::maxInt(0, lineBreakWidth - lineBreakMargin);
			if (hAlign == WidgetAlign::Center) breakWidth -= lineBreakMargin;

			s32 lineWidth = 0;
			size_t wordStart = 0;
			bool firstWord = true;
			bool newLine = false;
			std::string curWord = ""; 
			s32 insertOffset = 0;

			bool startsWithSpace = (text.length() > 0 && text[0] == ' ');

			for (size_t i = 0; i < text.length(); ++i) {

				char& cur = text[i];

				if (cur == '\n') {

					if (!curWord.empty()) {

						if (lineWidth > breakWidth) {

							if (!firstWord || startsWithSpace) {

								startsWithSpace = false;

								textDisplay.insert(textDisplay.begin() + wordStart + insertOffset, '\n');

								i = wordStart - 1;
								++insertOffset;

							}

						}

						curWord = "";

					}

					startsWithSpace = (i < text.length() - 1 && text[i + 1] == ' ');

					lineWidth = 0;
					newLine = true;
					firstWord = true;

				}
				else if (cur == ' ') {
					
					bool nextSpace = (i < text.length() - 1 && text[i + 1] == ' ');

					if (!curWord.empty() && !nextSpace) {

						if (lineWidth > breakWidth) {

							if (!firstWord || startsWithSpace) {

								startsWithSpace = false;

								textDisplay.insert(textDisplay.begin() + wordStart + insertOffset, '\n');

								lineWidth = 0;
								i = wordStart - 1;
								newLine = true;
								firstWord = true;
								++insertOffset;

							}

						}

						if (!newLine) firstWord = false;

						curWord = "";

					}

				}
				else {

					if (curWord.empty()) wordStart = i;
					curWord += cur;

				}

				if(!newLine) lineWidth += renderer->getStringWidth(fntText, cur);
				newLine = false;

			}

			if (!curWord.empty() && lineWidth > breakWidth && (!firstWord || startsWithSpace)) {
				textDisplay.insert(textDisplay.begin() + wordStart + insertOffset, '\n');
			}

		}

	}

	void Label::setExtendLineBreakToParent(bool _extend) {

		if (_extend != extendLineBreakToParent) {

			extendLineBreakToParent = _extend;

			//This updates the ignoreX parameter in calculateContentSize.
			updateRootTransform(true, false);

			if (_extend) {

				s32 offset = getLineBreakOffset(); //Account for padding and margin.
				setLineBreakWidth(getParentWidth() - offset);
				updateSize();

			}

		}

	}

	void Label::setLineBreakWidth(s32 _width) {

		if (_width < 0) _width = -1;
		lineBreakWidth = _width;

		calculateLineBreak();
		updateSize();

	}

	void Label::setLineBreakMargin(s32 _margin) {

		lineBreakMargin = _margin;

		calculateLineBreak();
		updateSize();

	}

	void Label::setFont(std::string _font) {

		Font* font = renderer->getTextureAtlasFont(_font);
		if (font == nullptr) return;

		fntText = font;

		calculateLineBreak();
		updateSize();

	}

	void Label::setText(std::string _text) {

		if (_text != text) {

			text = _text;

			calculateLineBreak();
			updateSize();

		}

	}

}