#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Shader.h"
#include "Engine/Math.h"
#include "Engine/GUI/ColorPicker.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/NumberEdit.h"
#include "Engine/GUI/LineEdit.h"

#include "App/GUI/ColorPalette.h"

namespace Zixel {

	ColorPicker::ColorPicker(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::ColorPicker;

		renderFocus = false;
		cutContent = true;

		sprAnchorCircle = renderer->getTextureAtlasSprite(GUI_COLOR_PICKER_SPR_ANCHOR_CIRCLE);
		sprAnchorTriangle = renderer->getTextureAtlasSprite(GUI_COLOR_PICKER_SPR_ANCHOR_TRIANGLE);
		sprSwap = renderer->getTextureAtlasSprite(GUI_COLOR_PICKER_SPR_SWAP);
		sprColorMode = renderer->getTextureAtlasSprite(GUI_COLOR_PICKER_SPR_COLOR_MODE);
		sprDisplayWheel = renderer->getTextureAtlasSprite(GUI_COLOR_PICKER_SPR_DISPLAY_WHEEL);
		sprDisplayChannels = renderer->getTextureAtlasSprite(GUI_COLOR_PICKER_SPR_DISPLAY_CHANNELS);

		fntText = renderer->getTextureAtlasFont(GUI_COLOR_PICKER_FONT);

		colorWheelShader = renderer->getShader("colorWheel");
		uniformWheelThickness = colorWheelShader->getUniformLocation("wheelThickness");

		colorSquareShader = renderer->getShader("colorSquare");
		uniformSquareHue = colorSquareShader->getUniformLocation("squareHue");

		colorBarShader = renderer->getShader("colorBar");
		uniformBarColorMode = colorBarShader->getUniformLocation("colorMode");
		uniformBarChannel = colorBarShader->getUniformLocation("channel");
		uniformBarColor = colorBarShader->getUniformLocation("color");

		colorPreviewShader = renderer->getShader("colorPreview");
		uniformPreviewRGBA = colorPreviewShader->getUniformLocation("rgba");

		TabGroup* tabGroup = gui->addTabGroup();

		std::string visibleStr = displayChannels ? "true" : "false";

		for (u8 i = 0; i < channelCount; ++i) {

			std::string maxNumStr = "255";

			if (i < 3) {

				if (colorMode == ColorMode::HSV) {
					maxNumStr = (i == 0) ? "360" : "100";
				}

			}

			editChannel[i] = gui->createWidget<NumberEdit>(this, "visible = " + visibleStr + "; roundNumber = true; minNumber = 0; maxNumber = " + maxNumStr + "; width = 29; height = 14; paddingLeft = 21; maxLineCharCount = 3;");
			tabGroup->addWidget(editChannel[i]);

		}

		editChannel[0]->setOnTextChanged(GUI_TEXT_EDIT_TEXT_CHANGED_CALLBACK(ColorPicker::onEditChannel0));
		editChannel[1]->setOnTextChanged(GUI_TEXT_EDIT_TEXT_CHANGED_CALLBACK(ColorPicker::onEditChannel1));
		editChannel[2]->setOnTextChanged(GUI_TEXT_EDIT_TEXT_CHANGED_CALLBACK(ColorPicker::onEditChannel2));
		editChannel[3]->setOnTextChanged(GUI_TEXT_EDIT_TEXT_CHANGED_CALLBACK(ColorPicker::onEditChannel3));

		for (u8 i = 0; i < channelCount; ++i) {
			editChannel[i]->setOnTextEditFocus(GUI_TEXT_EDIT_FOCUS_CALLBACK(ColorPicker::onEditFocus));
			editChannel[i]->setOnConfirm(GUI_TEXT_EDIT_FOCUS_CALLBACK(ColorPicker::onEditChannelConfirm));
		}

		editHex = gui->createWidget<TextEdit>(this, "visible = " + visibleStr + "; scrollHorVisible = false; scrollVerVisible = false; canScrollVer = false; maxLineCount = 1; centerCursorIfSingleLine = true; highlightCurLine = false; deselectTextOnDeselected = true; deselectOnEnter = true; filter = 0123456789ABCDEFabcdef; fillX = true; height = 14; marginLeft = 55; marginRight = 7; maxLineCharCount = 6;");
		editHex->setOnTextChanged(GUI_TEXT_EDIT_TEXT_CHANGED_CALLBACK(ColorPicker::onEditHex));
		editHex->setOnTextEditFocus(GUI_TEXT_EDIT_FOCUS_CALLBACK(ColorPicker::onEditFocus));
		editHex->setOnConfirm(GUI_TEXT_EDIT_CONFIRM_CALLBACK(ColorPicker::onEditHexConfirm));

		tabGroup->addWidget(editHex);

		updateResize();

		if (colorMode == ColorMode::HSV) {
			setHSVA(selectedColor, colorHue[(u8)selectedColor], colorSat[(u8)selectedColor], colorVal[(u8)selectedColor], colorAlpha[(u8)selectedColor], true, true, true);
		}
		else if (colorMode == ColorMode::RGB) {
			setRGBA(selectedColor, colorRed[(u8)selectedColor], colorGreen[(u8)selectedColor], colorBlue[(u8)selectedColor], colorAlpha[(u8)selectedColor], true, true, true);
		}

	}

	void ColorPicker::onResize(s32 _prevWidth, s32 _prevHeight) {
		updateResize();
	}

	void ColorPicker::update(f32 dt) {

		if (canInteract()) {

			s32 scrollY = (s32)round((f32)y - scrollVerOffset);

			if (gui->widgetSelected != this) {

				mouseOverComponent = ColorPickerComponent::None;

				if (gui->getWidgetMouseOver() == this) {

					s32 displayX = x + (viewportWidth - displayXOff - (displayWidth * displayCount) - (displayHorSpacing * (displayCount - 1)));

					//HSV color mode.
					if (Math::pointInRect(gui->mouseX, gui->mouseY, x + colorModeXOff, scrollY + colorModeYOff, colorModeWidth, colorModeHeight)) {

						gui->setTooltip(this, tooltipHSV);

						if (colorMode != ColorMode::HSV) {

							mouseOverComponent = ColorPickerComponent::ColorModeHSV;

							if (gui->isMousePressed(MOUSE_LEFT)) {

								gui->setWidgetFocused(this);

								colorMode = ColorMode::HSV;

								editChannel[0]->setMaxNumber(360.0);
								editChannel[1]->setMaxNumber(100.0);
								editChannel[2]->setMaxNumber(100.0);

								setHSVA(selectedColor, colorHue[(u8)selectedColor], colorSat[(u8)selectedColor], colorVal[(u8)selectedColor], colorAlpha[(u8)selectedColor], true, false);

							}

						}

					}

					//RGB color mode.
					else if (Math::pointInRect(gui->mouseX, gui->mouseY, x + colorModeXOff + colorModeWidth + colorModeHorSpacing , scrollY + colorModeYOff, colorModeWidth, colorModeHeight)) {

						gui->setTooltip(this, tooltipRGB);

						if (colorMode != ColorMode::RGB) {

							mouseOverComponent = ColorPickerComponent::ColorModeRGB;

							if (gui->isMousePressed(MOUSE_LEFT)) {

								gui->setWidgetFocused(this);

								colorMode = ColorMode::RGB;

								editChannel[0]->setMaxNumber(255.0);
								editChannel[1]->setMaxNumber(255.0);
								editChannel[2]->setMaxNumber(255.0);

								setRGBA(selectedColor, colorRed[(u8)selectedColor], colorGreen[(u8)selectedColor], colorBlue[(u8)selectedColor], colorAlpha[(u8)selectedColor], true, false);

							}

						}

					}

					//Display wheel.
					else if (Math::pointInRect(gui->mouseX, gui->mouseY, displayX, scrollY + displayYOff, displayWidth, displayHeight)) {

						gui->setTooltip(this, tooltipColorWheel);

						mouseOverComponent = ColorPickerComponent::DisplayWheel;

						if (gui->isMousePressed(MOUSE_LEFT)) {

							gui->setWidgetFocused(this);

							displayWheel = !displayWheel;
							updateResize();

						}

					}

					//Display channels.
					else if (Math::pointInRect(gui->mouseX, gui->mouseY, displayX + displayWidth + displayHorSpacing, scrollY + displayYOff, displayWidth, displayHeight)) {

						gui->setTooltip(this, tooltipColorChannels);

						mouseOverComponent = ColorPickerComponent::DisplayChannels;

						if (gui->isMousePressed(MOUSE_LEFT)) {

							gui->setWidgetFocused(this);

							displayChannels = !displayChannels;

							for (u8 i = 0; i < channelCount; ++i) {
								editChannel[i]->setVisible(displayChannels);
							}

							editHex->setVisible(displayChannels);

							updateResize();

						}

					}

					//Square picker.
					else if (displayWheel && Math::pointInRect(gui->mouseX, gui->mouseY, x + (viewportWidth / 2) - (squareSize / 2), scrollY + wheelYOff + (wheelSize / 2) - (squareSize / 2), squareSize, squareSize)) {
						
						if (gui->isMousePressed(MOUSE_LEFT)) {

							gui->setWidgetSelected(this);
							mouseOverComponent = ColorPickerComponent::Square;

							s32 deltaX = Math::clampInt(gui->mouseX - (x + (viewportWidth / 2) - (squareSize / 2)), 0, squareSize - 1);
							s32 deltaY = Math::clampInt(gui->mouseY - (scrollY + wheelYOff + (wheelSize / 2) - (squareSize / 2)), 0, squareSize - 1);

							u8 newSat = (u8)(((f64)deltaX / ((f64)squareSize - 1)) * 255.0);
							u8 newVal = 255 - (u8)(((f64)deltaY / ((f64)squareSize - 1)) * 255.0);

							u8 ind = (u8)selectedColor;
							startColor = getHSVA(selectedColor);
							startColorRGBA = getRGBA(selectedColor);

							if (newSat != colorSat[ind] || newVal != colorVal[ind]) {
								setHSVA(selectedColor, colorHue[ind], newSat, newVal, colorAlpha[ind]);
							}

						}

					}

					//Hue picker.
					else if (displayWheel
						  && Math::distance2DInt(gui->mouseX, gui->mouseY, x + (viewportWidth / 2), scrollY + wheelYOff + (wheelSize / 2)) >= (wheelSize / 2) - ((s32)wheelThickness - 1)
						  && Math::distance2DInt(gui->mouseX, gui->mouseY, x + (viewportWidth / 2), scrollY + wheelYOff + (wheelSize / 2)) <= (wheelSize / 2)) {

						if (gui->isMousePressed(MOUSE_LEFT)) {

							gui->setWidgetSelected(this);
							mouseOverComponent = ColorPickerComponent::Wheel;

							f64 dir = Math::direction2DDouble(gui->mouseX, gui->mouseY, x + (viewportWidth / 2), scrollY + wheelYOff + (wheelSize / 2));

							u8 ind = (u8)selectedColor;
							startColor = getHSVA(selectedColor);
							startColorRGBA = getRGBA(selectedColor);

							u8 newHue = (u8)round(dir / 360.0 * 255.0);
							if (newHue != colorHue[ind]) {
								setHSVA(selectedColor, newHue, colorSat[ind], colorVal[ind], colorAlpha[ind]);
							}

						}

					}

					else {

						s32 channelY = scrollY + wheelYOff + ((wheelSize + channelYOff) * displayWheel);
						s32 previewY = displayChannels ? (channelY + (channelHeight * channelCount) + (channelVerSpacing * (channelCount - 1)) + hexYOff + hexHeight + previewYOff) : (scrollY + wheelYOff + ((wheelSize + previewYOff) * displayWheel));

						//Primary color selected.
						if (Math::pointInRect(gui->mouseX, gui->mouseY, x + previewXOff, previewY, previewSize, previewSize)) {
							
							gui->setTooltip(this, tooltipPrimary);

							mouseOverComponent = ColorPickerComponent::Primary;

							if (gui->isMousePressed(MOUSE_LEFT)) {

								gui->setWidgetFocused(this);
								setSelectedColor(ColorSelected::Primary);

							}

						}

						//Secondary color selected.
						else if (Math::pointInRect(gui->mouseX, gui->mouseY, x + previewXOff + previewSize + previewHorSpacing, previewY, previewSize, previewSize)) {
							
							gui->setTooltip(this, tooltipSecondary);

							mouseOverComponent = ColorPickerComponent::Secondary;

							if (gui->isMousePressed(MOUSE_LEFT)) {

								gui->setWidgetFocused(this);
								setSelectedColor(ColorSelected::Secondary);

							}

						}

						//Color swap.
						else if (Math::pointInRect(gui->mouseX, gui->mouseY, x + previewXOff + (previewSize * 2) + previewHorSpacing + previewSwapXOff - 2, previewY + (previewSize / 2) - (sprSwap->sizeY / 2) - 2, sprSwap->sizeX + 4, sprSwap->sizeY + 4)) {

							gui->setTooltip(this, tooltipSwap);

							mouseOverComponent = ColorPickerComponent::Swap;

							if (gui->isMousePressed(MOUSE_LEFT)) {
								
								gui->setWidgetFocused(this);

								u8 tempHue = colorHue[0];
								u8 tempSat = colorSat[0];
								u8 tempVal = colorVal[0];
								u8 tempRed = colorRed[0];
								u8 tempGreen = colorGreen[0];
								u8 tempBlue = colorBlue[0];
								u8 tempAlpha = colorAlpha[0];

								colorHue[0] = colorHue[1];
								colorSat[0] = colorSat[1];
								colorVal[0] = colorVal[1];
								colorRed[0] = colorRed[1];
								colorGreen[0] = colorGreen[1];
								colorBlue[0] = colorBlue[1];
								colorAlpha[0] = colorAlpha[1];

								colorHue[1] = tempHue;
								colorSat[1] = tempSat;
								colorVal[1] = tempVal;
								colorRed[1] = tempRed;
								colorGreen[1] = tempGreen;
								colorBlue[1] = tempBlue;
								colorAlpha[1] = tempAlpha;

								if (colorMode == ColorMode::HSV) {
									setHSVA(selectedColor, colorHue[(u8)selectedColor], colorSat[(u8)selectedColor], colorVal[(u8)selectedColor], colorAlpha[(u8)selectedColor]);
								}
								else if (colorMode == ColorMode::RGB) {
									setRGBA(selectedColor, colorRed[(u8)selectedColor], colorGreen[(u8)selectedColor], colorBlue[(u8)selectedColor], colorAlpha[(u8)selectedColor]);
								}

								bool changed = (colorHue[0] != colorHue[1] || colorSat[0] != colorSat[1] || colorVal[0] != colorVal[1]
											 || colorRed[0] != colorRed[1] || colorGreen[0] != colorGreen[1] || colorBlue[0] != colorBlue[1]
										     || colorAlpha[0] != colorAlpha[1]);

								if (changed) {
									if (onColorChange) onColorChange(this, true, true);
								}

								if (onColorConfirm) onColorConfirm(this, changed, changed, { tempRed, tempGreen, tempBlue, tempAlpha }, { colorRed[0], colorGreen[0], colorBlue[0], colorAlpha[0] });

							}

						}
						
						//Channel bars.
						else {

							s32 channelWidth = (viewportWidth - channelX - channelHorSpacing);

							for (u8 i = 0; i < channelCount; ++i) {

								s32 curY = channelY + ((channelHeight + channelVerSpacing) * i);

								if (Math::pointInRect(gui->mouseX, gui->mouseY, x + channelX, curY, channelWidth, channelHeight)) {

									if (gui->isMousePressed(MOUSE_LEFT)) {

										gui->setWidgetSelected(this);

										mouseOverComponent = ColorPickerComponent::Channel;
										channelSelectInd = i;

										u8 ind = (u8)selectedColor;
										u8 value = (u8)round(((f32)Math::clampInt(gui->mouseX - (x + channelX), 0, channelWidth - 1) / ((f32)channelWidth - 1.0f)) * 255.0f);

										if (colorMode == ColorMode::HSV) {

											startColor = getHSVA(selectedColor);
											startColorRGBA = getRGBA(selectedColor);

											if (i == 0) {

												if (value != colorHue[ind]) {
													setHSVA(selectedColor, value, colorSat[ind], colorVal[ind], colorAlpha[ind]);
												}

											}
											else if (i == 1) {

												if (value != colorSat[ind]) {
													setHSVA(selectedColor, colorHue[ind], value, colorVal[ind], colorAlpha[ind]);
												}

											}
											else if (i == 2) {

												if (value != colorVal[ind]) {
													setHSVA(selectedColor, colorHue[ind], colorSat[ind], value, colorAlpha[ind]);
												}

											}
											else if (i == 3) {

												if (value != colorAlpha[ind]) {
													setHSVA(selectedColor, colorHue[ind], colorSat[ind], colorVal[ind], value);
												}

											}

										}
										else if (colorMode == ColorMode::RGB) {

											startColor = getRGBA(selectedColor);
											startColorRGBA = startColor;

											if (i == 0) {

												if (value != colorRed[ind]) {
													setRGBA(selectedColor, value, colorGreen[ind], colorBlue[ind], colorAlpha[ind]);
												}

											}
											else if (i == 1) {

												if (value != colorGreen[ind]) {
													setRGBA(selectedColor, colorRed[ind], value, colorBlue[ind], colorAlpha[ind]);
												}

											}
											else if (i == 2) {

												if (value != colorBlue[ind]) {
													setRGBA(selectedColor, colorRed[ind], colorGreen[ind], value, colorAlpha[ind]);
												}

											}
											else if (i == 3) {

												if (value != colorAlpha[ind]) {
													setRGBA(selectedColor, colorRed[ind], colorGreen[ind], colorBlue[ind], value);
												}

											}

										}

									}

									break;

								}
							}

						}

					}

				}

			}
			else {

				//Square picker.
				if (mouseOverComponent == ColorPickerComponent::Square) {

					s32 deltaX = Math::clampInt(gui->mouseX - (x + (viewportWidth / 2) - (squareSize / 2)), 0, squareSize - 1);
					s32 deltaY = Math::clampInt(gui->mouseY - (scrollY + wheelYOff + (wheelSize / 2) - (squareSize / 2)), 0, squareSize - 1);

					u8 newSat = (u8)(((f64)deltaX / ((f64)squareSize - 1)) * 255.0);
					u8 newVal = 255 - (u8)(((f64)deltaY / ((f64)squareSize - 1)) * 255.0);

					if (newSat != colorSat[(u8)selectedColor] || newVal != colorVal[(u8)selectedColor]) {
						setHSVA(selectedColor, colorHue[(u8)selectedColor], newSat, newVal, colorAlpha[(u8)selectedColor]);
					}

					if (!gui->isMouseDown(MOUSE_LEFT)) {

						gui->deselectWidget(this);
						mouseOverComponent = ColorPickerComponent::None;

						checkColorConfirm();

					}

				}

				//Hue picker.
				else if (mouseOverComponent == ColorPickerComponent::Wheel) {

					f64 dir = Math::direction2DDouble(gui->mouseX, gui->mouseY, x + (viewportWidth / 2), scrollY + wheelYOff + (wheelSize / 2));

					u8 newHue = (u8)round(dir / 360.0 * 255.0);
					if (newHue != colorHue[(u8)selectedColor]) {
						setHSVA(selectedColor, newHue, colorSat[(u8)selectedColor], colorVal[(u8)selectedColor], colorAlpha[(u8)selectedColor]);
					}

					if (!gui->isMouseDown(MOUSE_LEFT)) {

						gui->deselectWidget(this);
						mouseOverComponent = ColorPickerComponent::None;

						checkColorConfirm();

					}

				}

				//Channel bars.
				else if (mouseOverComponent == ColorPickerComponent::Channel) {

					s32 channelWidth = (viewportWidth - channelX - channelHorSpacing);
					u8 value = (u8)round(((f32)Math::clampInt(gui->mouseX - (x + channelX), 0, channelWidth - 1) / ((f32)channelWidth - 1.0f)) * 255.0f);

					if (colorMode == ColorMode::HSV) {

						if (channelSelectInd == 0) {

							if (value != colorHue[(u8)selectedColor]) {
								setHSVA(selectedColor, value, colorSat[(u8)selectedColor], colorVal[(u8)selectedColor], colorAlpha[(u8)selectedColor]);
							}

						}
						else if (channelSelectInd == 1) {

							if (value != colorSat[(u8)selectedColor]) {
								setHSVA(selectedColor, colorHue[(u8)selectedColor], value, colorVal[(u8)selectedColor], colorAlpha[(u8)selectedColor]);
							}

						}
						else if (channelSelectInd == 2) {

							if (value != colorVal[(u8)selectedColor]) {
								setHSVA(selectedColor, colorHue[(u8)selectedColor], colorSat[(u8)selectedColor], value, colorAlpha[(u8)selectedColor]);
							}

						}
						else if (channelSelectInd == 3) {

							if (value != colorAlpha[(u8)selectedColor]) {
								setHSVA(selectedColor, colorHue[(u8)selectedColor], colorSat[(u8)selectedColor], colorVal[(u8)selectedColor], value);
							}

						}

					}
					else if (colorMode == ColorMode::RGB) {

						if (channelSelectInd == 0) {

							if (value != colorRed[(u8)selectedColor]) {
								setRGBA(selectedColor, value, colorGreen[(u8)selectedColor], colorBlue[(u8)selectedColor], colorAlpha[(u8)selectedColor]);
							}

						}
						else if (channelSelectInd == 1) {

							if (value != colorGreen[(u8)selectedColor]) {
								setRGBA(selectedColor, colorRed[(u8)selectedColor], value, colorBlue[(u8)selectedColor], colorAlpha[(u8)selectedColor]);
							}

						}
						else if (channelSelectInd == 2) {

							if (value != colorBlue[(u8)selectedColor]) {
								setRGBA(selectedColor, colorRed[(u8)selectedColor], colorGreen[(u8)selectedColor], value, colorAlpha[(u8)selectedColor]);
							}

						}
						else if (channelSelectInd == 3) {

							if (value != colorAlpha[(u8)selectedColor]) {
								setRGBA(selectedColor, colorRed[(u8)selectedColor], colorGreen[(u8)selectedColor], colorBlue[(u8)selectedColor], value);
							}

						}

					}

				}

				if (!gui->isMouseDown(MOUSE_LEFT)) {

					gui->deselectWidget(this);
					mouseOverComponent = ColorPickerComponent::None;

				}

			}

		}
		else {

			gui->deselectWidget(this);

			mouseOverComponent = ColorPickerComponent::None;

		}

	}

	void ColorPicker::render() {

		s32 drawY = y - (s32)round(scrollVerOffset);

		s32 wheelCenterX = x + (viewportWidth / 2);
		s32 wheelCenterY = drawY + wheelYOff + (wheelSize / 2);

		s32 squareX = wheelCenterX - (squareSize / 2);
		s32 squareY = wheelCenterY - (squareSize / 2);

		if (displayWheel) {

			//Color wheel.
			renderer->setShader(colorWheelShader);
			colorWheelShader->setUniform1f(uniformWheelThickness, wheelThickness);

			renderer->renderRect(wheelCenterX - (wheelSize / 2), drawY + wheelYOff, wheelSize, wheelSize);

			renderer->resetShader();

			//Color square.
			renderer->setShader(colorSquareShader);
			colorSquareShader->setUniform1f(uniformSquareHue, colorHue[(u8)selectedColor] / 255.0f);

			renderer->renderRect(squareX, squareY, squareSize, squareSize);

			renderer->resetShader();

		}

		//Channel bars.
		s32 channelY = drawY + wheelYOff + ((wheelSize + channelYOff) * displayWheel);
		s32 hexY = channelY + ((channelHeight + channelVerSpacing) * 3) + channelHeight + hexYOff;

		if (displayChannels) {

			renderer->setShader(colorBarShader);
			colorBarShader->setUniform1i(uniformBarColorMode, (u8)colorMode);

			if (colorMode == ColorMode::HSV) {
				colorBarShader->setUniform3f(uniformBarColor, colorHue[(u8)selectedColor] / 255.0f, colorSat[(u8)selectedColor] / 255.0f, colorVal[(u8)selectedColor] / 255.0f);
			}
			else if (colorMode == ColorMode::RGB) {
				colorBarShader->setUniform3f(uniformBarColor, colorRed[(u8)selectedColor] / 255.0f, colorGreen[(u8)selectedColor] / 255.0f, colorBlue[(u8)selectedColor] / 255.0f);
			}

			for (u8 i = 0; i < channelCount; ++i) {

				s32 curY = (channelY + ((channelHeight + channelVerSpacing) * i));

				colorBarShader->setUniform1i(uniformBarChannel, i);
				renderer->renderRect(x + channelX, curY, viewportWidth - channelX - channelHorSpacing, channelHeight);

			}

			renderer->resetShader();

			for (u8 i = 0; i < channelCount; ++i) {

				s32 curY = (channelY + ((channelHeight + channelVerSpacing) * i));

				renderer->renderSprite(sprAnchorTriangle, 0, x + channelX + (s32)round((f32)(viewportWidth - channelX - channelHorSpacing - 1) * channelPickerX[i]) - (sprAnchorTriangle->sizeX / 2), curY + channelHeight - 5);

				std::string name = "";
				s32 xOff = 0;

				if (i == 3) {
					name = "A";
				}
				else if (colorMode == ColorMode::HSV) {

					if (i == 0) {

						name = "H";
						xOff = -1;

					}
					else if (i == 1) name = "S";
					else if (i == 2) name = "V";

				}
				else if (colorMode == ColorMode::RGB) {

					if (i == 0) name = "R";
					else if (i == 1) name = "G";
					else if (i == 2) name = "B";

				}

				renderer->renderText(fntText, name, x + 7 + xOff, curY + (channelHeight / 2), TextAlign::Left, TextAlign::Middle, GUI_COLOR_PICKER_TEXT_COL);

			}

			//Hex.
			std::string name = "Hex";
			renderer->renderText(fntText, name, x + 7, hexY + (hexHeight / 2), TextAlign::Left, TextAlign::Middle, GUI_COLOR_PICKER_TEXT_COL);

		}

		//Color preview.
		s32 previewY = displayChannels ? (hexY + hexHeight + previewYOff) : (drawY + wheelYOff + ((wheelSize + previewYOff) * displayWheel));

		s32 primaryX = x + previewXOff;
		s32 secondaryX = primaryX + previewSize + previewHorSpacing;

		//Secondary.
		renderer->renderRect(secondaryX, previewY, previewSize, previewSize, COLOR_BLACK);
		renderer->renderRect(secondaryX + 1, previewY + 1, previewSize - 2, previewSize - 2);

		renderer->setShader(colorPreviewShader);
		colorPreviewShader->setUniform4f(uniformPreviewRGBA, colorRed[1] / 255.0f, colorGreen[1] / 255.0f, colorBlue[1] / 255.0f, colorAlpha[1] / 255.0f);
		renderer->renderRect(secondaryX + 2, previewY + 2, previewSize - 4, previewSize - 4, COLOR_RED);
		renderer->resetShader();

		//Primary.
		renderer->renderRect(primaryX, previewY, previewSize, previewSize, COLOR_BLACK);
		renderer->renderRect(primaryX + 1, previewY + 1, previewSize - 2, previewSize - 2);

		renderer->setShader(colorPreviewShader);
		colorPreviewShader->setUniform4f(uniformPreviewRGBA, colorRed[0] / 255.0f, colorGreen[0] / 255.0f, colorBlue[0] / 255.0f, colorAlpha[0] / 255.0f);
		renderer->renderRect(primaryX + 2, previewY + 2, previewSize - 4, previewSize - 4, COLOR_RED);
		renderer->resetShader();

		Color4f primaryUnderlineColor = GUI_COLOR_PICKER_SELECTED_COLOR_UNDERLINE_COL;
		if (selectedColor != ColorSelected::Primary) primaryUnderlineColor = GUI_COLOR_PICKER_SELECTED_COLOR_UNDERLINE_COL_HOVER;

		Color4f secondaryUnderlineColor = GUI_COLOR_PICKER_SELECTED_COLOR_UNDERLINE_COL;
		if (selectedColor != ColorSelected::Secondary) secondaryUnderlineColor = GUI_COLOR_PICKER_SELECTED_COLOR_UNDERLINE_COL_HOVER;

		if (selectedColor == ColorSelected::Primary || mouseOverComponent == ColorPickerComponent::Primary) renderer->renderRect(primaryX, previewY + previewSize + previewUnderlineYOff, previewSize, 1, primaryUnderlineColor);
		if (selectedColor == ColorSelected::Secondary || mouseOverComponent == ColorPickerComponent::Secondary) renderer->renderRect(secondaryX, previewY + previewSize + previewUnderlineYOff, previewSize, 1, secondaryUnderlineColor);

		//Preview swap.
		renderer->renderSprite(sprSwap, (mouseOverComponent == ColorPickerComponent::Swap), secondaryX + previewSize + previewSwapXOff, previewY + (previewSize / 2) - (sprSwap->sizeY / 2) - 1);

		if (displayWheel) {

			//Hue picker.
			renderer->renderSprite(sprAnchorCircle, 0, wheelCenterX + huePickerX - (sprAnchorCircle->sizeX / 2), wheelCenterY + huePickerY - (sprAnchorCircle->sizeY / 2));

			//Square picker.
			renderer->renderSprite(sprAnchorCircle, 0, squareX + squarePickerX - (sprAnchorCircle->sizeX / 2), squareY + squarePickerY - (sprAnchorCircle->sizeY / 2));

		}

		//Display options.
		for (u8 i = 0; i < displayCount; ++i) {

			s32 displayX = x + (viewportWidth - displayXOff - (displayWidth * displayCount) - (displayHorSpacing * (displayCount - 1)) + (i * (displayWidth + displayHorSpacing)));

			bool displayBackground = false;
			bool displayMouseOver = false;

			if (i == 0) {

				displayBackground = displayWheel;
				displayMouseOver = (mouseOverComponent == ColorPickerComponent::DisplayWheel);

			}
			else if (i == 1) {

				displayBackground = displayChannels;
				displayMouseOver = (mouseOverComponent == ColorPickerComponent::DisplayChannels);

			}

			if (displayBackground || displayMouseOver) {
				renderer->render9P(sprColorMode, displayMouseOver, displayX, drawY + displayYOff, displayWidth, displayHeight);
			}

			if (i == 0) {
				renderer->renderSprite(sprDisplayWheel, 0, displayX + (displayWidth / 2) - (sprDisplayWheel->sizeX / 2), drawY + displayYOff + (displayHeight / 2) - (sprDisplayWheel->sizeY / 2));
			}else if (i == 1) {
				renderer->renderSprite(sprDisplayChannels, 0, displayX + (displayWidth / 2) - (sprDisplayWheel->sizeX / 2), drawY + displayYOff + (displayHeight / 2) - (sprDisplayWheel->sizeY / 2));
			}

		}

		//Color mode buttons.
		for (u8 i = 0; i < colorModeCount; ++i) {

			s32 over = (i == 0 && mouseOverComponent == ColorPickerComponent::ColorModeHSV && (u8)colorMode != i)
			        || (i == 1 && mouseOverComponent == ColorPickerComponent::ColorModeRGB && (u8)colorMode != i);

			if ((u8)colorMode == i || over) {
				renderer->render9P(sprColorMode, over, x + colorModeXOff + (i * (colorModeWidth + colorModeHorSpacing)), drawY + colorModeYOff, colorModeWidth, colorModeHeight);
			}

			std::string name = (i == 0) ? "HSV" : "RGB";
			renderer->renderText(fntText, name, x + colorModeXOff + (colorModeWidth / 2) + (i * (colorModeWidth + colorModeHorSpacing)) + 1, drawY + colorModeYOff + (colorModeHeight / 2), TextAlign::Center, TextAlign::Middle, GUI_COLOR_PICKER_TEXT_COL);

		}

	}

	void ColorPicker::updateResize() {

		s32 minHeightBeforeResize;
		if (displayWheel && !displayChannels) {
			minHeightBeforeResize = minHeightBeforeResizeWheel;
		}
		else if (!displayWheel && displayChannels) {
			minHeightBeforeResize = minHeightBeforeResizeChannels;
		}
		else if (!displayWheel && !displayChannels) {
			minHeightBeforeResize = minHeightBeforeResizeNone;
		}
		else {
			minHeightBeforeResize = minHeightBeforeResizeFull;
		}

		/*if (displayWheel && displayChannels) {
			minHeightBeforeResize = minHeightBeforeResizeFull;
		}
		else if (displayWheel) {
			minHeightBeforeResize = minHeightBeforeResizeWheel;
		}
		else if (displayChannels) {
			minHeightBeforeResize = minHeightBeforeResizeChannels;
		}
		else {
			minHeightBeforeResize = 0;
		}*/

		s32 delta = Math::maxInt(0, minHeightBeforeResize - viewportHeight);
		setWheelSize(maxWheelSize - delta);

		if (colorMode == ColorMode::HSV) {

			channelToX(channelPickerX[0], colorHue[(u8)selectedColor]);
			channelToX(channelPickerX[1], colorSat[(u8)selectedColor]);
			channelToX(channelPickerX[2], colorVal[(u8)selectedColor]);

		}
		else if (colorMode == ColorMode::RGB) {

			channelToX(channelPickerX[0], colorRed[(u8)selectedColor]);
			channelToX(channelPickerX[1], colorGreen[(u8)selectedColor]);
			channelToX(channelPickerX[2], colorBlue[(u8)selectedColor]);

		}

		channelToX(channelPickerX[3], colorAlpha[(u8)selectedColor]);

		s32 editY = wheelYOff + ((wheelSize + channelYOff) * displayWheel);

		for (u8 i = 0; i < channelCount; ++i) {
			editChannel[i]->setPadding(editChannel[i]->paddingLeft, editY + ((channelHeight + channelVerSpacing) * i), 0, 0);
		}

		s32 hexY = wheelYOff + ((wheelSize + channelYOff) * displayWheel) + (channelHeight * channelCount) + (channelVerSpacing * (channelCount - 1)) + hexYOff;
		editHex->setPadding(0, hexY, 0, 0);

		setInsideSize(0, viewportHeight + Math::maxInt(0, minHeightBeforeResize - ((maxWheelSize - minWheelSize) * displayWheel) - viewportHeight));

	}

	void ColorPicker::setWheelSize(s32 _size) {

		wheelSize = Math::maxInt(minWheelSize, _size);

		updateSquareSize();

		hueToXY(huePickerX, huePickerY, colorHue[(u8)selectedColor]);
		satValToXY(squarePickerX, squarePickerY, colorSat[(u8)selectedColor], colorVal[(u8)selectedColor]);

	}

	void ColorPicker::updateSquareSize() {

		s32 d = wheelSize - ((s32)wheelThickness * 2);
		squareSize = (s32)round(sqrt((f32)(d * d) / 2.0f));

	}

	void ColorPicker::hueToXY(s32& _x, s32& _y, s32 _hue) {

		f32 dir = ((255.0f - (f32)_hue) / 255.0f * 360.0f) + 180;
		f32 len = (((f32)wheelSize * 0.5f) - (wheelThickness * 0.5f)) + 1.0f;

		_x = (s32)(cos(dir * PI_F / 180.0f) * len);
		_y = -(s32)(sin(dir * PI_F / 180.0f) * len);

	}

	void ColorPicker::satValToXY(s32& _x, s32& _y, s32 _saturation, s32 _value) {

		f32 sat = (_saturation / 255.0f);
		f32 val = 1.0f - (_value / 255.0f);

		_x = (s32)round(sat * (squareSize - 1));
		_y = (s32)round(val * (squareSize - 1));

	}

	void ColorPicker::channelToX(f32& _x, s32 _value) {
		_x = ((f32)_value / 255.0f);
	}

	void ColorPicker::setSelectedColor(ColorSelected _selectedColor) {

		if (selectedColor != _selectedColor) {

			selectedColor = _selectedColor;

			u8 ind = (u8)selectedColor;

			if (colorMode == ColorMode::HSV) {
				setHSVA(selectedColor, colorHue[ind], colorSat[ind], colorVal[ind], colorAlpha[ind], true, true, false, false);
			}
			else if (colorMode == ColorMode::RGB) {
				setRGBA(selectedColor, colorRed[ind], colorGreen[ind], colorBlue[ind], colorAlpha[ind], true, true, false, false);
			}
		}

	}

	void ColorPicker::setInputHSVAChannels(u8 _hue, u8 _saturation, u8 _value, u8 _alpha) {

		std::string strVal = std::to_string((s32)(((f64)_hue / 255.0) * 360.0));
		editChannel[0]->setText(strVal, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

		strVal = std::to_string((s32)(((f64)_saturation / 255.0) * 100.0));
		editChannel[1]->setText(strVal, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

		strVal = std::to_string((s32)(((f64)_value / 255.0) * 100.0));
		editChannel[2]->setText(strVal, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

		strVal = std::to_string(_alpha);
		editChannel[3]->setText(strVal, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

	}

	void ColorPicker::setInputRGBAChannels(u8 _red, u8 _green, u8 _blue, u8 _alpha) {

		std::string strVal = std::to_string(_red);
		editChannel[0]->setText(strVal, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

		strVal = std::to_string(_green);
		editChannel[1]->setText(strVal, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

		strVal = std::to_string(_blue);
		editChannel[2]->setText(strVal, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

		strVal = std::to_string(_alpha);
		editChannel[3]->setText(strVal, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

	}

	void ColorPicker::setInputHex(u8 _red, u8 _green, u8 _blue) {

		std::string strVal = Color::RGBToHex({ _red, _green, _blue });
		editHex->setText(strVal, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

	}

	void ColorPicker::setHSVA(ColorSelected _selectedColor, u8 _hue, u8 _saturation, u8 _value, u8 _alpha, bool _setChannelInput, bool _setHexInput, bool _forceCalculationOfOtherModes, bool _callOnColorChange) {

		u8 ind = (u8)_selectedColor;

		u8 oldHue = colorHue[ind];
		u8 oldSat = colorSat[ind];
		u8 oldVal = colorVal[ind];

		u8 oldRed = colorRed[ind];
		u8 oldGreen = colorGreen[ind];
		u8 oldBlue = colorBlue[ind];

		u8 oldAlpha = colorAlpha[ind];

		Color3 rgb;

		colorAlpha[ind] = _alpha;

		if (_hue != oldHue || _saturation != oldSat || _value != oldVal || _forceCalculationOfOtherModes) {
			
			colorHue[ind] = _hue;
			colorSat[ind] = _saturation;
			colorVal[ind] = _value;

			rgb = Color::HSVToRGB({ _hue, _saturation, _value });
			colorRed[ind] = rgb.r;
			colorGreen[ind] = rgb.g;
			colorBlue[ind] = rgb.b;

		}
		else {
			rgb = { colorRed[ind], colorGreen[ind], colorBlue[ind] };
		}

		if (selectedColor == _selectedColor) {

			hueToXY(huePickerX, huePickerY, _hue);
			satValToXY(squarePickerX, squarePickerY, _saturation, _value);

			channelToX(channelPickerX[3], _alpha);

			if (colorMode == ColorMode::HSV) {

				channelToX(channelPickerX[0], _hue);
				channelToX(channelPickerX[1], _saturation);
				channelToX(channelPickerX[2], _value);

				if (_setChannelInput) {
					setInputHSVAChannels(_hue, _saturation, _value, _alpha);
				}

			}
			else if (colorMode == ColorMode::RGB) {

				channelToX(channelPickerX[0], rgb.r);
				channelToX(channelPickerX[1], rgb.g);
				channelToX(channelPickerX[2], rgb.b);

				if (_setChannelInput) {
					setInputRGBAChannels(rgb.r, rgb.g, rgb.b, _alpha);
				}

			}

			if (_setHexInput) {
				setInputHex(rgb.r, rgb.g, rgb.b);
			}

		}

		if (_hue != oldHue || _saturation != oldSat || _value != oldVal || rgb.r != oldRed || rgb.g != oldGreen || rgb.b != oldBlue || _alpha != oldAlpha) {
			if (onColorChange && _callOnColorChange) onColorChange(this, _selectedColor == ColorSelected::Primary, _selectedColor == ColorSelected::Secondary);
		}

	}

	void ColorPicker::setRGBA(ColorSelected _selectedColor, u8 _red, u8 _green, u8 _blue, u8 _alpha, bool _setChannelInput, bool _setHexInput, bool _forceCalculationOfOtherModes, bool _callOnColorChange) {

		u8 ind = (u8)_selectedColor;

		u8 oldRed = colorRed[ind];
		u8 oldGreen = colorGreen[ind];
		u8 oldBlue = colorBlue[ind];

		u8 oldHue = colorHue[ind];
		u8 oldSat = colorSat[ind];
		u8 oldVal = colorVal[ind];

		u8 oldAlpha = colorAlpha[ind];

		Color3 hsv;

		colorAlpha[ind] = _alpha;

		if (_red != oldRed || _green != oldGreen || _blue != oldBlue || _forceCalculationOfOtherModes) {

			colorRed[ind] = _red;
			colorGreen[ind] = _green;
			colorBlue[ind] = _blue;

			hsv = Color::RGBToHSV({ _red, _green, _blue });
			colorHue[ind] = hsv.r;
			colorSat[ind] = hsv.g;
			colorVal[ind] = hsv.b;

		}
		else {
			hsv = { colorHue[ind], colorSat[ind], colorVal[ind] };
		}

		if (selectedColor == _selectedColor) {

			hueToXY(huePickerX, huePickerY, hsv.r);
			satValToXY(squarePickerX, squarePickerY, hsv.g, hsv.b);

			channelToX(channelPickerX[3], _alpha);

			if (colorMode == ColorMode::HSV) {

				channelToX(channelPickerX[0], hsv.r);
				channelToX(channelPickerX[1], hsv.g);
				channelToX(channelPickerX[2], hsv.b);

				if (_setChannelInput) {
					setInputHSVAChannels(hsv.r, hsv.g, hsv.b, _alpha);
				}

			}
			else if (colorMode == ColorMode::RGB) {

				channelToX(channelPickerX[0], _red);
				channelToX(channelPickerX[1], _green);
				channelToX(channelPickerX[2], _blue);

				if (_setChannelInput) {
					setInputRGBAChannels(_red, _green, _blue, _alpha);
				}

			}

			if (_setHexInput) {
				setInputHex(_red, _green, _blue);
			}

		}

		if (_red != oldRed || _green != oldGreen || _blue != oldBlue || hsv.r != oldHue || hsv.g != oldSat || hsv.b != oldVal || _alpha != oldAlpha) {
			if (onColorChange && _callOnColorChange) onColorChange(this, _selectedColor == ColorSelected::Primary, _selectedColor == ColorSelected::Secondary);
		}

	}

	void ColorPicker::setData(ColorSelected _selectedColor, u8 _hue, u8 _saturation, u8 _value, u8 _red, u8 _green, u8 _blue, u8 _alpha, bool _setChannelInput, bool _setHexInput, bool _callOnColorChange) {

		u8 ind = (u8)_selectedColor;

		u8 oldHue = colorHue[ind];
		u8 oldSat = colorSat[ind];
		u8 oldVal = colorVal[ind];

		u8 oldRed = colorRed[ind];
		u8 oldGreen = colorGreen[ind];
		u8 oldBlue = colorBlue[ind];

		u8 oldAlpha = colorAlpha[ind];

		colorHue[ind] = _hue;
		colorSat[ind] = _saturation;
		colorVal[ind] = _value;
		colorRed[ind] = _red;
		colorGreen[ind] = _green;
		colorBlue[ind] = _blue;
		colorAlpha[ind] = _alpha;

		if (selectedColor == _selectedColor) {

			hueToXY(huePickerX, huePickerY, _hue);
			satValToXY(squarePickerX, squarePickerY, _saturation, _value);

			channelToX(channelPickerX[3], _alpha);

			if (colorMode == ColorMode::HSV) {

				channelToX(channelPickerX[0], _hue);
				channelToX(channelPickerX[1], _saturation);
				channelToX(channelPickerX[2], _value);

				if (_setChannelInput) {
					setInputHSVAChannels(_hue, _saturation, _value, _alpha);
				}

			}
			else if (colorMode == ColorMode::RGB) {

				channelToX(channelPickerX[0], _red);
				channelToX(channelPickerX[1], _green);
				channelToX(channelPickerX[2], _blue);

				if (_setChannelInput) {
					setInputRGBAChannels(_red, _green, _blue, _alpha);
				}

			}

			if (_setHexInput) {
				setInputHex(_red, _green, _blue);
			}

		}

		if (_hue != oldHue || _saturation != oldSat || _value != oldVal || _red != oldRed || _green != oldGreen || _blue != oldBlue || _alpha != oldAlpha) {
			if (onColorChange && _callOnColorChange) onColorChange(this, _selectedColor == ColorSelected::Primary, _selectedColor == ColorSelected::Secondary);
		}

	}

	void ColorPicker::checkColorConfirm() {
		if (!onColorConfirm) return;

		bool changed = false;

		if (colorMode == ColorMode::HSV) {
			if (!Color::match(startColor, getHSVA(selectedColor))) {
				changed = true;
			}
		}
		else if (colorMode == ColorMode::RGB) {
			if (!Color::match(startColor, getRGBA(selectedColor))) {
				changed = true;
			}
		}
		else {
			return;
		}

		Color4 colPrimary = { 0, 0, 0, 255 };
		Color4 colSecondary = { 0, 0, 0, 255 };

		if (selectedColor == ColorSelected::Primary) colPrimary = startColorRGBA;
		if (selectedColor == ColorSelected::Secondary) colSecondary = startColorRGBA;

		onColorConfirm(this, selectedColor == ColorSelected::Primary && changed, selectedColor == ColorSelected::Secondary && changed, colPrimary, colSecondary);
	}

	void ColorPicker::onEditChannel0(TextEdit* _textEdit) {

		std::string textStr;
		_textEdit->getLineText(textStr, 0);

		if (textStr != "") {

			f64 value = _textEdit->getNumber(0);

			//Hue.
			if (colorMode == ColorMode::HSV) {
				setHSVA(selectedColor, (u8)round((value / 360.0) * 255.0), colorSat[(u8)selectedColor], colorVal[(u8)selectedColor], colorAlpha[(u8)selectedColor], false);
			}

			//Red.
			else if (colorMode == ColorMode::RGB) {
				setRGBA(selectedColor, (u8)value, colorGreen[(u8)selectedColor], colorBlue[(u8)selectedColor], colorAlpha[(u8)selectedColor], false);
			}

		}

	}

	void ColorPicker::onEditChannel1(TextEdit* _textEdit) {

		std::string textStr;
		_textEdit->getLineText(textStr, 0);

		if (textStr != "") {

			f64 value = _textEdit->getNumber(0);

			//Saturation.
			if (colorMode == ColorMode::HSV) {
				setHSVA(selectedColor, colorHue[(u8)selectedColor], (u8)round((value / 100) * 255.0), colorVal[(u8)selectedColor], colorAlpha[(u8)selectedColor], false);
			}

			//Green.
			else if (colorMode == ColorMode::RGB) {
				setRGBA(selectedColor, colorRed[(u8)selectedColor], (u8)value, colorBlue[(u8)selectedColor], colorAlpha[(u8)selectedColor], false);
			}

		}

	}

	void ColorPicker::onEditChannel2(TextEdit* _textEdit) {

		std::string textStr;
		_textEdit->getLineText(textStr, 0);

		if (textStr != "") {

			f64 value = _textEdit->getNumber(0);

			//Value.
			if (colorMode == ColorMode::HSV) {
				setHSVA(selectedColor, colorHue[(u8)selectedColor], colorSat[(u8)selectedColor], (u8)round((value / 100) * 255.0), colorAlpha[(u8)selectedColor], false);
			}

			//Blue.
			else if (colorMode == ColorMode::RGB) {
				setRGBA(selectedColor, colorRed[(u8)selectedColor], colorGreen[(u8)selectedColor], (u8)value, colorAlpha[(u8)selectedColor], false);
			}

		}

	}

	void ColorPicker::onEditChannel3(TextEdit* _textEdit) {

		std::string textStr;
		_textEdit->getLineText(textStr, 0);

		//Alpha.
		if (textStr != "") {

			f64 value = _textEdit->getNumber(0);

			if (colorMode == ColorMode::HSV) {
				setHSVA(selectedColor, colorHue[(u8)selectedColor], colorSat[(u8)selectedColor], colorVal[(u8)selectedColor], (u8)value, false);
			}
			else if (colorMode == ColorMode::RGB) {
				setRGBA(selectedColor, colorRed[(u8)selectedColor], colorGreen[(u8)selectedColor], colorBlue[(u8)selectedColor], (u8)value, false);
			}

		}

	}

	void ColorPicker::onEditChannelConfirm(TextEdit* _textEdit) {
		checkColorConfirm();
	}

	void ColorPicker::onEditHex(TextEdit* _textEdit) {

		std::string textStr;
		_textEdit->getLineText(textStr, 0);

		if (textStr != "") {

			s32 delta = (s32)textStr.length() - 6;
			if (delta > 0) {

				_textEdit->deleteText(0, 6, 0, (s32)textStr.length() - 1, false);
				textStr.erase(6, delta);

			}

			Color3 rgb = Color::hexToRGB(textStr);
			setRGBA(selectedColor, rgb.r, rgb.g, rgb.b, colorAlpha[(u8)selectedColor], true, false);

		}

	}

	void ColorPicker::onEditHexConfirm(TextEdit* _textEdit) {

		std::string textStr;
		_textEdit->getLineText(textStr, 0);
		
		if (textStr == "") {

			textStr = Color::RGBToHex({ colorRed[(u8)selectedColor], colorGreen[(u8)selectedColor], colorBlue[(u8)selectedColor] });
			_textEdit->setText(textStr, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

		}
		else if (textStr.length() > 6) { //Probably redundant, but just being safe.

			s32 delta = (s32)textStr.length() - 6;

			_textEdit->deleteText(0, 6, 0, (s32)textStr.length() - 1, false);
			textStr.erase(6, delta);

		}
		else {

			while (textStr.length() < 6) {

				std::string insertStr = "0";
				_textEdit->insertText(0, _textEdit->getLineCharCount(0), insertStr, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);

				textStr += "0";

			}

		}

		std::string prevStr = textStr;
		for (size_t i = 0; i < textStr.length(); ++i) {

			char curChar = textStr[i];

			if (curChar == 'a') {
				textStr[i] = 'A';
			}
			else if (curChar == 'b') {
				textStr[i] = 'B';
			}
			else if (curChar == 'c') {
				textStr[i] = 'C';
			}
			else if (curChar == 'd') {
				textStr[i] = 'D';
			}
			else if (curChar == 'e') {
				textStr[i] = 'E';
			}
			else if (curChar == 'f') {
				textStr[i] = 'F';
			}

		}

		if (prevStr != textStr) {
			_textEdit->setText(textStr, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);
		}

		checkColorConfirm();

	}

	void ColorPicker::onEditFocus(TextEdit* _textEdit) {
		if (colorMode == ColorMode::HSV) startColor = getHSVA(selectedColor);
		else if (colorMode == ColorMode::RGB) startColor = getRGBA(selectedColor);

		startColorRGBA = getRGBA(selectedColor);
	}

	Color4 ColorPicker::getRGBA(ColorSelected _selectedColor) {
		s32 ind = (u8)_selectedColor;
		return { colorRed[ind], colorGreen[ind], colorBlue[ind], colorAlpha[ind] };
	}
	
	Color4 ColorPicker::getHSVA(ColorSelected _selectedColor) {
		s32 ind = (u8)_selectedColor;
		return { colorHue[ind], colorSat[ind], colorVal[ind], colorAlpha[ind] };
	}

	u8 ColorPicker::getAlpha(ColorSelected _selectedColor) {
		return colorAlpha[(u8)_selectedColor];
	}

	void ColorPicker::setOnColorChange(std::function<void(ColorPicker*, bool, bool)> _callback) {
		onColorChange = _callback;
	}

    void ColorPicker::setOnColorConfirm(std::function<void(ColorPicker*, bool, bool, Color4, Color4)> _callback) {
		onColorConfirm = _callback;
	}

}