#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/GUI/MenuBar.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/DropDownMenu.h"

namespace Zixel {

	MenuBar::MenuBar(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::MenuBar;

		sprMenuBar = renderer->getTextureAtlasSprite(GUI_MENU_BAR_SPR_BAR);
		sprMenuBarButton = renderer->getTextureAtlasSprite(GUI_MENU_BAR_SPR_BUTTON);

		fntText = renderer->getTextureAtlasFont(GUI_MENU_BAR_FONT);

		renderFocus = false;
		fillX = true;
		height = sprMenuBar->sizeY;

	}

	MenuBar::~MenuBar() {

		for (MenuBarButton* button : buttonList) {
			delete button;
		}

	}

	void MenuBar::close() {

		if (buttonSelected != nullptr && buttonSelected->menu != nullptr) {
			buttonSelected->menu->close(false);
		}
		
		if (gui->menuBarOpen == this) {
			gui->menuBarOpen = nullptr;
		}

		justOpened = false;
		buttonSelected = nullptr;

	}

	MenuBarButton* MenuBar::addButton(std::string _name, DropDownMenu* _menu, bool _enabled) {

		MenuBarButton* button = new MenuBarButton();
		button->menu = _menu;
		button->enabled = _enabled;
		button->name = _name;
		button->width = Math::maxInt(sprMenuBarButton->sizeX, GUI_MENU_BAR_TEXT_SPACING + renderer->getStringWidth(fntText, _name));

		buttonList.push_back(button);

		updateButtonPos();

		return button;

	}

	void MenuBar::updateButtonPos() {

		s32 buttonX = GUI_MENU_BAR_TEXT_SPACING - (GUI_MENU_BAR_TEXT_SPACING / 2);

		for (MenuBarButton* button : buttonList) {

			button->x = buttonX;
			buttonX += button->width;

		}

	}

	void MenuBar::onResize(s32 prevWidth, s32 prevHeight) {

		if (buttonSelected != nullptr && buttonSelected->menu != nullptr) {
			buttonSelected->menu->setPosition(x + buttonSelected->x + GUI_MENU_BAR_DROP_DOWN_HOR_OFFSET, y + height + GUI_MENU_BAR_DROP_DOWN_VER_OFFSET);
		}

	}

	void MenuBar::update(f32 dt) {

		buttonHovered = nullptr;

		if (canInteract()) {

			if (gui->menuBarOpen != this) {
				buttonSelected = nullptr;
			}

			bool resetHoveredKeyboard = true;

			if (gui->getWidgetMouseOver() == this) {

				for (MenuBarButton* button : buttonList) {

					if (Math::pointInRect(gui->mouseX, gui->mouseY, x + button->x, y, button->width, height)) {
						
						if (!gui->isMouseOverDropDownMenu()) {

							if (buttonHoveredKeyboard == button) {
								resetHoveredKeyboard = false;
							}
							else {

								buttonHoveredKeyboard = nullptr;

								if (button->enabled) {

									buttonHovered = button;

									if (buttonSelected != button) {

										if (gui->menuBarOpen == this) {

											gui->setWidgetFocused(this);

											if (buttonSelected != nullptr && buttonSelected->menu != nullptr) {
												buttonSelected->menu->close(false);
											}

											if (button->menu != nullptr) {
												button->menu->open(x + button->x + GUI_MENU_BAR_DROP_DOWN_HOR_OFFSET, y + height + GUI_MENU_BAR_DROP_DOWN_VER_OFFSET, this);
											}

										}

										buttonSelected = button;

									}

									if (gui->menuBarOpen != this && gui->isMousePressed(MOUSE_LEFT)) {

										gui->setWidgetFocused(this);

										gui->menuBarOpen = this;
										justOpened = true;

										if (button != nullptr && button->menu != nullptr) {

											button->menu->open(x + button->x + GUI_MENU_BAR_DROP_DOWN_HOR_OFFSET, y + height + GUI_MENU_BAR_DROP_DOWN_VER_OFFSET, this);
											buttonSelected = button;

										}

									}

								}

							}

							if (gui->menuBarOpen == this && gui->isMousePressed(MOUSE_LEFT) && !justOpened) {

								gui->setWidgetFocused(this);

								buttonHoveredKeyboard = nullptr;

								if (buttonSelected != nullptr && buttonSelected->menu != nullptr && buttonSelected->menu->menuBar == this) {
									buttonSelected->menu->close();
								}

								gui->menuBarOpen = nullptr;
								buttonSelected = nullptr;

							}

							break;

						}

					}

				}

			}
			else {

				if (gui->menuBarOpen == this && buttonSelected != nullptr && buttonSelected->menu == nullptr && gui->isMousePressed(MOUSE_LEFT_RIGHT_MIDDLE)) {
					
					gui->menuBarOpen = nullptr;
					buttonSelected = nullptr;

				}

			}

			if (gui->menuBarOpen == this) {

				if (justOpened && !gui->isMouseDown(MOUSE_LEFT)) {
					justOpened = false;
				}

			}

			if (buttonHovered == nullptr && buttonHoveredKeyboard != nullptr && resetHoveredKeyboard) {
				buttonHoveredKeyboard = nullptr;
			}

		}
		else {

			gui->deselectWidget(this);

			if (buttonSelected != nullptr && buttonSelected->menu != nullptr && buttonSelected->menu->menuBar == this) {
				buttonSelected->menu->close();
			}

			buttonSelected = nullptr;
			buttonHovered = nullptr;
			buttonHoveredKeyboard = nullptr;

			if(gui->menuBarOpen == this) gui->menuBarOpen = nullptr;
			justOpened = false;

		}

	}

	void MenuBar::render() {

		renderer->cutStart(x, y, widthDraw, height);

		renderer->render3PHor(sprMenuBar, 0, x, y, widthDraw);

		for (MenuBarButton* button : buttonList) {

			if (buttonSelected == button) {
				renderer->render3PHor(sprMenuBarButton, (gui->menuBarOpen == this), x + button->x, y + (height / 2) - (sprMenuBarButton->sizeY / 2), button->width);
			}

			if (button->name != "") {

				Color4f textCol;
				if (button->enabled) textCol = GUI_MENU_BAR_TEXT_COL; else textCol = GUI_MENU_BAR_TEXT_COL_DISABLED;

				renderer->renderText(fntText, button->name, x + button->x + (button->width / 2) + GUI_MENU_BAR_TEXT_HOR_OFFSET, y + (height / 2) - 1 + GUI_MENU_BAR_TEXT_VER_OFFSET, TextAlign::Center, TextAlign::Middle, textCol);

			}

		}

		renderer->cutEnd();

	}

}