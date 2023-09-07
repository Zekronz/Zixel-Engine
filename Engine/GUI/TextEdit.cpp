/*
    TextEdit.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Surface.h"
#include "Engine/Clipboard.h"
#include "Engine/Math.h"
#include "Engine/GUI/TextEdit.h"
#include "Engine/GUI/DropDownMenu.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/GUIMacros.h"

namespace Zixel {

	TextEdit::TextEdit(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {

		type = WidgetType::TextEdit;

		sprTextEdit = renderer->getTextureAtlasSprite(GUI_TEXT_EDIT_SPR);

		fntText = renderer->getTextureAtlasFont(GUI_TEXT_EDIT_FONT);
		fntLineNumber = renderer->getTextureAtlasFont(GUI_TEXT_EDIT_LINE_NUMBER_FONT);
		fntCharPosition = renderer->getTextureAtlasFont(GUI_TEXT_EDIT_CHAR_POSITION_FONT);

		cutContent = true;

		scrollToLineAlign = TextAlign::Top;
		scrollToCharAlign = TextAlign::Left;

		prevEnabled = enabledGlobal;

		std::vector<TextEditChar> line;
		lineList.push_back(line);
		lineWidthList.push_back(0);

		s32 cX, cY;
		getCursorXY(cX, cY);
		cursorX = cX;
		cursorXMoveTo = cX;
		cursorY = cY;

		overrideNonModifierShortcuts = true;
		updateOverrideTabKey();

		LocalShortcut tempShortcut; //TextEdit doesn't really use the built in system.

		tempShortcut.keyCode = KEY_D;
		tempShortcut.modifiers = GUI_MODIFIER_CTRL;
		addLocalShortcut(tempShortcut);

		tempShortcut.keyCode = KEY_A;
		tempShortcut.modifiers = GUI_MODIFIER_CTRL;
		addLocalShortcut(tempShortcut);

		tempShortcut.keyCode = KEY_C;
		tempShortcut.modifiers = GUI_MODIFIER_CTRL;
		addLocalShortcut(tempShortcut);

		tempShortcut.keyCode = KEY_V;
		tempShortcut.modifiers = GUI_MODIFIER_CTRL;
		addLocalShortcut(tempShortcut);

		tempShortcut.keyCode = KEY_X;
		tempShortcut.modifiers = GUI_MODIFIER_CTRL;
		addLocalShortcut(tempShortcut);

		//We don't have undo/redo support for TextEdits yet, but I'm just adding these here so we won't accidentally undo/redo the canvas.
		tempShortcut.keyCode = KEY_Z;
		tempShortcut.modifiers = GUI_MODIFIER_CTRL;
		addLocalShortcut(tempShortcut);

		tempShortcut.keyCode = KEY_D;
		tempShortcut.modifiers = GUI_MODIFIER_CTRL | GUI_MODIFIER_SHIFT;
		addLocalShortcut(tempShortcut);

		tempShortcut.keyCode = KEY_Y;
		tempShortcut.modifiers = GUI_MODIFIER_CTRL;
		addLocalShortcut(tempShortcut);

	}

	TextEdit::~TextEdit() {
		
		if (surf != nullptr) {
			delete surf;
		}

	}

	void TextEdit::onFocused() {

		gui->textEditSelected = this;

		if (selectTextOnFocused) {
			selectAll();
		}
		else {

			s32 lineCount = getLineCount();
			setCursorPos(lineCount - 1, (s32)lineList[(size_t)lineCount - 1].size(), true);

			cursorShow = true;
			cursorTimer = 0.0f;

		}

		if (onTextEditFocus) onTextEditFocus(this);

	}

	void TextEdit::onResize(s32 prevWidth, s32 prevHeight) {
		
		surfUpdate = true;

		if (centerIfSingleLine && maxLineCount == 1) {

			s32 cX, cY;
			getCursorXY(cX, cY);
			cursorX = cX;
			cursorY = cY;

		}

	}

	void TextEdit::update(f32 dt) {
		
		if (canInteract()) {

			if (!Math::pointInRect(gui->mouseX, gui->mouseY, x, y, viewportWidth, viewportHeight)) {

				if ((gui->isMousePressed(MOUSE_LEFT_RIGHT_MIDDLE)) && gui->textEditSelected == this) {
					deselect();
				}

			}
			else if (gui->getWidgetMouseOver() == this) {

				if (gui->textEditSelected != this) gui->setTooltip(this, tooltip, true);

				gui->setMouseCursor(CURSOR_IBEAM);

				if (gui->isMousePressed(MOUSE_LEFT_RIGHT_MIDDLE)) {

					gui->setWidgetSelected(this, false);

					if (selectMouse == 0) {

						if (gui->textEditSelected != this) {

							if (gui->textEditSelected != nullptr) {

								if (gui->textEditSelected->deselectTextOnDeselected) {
									gui->textEditSelected->deselectText();
								}

								if (gui->textEditSelected->checkConfirm(false) && gui->textEditSelected->onTextChanged) {
									gui->textEditSelected->onTextChanged(gui->textEditSelected);
								}

								if (gui->textEditSelected->onConfirm) {
									gui->textEditSelected->onConfirm(gui->textEditSelected);
								}

								gui->textEditSelected->cursorShow = false;
								gui->textEditSelected->surfUpdate = true;

							}

							cursorShow = true;
							cursorTimer = 0.0f;

						}
						
						if (gui->textEditSelected != this) {

							gui->textEditSelected = this;
							surfUpdate = true;

							if (onTextEditFocus) onTextEditFocus(this);

						}

						selectMousePos.x = gui->mouseX;
						selectMousePos.y = gui->mouseY;

						s32 selectLineStart = selectStartX, selectCharStart = selectStartY;
						s32 selectLineEnd = selectEndX, selectCharEnd = selectEndY;
						s32 oldLine = curLine, oldChar = curChar;

						s32 closestLine, closestChar;
						getClosestChar(closestLine, closestChar, gui->mouseX, gui->mouseY);
						
						bool horChange = (closestChar != curChar);
						bool verChange = (closestLine != curLine);

						if (verChange || horChange) {

							curLine = closestLine;
							curChar = closestChar;

							updateCursorPos();
							cursorXMoveTo = cursorX;

						}
						
						if (horChange) {
							checkScrollHor();
						}

						if (verChange) {
							checkScrollVer();
						}

						if (gui->isMousePressed(MOUSE_LEFT)) {

							selectMouse = 1;

							if (doubleClickTime < 0 || (renderer->getCurrentTime() - doubleClickTime) > GUI_TEXT_EDIT_DOUBLE_CLICK_TIME || curLine != doubleClickLine || curChar != doubleClickChar) {
								
								if (lineList[curLine].size() > 0) {

									doubleClickTime = renderer->getCurrentTime();
									doubleClickCount = 0;
									doubleClickLine = curLine;
									doubleClickChar = curChar;

								}
								else {

									doubleClickTime = -1;
									doubleClickCount = 0;
									doubleClickLine = -1;
									doubleClickChar = -1;

								}
								
								deselectText();
								
							}
							else {

								doubleClickCount = Math::minInt(doubleClickCount + 1, 3);
								doubleClickTime = renderer->getCurrentTime();

								doubleClickMouseX = gui->mouseX;
								doubleClickMouseY = gui->mouseY;
								doubleClickMove = false;

								//Select word.
								if (doubleClickCount == 1) {

									std::vector<TextEditChar>& charList = lineList[curLine];

									TextEditWordType wordType = TextEditWordType::Letter;
									s32 startChar = curChar, endChar = curChar;
									
									if (curChar == charList.size()) {

										wordType = charToWordType(charList[charList.size() - 1]);

										s32 i;
										for (i = (s32)charList.size() - 1; i >= 0; --i) {

											if (charToWordType(charList[i]) != wordType) {

												++i;
												break;

											}
											else if (i == 0) {
												break;
											}

										}

										startChar = i;
										endChar = (s32)charList.size() - 1;

									}
									else {

										wordType = charToWordType(charList[curChar]);

										s32 i;
										for (i = (s32)curChar; i >= 0; --i) {

											if (charToWordType(charList[i]) != wordType) {

												++i;
												break;

											}
											else if (i == 0) {
												break;
											}

										}

										startChar = i;

										for (i = (s32)curChar; i < charList.size(); ++i) {

											if (charToWordType(charList[i]) != wordType) {

												--i;
												break;

											}
											else if (i == charList.size() - 1) {
												break;
											}

										}

										endChar = i;

									}

									selectStartX = curLine;
									selectStartY = startChar;
									selectEndX = curLine;
									selectEndY = endChar;
									
									s32 prevChar = curChar;

									if (endChar - startChar + 1 > 1) {
										
										curChar = endChar + 1;
										selectSide = TextEditSelectSide::Right;

									}
									else {

										curChar = startChar;
										selectSide = TextEditSelectSide::Left;

									}

									if (curChar != prevChar) {

										updateCursorPos();
										cursorXMoveTo = cursorX;

									}

									surfUpdate = true;

								}
								else if (doubleClickCount == 2) {

									s32 lineCount = getLineCount();

									selectStartX = curLine;
									selectStartY = 0;
									selectEndX = curLine;
									selectEndY = (s32)lineList[curLine].size() - (curLine >= lineCount - 1);

									selectSide = TextEditSelectSide::Right;

									s32 prevLine = curLine, prevChar = curChar;

									if (curLine < lineCount - 1) {

										curChar = 0;
										++curLine;

									}
									else {
										curChar = (s32)lineList[curLine].size();
									}

									if (curLine != prevLine || curChar != prevChar) {

										updateCursorPos();
										cursorXMoveTo = cursorX;

									}

									surfUpdate = true;

								}
								else if (doubleClickCount >= 3) {

									selectStartX = 0;
									selectStartY = 0;

									selectEndX = getLineCount() - 1;
									selectEndY = (s32)lineList[selectEndX].size() - 1;

									selectSide = TextEditSelectSide::Right;

									s32 prevLine = curLine, prevChar = curChar;

									curLine = selectEndX;
									curChar = selectEndY + 1;

									if (curLine != prevLine || curChar != prevChar) {

										updateCursorPos();
										cursorXMoveTo = cursorX;

									}

									surfUpdate = true;

								}

							}

						}
						else if (gui->isMousePressed(MOUSE_RIGHT)) {

							selectMouse = 2;

							doubleClickTime = -1;
							doubleClickCount = 0;
							doubleClickLine = -1;
							doubleClickChar = -1;

							deselectText();

						}
						else if (gui->isMousePressed(MOUSE_MIDDLE)) {

							selectMouse = 3;

							doubleClickTime = -1;
							doubleClickCount = 0;
							doubleClickLine = -1;
							doubleClickChar = -1;

							deselectText();

						}

						if (selectStartX != selectLineStart || selectStartY != selectCharStart || selectEndX != selectLineEnd || selectEndY != selectCharEnd || oldLine != curLine || oldChar != curChar) {

							cursorShow = true;
							cursorTimer = 0.0f;

						}

					}

				}

			}

			if (gui->textEditSelected == this) {

				if (selectMouse == 1) {

					if (!gui->isMouseDown(MOUSE_LEFT)) {

						selectMouse = 0;
						gui->deselectWidget(this);

					}
					else {

						if (!doubleClickMove && (doubleClickCount == 0 || Math::distance2DInt(doubleClickMouseX, doubleClickMouseY, gui->mouseX, gui->mouseY) > GUI_TEXT_EDIT_DOUBLE_CLICK_MOVE_RANGE)) {
							doubleClickMove = true;
						}

						if (doubleClickMove && (gui->mouseX != selectMousePos.x || gui->mouseY != selectMousePos.y)) {

							doubleClickTime = -1;
							doubleClickCount = 0;
							doubleClickLine = -1;
							doubleClickChar = -1;

							selectMousePos.x = gui->mouseX;
							selectMousePos.y = gui->mouseY;

							s32 closestLine, closestChar;
							getClosestChar(closestLine, closestChar, gui->mouseX, gui->mouseY);
							
							if (selectStartX == -1) {

								if (closestLine > curLine || (curLine == closestLine && closestChar > curChar)) {

									selectStartX = curLine;
									selectStartY = curChar;

									if (closestLine > curLine && closestChar == 0) {

										selectEndX = curLine;
										selectEndY = (s32)lineList[curLine].size();

									}
									else {

										selectEndX = closestLine;
										selectEndY = closestChar - 1;

									}

									selectSide = TextEditSelectSide::Right;

								}
								else if (closestLine < curLine || (curLine == closestLine && closestChar < curChar)) {

									if (curChar - 1 < 0) {

										selectEndX = curLine - 1;
										selectEndY = (s32)lineList[(size_t)curLine - 1].size();

									}
									else {

										selectEndX = curLine;
										selectEndY = curChar - 1;

									}

									selectStartX = closestLine;
									selectStartY = closestChar;

									selectSide = TextEditSelectSide::Left;

								}

							}
							else {

								if (selectSide == TextEditSelectSide::Left) {

									if ((closestLine == selectEndX && closestChar == selectEndY + 1) || (selectEndY == lineList[selectEndX].size() && closestLine == selectEndX + 1 && closestChar == 0)){
										selectStartX = -1;
									}
									else if (closestLine > selectEndX || (closestLine == selectEndX && closestChar > selectEndY + 1)) {

										selectStartX = selectEndX;
										selectStartY = selectEndY + 1;

										if (closestChar - 1 < 0) {

											selectEndX = closestLine - 1;
											selectEndY = (s32)lineList[(size_t)closestLine - 1].size();

										}
										else {

											selectEndX = closestLine;
											selectEndY = closestChar - 1;

										}

										selectSide = TextEditSelectSide::Right;

									}
									else {

										selectStartX = closestLine;
										selectStartY = closestChar;

									}

								}
								else {

									if (closestLine == selectStartX && closestChar == selectStartY) {
										selectStartX = -1;
									}
									else if (closestLine < selectStartX || (closestLine == selectStartX && closestChar < selectStartY)) {

										selectEndX = selectStartX;
										selectEndY = selectStartY - 1;

										selectStartX = closestLine;
										selectStartY = closestChar;

										selectSide = TextEditSelectSide::Left;

									}
									else {

										if (closestChar - 1 < 0) {

											selectEndX = closestLine - 1;
											selectEndY = (s32)lineList[(size_t)closestLine - 1].size();

										}
										else {

											selectEndX = closestLine;
											selectEndY = closestChar - 1;

										}

									}

								}

							}

							if (closestLine != curLine || closestChar != curChar) {

								bool horChange = (closestChar != curChar);
								bool verChange = (closestLine != curLine);

								curLine = closestLine;
								curChar = closestChar;

								cursorShow = true;
								cursorTimer = 0.0f;

								updateCursorPos();
								cursorXMoveTo = cursorX;

								surfUpdate = true;

								if (horChange) {
									checkScrollHor();
								}

								if (verChange) {
									checkScrollVer();
								}

							}

						}

					}

				}
				else if (selectMouse == 2) {

					if (!gui->isMouseDown(MOUSE_RIGHT)) {

						selectMouse = 0;
						gui->deselectWidget(this);

						if (menu != nullptr) {
							menu->open(gui->mouseX + 1, gui->mouseY + 1);
						}

					}

				}
				else if (selectMouse == 3) {

					if (!gui->isMouseDown(MOUSE_MIDDLE)) {

						selectMouse = 0;
						gui->deselectWidget(this);

					}

				}

				if (keyShift && !gui->isKeyDown(KEY_SHIFT)) {

					keyShift = false;
					keyChar = 0;
					key = 0;
					keyStart = false;
					keyPress = false;

				}

				if (keyCtrl && !gui->isKeyDown(KEY_CONTROL)) {

					keyCtrl = false;
					keyChar = 0;
					key = 0;
					keyStart = false;
					keyPress = false;

				}

				if (keyAlt && !gui->isKeyDown(KEY_LEFT_ALT)) {

					keyAlt = false;
					keyChar = 0;
					key = 0;
					keyStart = false;
					keyPress = false;

				}

				if (keyRAlt && !gui->isKeyDown(KEY_RIGHT_ALT)) {

					keyRAlt = false;
					keyChar = 0;
					key = 0;
					keyStart = false;
					keyPress = false;

				}

				if (gui->isKeyPressed(KEY_ANY)) {

					if (gui->isKeyPressed(KEY_RIGHT_ALT)) {

						keyRAlt = true;
						keyChar = 0;
						key = 0;
						keyStart = false;
						keyPress = false;

					}
					else if (gui->isKeyPressed(KEY_SHIFT)) {

						keyShift = true;
						keyChar = 0;
						key = 0;
						keyStart = false;
						keyPress = false;

					}
					else if (gui->isKeyPressed(KEY_CONTROL)) {

						keyCtrl = true;
						keyChar = 0;
						key = 0;
						keyStart = false;
						keyPress = false;

					}
					else if (gui->isKeyPressed(KEY_LEFT_ALT)) {

						keyAlt = true;
						keyChar = 0;
						key = 0;
						keyStart = false;
						keyPress = false;

					}
					else {
						
						//I think this is fine.
						//if (gui->lastChar != 0) {

							key = gui->lastKey;
							keyChar = gui->lastChar;

							doKeyPress(key, keyChar, true);

						//}

					}

				}

				if (keyStart) {

					if (!gui->isKeyDown(key)) {

						keyStart = false;
						keyPress = false;
						keyTimerStart = 0.0f;
						keyChar = 0;

					}
					else {

						keyTimerStart += dt;
						if (keyTimerStart >= GUI_TEXT_EDIT_KEY_PRESS_START_TIME) {

							keyStart = false;
							keyPress = true;

							keyTimer = 0.0f;

							doKeyPress(key, keyChar, false);

						}

					}

				}
				else if (keyPress) {

					if (!gui->isKeyDown(key)) {

						keyPress = false;
						keyStart = false;
						keyTimer = 0.0f;
						keyChar = 0;

					}
					else {

						keyTimer += dt;
						if (keyTimer >= GUI_TEXT_EDIT_KEY_PRESS_TIME) {

							keyTimer = 0.0f;
							doKeyPress(key, keyChar, false);

						}

					}

				}

				if (cursorTimer < GUI_TEXT_EDIT_CURSOR_BLINK_SPEED) {

					cursorTimer += dt;
					if (cursorTimer >= GUI_TEXT_EDIT_CURSOR_BLINK_SPEED) {

						cursorTimer = 0.0f;
						cursorShow = !cursorShow;

					}

				}

			}

		}
		else {

			deselect();

			cursorShow = false;

			keyStart = false;
			keyPress = false;
			keyCtrl = false;
			keyShift = false;
			keyAlt = false;
			keyRAlt = false;

			selectMouse = 0;

			doubleClickTime = -1;
			doubleClickCount = 0;
			doubleClickLine = -1;
			doubleClickChar = -1;
			doubleClickMove = false;

		}

		if (textChanged || updateInsideSize) {
			
			textChanged = false;
			updateInsideSize = false;
			surfUpdate = true;

			calculateInsideSize();

		}

		if (scrollToLine != -1) {

			if (scrollToLineAlign == TextAlign::Top) {
				scrollLineTop(scrollToLine);
			}
			else if (scrollToLineAlign == TextAlign::Bottom) {
				scrollLineBottom(scrollToLine);
			}

			scrollToLine = -1;

		}

		if (scrollToChar >= 0) {

			if (scrollToCharAlign == TextAlign::Left) {
				scrollCharLeft(scrollToChar);
			}
			else if (scrollToCharAlign == TextAlign::Right) {
				scrollCharRight(scrollToChar);
			}

			scrollToChar = -1;

		}

	}

	void TextEdit::render() {

		s32 areaWidth, areaHeight;
		getTextAreaSize(areaWidth, areaHeight);

		s32 scrollHor = (s32)roundf(scrollHorOffset);
		s32 scrollVer = (s32)roundf(scrollVerOffset);

		s32 drawX = x - scrollHor;
		s32 drawY = y - scrollVer;

		s32 lineCount = getLineCount();

		renderer->render9P(sprTextEdit, 0, x, y, viewportWidth, viewportHeight);

		renderer->cutStart(x + GUI_TEXT_EDIT_CUTOFF_SIZE, y + GUI_TEXT_EDIT_CUTOFF_SIZE, viewportWidth - (GUI_TEXT_EDIT_CUTOFF_SIZE * 2), viewportHeight - (GUI_TEXT_EDIT_CUTOFF_SIZE * 2));

		if (highlightCurLine) {

			s32 highlightY;
			if (centerIfSingleLine && maxLineCount == 1) {
				highlightY = drawY + (areaHeight / 2) - (GUI_TEXT_EDIT_LINE_HEIGHT / 2);
			}
			else {
				highlightY = drawY + GUI_TEXT_EDIT_VER_SPACING + ((s32)curLine * GUI_TEXT_EDIT_LINE_HEIGHT);
			}

			renderer->renderRect(x + (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers), highlightY, areaWidth, GUI_TEXT_EDIT_LINE_HEIGHT, GUI_TEXT_EDIT_LINE_SELECTED_COL);

		}

		s32 surfX = x + (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers);
		s32 surfY = y;

		if (surfScrollX != scrollHor || surfScrollY != scrollVer) {

			surfScrollX = scrollHor;
			surfScrollY = scrollVer;

			surfUpdate = true;

		}

		if (prevEnabled != enabledGlobal) {

			prevEnabled = enabledGlobal;
			surfUpdate = true;

		}

		if (surf == nullptr || surfUpdate) {

			if (surf != nullptr) {

				delete surf;
				surf = nullptr;

			}

			surfUpdate = false;

			surfWidth = areaWidth;
			surfHeight = areaHeight;

			if (surfWidth > 0 && surfHeight > 0) {
				
				surf = new Surface(renderer, surfWidth, surfHeight);

				renderer->cutPause();
				renderer->bindSurface(surf);
				renderer->saveBlendModeState();
				renderer->setBlendMode(Blend::InvDestAlpha, Blend::One);

				for (s32 i = (Math::maxInt(0, scrollVer - GUI_TEXT_EDIT_VER_SPACING) / GUI_TEXT_EDIT_LINE_HEIGHT); i < lineCount; ++i) {

					std::vector<TextEditChar>& charList = lineList[i];

					s32 textX = drawX + GUI_TEXT_EDIT_HOR_SPACING + (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers) - surfX;
					s32 textPrevSpacing = 0;

					s32 textY;
					if (centerIfSingleLine && maxLineCount == 1) {
						textY = drawY + (areaHeight / 2) - surfY;
					}
					else {
						textY = drawY + GUI_TEXT_EDIT_VER_SPACING + (i * GUI_TEXT_EDIT_LINE_HEIGHT) + (GUI_TEXT_EDIT_LINE_HEIGHT / 2) - surfY;
					}

					if (i == 0 && placeholderStyle == TextEditPlaceholderStyle::Empty && canDisplayPlaceholder()) {

						renderer->renderText(fntText, placeholderText, surfWidth - GUI_TEXT_EDIT_HOR_SPACING, textY, TextAlign::Right, TextAlign::Middle, GUI_TEXT_EDIT_PLACEHOLDER_COL);
						break;

					}

					s32 charCount = (s32)charList.size();

					for (s32 j = 0; j <= charCount; ++j) {

						bool isSelected = (selectStartX >= 0 && selectStartX <= i && selectEndX >= i && (selectStartY <= j || selectStartX < i) && (selectEndY >= j || selectEndX > i));

						if (j < charCount) {

							TextEditChar& c = charList[j];

							if (c.type == TextEditCharType::Text) {

								std::string text;
								text += (textMask != 0) ? textMask : c.c;

								s32 xTo = textX + renderer->getStringWidth(fntText, text);
								if (j + 1 < charCount && charList[(size_t)j + 1].type == TextEditCharType::Sprite) {

									textPrevSpacing = GUI_TEXT_EDIT_SPRITE_SPACING;
									xTo += GUI_TEXT_EDIT_SPRITE_SPACING;

								}
								else {

									textPrevSpacing = GUI_TEXT_EDIT_CHAR_SPACING;
									xTo += GUI_TEXT_EDIT_CHAR_SPACING;

								}

								if (xTo >= 0 && textX < surfWidth) {

									bool blend = false;

									if (isSelected) {

										renderer->renderRect(textX, textY - (GUI_TEXT_EDIT_LINE_HEIGHT / 2), xTo - textX, GUI_TEXT_EDIT_LINE_HEIGHT, GUI_TEXT_EDIT_TEXT_SELECTED_COL);
										blend = true;

									}
									else if (c.colBackground.a > 0) {

										renderer->renderRect(textX, textY - (GUI_TEXT_EDIT_LINE_HEIGHT / 2), xTo - textX, GUI_TEXT_EDIT_LINE_HEIGHT, Color::toColor4f(c.colBackground));
										blend = true;

									}

									if (blend) {
										renderer->setBlendMode(Blend::SrcAlpha, Blend::InvSrcAlpha, Blend::InvDestAlpha, Blend::One);
									}

									Color4f textCol = GUI_TEXT_EDIT_CHAR_FOREGROUND_DISABLED_COL;
									if (enabledGlobal) textCol = Color::toColor4f(c.colForeground);
									renderer->renderText(fntText, text, textX, textY + GUI_TEXT_EDIT_TEXT_VER_OFFSET, TextAlign::Left, TextAlign::Middle, textCol);

									renderer->setBlendMode(Blend::InvDestAlpha, Blend::One);

								}

								textX = xTo;

							}
							else if (c.type == TextEditCharType::Sprite) {

								textPrevSpacing = GUI_TEXT_EDIT_SPRITE_SPACING;
								s32 xTo = textX + (c.spr->sizeX + GUI_TEXT_EDIT_SPRITE_SPACING);

								if (xTo >= 0 && textX < surfWidth) {

									bool blend = false;

									if (isSelected) {

										renderer->renderRect(textX, textY - (GUI_TEXT_EDIT_LINE_HEIGHT / 2), xTo - textX, GUI_TEXT_EDIT_LINE_HEIGHT, GUI_TEXT_EDIT_TEXT_SELECTED_COL);
										blend = true;

									}
									else if (c.colBackground.a > 0) {

										renderer->renderRect(textX, textY - (GUI_TEXT_EDIT_LINE_HEIGHT / 2), xTo - textX, GUI_TEXT_EDIT_LINE_HEIGHT, Color::toColor4f(c.colBackground));
										blend = true;

									}

									if (blend) {
										renderer->setBlendMode(Blend::SrcAlpha, Blend::InvSrcAlpha, Blend::InvDestAlpha, Blend::One);
									}

									renderer->renderSprite(c.spr, c.sub, textX, textY - (c.spr->sizeY / 2));

									renderer->setBlendMode(Blend::InvDestAlpha, Blend::One);

								}

								textX = xTo;

							}
							else if (c.type == TextEditCharType::Tab) {

								s32 tabWidth = (renderer->getStringWidth(fntText, ' ') * GUI_TEXT_EDIT_TAB_SIZE);
								s32 tabFloor = ((textX - drawX - GUI_TEXT_EDIT_HOR_SPACING - (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers) + surfX) / tabWidth) * tabWidth;
								tabFloor += (drawX + GUI_TEXT_EDIT_HOR_SPACING + (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers) - surfX);
								tabWidth -= ((textX - tabFloor));

								textPrevSpacing = 0;
								s32 xTo = textX + tabWidth;

								if (xTo >= 0 && textX < surfWidth) {

									if (isSelected) {
										renderer->renderRect(textX, textY - (GUI_TEXT_EDIT_LINE_HEIGHT / 2), xTo - textX, GUI_TEXT_EDIT_LINE_HEIGHT, GUI_TEXT_EDIT_TEXT_SELECTED_COL);
									}
									else if (c.colBackground.a > 0) {
										renderer->renderRect(textX, textY - (GUI_TEXT_EDIT_LINE_HEIGHT / 2), xTo - textX, GUI_TEXT_EDIT_LINE_HEIGHT, Color::toColor4f(c.colBackground));
									}

								}

								textX = xTo;

							}

						}
						else {

							if (isSelected && textX >= 0 && textX + GUI_TEXT_EDIT_SELECT_NEW_LINE_SIZE - 1 < surfWidth) {
								renderer->renderRect(textX, textY - (GUI_TEXT_EDIT_LINE_HEIGHT / 2), GUI_TEXT_EDIT_SELECT_NEW_LINE_SIZE, GUI_TEXT_EDIT_LINE_HEIGHT, GUI_TEXT_EDIT_TEXT_SELECTED_COL);
							}

						}

						if (textX + surfX - drawX >= areaWidth + scrollHor + (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers)) {
							break;
						}

					}

					if (textY + surfY - drawY >= areaHeight + scrollVer) {
						break;
					}

					if (i == 0 && placeholderStyle == TextEditPlaceholderStyle::BehindText && canDisplayPlaceholder()) {
						renderer->renderText(fntText, placeholderText, textX + textPrevSpacing + GUI_TEXT_EDIT_PLACEHOLDER_SPACING, textY + GUI_TEXT_EDIT_TEXT_VER_OFFSET, TextAlign::Left, TextAlign::Middle, GUI_TEXT_EDIT_PLACEHOLDER_COL);
					}

				}

				renderer->restoreBlendModeState();

				renderer->unbindSurface();
				renderer->cutResume();

			}
			
		}

		if (surfWidth > 0 && surfHeight > 0) {
			renderer->renderSurface(surf, surfX, surfY);
		}

		if (cursorShow) {
			renderer->renderRect(drawX + cursorX - (GUI_TEXT_EDIT_CURSOR_WIDTH / 2), drawY + cursorY + (GUI_TEXT_EDIT_LINE_HEIGHT / 2) - (GUI_TEXT_EDIT_CURSOR_HEIGHT / 2), GUI_TEXT_EDIT_CURSOR_WIDTH, GUI_TEXT_EDIT_CURSOR_HEIGHT, GUI_TEXT_EDIT_CURSOR_COL);
		}

		renderer->cutEnd();

		renderer->cutStart(x, y, viewportWidth, viewportHeight);

		if (showLineNumbers) {

			renderer->render9P(sprTextEdit, 1, x, y, GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH, areaHeight);

			std::string lineNumberText = "0";

			for (s32 i = (Math::maxInt(0, (scrollVer - GUI_TEXT_EDIT_VER_SPACING)) / GUI_TEXT_EDIT_LINE_HEIGHT); i < lineCount; ++i) {

				s32 textY;
				if (centerIfSingleLine && maxLineCount == 1) {
					textY = (areaHeight / 2);
				}
				else {
					textY = GUI_TEXT_EDIT_VER_SPACING + (i * GUI_TEXT_EDIT_LINE_HEIGHT) + (GUI_TEXT_EDIT_LINE_HEIGHT / 2);
				}

				lineNumberText = std::to_string(i + 1);
				renderer->renderText(fntLineNumber, lineNumberText, x + GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH - 1 - GUI_TEXT_EDIT_LINE_NUMBERS_SPACING, drawY + textY + GUI_TEXT_EDIT_LINE_NUMBERS_VER_OFFSET, TextAlign::Right, TextAlign::Middle, GUI_TEXT_EDIT_LINE_NUMBER_COL);

				if (textY >= areaHeight + scrollVer) break;

			}

		}

		if (showCharPosition) {

			renderer->render9P(sprTextEdit, 2, x, y + areaHeight, viewportWidth, GUI_TEXT_EDIT_CHAR_POSITION_HEIGHT);

			std::string charPositionText = "Line: " + std::to_string(curLine + 1) + "/" + std::to_string(lineCount) + " Char: " + std::to_string(curChar + 1);
			renderer->renderText(fntCharPosition, charPositionText, x + GUI_TEXT_EDIT_CHAR_POSITION_SPACING, y + viewportHeight - (GUI_TEXT_EDIT_CHAR_POSITION_HEIGHT / 2) + GUI_TEXT_EDIT_CHAR_POSITION_VER_OFFSET, TextAlign::Left, TextAlign::Middle, GUI_TEXT_EDIT_CHAR_POSITION_COL);

		}

		renderer->cutEnd();

	}

	void TextEdit::getTextAreaSize(s32& _width, s32& _height) {

		_width = viewportWidth - (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers);
		_height = viewportHeight - Math::maxInt(GUI_TEXT_EDIT_CUTOFF_SIZE, (GUI_TEXT_EDIT_CHAR_POSITION_HEIGHT * showCharPosition));

	}

	void TextEdit::getCharXY(s32& _x, s32& _y, s32 _linePos, s32 _charPos, bool _middle, bool _local, bool _end) {

		_linePos = Math::clampInt(_linePos, 0, (s32)lineList.size() - 1);

		std::vector<TextEditChar>& charList = lineList[_linePos];

		s32 charCount = (s32)charList.size();
		_charPos = Math::clampInt(_charPos, 0, charCount);

		s32 textX = GUI_TEXT_EDIT_HOR_SPACING + (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers * !_local);

		s32 count = _charPos;
		if (_end && _charPos < charCount) {
			++count;
		}

		for (s32 i = 0; i < count; ++i) {

			TextEditChar& c = charList[i];

			if (c.type == TextEditCharType::Text) {

				textX += renderer->getStringWidth(fntText, (textMask != 0) ? textMask : c.c);

				if (i + 1 < charCount && charList[(size_t)i + 1].type == TextEditCharType::Sprite) {
					textX += GUI_TEXT_EDIT_SPRITE_SPACING / (1 + (_middle && i == _charPos - 1));
				}
				else {
					textX += GUI_TEXT_EDIT_CHAR_SPACING / (1 + (_middle && i == _charPos - 1));
				}

			}
			else if (c.type == TextEditCharType::Sprite) {

				if (c.spr != nullptr) {
					textX += c.spr->sizeX + (GUI_TEXT_EDIT_SPRITE_SPACING / (1 + (_middle && i == _charPos - 1)));
				}

			}
			else if (c.type == TextEditCharType::Tab) {

				s32 tabWidth = (renderer->getStringWidth(fntText, ' ') * GUI_TEXT_EDIT_TAB_SIZE);
				s32 tabFloor = ((textX - GUI_TEXT_EDIT_HOR_SPACING - (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers * !_local)) / tabWidth) * tabWidth;
				tabFloor += (GUI_TEXT_EDIT_HOR_SPACING + (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers * !_local));
				tabWidth -= ((textX - tabFloor));

				textX += tabWidth;

			}

		}

		s32 areaWidth, areaHeight;
		getTextAreaSize(areaWidth, areaHeight);

		_x = textX - _end;
		_y = (centerIfSingleLine && maxLineCount == 1) ? (areaHeight / 2) - (GUI_TEXT_EDIT_LINE_HEIGHT / 2) : GUI_TEXT_EDIT_VER_SPACING + ((s32)_linePos * GUI_TEXT_EDIT_LINE_HEIGHT);

	}

	void TextEdit::getClosestChar(s32& _linePos, s32& _charPos, s32 _x, s32 _y, bool _middle, bool _includeScroll) {

		_linePos = Math::clampInt((_y - y + (s32)roundf(scrollVerOffset) - GUI_TEXT_EDIT_VER_SPACING) / GUI_TEXT_EDIT_LINE_HEIGHT, 0, (s32)lineList.size() - 1);
		_charPos = 0;
		
		bool hasSetCharX = false;
		s32 charX = 0;

		std::vector<TextEditChar>& charList = lineList[_linePos];
		s32 charCount = (s32)charList.size();

		s32 textX = x - (s32)roundf(scrollHorOffset) + GUI_TEXT_EDIT_HOR_SPACING + (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers);

		for (s32 i = 0; i < charCount; ++i) {

			if (!hasSetCharX || (hasSetCharX && Math::absInt(textX - _x) < charX)) {

				hasSetCharX = true;
				charX = Math::absInt(textX - _x);

				_charPos = i;

			}

			TextEditChar& c = charList[i];

			if (c.type == TextEditCharType::Text) {

				textX += renderer->getStringWidth(fntText, (textMask != 0) ? textMask : c.c);

				if (i + 1 < charCount && charList[(size_t)i + 1].type == TextEditCharType::Sprite) {
					textX += GUI_TEXT_EDIT_SPRITE_SPACING / (1 + (_middle && i == _charPos - 1));
				}
				else {
					textX += GUI_TEXT_EDIT_CHAR_SPACING / (1 + (_middle && i == _charPos - 1));
				}

			}
			else if (c.type == TextEditCharType::Sprite) {

				if (c.spr != nullptr) {
					textX += c.spr->sizeX + (GUI_TEXT_EDIT_SPRITE_SPACING / (1 + (_middle && i == _charPos - 1)));
				}

			}
			else if (c.type == TextEditCharType::Tab) {

				s32 tabWidth = (renderer->getStringWidth(fntText, ' ') * GUI_TEXT_EDIT_TAB_SIZE);
				s32 tabFloor = ((textX - x + (s32)roundf(scrollHorOffset) - GUI_TEXT_EDIT_HOR_SPACING - (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers)) / tabWidth) * tabWidth;
				tabFloor += (x - (s32)roundf(scrollHorOffset) + GUI_TEXT_EDIT_HOR_SPACING + (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers));
				tabWidth -= ((textX - tabFloor));

				textX += tabWidth;

			}

			if (!hasSetCharX || (hasSetCharX && Math::absInt(textX - _x) < charX)) {

				hasSetCharX = true;
				charX = Math::absInt(textX - _x);

				_charPos = i + 1;

			}

		}

	}

	void TextEdit::getCursorXY(s32& _x, s32& _y, bool _local) {
		getCharXY(_x, _y, curLine, curChar, true, _local);
	}

	void TextEdit::setCursorPos(s32 _linePos, s32 _charPos, bool _scrollToCursor) {

		s32 prevLine = curLine;
		s32 prevChar = curChar;

		curLine = Math::clampInt(_linePos, 0, (s32)getLineCount() - 1);
		curChar = Math::clampInt(_charPos, 0, getLineCharCount(curLine));

		if (curLine != prevLine || curChar != prevChar) {

			updateCursorPos();
			cursorXMoveTo = cursorX;

		}

		if (_scrollToCursor) {

			if (curChar != prevChar) {
				checkScrollHor();
			}

			if (curLine != prevLine) {
				checkScrollVer();
			}

		}

	}

	void TextEdit::updateCursorPos() {
		getCursorXY(cursorX, cursorY);
	}

	s32 TextEdit::getLineY(s32 _linePos) {

		s32 areaWidth, areaHeight;
		getTextAreaSize(areaWidth, areaHeight);

		if (centerIfSingleLine && maxLineCount == 1) {
			return (areaHeight / 2) - (GUI_TEXT_EDIT_LINE_HEIGHT / 2);
		}

		return ((s32)_linePos * GUI_TEXT_EDIT_LINE_HEIGHT) + GUI_TEXT_EDIT_VER_SPACING;

	}

	void TextEdit::updateLineWidth(s32 _linePos) {

		if (_linePos < 0 || _linePos >= getLineCount()) {

			ZIXEL_WARN("Error in TextEdit::updateLineWidth. _linePos={} exceeds limits {}-{}.", _linePos, 0, getLineCount());
			return;

		}
		
		std::vector<TextEditChar>& charList = lineList[_linePos];
		
		s32 newWidth = 0;
		s32 charCount = (s32)charList.size();

		for (s32 i = 0; i < charCount; ++i) {

			TextEditChar& curChar = charList[i];
			
			if (curChar.type == TextEditCharType::Text) {

				newWidth += renderer->getStringWidth(fntText, (textMask != 0) ? textMask : curChar.c);

				if (i + 1 < charCount && charList[(size_t)i + 1].type == TextEditCharType::Sprite) {
					newWidth += GUI_TEXT_EDIT_SPRITE_SPACING;
				}
				else {
					newWidth += GUI_TEXT_EDIT_CHAR_SPACING;
				}

			}
			else if (curChar.type == TextEditCharType::Sprite) {
				newWidth += (curChar.spr->sizeX + GUI_TEXT_EDIT_SPRITE_SPACING);
			}
			else if (curChar.type == TextEditCharType::Tab) {

				s32 tabWidth = (renderer->getStringWidth(fntText, ' ') * GUI_TEXT_EDIT_TAB_SIZE);
				s32 tabFloor = (newWidth / tabWidth) * tabWidth;
				tabWidth -= ((newWidth - tabFloor));

				newWidth += tabWidth;

			}

		}
		
		lineWidthList[_linePos] = newWidth;
		
		if (newWidth > maxLineWidth) {
			maxLineWidth = newWidth;
		}

	}
	
	void TextEdit::updateMaxLineWidth() {

		maxLineWidth = 0;

		for (s32& lineWidth : lineWidthList) {

			if (lineWidth > maxLineWidth) {
				maxLineWidth = lineWidth;
			}

		}

	}

	void TextEdit::scrollLineTop(s32 _linePos) {

		s32 yTo = getLineY(_linePos) - GUI_TEXT_EDIT_CUTOFF_SIZE;

		if ((s32)roundf(scrollVerOffset) != yTo) {

			setScrollOffset(scrollHorOffset, (f32)yTo);
			surfUpdate = true;

		}

	}

	void TextEdit::scrollLineBottom(s32 _linePos) {

		s32 areaWidth, areaHeight;
		getTextAreaSize(areaWidth, areaHeight);

		s32 yTo = getLineY(_linePos) + GUI_TEXT_EDIT_LINE_HEIGHT - areaHeight;

		if ((s32)roundf(scrollVerOffset) != yTo) {

			setScrollOffset(scrollHorOffset, (f32)yTo);
			surfUpdate = true;

		}

	}

	void TextEdit::scrollCharLeft(s32 _charX) {

		s32 xTo = _charX - 1 - Math::maxInt(0, GUI_TEXT_EDIT_CUTOFF_SIZE - GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH);

		if ((s32)roundf(scrollHorOffset) != xTo) {

			setScrollOffset((f32)xTo, scrollVerOffset);
			surfUpdate = true;

		}

	}

	void TextEdit::scrollCharRight(s32 _charX) {

		s32 areaWidth, areaHeight;
		getTextAreaSize(areaWidth, areaHeight);

		s32 xTo = _charX - areaWidth + GUI_TEXT_EDIT_CUTOFF_SIZE + 2;

		if ((s32)roundf(scrollHorOffset) != xTo) {

			setScrollOffset((f32)xTo, scrollVerOffset);
			surfUpdate = true;

		}

	}

	void TextEdit::checkScrollHor() {

		s32 areaWidth, areaHeight;
		getTextAreaSize(areaWidth, areaHeight);

		s32 posX, posY;
		getCursorXY(posX, posY, true);

		s32 deltaX = posX - (s32)roundf(scrollHorOffset);

		if (deltaX >= areaWidth - GUI_TEXT_EDIT_CUTOFF_SIZE) {

			scrollToChar = posX;
			scrollToCharAlign = TextAlign::Right;

		}
		else if (deltaX < Math::maxInt(0, GUI_TEXT_EDIT_CUTOFF_SIZE - GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH)) {

			scrollToChar = posX;
			scrollToCharAlign = TextAlign::Left;

		}

	}

	void TextEdit::checkScrollVer() {

		s32 areaWidth, areaHeight;
		getTextAreaSize(areaWidth, areaHeight);

		s32 lineY = getLineY(curLine);

		if (lineY - (s32)roundf(scrollVerOffset) < GUI_TEXT_EDIT_CUTOFF_SIZE) {

			scrollToLine = curLine;
			scrollToLineAlign = TextAlign::Top;

		}
		else if (lineY + GUI_TEXT_EDIT_LINE_HEIGHT - 1 - (s32)roundf(scrollVerOffset) >= areaHeight) {

			scrollToLine = curLine;
			scrollToLineAlign = TextAlign::Bottom;

		}

	}

	void TextEdit::calculateInsideSize() {

		s32 maxWidth = maxLineWidth + (GUI_TEXT_EDIT_HOR_SPACING * 2) + (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers) + GUI_TEXT_EDIT_CUTOFF_SIZE;
		s32 maxHeight = (getLineCount() * GUI_TEXT_EDIT_LINE_HEIGHT) + (GUI_TEXT_EDIT_VER_SPACING * 2 * !(centerIfSingleLine && maxLineCount == 1)) + Math::maxInt(GUI_TEXT_EDIT_CUTOFF_SIZE, (GUI_TEXT_EDIT_CHAR_POSITION_HEIGHT * showCharPosition));

		if (maxWidth != insideWidth || maxHeight != insideHeight) {
			setInsideSize(maxWidth, maxHeight);
		}

	}

	bool TextEdit::isLetter(char _char) {

		return (StringHelper::isLetter(_char) || StringHelper::isDigit(_char) || _char == '_');

	}

	TextEditWordType TextEdit::charToWordType(TextEditChar& _char) {

		if (_char.type == TextEditCharType::Text) {

			char text = (textMask != 0) ? textMask : _char.c;

			if (isLetter(text)) {
				return TextEditWordType::Letter;
			}
			else if (text == ' ') {
				return TextEditWordType::SpaceAndTab;
			}
			else {
				return TextEditWordType::Other;
			}

		}
		else if (_char.type == TextEditCharType::Sprite) {
			return TextEditWordType::Sprite;
		}
		else if (_char.type == TextEditCharType::Tab) {
			return TextEditWordType::SpaceAndTab;
		}

		return TextEditWordType::Letter;

	}

	bool TextEdit::canDisplayPlaceholder() {

		if (placeholderText.empty()) return false;

		if (placeholderStyle == TextEditPlaceholderStyle::Empty) return (isEmpty() && gui->textEditSelected != this);
		if (placeholderStyle == TextEditPlaceholderStyle::BehindText) return (maxLineCount == 1);

		return false;

	}

	void TextEdit::updateOverrideTabKey() {
		overrideTabKey = (StringHelper::contains(textFilter, '\t') && !numberOnly && !readOnly);
	}

	bool TextEdit::checkConfirm(bool _callOnConfirm) {
		
		bool changed = false;

		if (numberOnly) {
			
			s32 lineCount = getLineCount();

			for (s32 i = (s32)lineCount - 1; i >= 0; --i) {

				std::string text;
				getLineText(text, i);
				
				if (text != "") {

					f64 value = getNumber(i, 0, false);

					std::string prevText = text; 

					if (numberHasMinValue && value < numberMin) {
						StringHelper::toString(text, numberMin, (round(numberMin) != numberMin && !numberRound) ? numberDecimalPlaces : 0);
					}
					else if (numberHasMaxValue && value > numberMax) {
						StringHelper::toString(text, numberMax, (round(numberMax) != numberMax && !numberRound) ? numberDecimalPlaces : 0);
					}
					else {
						StringHelper::toString(text, value, (round(value) != value && !numberRound) ? numberDecimalPlaces : 0);
					}

					if (prevText != text) {

						s32 charCount = getLineCharCount(i);

						if (charCount > 0) {

							if (deleteText(i, 0, i, charCount - 1, false)) {
								changed = true;
							}

						}

						s32 resultLine, resultChar;
						bool resultChanged;
						insertText(resultLine, resultChar, resultChanged, i, 0, text, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

						if (curLine == resultLine) {
							setCursorPos(resultLine, resultChar);
						}

						if (resultChanged) {
							changed = true;
						}

					}

				}
				else if(maxLineCount == 1) {

					if (numberHasMinValue && numberMin > 0.0) {
						StringHelper::toString(text, numberMin, (round(numberMin) != numberMin && !numberRound) ? numberDecimalPlaces : 0);
					}
					else if (numberHasMaxValue && numberMax < 0.0) {
						StringHelper::toString(text, numberMax, (round(numberMax) != numberMax && !numberRound) ? numberDecimalPlaces : 0);
					}
					else {
						text = "0";
					}

					s32 charCount = getLineCharCount(i);

					if (charCount > 0) {

						if (deleteText(i, 0, i, charCount - 1, false)) {
							changed = true;
						}

					}

					s32 resultLine, resultChar;
					bool resultChanged;
					insertText(resultLine, resultChar, resultChanged, i, 0, text, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

					if(curLine == resultLine){
						setCursorPos(resultLine, resultChar);
					}

					if (resultChanged) {
						changed = true;
					}

				}

			}

		}

		if (_callOnConfirm && onConfirm) {
			onConfirm(this);
		}

		return changed;

	}

	void TextEdit::doKeyPress(u16 _key, char _char, bool _start) {

		bool run = true; //If a key press successfully occured or not.
		bool repeatKey = false; //If the key press should repeat or not.
		bool shouldUpdateMoveToX = false; //If cursorXMoveTo should be updated or not.
		bool callOnConfirm = false; //If onConfirm should be called at the end or not.
		bool callOnReturn = false; //If onReturn should be called at the end or not.
		bool changed = false; //If the text was changed or not.

		s32 prevLine = curLine;
		s32 prevChar = curChar;

		if (_key == KEY_LEFT) {

			shouldUpdateMoveToX = true;

			curLinePrev = curLine;
			curCharPrev = curChar;

			if (!keyCtrl) {
				
				if (curChar > 0) {
					curChar--;
				}
				else {

					if (curLine > 0) {

						--curLine;
						curChar = (s32)lineList[curLine].size();

					}

				}

			}
			else {
				
				if (curLine > 0 || curChar > 0) {

					std::vector<TextEditChar>& charList = lineList[curLine];

					if (curChar > 0) {

						u8 state = 0;
						s32 i;
						for (i = curChar - 1; i >= 0; --i) {

							TextEditChar& c = charList[i];

							if (state == 0) {

								if (c.type == TextEditCharType::Text && isLetter((textMask != 0) ? textMask : c.c)) {
									state = 1;
								}
								else {
									state = 2;
								}

							}
							//Word.
							else if (state == 1) {

								if (c.type != TextEditCharType::Text || (c.type == TextEditCharType::Text && !isLetter((textMask != 0) ? textMask : c.c))) {
									break;
								}

							}
							//Other.
							else if (state == 2) {

								if (c.type == TextEditCharType::Text && isLetter((textMask != 0) ? textMask : c.c)) {
									break;
								}

							}

						}

						curChar = i + 1;

					}
					else if (curLine > 0) {
						
						--curLine;

						u8 state = 0;
						s32 i = 0;
						for (i = (s32)charList.size() - 1; i >= 0; --i) {

							TextEditChar& c = charList[i];

							if (state == 0) {

								if (c.type == TextEditCharType::Text && isLetter((textMask != 0) ? textMask : c.c)) {
									state = 1;
								}
								else {
									state = 2;
								}

							}
							//Word.
							else if (state == 1) {

								if (c.type != TextEditCharType::Text || (c.type == TextEditCharType::Text && !isLetter((textMask != 0) ? textMask : c.c))) {
									break;
								}

							}
							//Other.
							else if (state == 2) {

								if (c.type == TextEditCharType::Text && isLetter((textMask != 0) ? textMask : c.c)) {
									break;
								}

							}

						}

						curChar = i + 1;

					}

				}

			}

			if (keyShift) {
				doSelectLeft();
			}
			else {
				deselectText();
			}

		}
		else if (_key == KEY_RIGHT) {

			shouldUpdateMoveToX = true;

			curLinePrev = curLine;
			curCharPrev = curChar;

			if (!keyCtrl) {

				if (curChar < lineList[curLine].size()) {
					++curChar;
				}
				else {

					if (curLine < getLineCount() - 1) {

						curChar = 0;
						++curLine;

					}

				}

			}
			else {

				std::vector<TextEditChar>* charList = &lineList[curLine];

				if (curLine < getLineCount() - 1 || curChar < charList->size()) {

					if (curChar < charList->size()) {

						u8 state = 0;
						s32 i;
						for (i = curChar; i < charList->size(); ++i) {

							TextEditChar& c = charList->at(i);

							if (state == 0) {

								if (c.type == TextEditCharType::Text && isLetter((textMask != 0) ? textMask : c.c)) {
									state = 1;
								}
								else {
									state = 2;
								}

							}
							//Word.
							else if (state == 1) {

								if (c.type != TextEditCharType::Text || (c.type == TextEditCharType::Text && !isLetter((textMask != 0) ? textMask : c.c))) {
									break;
								}

							}
							//Other.
							else if (state == 2) {

								if (c.type == TextEditCharType::Text && isLetter((textMask != 0) ? textMask : c.c)) {
									break;
								}

							}

						}

						curChar = i;

					}
					else if (curLine < getLineCount() - 1) {

						++curLine;
						charList = &lineList[curLine];

						u8 state = 0;
						s32 i;
						for (i = 0; i < charList->size(); ++i) {

							TextEditChar& c = charList->at(i);

							if (state == 0) {

								if (c.type == TextEditCharType::Text && isLetter((textMask != 0) ? textMask : c.c)) {
									state = 1;
								}
								else {
									state = 2;
								}

							}
							//Word.
							else if (state == 1) {

								if (c.type != TextEditCharType::Text || (c.type == TextEditCharType::Text && !isLetter((textMask != 0) ? textMask : c.c))) {
									break;
								}

							}
							//Other.
							else if (state == 2) {

								if (c.type == TextEditCharType::Text && isLetter((textMask != 0) ? textMask : c.c)) {
									break;
								}

							}

						}

						curChar = i;

					}

				}

			}

			if (keyShift) {
				doSelectRight();
			}
			else {
				deselectText();
			}

		}
		else if (_key == KEY_UP) {

			if (!keyCtrl) {

				if (!keyAlt) {

					if (curLine > 0) {

						curLinePrev = curLine;
						curCharPrev = curChar;

						--curLine;

						if (keyShift && selectStartX < 0) {
							cursorXMoveTo = cursorX;
						}

						getClosestChar(curLine, curChar, x - (s32)roundf(scrollHorOffset) + cursorXMoveTo, y - (s32)roundf(scrollVerOffset) + cursorY - (GUI_TEXT_EDIT_LINE_HEIGHT / 2));

						if (keyShift) {
							doSelectUp();
						}
						else {
							deselectText();
						}

					}
					else if (!keyShift) {
						deselectText();
					}

				}
				else if(!readOnly) {

					s32 lineStart = (selectStartX >= 0) ? selectStartX : curLine;
					s32 lineEnd = (selectStartX >= 0) ? selectEndX : curLine;

					if (lineStart > 0) {

						std::vector<TextEditChar> charList = lineList[(size_t)lineStart - 1];
						s32 lineWidth = lineWidthList[(size_t)lineStart - 1];

						lineList.erase(lineList.begin() + lineStart - 1);
						lineWidthList.erase(lineWidthList.begin() + lineStart - 1);

						lineList.insert(lineList.begin() + lineEnd, charList);
						lineWidthList.insert(lineWidthList.begin() + lineEnd, lineWidth);

						if (selectStartX >= 0) {

							--selectStartX;
							--selectEndX;

						}

						--curLine;

						changed = true;

						if (lineStart - 1 > 0) {
							repeatKey = true;
						}

					}

				}

			}
			else {

				setScrollOffset(scrollHorOffset, ((scrollVerOffset - GUI_TEXT_EDIT_LINE_HEIGHT) / GUI_TEXT_EDIT_LINE_HEIGHT) * GUI_TEXT_EDIT_LINE_HEIGHT);

				if (scrollVerBarPos > 0) {
					repeatKey = true;
				}

			}

		}
		else if (_key == KEY_DOWN) {

			if (!keyCtrl) {

				if (!keyAlt) {

					if (curLine < getLineCount() - 1) {

						curLinePrev = curLine;
						curCharPrev = curChar;

						++curLine;

						if (keyShift && selectStartX < 0) {
							cursorXMoveTo = cursorX;
						}

						getClosestChar(curLine, curChar, x - (s32)roundf(scrollHorOffset) + cursorXMoveTo, y - (s32)roundf(scrollVerOffset) + cursorY + (s32)(GUI_TEXT_EDIT_LINE_HEIGHT * 1.5));

						if (keyShift) {
							doSelectDown();
						}
						else {
							deselectText();
						}

					}
					else if (!keyShift) {
						deselectText();
					}

				}
				else if(!readOnly) {

					s32 lineStart = (selectStartX >= 0) ? selectStartX : curLine;
					s32 lineEnd = (selectStartX >= 0) ? selectEndX : curLine;

					if (lineEnd < getLineCount() - 1) {

						std::vector<TextEditChar> charList = lineList[(size_t)lineEnd + 1];
						s32 lineWidth = lineWidthList[(size_t)lineEnd + 1];

						lineList.erase(lineList.begin() + lineEnd + 1);
						lineWidthList.erase(lineWidthList.begin() + lineEnd + 1);

						lineList.insert(lineList.begin() + lineStart, charList);
						lineWidthList.insert(lineWidthList.begin() + lineStart, lineWidth);

						if (selectStartX >= 0) {

							++selectStartX;
							++selectEndX;

						}

						++curLine;

						changed = true;

						if (lineEnd + 1 < getLineCount() - 1) {
							repeatKey = true;
						}

					}

				}

			}
			else {

				setScrollOffset(scrollHorOffset, ((scrollVerOffset + GUI_TEXT_EDIT_LINE_HEIGHT) / GUI_TEXT_EDIT_LINE_HEIGHT) * GUI_TEXT_EDIT_LINE_HEIGHT);

				if (scrollVerBarPos < scrollVerBarMaxPos) {
					repeatKey = true;
				}

			}

		}
		else if (_key == KEY_ENTER) {

			if (!readOnly && (textFilter == "" || StringHelper::contains(textFilter, '\n'))) {

				shouldUpdateMoveToX = true;

				std::string insertStr = "\n";

				s32 tabCount = 0;
				s32 tabCountLine = (selectStartX >= 0) ? selectStartX : curLine;
				s32 tabCountChar = (selectStartX >= 0) ? selectStartY : curChar;

				std::vector<TextEditChar>& charList = lineList[tabCountLine];

				for (s32 i = 0; i < tabCountChar; ++i) {

					TextEditChar& c = charList[i];

					if (c.type == TextEditCharType::Tab) {

						insertStr += '\t';
						++tabCount;

					}
					else {
						break;
					}

				}

				s32 lineCount = getLineCount();

				if (selectStartX >= 0) {

					if (maxLineCount < 0 || maxLineCount > 1) {

						bool singleLine = !(selectStartX != selectEndX || selectEndY == lineList[selectEndX].size());

						if (deleteText(selectStartX, selectStartY, selectEndX, selectEndY, false)) {
							changed = true;
						}

						s32 resultLine, resultChar;
						bool resultChanged;
						insertText(resultLine, resultChar, resultChanged, selectStartX, selectStartY, insertStr, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

						if (resultChanged) {
							changed = true;
						}

						if (singleLine) {

							if (getLineCount() > lineCount) {

								++curLine;
								curChar = tabCount;

							}

						}
						else {

							curLine = tabCountLine + 1;
							curChar = tabCount;

						}

						deselectText();

					}

				}
				else {

					if (maxLineCount < 0 || lineCount < maxLineCount) {

						s32 resultLine, resultChar;
						bool resultChanged;
						insertText(resultLine, resultChar, resultChanged, curLine, curChar, insertStr, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);
						
						if (resultChanged) {
							changed = true;
						}

						if (getLineCount() > lineCount) {

							++curLine;
							curChar = tabCount;

						}

					}

				}

			}

			if (checkConfirm(false)) {
				changed = true;
			}

			callOnConfirm = true;

			if (deselectOnEnter) {

				deselect(false);
				gui->textEditDeselectedWithEnter = true;

				callOnReturn = true;

			}

		}
		else if (_key == KEY_BACKSPACE) {

			if (!readOnly) {

				shouldUpdateMoveToX = true;

				if (selectStartX >= 0) {

					curLine = selectStartX;
					curChar = selectStartY;

					if (deleteText(selectStartX, selectStartY, selectEndX, selectEndY, false)) {
						changed = true;
					}

					if (curChar > 0 || curLine > 0) {
						repeatKey = true;
					}

					deselectText();

				}
				else {

					if (curChar > 0) {

						if (!keyCtrl) {

							--curChar;

							if (deleteText(curLine, curChar, curLine, curChar, false)) {
								changed = true;
							}

						}
						else {

							std::vector<TextEditChar>& charList = lineList[curLine];

							u8 state = 0;
							s32 i;
							for (i = (s32)curChar - 1; i >= 0; --i) {

								TextEditChar& c = charList[i];

								if (state == 0) {

									if (c.type == TextEditCharType::Text && isLetter((textMask != 0) ? textMask : c.c)) {
										state = 1;
									}
									else {
										state = 2;
									}

								}
								//Word.
								else if (state == 1) {

									if (c.type != TextEditCharType::Text || (c.type == TextEditCharType::Text && !isLetter((textMask != 0) ? textMask : c.c))) {
										break;
									}

								}
								//Other.
								else if (state == 2) {

									if (c.type == TextEditCharType::Text && isLetter((textMask != 0) ? textMask : c.c)) {
										break;
									}

								}

							}

							if (deleteText(curLine, (s32)i + 1, curLine, curChar - 1, false)) {
								changed = true;
							}

							curChar = (s32)i + 1;

						}

					}
					else if (curLine > 0) {

						--curLine;
						curChar = (s32)lineList[curLine].size();

						if (deleteText(curLine, curChar, curLine, curChar, false)) {
							changed = true;
						}

					}

				}

			}

		}
		else if (_key == KEY_DELETE) {

			if (!readOnly) {

				shouldUpdateMoveToX = true;

				if (selectStartX >= 0) {

					curLine = selectStartX;
					curChar = selectStartY;

					if (deleteText(selectStartX, selectStartY, selectEndX, selectEndY, false)) {
						changed = true;
					}

					deselectText();

					if (curChar < lineList[curLine].size() || curLine < getLineCount() - 1) {
						repeatKey = true;
					}

				}
				else if (curChar < lineList[curLine].size() || curLine < getLineCount() - 1) {

					if (!keyCtrl) {

						if (deleteText(curLine, curChar, curLine, curChar, false)) {
							changed = true;
						}

						if (curChar < lineList[curLine].size() || curLine < getLineCount() - 1) {
							repeatKey = true;
						}

					}
					else {

						std::vector<TextEditChar>& charList = lineList[curLine];

						u8 state = 0;
						s32 i;
						for (i = curChar; i < charList.size(); ++i) {

							TextEditChar& c = charList[i];

							if (state == 0) {

								if (c.type == TextEditCharType::Text && isLetter((textMask != 0) ? textMask : c.c)) {
									state = 1;
								}
								else {
									state = 2;
								}

							}
							//Word.
							else if (state == 1) {

								if (c.type != TextEditCharType::Text || (c.type == TextEditCharType::Text && !isLetter((textMask != 0) ? textMask : c.c))) {
									break;
								}

							}
							//Other.
							else if (state == 2) {

								if (c.type == TextEditCharType::Text && isLetter((textMask != 0) ? textMask : c.c)) {
									break;
								}

							}

						}

						if (deleteText(curLine, curChar, curLine, i - 1, false)) {
							changed = true;
						}

						if (curChar < lineList[curLine].size() || curLine < getLineCount() - 1) {
							repeatKey = true;
						}

					}

				}

			}

		}
		else if (_key == KEY_TAB) {

			if (!readOnly && !numberOnly && (textFilter == "" || StringHelper::contains(textFilter, '\t'))) {
				
				shouldUpdateMoveToX = true;

				if (!keyShift) {

					if (selectStartX >= 0) {

						bool extraLine = (selectEndY == lineList[selectEndX].size());

						if (selectStartX != selectEndX || extraLine) {

							bool moveSelectStartY = false;
							bool containsSpaces = false;
							bool onlyWhiteSpace = true;

							if (selectStartY > 0) {

								for (s32 i = selectStartY - 1; i >= 0; --i) {

									TextEditChar& temp = lineList[selectStartX][i];

									if (temp.type != TextEditCharType::Tab) {

										if (temp.type == TextEditCharType::Text && temp.c != ' ') {

											onlyWhiteSpace = false;
											moveSelectStartY = true;

										}
										else {
											containsSpaces = true;
										}

									}

								}

							}

							if (!moveSelectStartY && onlyWhiteSpace && containsSpaces) {
								selectStartY = 0;
							}

							for (s32 i = selectStartX; i <= selectEndX; ++i) {

								std::string insertStr = "\t";

								s32 resultLine, resultChar;
								bool resultChanged;

								insertText(resultLine, resultChar, resultChanged, i, 0, insertStr, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

								if (resultChanged) {
									changed = true;
								}

								repeatKey = true;

							}

							if (moveSelectStartY) {

								++selectStartY;
								++curChar;

							}
							else {

								if (selectSide == TextEditSelectSide::Right && !extraLine) {
									++curChar;
								}

							}

							++selectEndY;

						}
						else {

							if (deleteText(selectStartX, selectStartY, selectEndX, selectEndY, false)) {
								changed = true;
							}

							std::string insertStr = "\t";

							s32 resultLine, resultChar;
							bool resultChanged;

							insertText(resultLine, resultChar, resultChanged, selectStartX, selectStartY, insertStr, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);
							
							if (resultChanged) {
								changed = true;
							}

							curChar = selectStartY + 1;
							repeatKey = true;

							deselectText();

						}

					}
					else {

						std::string insertStr = "\t";

						s32 resultLine, resultChar;
						bool resultChanged;

						insertText(resultLine, resultChar, resultChanged, curLine, curChar, insertStr, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

						if (resultChanged) {
							changed = true;
						}

						++curChar;

					}

				}
				else {

					if (selectStartX >= 0) {

						//@Consider: We could potentially try to realign all the lines to the closest tab.

						for (s32 i = selectStartX; i <= selectEndX; ++i) {

							std::vector<TextEditChar>& charList = lineList[i];

							if (charList.size() > 0 && charList[0].type == TextEditCharType::Tab) {

								bool moveBackStartY = false;
								 
								if (i == selectStartX && selectStartY > 0) {

									for (s32 j = 0; j < selectStartY; ++j) {

										if (charList[j].type != TextEditCharType::Tab) {

											moveBackStartY = true;
											break;

										}

									}

									if (charList[selectStartY].type != TextEditCharType::Tab) {
										moveBackStartY = true;
									}

								}

								if (moveBackStartY) {
									--selectStartY;
								}

								if (selectEndX == i && selectEndY > 0) {
									--selectEndY;
								}

								if (curLine == i && curChar > 0) {
									--curChar;
								}

								if (deleteText(i, 0, i, 0, false)) {
									changed = true;
								}

								if (charList.size() > 0 && charList[0].type == TextEditCharType::Tab) {
									repeatKey = true;
								}

							}

						}

					}
					else {

						if (curChar > 0) {
							
							std::vector<TextEditChar>& charList = lineList[curLine];

							TextEditChar& c = charList[(size_t)curChar - 1];

							if (c.type == TextEditCharType::Tab) {

								--curChar;

								if (deleteText(curLine, curChar, curLine, curChar, false)) {
									changed = true;
								}

							}
							else if (c.type == TextEditCharType::Text && c.c == ' ') {

								s32 spaceWidth = renderer->getStringWidth(fntText, ' ');
								s32 cursorSpace = (cursorX - GUI_TEXT_EDIT_HOR_SPACING - (GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH * showLineNumbers)) / spaceWidth * spaceWidth;

								s32 tabWidth = (spaceWidth * GUI_TEXT_EDIT_TAB_SIZE);
								s32 tabFloor = (cursorSpace - 1) / tabWidth;
								               
								s32 numSpaces = ((cursorSpace - (tabWidth * tabFloor)) / spaceWidth);

								s32 indEnd = curChar - 1;
								s32 indStart = curChar - 1;
								 
								while (numSpaces > 0) {

									TextEditChar& temp = charList[indStart];
									if (temp.type == TextEditCharType::Text && temp.c != ' ') {

										++indStart;
										break;

									}

									--numSpaces;

									if (numSpaces <= 0) {
										break;
									}

									--indStart;

									if (indStart < 0) {

										indStart = 0;
										break;

									}

								}
								
								curChar = indStart;

								if (deleteText(curLine, indStart, curLine, indEnd, false)) {
									changed = true;
								}

							}

						}

					}

				}

			}

		}
		else if (_key == KEY_HOME) {

			shouldUpdateMoveToX = true;

			curLinePrev = curLine;
			curCharPrev = curChar;

			if (!keyCtrl) {

				std::vector<TextEditChar>& charList = lineList[curLine];

				s32 i;
				for (i = 0; i < charList.size(); ++i) {

					if (charList[i].type != TextEditCharType::Tab) {
						break;
					}

				}

				if (curChar > i || curChar == 0) curChar = i;
				else curChar = 0;

				if (keyShift) {
					doSelectHome();
				}
				else {
					deselectText();
				}

			}
			else {

				curLine = 0;
				curChar = 0;

				if (keyShift) {
					doSelectPageUp();
				}
				else {
					deselectText();
				}

			}

		}
		else if (_key == KEY_END) {

			shouldUpdateMoveToX = true;

			curLinePrev = curLine;
			curCharPrev = curChar;

			if (!keyCtrl) {

				curChar = (s32)lineList[curLine].size();

				if (keyShift) {
					doSelectEnd();
				}
				else {
					deselectText();
				}

			}
			else {

				curLine = getLineCount() - 1;
				curChar = (s32)lineList[curLine].size();

				if (keyShift) {
					doSelectPageDown();
				}
				else {
					deselectText();
				}

			}

		}
		else if (_key == KEY_PAGE_UP) {

			shouldUpdateMoveToX = true;

			curLinePrev = curLine;
			curCharPrev = curChar;

			getClosestChar(curLine, curChar, x - (s32)roundf(scrollHorOffset) + cursorX, y - (s32)roundf(scrollVerOffset) + GUI_TEXT_EDIT_VER_SPACING + (GUI_TEXT_EDIT_LINE_HEIGHT / 2), true, false);

			if (keyShift) {
				doSelectPageUp();
			}
			else {
				deselectText();
			}

		}
		else if (_key == KEY_PAGE_DOWN) {

			shouldUpdateMoveToX = true;

			curLinePrev = curLine;
			curCharPrev = curChar;

			getClosestChar(curLine, curChar, x - (s32)roundf(scrollHorOffset) + cursorX, y - (s32)roundf(scrollVerOffset) + GUI_TEXT_EDIT_VER_SPACING + (((s32)getLineCount() - 1) * GUI_TEXT_EDIT_LINE_HEIGHT) + (GUI_TEXT_EDIT_LINE_HEIGHT / 2), true, false);

			if (keyShift) {
				doSelectPageDown();
			}
			else {
				deselectText();
			}

		}
		else if (_key == KEY_ESCAPE) {

			deselectText();

		}
		else if (keyCtrl) {

			if (_key == KEY_D) {

				if (!readOnly) {

					shouldUpdateMoveToX = true;

					if (selectStartX >= 0) {

						std::vector<TextEditChar>& charList = lineList[selectStartX];
						
						bool newLine = (selectStartX != selectEndX || selectEndY == charList.size());
						
						if (!newLine) {
							
							s32 count = 1;
							for (s32 i = selectStartY; i <= selectEndY; ++i) {

								charList.insert(charList.begin() + selectEndY + count, charList[i]);

								changed = true;

								++count;

							}

							selectStartY += (count - 1);
							selectEndY += (count - 1);
							selectSide = TextEditSelectSide::Right;
							curChar = selectEndY + 1;

							updateLineWidth(curLine);

						}
						else {
							
							if (maxLineCount < 0 || getLineCount() < maxLineCount) {
								
								bool extraLine = (selectEndY == (s32)lineList[selectEndX].size());
								s32 lineCount = extraLine;

								s32 newSelectStartY = -1;
								s32 newSelectEndY = -1;
								
								for (s32 i = selectStartX; i < selectEndX + extraLine; ++i) {

									if (maxLineCount < 0 || getLineCount() < maxLineCount) {

										if (lineCount == 0) {

											lineList.insert(lineList.begin() + selectEndX + 1, lineList[(size_t)selectEndX]);
											lineWidthList.insert(lineWidthList.begin() + selectEndX + 1, lineWidthList[(size_t)selectEndX]);
											
											for (s32 j = (s32)lineList[selectEndX].size() - 1; j > selectEndY; --j) {
												lineList[selectEndX].erase(lineList[selectEndX].begin() + j);
											}
											
											std::vector<TextEditChar>& charList = lineList[i];
											s32 length = (s32)charList.size();
											
											for (s32 j = selectStartY; j < length; ++j) {
												lineList[selectEndX].push_back(charList[j]);
											}
											
											updateLineWidth(selectEndX);

										}
										else {

											if (i == selectStartX) {

												std::vector<TextEditChar> newList;
												
												lineList.insert(lineList.begin() + selectEndX + lineCount, newList);
												lineWidthList.insert(lineWidthList.begin() + selectEndX + lineCount, 0);
												
												std::vector<TextEditChar>& charList = lineList[i];
												
												for (s32 j = selectStartY; j < charList.size(); ++j) {
													lineList[(size_t)selectEndX + (size_t)lineCount].push_back(charList[j]);
												}

												updateLineWidth(selectEndX + lineCount);

											}
											else {

												lineList.insert(lineList.begin() + selectEndX + lineCount, lineList[i]);
												lineWidthList.insert(lineWidthList.begin() + selectEndX + lineCount, lineWidthList[i]);
												
											}

										}

										changed = true;

									}
									else {
										break;
									}

									++lineCount;

								}

								selectStartX += (lineCount - extraLine);
								selectEndX += (lineCount - extraLine);
								
								if (extraLine) {

									selectStartY = 0;
									selectEndY = (s32)lineList[selectEndX].size();

								}
								else {
									selectStartY = selectEndY + 1;
								}

								curLine = (selectSide == TextEditSelectSide::Left) ? selectStartX : selectEndX + extraLine;
								curChar = (selectSide == TextEditSelectSide::Left) ? selectStartY : (extraLine ? 0 : selectEndY + 1);

							}

						}

					}
					else {

						if (maxLineCount < 0 || getLineCount() < maxLineCount) {

							++curLine;

							lineList.insert(lineList.begin() + curLine, lineList[(size_t)curLine - 1]);
							lineWidthList.insert(lineWidthList.begin() + curLine, lineWidthList[(size_t)curLine - 1]);

							changed = true;

						}

					}

				}

			}
			else if (_key == KEY_A) {

				if (!isEmpty()) {

					shouldUpdateMoveToX = true;

					s32 lineCount = getLineCount();

					bool extraLine = (lineList[(size_t)lineCount - 1].size() <= 0);

					std::vector<TextEditChar>& charList = lineList[(size_t)lineCount - 1 - (s32)extraLine];

					s32 selectLineStart = selectStartX, selectCharStart = selectStartY;
					s32 selectLineEnd = selectEndX, selectCharEnd = selectEndY;

					selectStartX = 0;
					selectStartY = 0;

					selectEndX = lineCount - 1 - (s32)extraLine;
					selectEndY = (charList.size() <= 0) ? 0 : (s32)charList.size() - (s32)(!extraLine);

					selectSide = TextEditSelectSide::Right;

					curLine = selectEndX + (s32)extraLine;
					curChar = extraLine ? 0 : selectEndY + 1;

					cursorShow = true;
					cursorTimer = 0.0f;

					if (selectLineStart != selectStartX || selectCharStart != selectStartY || selectLineEnd != selectEndX || selectCharEnd != selectEndY) {
						surfUpdate = true;
					}

				}

			}
			else if (_key == KEY_C) {

				if (selectStartX >= 0) {
					//copyText(selectStartX, selectStartY, selectEndX, selectEndY);
					copySelection();
				}
				else {
					copyText(curLine, 0, curLine, (s32)lineList[curLine].size() - 1);
				}

			}
			else if (_key == KEY_X) {

				if (!readOnly) {

					if (selectStartX >= 0) {

						shouldUpdateMoveToX = true;

						copyText(selectStartX, selectStartY, selectEndX, selectEndY);

						if (deleteText(selectStartX, selectStartY, selectEndX, selectEndY, false)) {
							changed = true;
						}

						curLine = selectStartX;
						curChar = selectStartY;

						deselectText();

					}
					else {

						std::string copyStr = getText(curLine, 0, curLine, (s32)lineList[curLine].size()) + "\n";
						Clipboard::setText(copyStr);

						if (curLine == getLineCount() - 1 && curLine > 0) {

							s32 closestLine, closestChar;
							getClosestChar(closestLine, closestChar, x - (s32)roundf(scrollHorOffset) + cursorXMoveTo, y - (s32)roundf(scrollVerOffset) + cursorY - (GUI_TEXT_EDIT_LINE_HEIGHT / 2));

							if (deleteText(curLine - 1, (s32)lineList[(size_t)curLine - 1].size(), curLine, (s32)lineList[curLine].size(), false)) {
								changed = true;
							}

							curChar = closestChar;

						}
						else {

							if (deleteText(curLine, 0, curLine, (s32)lineList[curLine].size(), false)) {
								changed = true;
							}

						}

					}

					if (curLine > 0 || (curLine == 0 && lineList[0].size() > 0)) {
						repeatKey = true;
					}

				}

			}
			else if (_key == KEY_V) {

				if (!readOnly) {

					std::string clipboardStr;
					bool clipboardHasText = Clipboard::getText(clipboardStr);

					if (clipboardHasText) {

						shouldUpdateMoveToX = true;

						if (selectStartX >= 0) {

							if (deleteText(selectStartX, selectStartY, selectEndX, selectEndY, false)) {
								changed = true;
							}

							curLine = selectStartX;
							curChar = selectStartY;

							deselectText();

						}

						s32 resultLine, resultChar;
						bool resultChanged;
						insertText(resultLine, resultChar, resultChanged, curLine, curChar, clipboardStr, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

						curLine = resultLine;
						curChar = resultChar;

						if (resultChanged) {
							changed = true;
						}

						repeatKey = true;

					}

				}

			}

		}
		else {

			if (!readOnly) {

				if (_start) {

					keyChar = _char;

					if (keyChar == 0) run = false;
					else {

						shouldUpdateMoveToX = true;

						if (selectStartX >= 0) {

							if (deleteText(selectStartX, selectStartY, selectEndX, selectEndY, false)) {
								changed = true;
							}

							curLine = selectStartX;
							curChar = selectStartY;

							deselectText();

						}

						std::string insertStr;
						insertStr += keyChar;

						s32 resultLine, resultChar;
						bool resultChanged;
						insertText(resultLine, resultChar, resultChanged, curLine, curChar, insertStr, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

						curLine = resultLine;
						curChar = resultChar;

						if (resultChanged) {
							changed = true;
						}

					}

				}
				else {

					if (keyChar != 0) {

						shouldUpdateMoveToX = true;

						if (selectStartX >= 0) {

							if (deleteText(selectStartX, selectStartY, selectEndX, selectEndY, false)) {
								changed = true;
							}

							curLine = selectStartX;
							curChar = selectStartY;

							deselectText();

						}

						std::string insertStr;
						insertStr += keyChar;

						s32 resultLine, resultChar;
						bool resultChanged;
						insertText(resultLine, resultChar, resultChanged, curLine, curChar, insertStr, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

						curLine = resultLine;
						curChar = resultChar;

						if (resultChanged) {
							changed = true;
						}

					}
					else run = false;

				}

			}
			else run = false;

		}

		if (run && _start) {

			key = _key;
			keyStart = true;
			keyPress = false;
			keyTimerStart = 0.0f;

		}
		else if (!run) {

			keyStart = false;
			keyPress = false;

		}

		bool horChange = (prevChar != curChar);
		bool verChange = (prevLine != curLine);

		if (horChange || verChange || changed) {

			if (gui->textEditSelected) {
				cursorShow = true; //We have to do this because if we press enter and it deselects we don't wanna show the cursor again.
			}

			cursorTimer = 0.0f;

			checkScrollHor();
			checkScrollVer();

			updateCursorPos();

			if (shouldUpdateMoveToX) {
				cursorXMoveTo = cursorX;
			}

		}
		
		if(!horChange && !verChange && !changed && !repeatKey) {

			keyStart = false;
			keyPress = false;

		}

		if (changed) {

			surfUpdate = true;

			textChanged = true;

			if (onTextChanged) {
				onTextChanged(this);
			}

		}

		if (callOnConfirm && onConfirm) {
			onConfirm(this);
		}

		if (callOnReturn && onReturn) {
			onReturn(this);
		}

	}
	
	void TextEdit::doSelectLeft() {

		s32 selectLineStart = selectStartX, selectCharStart = selectStartY;
		s32 selectLineEnd = selectEndX, selectCharEnd = selectEndY;

		//No selection.
		if (selectStartX == -1) {

			if (curLine != curLinePrev || curChar != curCharPrev) {

				if (curCharPrev == 0) {

					selectEndX = curLine;
					selectEndY = (s32)lineList[curLine].size();

				}
				else {

					selectEndX = curLinePrev;
					selectEndY = curCharPrev - 1;
					
				}

				selectStartX = curLine;
				selectStartY = curChar;

				selectSide = TextEditSelectSide::Left;

			}

			
		}

		//Selection already exists.
		else {

			if (selectSide == TextEditSelectSide::Left) {

				selectStartX = curLine;
				selectStartY = curChar;

			}
			else {

				if (curLine == selectStartX && curChar == selectStartY) {
					selectStartX = -1;
				}
				else if (curLine > selectStartX || (curLine == selectStartX && curChar > selectStartY)) {

					if (curChar == 0) {

						if (curLine > 0) {

							selectEndX = curLine - 1;
							selectEndY = (s32)lineList[(size_t)curLine - 1].size();

						}

					}
					else {

						selectEndX = curLine;
						selectEndY = curChar - 1;

					}

				}
				else {

					selectEndX = selectStartX;
					selectEndY = selectStartY - 1;

					selectStartX = curLine;
					selectStartY = curChar;

					selectSide = TextEditSelectSide::Left;

				}

			}

		}
		
		if (selectLineStart != selectStartX || selectCharStart != selectStartY || selectLineEnd != selectEndX || selectCharEnd != selectEndY) {
			surfUpdate = true;
		}

	}

	void TextEdit::doSelectRight() {

		s32 selectLineStart = selectStartX, selectCharStart = selectStartY;
		s32 selectLineEnd = selectEndX, selectCharEnd = selectEndY;

		//No selection.
		if (selectStartX == -1) {

			if (curLine != curLinePrev || curChar != curCharPrev) {

				selectStartX = curLinePrev;
				selectStartY = curCharPrev;

				if (curChar == 0) {

					selectEndX = curLine - 1;
					selectEndY = (s32)lineList[(size_t)curLine - 1].size();

				}
				else {

					selectEndX = curLine;
					selectEndY = curChar - 1;

				}

				selectSide = TextEditSelectSide::Right;

			}


		}

		//Selection already exists.
		else {

			if (selectSide == TextEditSelectSide::Left) {

				if (curLine == selectEndX && curChar == selectEndY + 1) {
					selectStartX = -1;
				}
				else if (curLine < selectEndX || (curLine == selectEndX && curChar <= selectEndY)) {

					selectStartX = curLine;
					selectStartY = curChar;

				}
				else {
					
					if (selectStartX == selectEndX && selectStartY == selectEndY && selectStartY == lineList[selectStartX].size()) { //Added this fix. Not 100% sure if this will cause any unexpected behaviour, but I fail to see any reason why it would.
						selectStartX = -1;
					}
					else {

						selectStartX = selectEndX;
						selectStartY = selectEndY + 1;

						selectEndX = curLine;
						selectEndY = curChar - 1;

						selectSide = TextEditSelectSide::Right;

					}

				}

			}
			else {

				if (curChar == 0) {

					if (curLinePrev < curLine) {

						selectEndX = curLine - 1;
						selectEndY = (s32)lineList[(size_t)curLine - 1].size();

					}

				}
				else {

					selectEndX = curLine;
					selectEndY = curChar - 1;

				}

			}

		}

		if (selectLineStart != selectStartX || selectCharStart != selectStartY || selectLineEnd != selectEndX || selectCharEnd != selectEndY) {
			surfUpdate = true;
		}

	}

	void TextEdit::doSelectUp() {

		s32 selectLineStart = selectStartX, selectCharStart = selectStartY;
		s32 selectLineEnd = selectEndX, selectCharEnd = selectEndY;

		//No selection.
		if (selectStartX == -1) {

			selectStartX = curLine;
			selectStartY = curChar;

			if (curCharPrev == 0) {

				selectEndX = curLine;
				selectEndY = (s32)lineList[curLine].size();

			}
			else {

				selectEndX = curLinePrev;
				selectEndY = curCharPrev - 1;
			}

			selectSide = TextEditSelectSide::Left;

		}

		//Selection already exists.
		else {

			if (selectSide == TextEditSelectSide::Left) {

				selectStartX = curLine;
				selectStartY = curChar;

			}
			else {

				if (curLine < selectStartX) {

					selectEndX = selectStartX;
					selectEndY = selectStartY - 1;

					selectStartX = curLine;
					selectStartY = curChar;

					selectSide = TextEditSelectSide::Left;

				}
				else if (curLine == selectStartX) {

					if (curChar == selectStartY) {
						selectStartX = -1;
					}
					else if (curChar < selectStartY) {

						selectEndX = selectStartX;
						selectEndY = selectStartY - 1;

						selectStartX = curLine;
						selectStartY = curChar;
						
						selectSide = TextEditSelectSide::Left;

					}
					else {

						selectEndX = curLine;
						selectEndY = curChar - 1;

					}

				}
				else {

					selectEndX = curLine;
					selectEndY = curChar - 1;

				}

			}

		}

		if (selectLineStart != selectStartX || selectCharStart != selectStartY || selectLineEnd != selectEndX || selectCharEnd != selectEndY) {
			surfUpdate = true;
		}

	}

	void TextEdit::doSelectDown() {

		s32 selectLineStart = selectStartX, selectCharStart = selectStartY;
		s32 selectLineEnd = selectEndX, selectCharEnd = selectEndY;

		//No selection.
		if (selectStartX == -1) {

			selectStartX = curLinePrev;
			selectStartY = curCharPrev;

			if (curChar == 0) {

				selectEndX = curLinePrev;
				selectEndY = (s32)lineList[curLinePrev].size();

			}
			else {

				selectEndX = curLine;
				selectEndY = curChar - 1;

			}

			selectSide = TextEditSelectSide::Right;

		}

		//Selection already exists.
		else {

			if (selectSide == TextEditSelectSide::Right) {

				if (curChar == 0) {

					selectEndX = curLinePrev;
					selectEndY = (s32)lineList[curLinePrev].size();

				}
				else {

					selectEndX = curLine;
					selectEndY = curChar - 1;

				}

			}
			else {

				if (curLine < selectEndX) {

					selectStartX = curLine;
					selectStartY = curChar;

				}
				else if (curLine == selectEndX) {

					if (curChar == selectEndY + 1) {
						selectStartX = -1;
					}
					else if (curChar < selectEndY) {

						selectStartX = curLine;
						selectStartY = curChar;

					}
					else {

						selectStartX = selectEndX;
						selectStartY = selectEndY + 1;

						selectEndX = curLine;
						selectEndY = curChar - 1;

						selectSide = TextEditSelectSide::Right;

					}

				}
				else {

					if (selectStartX == selectEndX && curChar == 0) {
						selectStartX = -1;
					}
					else {

						selectStartX = selectEndX;
						selectStartY = selectEndY + 1;

						selectEndX = curLine;
						selectEndY = curChar - 1;

						selectSide = TextEditSelectSide::Right;

					}

				}

			}

		}

		if (selectLineStart != selectStartX || selectCharStart != selectStartY || selectLineEnd != selectEndX || selectCharEnd != selectEndY) {
			surfUpdate = true;
		}

	}

	void TextEdit::doSelectPageUp() {

		s32 selectLineStart = selectStartX, selectCharStart = selectStartY;
		s32 selectLineEnd = selectEndX, selectCharEnd = selectEndY;

		//No selection.
		if (selectStartX == -1) {

			if (curLinePrev > 0) {

				selectStartX = curLine;
				selectStartY = curChar;

				if (curCharPrev == 0) {

					selectEndX = curLinePrev - 1;
					selectEndY = (s32)lineList[(size_t)curLinePrev - 1].size();

				}
				else {

					selectEndX = curLinePrev;
					selectEndY = curCharPrev - 1;

				}

				selectSide = TextEditSelectSide::Left;

			}
			else {

				if (curCharPrev > 0) {

					selectEndX = curLinePrev;
					selectEndY = curCharPrev - 1;

					selectStartX = curLine;
					selectStartY = curChar;

					selectSide = TextEditSelectSide::Left;

				}

			}

		}

		//Selection already exists.
		else {

			if (selectSide == TextEditSelectSide::Left) {

				selectStartX = curLine;
				selectStartY = curChar;

			}
			else {

				if (selectStartX > 0) {

					selectEndX = selectStartX;
					selectEndY = selectStartY - 1;

					selectStartX = curLine;
					selectStartY = curChar;

					selectSide = TextEditSelectSide::Left;

				}
				else {

					if (curLine == selectStartX && curChar == selectStartY) {
						selectStartX = -1;
					}
					else if (curChar < selectStartY) {

						selectEndX = selectStartX;
						selectEndY = selectStartY - 1;

						selectStartX = curLine;
						selectStartY = curChar;

						selectSide = TextEditSelectSide::Left;

					}
					else {

						selectEndX = curLine;
						selectEndY = curChar - 1;

					}

				}

			}

		}

		if (selectLineStart != selectStartX || selectCharStart != selectStartY || selectLineEnd != selectEndX || selectCharEnd != selectEndY) {
			surfUpdate = true;
		}

	}

	void TextEdit::doSelectPageDown() {

		s32 selectLineStart = selectStartX, selectCharStart = selectStartY;
		s32 selectLineEnd = selectEndX, selectCharEnd = selectEndY;

		s32 lineCount = getLineCount();

		//No selection.
		if (selectStartX == -1) {

			if (curLinePrev < lineCount - 1) {
				
				selectStartX = curLinePrev;
				selectStartY = curCharPrev;

				if (curChar > 0) {

					selectEndX = curLine;
					selectEndY = curChar - 1;

				}
				else {

					selectEndX = curLine - 1;
					selectEndY = (s32)lineList[(size_t)curLine - 1].size();

				}

				selectSide = TextEditSelectSide::Right;

			}
			else {

				if (curCharPrev < lineList[curLine].size()) {

					selectStartX = curLinePrev;
					selectStartY = curCharPrev;

					selectEndX = curLine;
					selectEndY = curChar - 1;

					selectSide = TextEditSelectSide::Right;

				}

			}

		}

		//Selection already exists.
		else {

			if (selectSide == TextEditSelectSide::Left) {

				if (selectEndX < lineCount - 1) {

					selectStartX = selectEndX;
					selectStartY = selectEndY + 1;

					if (curChar > 0) {

						selectEndX = curLine;
						selectEndY = curChar - 1;

					}
					else {

						selectEndX = curLine - 1;
						selectEndY = (s32)lineList[(size_t)curLine - 1].size();

					}

					selectSide = TextEditSelectSide::Right;

				}
				else {

					if (curLine == selectEndX && curChar == selectEndY + 1) {
						selectStartX = -1;
					}
					else if(curChar < selectEndY) {
						
						selectStartX = curLine;
						selectStartY = curChar;

					}
					else {

						selectStartX = selectEndX;
						selectStartY = selectEndY + 1;

						selectEndX = curLine;
						selectEndY = curChar - 1;

						selectSide = TextEditSelectSide::Right;

					}

				}

			}
			else {

				if (curChar > 0) {

					selectEndX = curLine;
					selectEndY = curChar - 1;

				}
				else {

					selectEndX = curLine - 1;
					selectEndY = (s32)lineList[(size_t)curLine - 1].size();

				}

			}

		}

		if (selectLineStart != selectStartX || selectCharStart != selectStartY || selectLineEnd != selectEndX || selectCharEnd != selectEndY) {
			surfUpdate = true;
		}

	}

	void TextEdit::doSelectHome() {

		s32 selectLineStart = selectStartX, selectCharStart = selectStartY;
		s32 selectLineEnd = selectEndX, selectCharEnd = selectEndY;

		//No selection.
		if (selectStartX == -1) {

			if (curChar != curCharPrev) {

				if (curChar < curCharPrev) {

					selectEndX = curLine;
					selectEndY = curCharPrev - 1;

					selectStartX = curLine;
					selectStartY = curChar;

					selectSide = TextEditSelectSide::Left;

				}
				else {

					selectStartX = curLine;
					selectStartY = curCharPrev;

					selectEndX = curLine;
					selectEndY = curChar - 1;

					selectSide = TextEditSelectSide::Right;

				}

			}

		}

		//Selection already exists.
		else {

			if (selectSide == TextEditSelectSide::Left) {

				if (curLine == selectEndX && curChar == selectEndY + 1) {
					selectStartX = -1;
				}
				else if (curChar <= selectEndY || curLine < selectEndX) {
					
					selectStartX = curLine;
					selectStartY = curChar;

				}
				else {

					selectStartX = selectEndX;
					selectStartY = selectEndY + 1;

					selectEndX = curLine;
					selectEndY = curChar - 1;

					selectSide = TextEditSelectSide::Right;

				}

			}
			else {

				if (curLine == selectStartX && curChar == selectStartY) {
					selectStartX = -1;
				}
				else if (curChar > selectStartY || curLine > selectStartX) {

					if (curChar > 0) {

						selectEndX = curLine;
						selectEndY = curChar - 1;

					}
					else {

						selectEndX = curLine - 1;
						selectEndY = (s32)lineList[(size_t)curLine - 1].size();

					}

				}
				else {

					selectEndX = selectStartX;
					selectEndY = selectStartY - 1;

					selectStartX = curLine;
					selectStartY = curChar;

					selectSide = TextEditSelectSide::Left;

				}

			}

		}

		if (selectLineStart != selectStartX || selectCharStart != selectStartY || selectLineEnd != selectEndX || selectCharEnd != selectEndY) {
			surfUpdate = true;
		}

	}

	void TextEdit::doSelectEnd() {

		s32 selectLineStart = selectStartX, selectCharStart = selectStartY;
		s32 selectLineEnd = selectEndX, selectCharEnd = selectEndY;

		//No selection.
		if (selectStartX == -1) {

			if (curCharPrev < lineList[curLine].size()) {

				selectStartX = curLine;
				selectStartY = curCharPrev;

				selectEndX = curLine;
				selectEndY = curChar - 1;

				selectSide = TextEditSelectSide::Right;

			}

		}

		//Selection already exists.
		else {

			if (selectSide == TextEditSelectSide::Left) {

				if (curLine == selectEndX && curChar == selectEndY + 1) {
					selectStartX = -1;
				}
				else {

					if (selectStartX < selectEndX) {

						selectStartX = curLine;
						selectStartY = curChar;

					}
					else {

						selectStartX = selectEndX;
						selectStartY = selectEndY + 1;

						selectEndX = curLine;
						selectEndY = curChar - 1;

						selectSide = TextEditSelectSide::Right;

					}

				}

			}
			else {

				if (curCharPrev < lineList[curLine].size()) {

					selectEndX = curLine;
					selectEndY = curChar - 1;

				}

			}

		}

		if (selectLineStart != selectStartX || selectCharStart != selectStartY || selectLineEnd != selectEndX || selectCharEnd != selectEndY) {
			surfUpdate = true;
		}

	}

	void TextEdit::insertText(s32 _linePos, s32 _charPos, std::string& _text, Color4f _foregroundCol, Color4f _backgroundCol, bool _callOnTextChanged) {

		s32 resultLine, resultChar;
		bool resultChanged;

		insertText(resultLine, resultChar, resultChanged, _linePos, _charPos, _text, _foregroundCol, _backgroundCol, _callOnTextChanged);
	}

	void TextEdit::insertText(s32& _resultLine, s32& _resultChar, bool& _resultChanged, s32 _linePos, s32 _charPos, std::string& _text, Color4f _foregroundCol, Color4f _backgroundCol, bool _callOnTextChanged) {
		
		bool changed = false;

		bool shouldUpdateMaxWidth = false;

		_linePos = Math::maxInt(0, _linePos);
		_charPos = Math::maxInt(0, _charPos);

		if (maxLineCount > 0 && _linePos >= maxLineCount) {
			_linePos = maxLineCount - 1;
		}

		while (_linePos + 1 > getLineCount()) {
			
			std::vector<TextEditChar> newCharList;
			lineList.push_back(newCharList);
			lineWidthList.push_back(0);

			changed = true;
			surfUpdate = true;

		}
		
		std::vector<TextEditChar>* charList = &(lineList[_linePos]);

		bool isTag = false;
		u8 tagState = 0;
		s32 tagStart = 0;
		std::string tagName = "";
		std::string tagData = "";

		s32 pointCount = 0;
		s32 negCount = 0;
		
		if (numberOnly) {

			for (s32 i = 0; i < charList->size(); ++i) {

				TextEditChar& c = charList->at(i);

				if (c.type == TextEditCharType::Text) {
					
					if (c.c == '.') {
						++pointCount;
					}
					else if (c.c == '-') {
						++negCount;
					}

				}

			}

		}

		std::vector<s32> updateList;

		s32 startPos = Math::maxInt(0, _charPos);
		s32 ind = 0;

		for (s32 i = 0; i < _text.length(); ++i) {
			
			char curChar = _text[i];

			//Is tag.
			if (isTag) {

				if (curChar == '\n' || (curChar == ']' && tagState != 1)) {

					isTag = false;

					if (textFilter == "" && !numberOnly) {

						std::string tagStr = _text.substr(tagStart, (size_t)i - (size_t)tagStart + 1);

						for (s32  j = 0; j < tagStr.length(); ++j) {

							char tagChar = tagStr[j];

							s32 pos = Math::minInt((s32)charList->size(), startPos + ind);
							++ind;

							if (tagChar == '\n') {

								if (maxLineCount < 0 || _linePos + 1 < maxLineCount) {

									++_linePos;

									pointCount = 0;
									negCount = 0;

									startPos = 0;
									_charPos = 0;
									ind = 0;

									std::vector<TextEditChar>& prevCharList = *charList;
									
									std::vector<TextEditChar> newCharList;
									lineList.insert(lineList.begin() + _linePos, newCharList);
									lineWidthList.insert(lineWidthList.begin() + _linePos, 0);
									
									charList = &(lineList[_linePos]);

									s32 s = (s32)prevCharList.size() - pos;

									if (s > 0 && lineWidthList[(size_t)_linePos - 1] == maxLineWidth) {
										shouldUpdateMaxWidth = true;
									}

									while(s > 0) {

										charList->push_back(prevCharList[pos]);
										prevCharList.erase(prevCharList.begin() + pos);

										--s;

									}
									
									if (numberOnly) {

										for (s32 i = 0; i < charList->size(); ++i) {

											TextEditChar& c = charList->at(i);

											if (c.type == TextEditCharType::Text) {

												if (c.c == '.') {
													++pointCount;
												}
												else if (c.c == '-') {
													++negCount;
												}

											}

										}

									}

									if (std::find(updateList.begin(), updateList.end(), _linePos - 1) == updateList.end()) {
										updateList.push_back(_linePos - 1);
									}

									if (std::find(updateList.begin(), updateList.end(), _linePos) == updateList.end()) {
										updateList.push_back(_linePos);
									}

								}

							}
							else {

								if (maxLineCharCount < 0 || charList->size() < maxLineCharCount) {

									TextEditChar c;

									if (tagChar == '\t') {

										c.type = TextEditCharType::Tab;
										c.colBackground = Color::toColor4(_backgroundCol);

									}
									else {

										c.type = TextEditCharType::Text;
										c.c = tagChar;
										c.colBackground = Color::toColor4(_backgroundCol);
										c.colForeground = Color::toColor4(_foregroundCol);

									}

									charList->insert(charList->begin() + pos, c);

									++_charPos;

									if (std::find(updateList.begin(), updateList.end(), _linePos) == updateList.end()) {
										updateList.push_back(_linePos);
									}

								}

							}

						}

					}

					changed = true;
					surfUpdate = true;

				}
				else {

					//Get tag name.
					if (tagState == 0) {

						if (curChar == '=') {
							++tagState;
						}
						else {
							tagName += curChar;
						}

						//Get values.
					}
					else if (tagState == 1) {

						if (curChar == ']') {

							isTag = false;

							bool error = false;

							std::vector<std::string> values;
							StringHelper::split(values, tagData, ',');

							for (std::string& val : values) {

								StringHelper::trim(val);

								if (val == "") {

									error = true;
									break;

								}

							}

							if (!error) {

								//Sprite tag.
								if (tagName == "SPR" && allowSprites && allowPastingSprites && !numberOnly) {

									if (values.size() > 0) {

										Sprite* spr = renderer->getTextureAtlasSprite(values[0]);
										if (spr != nullptr && (maxLineCharCount < 0 || charList->size() < maxLineCharCount)) {

											s32 sub = 0;

											if (values.size() > 1) {
												StringHelper::toInt(values[1], sub);
											}

											s32 pos = Math::minInt((s32)charList->size(), startPos + ind);
											++ind;

											TextEditChar c;
											c.type = TextEditCharType::Sprite;
											c.colBackground = Color::toColor4(_backgroundCol);
											c.spr = spr;
											c.sub = sub;

											charList->insert(charList->begin() + pos, c);

											changed = true;

											++_charPos;

											if (std::find(updateList.begin(), updateList.end(), _linePos) == updateList.end()) {
												updateList.push_back(_linePos);
											}

										}

									}
									else {
										error = true;
									}

								}
								else {
									error = true;
								}

							}

							//Invalid tag or tag data.
							if (error && textFilter == "" && !numberOnly) {

								std::string tagStr = _text.substr(tagStart, (size_t)i - (size_t)tagStart + 1);

								for (char tagChar : tagStr) {

									if (maxLineCharCount >= 0 && charList->size() >= maxLineCharCount) break;

									s32 pos = Math::minInt((s32)charList->size(), startPos + ind);
									++ind;

									TextEditChar c;

									if (tagChar == '\t') {

										c.type = TextEditCharType::Tab;
										c.colBackground = Color::toColor4(_backgroundCol);

									}
									else {

										c.type = TextEditCharType::Text;
										c.c = tagChar;
										c.colBackground = Color::toColor4(_backgroundCol);
										c.colForeground = Color::toColor4(_foregroundCol);

									}

									charList->insert(charList->begin() + pos, c);

									changed = true;

									++_charPos;

									if (std::find(updateList.begin(), updateList.end(), _linePos) == updateList.end()) {
										updateList.push_back(_linePos);
									}

								}

							}

						}
						else {
							tagData += curChar;
						}

					}

				}

			//Not tag.
			}
			else {

				s32 pos = 0;

				bool foundTagStart = (!numberOnly && curChar == '[' && i != _text.length() - 1);

				//Probably a nicer way of doing this but I don't give a fuck.
				if (!foundTagStart) {

					pos = Math::minInt((s32)charList->size(), startPos + ind);
					++ind;

				}

				if (foundTagStart) {

					isTag = true;
					tagStart = i;
					tagState = 0;
					tagName = "";
					tagData = "";

				}
				else if (curChar == '\n') {

					if (textFilter == "" || StringHelper::contains(textFilter, curChar)) {

						if (maxLineCount < 0 || _linePos + 1 < maxLineCount) {

							++_linePos;

							pointCount = 0;
							negCount = 0;

							startPos = 0;
							_charPos = 0;
							ind = 0;

							std::vector<TextEditChar> newCharList;
							lineList.insert(lineList.begin() + _linePos, newCharList);
							lineWidthList.insert(lineWidthList.begin() + _linePos, 0);

							charList = &(lineList[_linePos]);
							std::vector<TextEditChar>& prevCharList = lineList[(size_t)_linePos - 1];

							changed = true;
							surfUpdate = true;

							s32 j = 0;
							s32 s = (s32)(prevCharList.size() - pos);

							if (s > 0) {

								if (lineWidthList[(size_t)_linePos - 1] == maxLineWidth) {
									shouldUpdateMaxWidth = true;
								}

								if (std::find(updateList.begin(), updateList.end(), _linePos - 1) == updateList.end()) {
									updateList.push_back(_linePos - 1);
								}

								if (std::find(updateList.begin(), updateList.end(), _linePos) == updateList.end()) {
									updateList.push_back(_linePos);
								}

							}

							while (s > 0) {

								charList->insert(charList->begin() + j, prevCharList[pos]);
								prevCharList.erase(prevCharList.begin() + pos);

								changed = true;
								surfUpdate = true;

								++j;
								--s;

							}

							if (numberOnly) {

								auto it = charList->begin();
								while (it != charList->end()) {

									TextEditChar& c = *it;

									if (c.c == '.') {

										if (pointCount <= 0 && !numberRound) {

											++pointCount;
											++it;

										}
										else {
											it = charList->erase(it);
										}

									}
									else if (c.c == '-') {

										bool inRange = !(numberHasMinValue && numberMin >= 0.0f);

										if (it == charList->begin() && negCount <= 0 && inRange) {

											++negCount;
											++it;

										}
										else {
											it = charList->erase(it);
										}

									}
									else {

										if (StringHelper::isDigit(c.c)) {
											++it;
										}
										else {
											it = charList->erase(it);
										}

									}

								}

							}

						}

					}

				}
				else {
					
					if (textFilter == "" || StringHelper::contains(textFilter, curChar)) {

						bool valid = true;
						if (numberOnly) {

							if (curChar == '-') {

								if (pos > 0 || negCount > 0 || (numberHasMinValue && numberMin >= 0)) {
									valid = false;
								}

							}
							else if (curChar == '.') {

								if (pointCount > 0 || numberRound) {
									valid = false;
								}

							}
							else if (!StringHelper::isDigit(curChar)) {
								valid = false;
							}
							else {

								if (negCount > 0 && pos == 0) {
									valid = false;
								}

							}

						}

						if (valid && (maxLineCharCount < 0 || charList->size() < maxLineCharCount)) {

							if (curChar == '.') {
								++pointCount;
							}

							TextEditChar c;

							if (curChar == '\t') {

								c.type = TextEditCharType::Tab;
								c.colBackground = Color::toColor4(_backgroundCol);

							}
							else {

								c.type = TextEditCharType::Text;
								c.c = curChar;
								c.colBackground = Color::toColor4(_backgroundCol);
								c.colForeground = Color::toColor4(_foregroundCol);

							}
							
							charList->insert(charList->begin() + pos, c);
							
							++_charPos;

							changed = true;
							surfUpdate = true;

							if (std::find(updateList.begin(), updateList.end(), _linePos) == updateList.end()) {
								updateList.push_back(_linePos);
							}

						}
						else {
							--ind;
						}

					}

				}

			}

		}

		if (selectStartX >= 0) {

			selectStartX = Math::minInt(selectStartX, getLineCount() - 1);
			selectEndX = Math::minInt(selectEndX, getLineCount() - 1);

			selectStartY = Math::minInt(selectStartY, (s32)lineList[selectStartX].size());
			selectEndY = Math::minInt(selectEndY, (s32)lineList[selectEndX].size());

		}
		
		for (s32& line : updateList) {
			updateLineWidth(line);
		}
		
		if (shouldUpdateMaxWidth) {
			updateMaxLineWidth();
		}

		if (changed) {

			textChanged = true;

			if (_callOnTextChanged && onTextChanged) {
				onTextChanged(this);
			}

		}

		_resultLine = _linePos;
		_resultChar = _charPos;
		_resultChanged = changed;

	}

	void TextEdit::insertSprite(s32 _linePos, s32 _charPos, Sprite* _sprite, s32 _sub, Color4f _backgroundCol, bool _callOnTextChanged) {

		s32 resultLine, resultChar;
		bool resultChanged;

		insertSprite(resultLine, resultChar, resultChanged, _linePos, _charPos, _sprite, _sub, _backgroundCol, _callOnTextChanged);
	}

	void TextEdit::insertSprite(s32& _resultLine, s32& _resultChar, bool& _resultChanged, s32 _linePos, s32 _charPos, Sprite* _sprite, s32 _sub, Color4f _backgroundCol, bool _callOnTextChanged) {

		bool changed = false;
		bool updateMaxWidth = false;

		while (_linePos + 1 > getLineCount()) {

			std::vector<TextEditChar> newCharList;
			lineList.push_back(newCharList);
			lineWidthList.push_back(0);

			changed = true;
			surfUpdate = true;

			if (maxLineCount > 0 && getLineCount() >= maxLineCount) {
				break;
			}

		}

		if (allowSprites && !numberOnly) {

			_linePos = Math::maxInt(0, _linePos);

			if (maxLineCount > 0 && _linePos >= maxLineCount) {

				_linePos = maxLineCount - 1;
				_charPos = (s32)lineList[_linePos].size();

			}

			std::vector<TextEditChar>& charList = lineList[_linePos];

			if (maxLineCharCount < 0 || charList.size() < maxLineCharCount) {

				_charPos = Math::clampInt(_charPos, 0, (s32)charList.size());

				TextEditChar c;
				c.type = TextEditCharType::Sprite;
				c.spr = _sprite;
				c.sub = _sub;
				c.colBackground = Color::toColor4(_backgroundCol);

				charList.insert(charList.begin() + _charPos, c);

				++_charPos;

				changed = true;
				surfUpdate = true;

				updateLineWidth(_linePos);

			}

		}

		if (changed) {

			textChanged = true;

			if (_callOnTextChanged && onTextChanged) {
				onTextChanged(this);
			}

		}

		_resultLine = _linePos;
		_resultChar = _charPos;
		_resultChanged = changed;

	}

	bool TextEdit::deleteText(s32 _lineStart, s32 _charStart, s32 _lineEnd, s32 _charEnd, bool _callOnTextChanged) {

		bool changed = false;
		bool shouldUpdateMaxWidth = false;

		s32 lineCount = getLineCount();

		_lineStart = Math::clampInt(_lineStart, 0, lineCount - 1);
		_lineEnd = Math::clampInt(_lineEnd, 0, lineCount - 1);

		if (_lineStart > _lineEnd) {

			s32 prevLine = _lineEnd;
			s32 prevChar = _charEnd;

			_lineEnd = _lineStart;
			_charEnd = _charStart;

			_lineStart = prevLine;
			_charStart = prevChar;

		}
		else if (_lineStart == _lineEnd && _charStart > _charEnd) {

			s32 prevChar = _charEnd;

			_charEnd = _charStart;
			_charStart = prevChar;

		}

		std::vector<TextEditChar>& charStartList = lineList[_lineStart];
		std::vector<TextEditChar>* charEndList = &lineList[_lineEnd];

		_charStart = Math::clampInt(_charStart, 0, (s32)charStartList.size());
		_charEnd = Math::clampInt(_charEnd, 0, (s32)charEndList->size());

		if (_lineStart == _lineEnd) {

			bool newLine = (_charEnd == charEndList->size());

			s32 count = (_charEnd - _charStart + 1 - newLine);

			if (count > 0 && lineWidthList[_lineStart] == maxLineWidth) {
				shouldUpdateMaxWidth = true;
			}

			while (count > 0) {

				charStartList.erase(charStartList.begin() + _charStart);

				changed = true;
				surfUpdate = true;

				--count;

			}

			if (newLine && _lineStart < lineCount - 1) {

				charEndList = &lineList[(size_t)_lineStart + 1];

				count = (s32)charEndList->size();

				if (count > 0 && lineWidthList[(size_t)_lineStart + 1] == maxLineWidth) {
					shouldUpdateMaxWidth = true;
				}

				while (count > 0) {

					if (maxLineCharCount < 0 || charStartList.size() < maxLineCharCount) charStartList.push_back(charEndList->at(0));
					charEndList->erase(charEndList->begin());

					--count;

				}

				lineList.erase(lineList.begin() + _lineStart + 1);
				lineWidthList.erase(lineWidthList.begin() + _lineStart + 1);

				changed = true;
				surfUpdate = true;

			}

		}
		else {
			
			bool newLine = (_charEnd == charEndList->size());

			//Remove remaining characters on first line.
			s32 count = (s32)(charStartList.size() - _charStart);

			if (count > 0 && lineWidthList[_lineStart] == maxLineWidth) {
				shouldUpdateMaxWidth = true;
			}

			while (count > 0) {

				charStartList.erase(charStartList.begin() + _charStart);

				changed = true;
				surfUpdate = true;

				--count;

			}

			//Remove characters on last line and append the non-selected ones to the first line.
			for (s32 i = (s32)charEndList->size() - 1; i >= 0; --i) {

				if (i > _charEnd && (maxLineCharCount < 0 || charStartList.size() < maxLineCharCount)) {
					charStartList.insert(charStartList.begin() + _charStart, charEndList->at(i));
				}

				charEndList->erase(charEndList->begin() + i);

				if (lineWidthList[_lineEnd] == maxLineWidth) {
					shouldUpdateMaxWidth = true;
				}

				changed = true;
				surfUpdate = true;

			}

			//Delete lines.
			for (s32 i = (s32)_lineEnd; i > (s32)_lineStart; --i) {

				if (lineWidthList[i] == maxLineWidth) {
					shouldUpdateMaxWidth = true;
				}
				
				lineList.erase(lineList.begin() + i);
				lineWidthList.erase(lineWidthList.begin() + i);

				changed = true;
				surfUpdate = true;

			}

			if (newLine && _lineStart < getLineCount() - 1) {

				charEndList = &lineList[(size_t)_lineStart + 1];

				for (s32 i = (s32)charEndList->size() - 1; i >= 0; --i) {

					if (maxLineCharCount < 0 || charStartList.size() < maxLineCharCount) charStartList.push_back(charEndList->at(0));
					charEndList->erase(charEndList->begin());

				}

				if (lineWidthList[(size_t)_lineStart + 1] == maxLineWidth) {
					shouldUpdateMaxWidth = true;
				}
				
				lineList.erase(lineList.begin() + _lineStart + 1);
				lineWidthList.erase(lineWidthList.begin() + _lineStart + 1);

				changed = true;
				surfUpdate = true;

			}

		}

		if (numberOnly) {

			std::vector<TextEditChar>& charList = lineList[_lineStart];

			s32 pointCount = 0;
			s32 negCount = 0;

			auto it = charList.begin();
			while (it != charList.end()) {

				TextEditChar& c = *it;

				if (c.c == '.') {

					if (pointCount <= 0 && !numberRound) {

						++pointCount;
						++it;

					}
					else {
						it = charList.erase(it);
					}

				}
				else if (c.c == '-') {

					bool inRange = !(numberHasMinValue && numberMin >= 0.0f);

					if (it == charList.begin() && negCount <= 0 && inRange) {

						++negCount;
						++it;

					}
					else {
						it = charList.erase(it);
					}

				}
				else {

					if (StringHelper::isDigit(c.c)) {
						++it;
					}
					else {
						it = charList.erase(it);
					}

				}

			}

		}

		updateLineWidth(_lineStart);

		if (selectStartX >= 0) {

			selectStartX = Math::minInt(selectStartX, getLineCount() - 1);
			selectEndX = Math::minInt(selectEndX, getLineCount() - 1);

			selectStartY = Math::minInt(selectStartY, (s32)lineList[selectStartX].size());
			selectEndY = Math::minInt(selectEndY, (s32)lineList[selectEndX].size());

		}

		s32 prevLine = curLine;
		s32 prevChar = curChar;

		curLine = Math::clampInt(curLine, 0, getLineCount() - 1);
		curChar = Math::clampInt(curChar, 0, (s32)lineList[curLine].size());

		if (prevLine != curLine || prevChar != curChar) {

			updateCursorPos();
			cursorXMoveTo = cursorX;

		}

		if (shouldUpdateMaxWidth) {
			updateMaxLineWidth();
		}

		if (changed) {

			textChanged = true;

			if (_callOnTextChanged && onTextChanged) {
				onTextChanged(this);
			}

		}

		return changed;

	}

	bool TextEdit::deleteLine(s32 _linePos, bool _callOnTextChanged) {

		s32 lineCount = getLineCount();

		if (_linePos < 0 || _linePos >= lineCount) {

			ZIXEL_WARN("Error in TextEdit::deleteLine. '_linePos' ({}) out of range. Valid range: {} - {}.", _linePos, 0, lineCount - 1);
			return false;

		}

		if (_linePos == 0) {
			return deleteText(_linePos, 0, _linePos, getLineCharCount(_linePos), _callOnTextChanged);
		}

		return deleteText(_linePos - 1, getLineCharCount(_linePos - 1), _linePos, getLineCharCount(_linePos) - 1, _callOnTextChanged);

	}

	bool TextEdit::clearText(bool _callOnTextChanged) {

		deselectText();

		s32 lineEnd = getLineCount() - 1;
		return deleteText(0, 0, lineEnd, getLineCharCount(lineEnd) - 1, _callOnTextChanged);

	}

	void TextEdit::setText(std::string& _text, Color4f _foregroundCol, Color4f _backgroundCol, bool _callOnTextChanged) {

		bool changed = clearText(false);

		s32 resultLine, resultChar;
		bool resultChanged;
		insertText(resultLine, resultChar, resultChanged, 0, 0, _text, _foregroundCol, _backgroundCol, false);

		setCursorPos(resultLine, resultChar);

		if (_callOnTextChanged && onTextChanged && (changed || resultChanged)) {
			onTextChanged(this);
		}
		
	}

	std::string TextEdit::getText(s32 _lineStart, s32 _charStart, s32 _lineEnd, s32 _charEnd) {

		s32 lineCount = getLineCount();

		_lineStart = Math::clampInt(_lineStart, 0, lineCount - 1);
		_lineEnd = Math::clampInt(_lineEnd, 0, lineCount - 1);

		if (_lineStart > _lineEnd) {

			s32 prevLine = _lineEnd;
			s32 prevChar = _charEnd;

			_lineEnd = _lineStart;
			_charEnd = _charStart;

			_lineStart = prevLine;
			_charStart = prevChar;

		}
		else if (_lineStart == _lineEnd && _charStart > _charEnd) {

			s32 prevChar = _charEnd;

			_charEnd = _charStart;
			_charStart = prevChar;

		}

		if (_charEnd < 0 || _charStart < 0) {
			return "";
		}

		std::string copyStr = "";

		for (s32 i = _lineStart; i <= _lineEnd; ++i) {

			std::vector<TextEditChar>& charList = lineList[i];

			if (charList.size() > 0) {

				s32 start = (i == _lineStart) ? _charStart : 0;
				s32 end = (i == _lineEnd) ? _charEnd : (s32)charList.size() - 1;

				for (s32 j = start; j <= end; ++j) {

					TextEditChar& c = charList[j];

					if (c.type == TextEditCharType::Text) {
						copyStr += c.c;
					}
					else if (c.type == TextEditCharType::Sprite) {

						std::string subStr;
						StringHelper::toString(subStr, c.sub);

						copyStr += "[SPR=" + c.spr->name + "," + subStr + "]";

					}
					else if (c.type == TextEditCharType::Tab) {
						copyStr += '\t';
					}

				}

			}

			if (i < _lineEnd) {
				copyStr += '\n';
			}

		}

		return copyStr;

	}

	bool TextEdit::copyText(s32 _lineStart, s32 _charStart, s32 _lineEnd, s32 _charEnd) {

		std::string copyStr = getText(_lineStart, _charStart, _lineEnd, _charEnd);
		return Clipboard::setText(copyStr);

	}

	bool TextEdit::copySelection() {

		if (selectStartX == -1) return false;

		bool extraLine = (selectEndY == (s32)lineList[selectEndX].size());
		s32 endChar = Math::maxInt(0, selectEndY - (s32)extraLine);

		std::string copyStr = getText(selectStartX, selectStartY, selectEndX, endChar);
		if (extraLine) copyStr += "\n";
		return Clipboard::setText(copyStr);

	}

	bool TextEdit::isEmpty() {
		return (lineList.size() == 1 && lineList[0].size() <= 0);
	}

	void TextEdit::selectAll() {

		if (isEmpty()) return;

		s32 lineCount = getLineCount();

		bool extraLine = (lineList[(size_t)lineCount - 1].size() <= 0);

		std::vector<TextEditChar>& charList = lineList[(size_t)lineCount - 1 - (s32)extraLine];

		s32 prevStartX = selectStartX, prevStartY = selectStartY;
		s32 prevEndX = selectEndX, prevEndY = selectEndY;

		selectStartX = 0;
		selectStartY = 0;

		selectEndX = lineCount - 1 - (s32)extraLine;
		selectEndY = (charList.size() <= 0) ? 0 : (s32)charList.size() - (s32)(!extraLine);

		selectSide = TextEditSelectSide::Right;

		setCursorPos(selectEndX + (s32)extraLine, extraLine ? 0 : selectEndY + 1);

		cursorShow = true;
		cursorTimer = 0.0f;

		if (prevStartX != selectStartX || prevStartY != selectStartY || prevEndX != selectEndX || prevEndY != selectEndY) {
			surfUpdate = true;
		}

	}

	s32 TextEdit::getLineCount() {
		return (s32)lineList.size();
	}

	s32 TextEdit::getLineCharCount(s32 _linePos) {

		if (_linePos < 0 || _linePos >= getLineCount()) {

			ZIXEL_WARN("Error in TextEdit::getLineCharCount. _linePos={} exceeds limits {}-{}.", _linePos, 0, getLineCount() - 1);
			return 0;

		}

		return (s32)lineList[_linePos].size();

	}

	void TextEdit::getLineText(std::string& _text, s32 _linePos) {

		_text.clear();

		_linePos = Math::clampInt(_linePos, 0, getLineCount() - 1);

		std::vector<TextEditChar>& charList = lineList[_linePos];

		for (TextEditChar& c : charList) {

			if (c.type == TextEditCharType::Text) {
				_text += c.c;
			}else if (c.type == TextEditCharType::Sprite) {

				std::string subStr;
				StringHelper::toString(subStr, c.sub);

				_text += "[SPR=" + c.spr->name + "," + subStr + "]";

			}else if (c.type == TextEditCharType::Tab) {
				_text += "\t";
			}

		}
		
	}

	void TextEdit::setTextMask(char _textMask) {

		if (textMask != _textMask) {

			textMask = _textMask;
			surfUpdate = true;

			maxLineWidth = 0;
			for (s32 i = 0; i < getLineCount(); ++i) {
				updateLineWidth(i);
			}

			calculateInsideSize();
			
			s32 prevX = cursorX;
			updateCursorPos();

			cursorXMoveTo += (cursorX - prevX);

		}

	}

	void TextEdit::setTextFilter(std::string _filter) {

		textFilter = _filter;
		updateOverrideTabKey();

	}

	void TextEdit::setPlaceholderText(const std::string _placeHolderText) {

		std::string newPlaceholder = _placeHolderText;
		StringHelper::removeAll(newPlaceholder, '\n');

		if (newPlaceholder != placeholderText) {

			placeholderText = newPlaceholder;
			surfUpdate = true;

		}

	}

	void TextEdit::setPlaceholderStyle(const TextEditPlaceholderStyle _style) {

		if (_style != placeholderStyle) {

			placeholderStyle = _style;
			if (!placeholderText.empty()) surfUpdate = true;

		}

	}

	void TextEdit::setNumberOnly(bool _numberOnly) {

		numberOnly = _numberOnly;
		updateOverrideTabKey();

	}

	f64 TextEdit::getNumber(s32 _linePos, f64 _defaultValue, bool _clampValueToRange) {

		std::string text;
		getLineText(text, _linePos);

		f64 value;
		if (text != "" && StringHelper::toDouble(text, value)) {

			if (numberRound) {
				value = round(value);
			}

			if (_clampValueToRange) {

				if (numberHasMinValue && value < numberMin) {
					value = numberMin;
				}

				if (numberHasMaxValue && value > numberMax) {
					value = numberMax;
				}

			}

			return value;

		}

		return _defaultValue;

	}

	void TextEdit::setMinNumber(f64 _minNumber) {

		numberHasMinValue = true;
		numberMin = _minNumber;

		if (numberHasMaxValue && numberMax < _minNumber) {

			numberMin = numberMax;
			ZIXEL_WARN("Error in TextEdit::setMinNumber. Trying to set a minimum number '{}' that is larger than the specified maximum number '{}'.", _minNumber, numberMax);

		}

	}

	void TextEdit::setMaxNumber(f64 _maxNumber) {
		
		numberHasMaxValue = true;
		numberMax = _maxNumber;

		if (numberHasMinValue && numberMin > _maxNumber) {

			numberMax = numberMin;
			ZIXEL_WARN("Error in TextEdit::setMaxNumber. Trying to set a maximum number '{}' that is smaller than the specified minimum number '{}'.", _maxNumber, numberMin);

		}

	}

	void TextEdit::setNumberRange(f64 _minNumber, f64 _maxNumber) {

		numberHasMinValue = true;
		numberHasMaxValue = true;

		numberMin = (_minNumber < _maxNumber) ? _minNumber : _maxNumber;
		numberMax = (_minNumber < _maxNumber) ? _maxNumber : _minNumber;

		if (_minNumber > _maxNumber) {
			ZIXEL_WARN("Error in TextEdit::setNumberRange. Minimum value '{}' is larger than the maximum value '{}'.", _minNumber, _maxNumber);
		}

	}

	void TextEdit::setReadOnly(bool _readOnly) {

		readOnly = _readOnly;
		updateOverrideTabKey();

	}

	bool TextEdit::setMaxLineCharCount(s32 _maxLineCharCount, bool _callOnTextChanged) {

		if (maxLineCharCount == _maxLineCharCount) return false;

		if (_maxLineCharCount < 0 && maxLineCharCount >= 0) {

			maxLineCharCount = -1;
			return false;

		}
		
		if (_maxLineCharCount > maxLineCharCount && maxLineCharCount >= 0) {

			maxLineCharCount = _maxLineCharCount;
			return false;

		}

		maxLineCharCount = _maxLineCharCount;

		bool changed = false;

		s32 lineCount = getLineCount();
		for (s32 i = 0; i < lineCount; ++i) {

			std::vector<TextEditChar>& charList = lineList[i];

			s32 charCount = (s32)charList.size();
			if (charCount <= maxLineCharCount) continue;

			s32 d = charCount - maxLineCharCount;
			if (deleteText(i, charCount - d, i, charCount - 1, false)) changed = true;

		}

		if (changed && _callOnTextChanged && onTextChanged) {
			onTextChanged(this);
		}

		return false;

	}
	
	bool TextEdit::setMaxLineCount(s32 _maxLineCount, bool _callOnTextChanged) {

		if (_maxLineCount <= 0) {

			if (maxLineCount > 0) {

				maxLineCount = -1;
				updateCursorPos();

			}

			return false;

		}

		if (maxLineCount != _maxLineCount) {

			maxLineCount = _maxLineCount;

			s32 lineCount = getLineCount();
			if (lineCount > maxLineCount) {

				bool changed = false;

				for (s32 i = lineCount - 1; i >= maxLineCount; --i) {

					if (deleteLine(i, false)) {
						changed = true;
					}

				}

				if (changed && _callOnTextChanged && onTextChanged) {
					onTextChanged(this);
				}

				return changed;

			}

		}

		return false;

	}

	void TextEdit::setCenterCursorIfSingleLine(bool _centerIfSingleLine) {

		if (centerIfSingleLine != _centerIfSingleLine) {

			centerIfSingleLine = _centerIfSingleLine;

			if (maxLineCount == 1) {
				updateCursorPos();
			}

		}

	}

	void TextEdit::setShowLineNumbers(bool _showLineNumbers) {

		if (showLineNumbers != _showLineNumbers) {

			showLineNumbers = _showLineNumbers;
			surfUpdate = true;

			calculateInsideSize();

			s32 prevX = cursorX;
			updateCursorPos();

			cursorXMoveTo += (cursorX - prevX);

		}

	}

	void TextEdit::setShowCharPosition(bool _showCharPosition) {

		if (showCharPosition != _showCharPosition) {

			showCharPosition = _showCharPosition;
			surfUpdate = true;

			calculateInsideSize();
			updateCursorPos();

		}

	}

	void TextEdit::setFont(std::string _font) {

		Font* font = renderer->getTextureAtlasFont(_font);
		if (font == nullptr || font == fntText) return;

		fntText = font;

		s32 lineCount = getLineCount();
		for (s32 i = 0; i < lineCount; ++i) updateLineWidth(i);

		updateMaxLineWidth();
		calculateInsideSize();
		updateCursorPos();

		surfUpdate = true;

	}

	void TextEdit::deselectText() {

		if(selectStartX != -1) {

			selectStartX = -1;
			surfUpdate = true;

		}

	}

	void TextEdit::deselect(bool _checkConfirm, bool _keepFocus) {

		if (gui->textEditSelected == this) {
			gui->textEditSelected = nullptr;

			bool changed = false;

			if (_checkConfirm) {

				if (checkConfirm(false)) {

					changed = true;

					if (onTextChanged) {
						onTextChanged(this);
					}

				}

				if (onConfirm) {
					onConfirm(this);
				}

			}

			if (deselectTextOnDeselected) {
				deselectText();
			}

			cursorShow = false;
			surfUpdate = true;

			selectMouse = 0;

		}

		gui->deselectWidget(this);

		if ((!_keepFocus || shouldUnfocus()) && gui->widgetFocused == this) {
			gui->unfocusWidget(this);
		}

	}

	void TextEdit::setOnTextChanged(std::function<void(TextEdit*)> _callback) {
		onTextChanged = _callback;
	}

	void TextEdit::setOnTextEditFocus(std::function<void(TextEdit*)> _callback) {
		onTextEditFocus = _callback;
	}

	void TextEdit::setOnConfirm(std::function<void(TextEdit*)> _callback) {
		onConfirm = _callback;
	}

	void TextEdit::setOnReturn(std::function<void(TextEdit*)> _callback) {
		onReturn = _callback;
	}

}