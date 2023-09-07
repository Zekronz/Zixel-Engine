#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/GUI/Window.h"
#include "Engine/GUI/DockArea.h"
#include "Engine/GUI/DockContainer.h"
#include "Engine/GUI/DockTab.h"
#include "Engine/GUI/Label.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/GUIMacros.h"

namespace Zixel {

	Window::Window(GUI* _gui) {

		gui = _gui;
		renderer = _gui->renderer;

		gui->allWindowsList.push_back(this);

		sprScrollVer = renderer->getTextureAtlasSprite(GUI_SCROLL_SPR_VER);
		sprScrollHor = renderer->getTextureAtlasSprite(GUI_SCROLL_SPR_HOR);
		sprScrollCorner = renderer->getTextureAtlasSprite(GUI_SCROLL_SPR_CORNER);
		sprTitleBar = renderer->getTextureAtlasSprite(GUI_WINDOW_SPR_TITLE_BAR);
		sprContainer = renderer->getTextureAtlasSprite(GUI_WINDOW_SPR_CONTAINER);
		sprClose = renderer->getTextureAtlasSprite(GUI_WINDOW_SPR_CLOSE);
		sprMaximize = renderer->getTextureAtlasSprite(GUI_WINDOW_SPR_MAXIMIZE);

		fntTitle = renderer->getTextureAtlasFont(GUI_WINDOW_FONT);
			
		std::string dots = "...";
		actualMinWidth = Math::maxInt(Math::maxInt(sprTitleBar->sizeX, sprContainer->sizeX), (width - getAvailableTitleSpace()) + (GUI_WINDOW_TITLE_HOR_SPACING * 2) + renderer->getStringWidth(fntTitle, dots));
		actualMinHeight = sprTitleBar->sizeY + sprContainer->sizeY;

		minWidth = actualMinWidth;
		minHeight = actualMinHeight;

	}

	Window::~Window() {

		for (s32 i = (s32)widgetList.size() - 1; i >= 0; --i) {
			gui->destroyWidget(widgetList[i]);
		}

	}

	void Window::checkScrollbars() {

		s32 mouseX = gui->mouseX;
		s32 mouseY = gui->mouseY;

		s32 containerX = getContainerX();
		s32 containerY = getContainerY();
		s32 containerWidth = getContainerWidth();
		s32 containerHeight = getContainerHeight();

		if ((gui->widgetMouseOverScroll == nullptr
			|| (scrollVerShow && Math::pointInRect(mouseX, mouseY, containerX + containerWidth - sprScrollVer->sizeX, containerY, sprScrollVer->sizeX, containerHeight))
			|| (scrollHorShow && Math::pointInRect(mouseY, mouseY, containerX, containerY + containerHeight - sprScrollHor->sizeY, containerWidth, sprScrollHor->sizeY)))
			&& gui->windowMouseOver == this && Math::pointInRect(mouseX, mouseY, containerX, containerY, containerWidth, containerHeight)) {

			s32 cW = contentWidth - (containerWidth - (scrollVerShow * sprScrollVer->sizeX));
			s32 cH = contentHeight - (containerHeight - (scrollHorShow * sprScrollHor->sizeY));

			if (cW > 0 || cH > 0) {

				if (canInteract()) {

					bool update = false;

					if (scrollVerShow) {

						if (Math::pointInRect(mouseX, mouseY, containerX + containerWidth - sprScrollVer->sizeX, containerY + scrollVerBarPos, sprScrollVer->sizeX, scrollVerBarLen)) {

							scrollVerInd = 1;

							if (gui->isMousePressed(MOUSE_LEFT)) {

								gui->setWindowScroll(this);
								gui->widgetScrollDir = 0; //Widgets and windows share this.
								gui->widgetScrollMousePos = mouseY;
								gui->widgetScrollMousePosPrev = mouseY;
								gui->widgetScrollBarPos = scrollVerBarPos;

								scrollVerInd = 2;

							}

						}
						else scrollVerInd = 0;

					}

					if (scrollHorShow) {

						if (Math::pointInRect(mouseX, mouseY, containerX + scrollHorBarPos, containerY + containerHeight - sprScrollHor->sizeY, scrollHorBarLen, sprScrollHor->sizeY)) {

							scrollHorInd = 1;

							if (gui->isMousePressed(MOUSE_LEFT)) {

								gui->setWindowScroll(this);
								gui->widgetScrollDir = 1; //Widgets and windows share this.
								gui->widgetScrollMousePos = mouseX;
								gui->widgetScrollMousePosPrev = mouseX;
								gui->widgetScrollBarPos = scrollHorBarPos;

								scrollHorInd = 2;

							}

						}
						else scrollHorInd = 0;

					}

					//if (ZixUI_get_selected_widget() != id) { Pretty sure this wasn't actually needed.

					if (cH > 0 && !gui->isKeyDown(KEY_CONTROL)) {

						if (!gui->isKeyDown(KEY_SHIFT) || cW <= 0) {

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

					if (cW > 0 && !gui->isKeyDown(KEY_CONTROL)) {

						if (gui->isKeyDown(KEY_SHIFT) || cH <= 0) {

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

					scrollVerInd = 0;
					scrollHorInd = 0;

				}

			}
			else {

				scrollVerInd = 0;
				scrollHorInd = 0;

			}

		}
		else {

			scrollVerInd = 0;
			scrollHorInd = 0;

		}

	}

	void Window::moveScrollbars() {

		s32 mouseX = gui->mouseX;
		s32 mouseY = gui->mouseY;

		s32 containerX = getContainerX();
		s32 containerY = getContainerY();
		s32 containerWidth = getContainerWidth();
		s32 containerHeight = getContainerHeight();

		if (gui->widgetScrollDir == 0) {

			if (gui->widgetScrollMousePosPrev != mouseY) {

				gui->widgetScrollMousePosPrev = mouseY;

				s32 pos = scrollVerBarPos;

				scrollVerBarPos = Math::clampInt(gui->widgetScrollBarPos + (mouseY - gui->widgetScrollMousePos), 0, scrollVerBarMaxPos);
				scrollVerOffset = (((f32)scrollVerBarPos / (f32)scrollVerBarMaxPos) * (f32)(contentHeight - (containerHeight - (sprScrollHor->sizeY * scrollHorShow))));

				if (pos != scrollVerBarPos) {

					for (Widget* child : widgetList) {
						child->updateTransform(true, false);
					}

				}

			}

		}
		else {

			if (gui->widgetScrollMousePosPrev != mouseX) {

				gui->widgetScrollMousePosPrev = mouseX;

				s32 pos = scrollHorBarPos;

				scrollHorBarPos = Math::clampInt(gui->widgetScrollBarPos + (mouseX - gui->widgetScrollMousePos), 0, scrollHorBarMaxPos);
				scrollHorOffset = (((f32)scrollHorBarPos / (f32)scrollHorBarMaxPos) * (f32)(contentWidth - (containerWidth - (sprScrollVer->sizeX * scrollVerShow))));

				if (pos != scrollHorBarPos) {

					for (Widget* child : widgetList) {
						child->updateTransform(true, false);
					}

				}

			}

		}

		if (!gui->isMouseDown(MOUSE_LEFT)) {

			gui->setWindowScroll(nullptr);

			if (gui->widgetScrollDir == 0) scrollVerInd = Math::pointInRect(mouseX, mouseY, containerX + containerWidth - sprScrollVer->sizeX, containerY + scrollVerBarPos, sprScrollVer->sizeX, scrollVerBarLen);
			else scrollHorInd = Math::pointInRect(mouseX, mouseY, containerX + scrollHorBarPos, containerY + containerHeight - sprScrollHor->sizeY, scrollHorBarLen, sprScrollHor->sizeY);

		}

	}

	void Window::calculateContentSize(s32& maxX, s32& maxY) {

		contentWidth = 0;
		contentHeight = 0;

		s32 childMaxX = 0, childMaxY = 0;
		for (Widget* child : widgetList) {

			child->calculateContentSize(childMaxX, childMaxY);

			s32 cX = child->xNoScroll + child->widthDraw - 1;
			s32 cY = child->yNoScroll + child->heightDraw - 1;

			bool ignoreX = false;
			bool ignoreY = false;

			if (child->type == WidgetType::Label) {

				Label* label = (Label*)child;
				if (label->extendLineBreakToParent) ignoreX = true;

				//Labels are handled differently now and we might not need this anymore.
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

		contentWidth = Math::maxInt(0, maxX - x - GUI_WINDOW_BORDER_LEFT + 1);
		contentHeight = Math::maxInt(0, maxY - y - GUI_WINDOW_BORDER_TOP - sprTitleBar->sizeY + 1);

	}

	void Window::calculateContentSize() {

		s32 maxX = 0, maxY = 0;
		calculateContentSize(maxX, maxY);

	}

	void Window::updateScrollbars() {

		scrollVerShow = false;
		scrollHorShow = false;

		s32 containerWidth = getContainerWidth();
		s32 containerHeight = getContainerHeight();

		if (contentHeight - containerHeight > 0) {

			if (scrollVerVisible) {

				scrollVerShow = true;

				//@TODO: Recalculate content width here in case of fill_x
						//We also need to take into account the margin and the padding of the widget, as that could cause to still be outside the viewport.

				if (scrollHorVisible && contentWidth - (containerWidth - sprScrollVer->sizeX) > 0)
					scrollHorShow = true;

			}

		}
		else if (contentWidth - containerWidth > 0) {

			if (scrollHorVisible) {

				scrollHorShow = true;

				if (scrollVerVisible && contentHeight - (containerHeight - sprScrollHor->sizeY) > 0)
					scrollVerShow = true;

			}

		}

		s32 viewportWidth = containerWidth - (sprScrollVer->sizeX * scrollVerShow);
		s32 viewportHeight = containerHeight - (sprScrollHor->sizeY * scrollHorShow);

		s32 offsetChanged = false;

		if (contentHeight - viewportHeight > 0) {

			scrollVerLen = Math::maxInt(0, viewportHeight);
			scrollVerBarLen = Math::maxInt(GUI_SCROLLBAR_MIN_LENGTH, (s32)roundf((f32)scrollVerLen / ((f32)contentHeight / (f32)viewportHeight)));
			scrollVerBarMaxPos = (scrollVerLen - scrollVerBarLen);

			f32 prevOffset = scrollVerOffset;

			if (scrollVerOffset > 0.0f) {

				scrollVerOffset = Math::minFloat(scrollVerOffset, (f32)(contentHeight - viewportHeight));
				scrollVerBarPos = (s32)roundf((scrollVerOffset / (f32)(contentHeight - viewportHeight)) * (f32)scrollVerBarMaxPos);

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
			scrollHorBarLen = Math::maxInt(GUI_SCROLLBAR_MIN_LENGTH, (s32)roundf((f32)scrollHorLen / ((f32)contentWidth / (f32)viewportWidth)));
			scrollHorBarMaxPos = (scrollHorLen - scrollHorBarLen);

			f32 prevOffset = scrollHorOffset;

			if (scrollHorOffset > 0.0f) {

				scrollHorOffset = Math::minFloat(scrollHorOffset, (f32)(contentWidth - viewportWidth));
				scrollHorBarPos = (s32)roundf((scrollHorOffset / (f32)(contentWidth - viewportWidth)) * (f32)scrollHorBarMaxPos);

			}

			if (prevOffset != scrollVerOffset) {
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

	void Window::updateTransform(bool updateChildren, bool checkResize) {

		if (updateChildren) {

			for (s32 i = (s32)widgetList.size() - 1; i >= 0; --i) {

				Widget* child = widgetList[i];
				child->updateTransform(true, checkResize);

			}

		}

		calculateContentSize();

		bool showH = scrollHorShow;
		bool showV = scrollVerShow;

		scrollHorOffsetPrev = scrollHorOffset;
		scrollVerOffsetPrev = scrollVerOffset;

		updateScrollbars();

		if (showH != scrollHorShow || showV != scrollVerShow) {

			for (s32 i = (s32)widgetList.size() - 1; i >= 0; --i) {

				Widget* child = widgetList[i];
				child->updateTransform(true, checkResize);

			}

			calculateContentSize();
			updateScrollbars();

		}

	}

	bool Window::canInteract() {

		return (
			(gui->windowSelected == nullptr || gui->windowSelected == this)
			&& (!gui->hasPopupWindow() || gui->getPopupWindow() == this)
			 &&  gui->widgetSelected == nullptr && gui->widgetScroll == nullptr && gui->windowScroll == nullptr
			 && gui->dropDownMenuOpen == nullptr
			);

	}

	Window* Window::getRootWindow() {

		if (!docked) return this;

		Widget* dockAreaWidget = (Widget*)dockTab->container->dockArea;
		if (dockAreaWidget->window == nullptr) return this;
		
		return dockAreaWidget->window->getRootWindow();

	}

	void Window::moveToFront() {

		if (deleted) return;

		if (!docked) {

			const auto& it = std::find(gui->openedWindowsList.begin(), gui->openedWindowsList.end(), this);
			if (it != gui->openedWindowsList.end()) gui->openedWindowsList.erase(it);

			if (!gui->hasPopupWindow() || popup) gui->openedWindowsList.insert(gui->openedWindowsList.begin(), this);
			else {

				s32 i;
				for (i = 0; i < gui->openedWindowsList.size(); ++i) {
					if (!gui->openedWindowsList[i]->popup) break;
				}

				gui->openedWindowsList.insert(gui->openedWindowsList.begin() + i, this);

			}

		}

	}

	bool Window::isInFront(bool _includePopupWindows) {

		if (gui->openedWindowsList.size() <= 0) return false;
		if(_includePopupWindows) return (gui->openedWindowsList[0] == this);

		for (s32 i = 0; i < gui->openedWindowsList.size(); ++i) {
			if (!gui->openedWindowsList[i]->popup) return (gui->openedWindowsList[i] == this);
		}

		return false;

	}

	void Window::open(s32 _x, s32 _y, s32 _width, s32 _height, bool _popup) {

		if (deleted) return;

		if (gui->dockTabDrag != nullptr && gui->dockTabDrag->window == this) {

			delete gui->dockTabDrag;

			gui->dockTabDrag = nullptr;
			gui->dockTabDragArea = nullptr;

		}

		if (docked) {
			__close(true, true, true, false, false);
		}

		popup = _popup;

		if (!opened || !isInFront(popup)) {

			gui->stopTabbing();
			gui->stopGlobalShortcut();
			gui->stopLocalShortcut();

			moveToFront();

			if (!opened) {

				opened = true;
				maximized = false;

				setRect(_x, _y, (_width >= 0) ? _width : width, (_height >= 0) ? _height : height);
				updateTransform(true);

				for (Widget* child : widgetList) {
					child->setVisibleGlobal(true);
				}

			}

			gui->updateWindowMouseOver();

		}

	}
	
	void Window::open() {
		open(x, y, width, height, false);
	}
	void Window::openPopup() {
		open(x, y, width, height, true);
	}

	void Window::close(bool _callOnClose, bool _callOnPreClose) {
		__close(true, false, true, _callOnClose, _callOnPreClose);
	}

	void Window::__close(bool _destroyTabReference, bool _forceSetDestroyDockContainerIfZeroTabs, bool _destroyDockContainerIfZeroTabs, bool _callOnClose, bool _callOnPreClose) {

		if (_callOnPreClose && onPreClose) {
			if (!onPreClose(this)) return;
		}

		DockTab* tabToUpdate = nullptr;

		if (gui->windowSelected == this) {
			gui->windowSelected = nullptr;
		}

		if (gui->windowMouseOver == this) {

			gui->windowMouseOver = nullptr;
			gui->windowMouseOverContainer = false;

		}

		if (gui->windowScroll == this) {
			gui->windowScroll = nullptr;
		}
		
		bool wasDocked = docked;
		DockContainer* prevContainer = nullptr;

		if (!docked && opened) {

			for (s32 i = (s32)gui->openedWindowsList.size() - 1; i >= 0; --i) {

				if (gui->openedWindowsList[i] == this) {

					gui->openedWindowsList.erase(gui->openedWindowsList.begin() + i);
					break;

				}

			}

			maximized = false;
			opened = false;
			popup = false;

			if (_callOnClose && onClose) {

				bool destroyContainer = false;
				onClose(this, wasDocked, prevContainer, destroyContainer);

			}

		}
		else if(docked && opened) {

			DockContainer* container = dockTab->container;
			bool selected = (container->tabSelected == dockTab);

			auto it = std::find(container->tabList.begin(), container->tabList.end(), dockTab);
			s32 ind = (s32)(it - container->tabList.begin());

			if (it != container->tabList.end()) {

				container->tabList.erase(it);
				container->removeTabFromOrder(dockTab);

				if (_destroyTabReference) {
					delete dockTab;
				}

			}

			s32 tabCount = (s32)container->tabList.size();

			if (tabCount <= 0) {
				container->tabSelected = nullptr;
			}
			else if (selected) {

				if (container->tabOrderList.size() > 0) {
					container->tabSelected = container->tabOrderList[container->tabOrderList.size() - 1];
				}
				else {

					if (tabCount > 1) {

						if (ind > 0 && ind == tabCount) {
							ind--;
						}

						container->tabSelected = container->tabList[ind];

					}
					else {
						container->tabSelected = container->tabList[0];
					}

				}

			}

			opened = false;
			popup = false;
			maximized = false;
			docked = false;
			dockTab = nullptr;

			container->minWidth = GUI_DOCK_CONTAINER_MIN_WIDTH;
			container->minHeight = GUI_DOCK_CONTAINER_MIN_HEIGHT;

			for (DockTab* tab : container->tabList) {

				if (tab->window->minWidth > container->minWidth && tab->window->hasSetMinWidth) {
					container->minWidth = tab->window->minWidth;
				}

				if (tab->window->minHeight > container->minHeight && tab->window->hasSetMinHeight) {
					container->minHeight = tab->window->minHeight;
				}

			}

			DockArea* dockArea = container->dockArea;
			Widget* dockAreaWidget = (Widget*)dockArea;

			bool destroyContainer = true;
			if (_callOnClose && onClose) onClose(this, wasDocked, prevContainer, destroyContainer);

			if (_forceSetDestroyDockContainerIfZeroTabs) destroyContainer = _destroyDockContainerIfZeroTabs;

			if (tabCount > 0) {

				container->updateTabs();
				tabToUpdate = container->tabSelected;

			}
			else if (destroyContainer) {
				dockArea->destroyContainer(container);
			}

			if (tabCount > 0 || (tabCount <= 0 && !destroyContainer)) prevContainer = container;

			s32 minAreaWidth = dockArea->getMinAreaWidth();
			s32 minAreaHeight = dockArea->getMinAreaHeight();

			if (dockAreaWidget->widthDraw < minAreaWidth) {
				dockArea->fixHorizontal(false);
			}

			if (dockAreaWidget->heightDraw < minAreaHeight) {
				dockArea->fixVertical(false);
			}
			
			if (dockAreaWidget->widthDraw < minAreaWidth || dockAreaWidget->heightDraw < minAreaHeight) {

				dockArea->realignSplits();
				dockArea->updateAllContainerSizes();

			}

		}

		for (Widget* child : widgetList) {
			child->setVisibleGlobal(false);
		}

		if (tabToUpdate != nullptr && tabToUpdate->window->onFocus) {
			tabToUpdate->window->onFocus(tabToUpdate->window);
		}

	}

	void Window::maximize() {

		if (deleted) return;

		if (!docked) {

			if (!maximized) {

				maximized = true;

				xPrevState = x;
				yPrevState = y;

				widthPrevState = width;
				heightPrevState = height;

				s32 displayWidth, displayHeight;
				renderer->getWindowSize(displayWidth, displayHeight);

				setRect(0, 0, displayWidth, displayHeight);

			}
			else {

				maximized = false;
				setRect(xPrevState, yPrevState, widthPrevState, heightPrevState);

			}

		}

	}

	void Window::center() {

		if (!docked && !maximized) {
			setRect((renderer->windowWidth / 2) - (width / 2), (renderer->windowHeight / 2) - (height / 2));
		}

	}

	DockTab* Window::dockTo(DockContainer* _container) {

		if (deleted) return nullptr;

		__close(true, false, false, false, false);
		
		if (_container->dockArea != dockArea) {
			return nullptr;
		}
		
		if ((_container->minWidth < minWidth && hasSetMinWidth) || (_container->minHeight < minHeight && hasSetMinHeight)) {
			
			if (_container->minWidth < minWidth && hasSetMinWidth) {

				_container->minWidth = minWidth;
				dockArea->fixHorizontal(false);

			}

			if (_container->minHeight < minHeight && hasSetMinHeight) {

				_container->minHeight = minHeight;
				dockArea->fixVertical(false);

			}

			dockArea->realignSplits();
			dockArea->updateAllContainerSizes();

		}
		
		opened = true;
		maximized = false;
		docked = true;

		dockTab = _container->addTab(this);
		_container->updateTabs();

		dockTab->updateWindowRect();

		updateTransform(true);

		for (Widget* child : widgetList) {
			child->setVisibleGlobal(true);
		}

		return dockTab;

	}

	bool Window::isOpened() {
		return opened;
	}

	void Window::setOpened(bool _opened) {
		opened = _opened;
	}

	bool Window::isMaximized() {
		return maximized;
	}

	void Window::setMaximized(bool _maximized) {
		maximized = _maximized;
	}

	bool Window::isFocused() {
		if (!docked) return (gui->openedWindowsList[0] == this);
		return false;
	}

	bool Window::isDocked() {
		return docked;
	}

	void Window::setDocked(bool _docked) {
		docked = _docked;
	}

	void Window::setDockArea(DockArea* _area) {
		dockArea = _area;
	}

	void Window::setDockTab(DockTab* _tab) {
		dockTab = _tab;
	}

	void Window::setDockFixedSize(bool _fixedWidth, bool _fixedHeight) {

		dockFixedWidth = _fixedWidth;
		dockFixedHeight = _fixedHeight;

	}

	void Window::setTitle(UTF8String _title) {

		if (title == _title) return;

		title = _title;
		updateTitleDraw();

		if (docked) {
			dockTab->container->updateTabs();
		}

	}

	UTF8String Window::getTitle() {
		return title;
	}

	bool Window::isMouseOverTitle() {
		s32 titleBarHeight = sprTitleBar->sizeY;
		s32 availableSpace = Math::maxInt(0, getAvailableTitleSpace() - (GUI_WINDOW_TITLE_HOR_SPACING * 2));

		if (availableSpace <= 0) return false;

		s32 titleWidth = renderer->getStringWidth(fntTitle, titleDraw);
		s32 titleHeight = renderer->getStringHeight(fntTitle, titleDraw);

		return (Math::pointInRect(gui->mouseX, gui->mouseY, x + GUI_WINDOW_TITLE_HOR_SPACING, y + (sprTitleBar->sizeY / 2) - (titleHeight / 2), Math::minInt(titleWidth, availableSpace), titleHeight));
	}

	void Window::setTooltip(UTF8String _tooltip) {
		tooltip = _tooltip;
	}

	void Window::setSaveState(bool _enable) {

		if (_enable != saveState) {

			saveState = _enable;

			updateTitleDraw();

			if (docked) {
				
				dockTab->updateTitleShow();
				dockTab->container->updateTabs();

			}

		}

	}

	void Window::setMinimumSize(s32 _width, s32 _height) {
		
		_width = Math::clampInt((_width < 0) ? minWidth : _width, actualMinWidth, 9999);
		_height = Math::clampInt((_height < 0) ? minHeight : _height, actualMinHeight, 9999);

		if (_width != minWidth || _height != minHeight) {

			minWidth = _width;
			minHeight = _height;

			hasSetMinWidth = true;
			hasSetMinHeight = true;

			s32 newWidth = (width < minWidth) ? minWidth : width;
			s32 newHeight = (height < minHeight) ? minHeight : height;

			if (width != newWidth || height != newHeight) {
				setRect(x, y, newWidth, newHeight);
			}

		}

	}

	bool Window::setRect(s32 _x, s32 _y, s32 _width, s32 _height, bool _setFloatPos) {

		s32 xPrev = x, yPrev = y, widthPrev = width, heightPrev = height;

		if (!docked) {

			if(_width >= 0) width = Math::clampInt(_width, minWidth, 9999);
			if(_height >= 0) height = Math::clampInt(_height, minHeight, 9999);

			s32 displayWidth, displayHeight;
			renderer->getWindowSize(displayWidth, displayHeight);

			x = Math::clampInt(_x, -width + GUI_WINDOW_CLAMP_BORDER, displayWidth - GUI_WINDOW_CLAMP_BORDER);
			y = Math::clampInt(_y, 0, Math::maxInt(0, displayHeight - GUI_WINDOW_CLAMP_BORDER));
			
			if (_setFloatPos) {

				xFloat = (f32)x;
				yFloat = (f32)y;

			}

			if (xPrev != x || yPrev != y || widthPrev != width || heightPrev != height) {

				if (widthPrev != width || heightPrev != height) updateTitleDraw();
				updateTransform(true);

				//@TODO: Window resize callback.

				return true;

			}

		}
		else {

			x = _x;
			y = _y;

			if(_width >= 0) width = Math::maxInt(0, _width);
			if(_height >= 0) height = Math::maxInt(0, _height);

			if (xPrev != x || yPrev != y || widthPrev != width || heightPrev != height) {

				updateTransform(true);

				//@TODO: Window resize callback.

				return true;

			}

		}

		return false;
	}

	bool Window::setSize(s32 _width, s32 _height) {
		return setRect(x, y, _width, _height);
	}

	void Window::setDefaultSize(s32 _width, s32 _height) {

		hasDefaultSize = true;
		defaultWidth = Math::clampInt(_width, 0, 9999);
		defaultHeight = Math::clampInt(_height, 0, 9999);

	}

	void Window::resetToDefaultSize() {
		if (hasDefaultSize) setSize(defaultWidth, defaultHeight);
	}

	s32 Window::getDefaultWidth() {
		return defaultWidth;
	}

	s32 Window::getDefaultHeight() {
		return defaultHeight;
	}

	s32 Window::getX() {
		return x;
	}

	s32 Window::getY() {
		return y;
	}

	s32 Window::getWidth() {
		return width;
	}

	s32 Window::getHeight() {
		return height;
	}

	s32 Window::getContainerX() {
		if (!docked) return (maximized ? 0 : x + GUI_WINDOW_BORDER_LEFT);
		return x;
	}

	s32 Window::getContainerY() {
		if (!docked) return (maximized ? sprTitleBar->sizeY : y + GUI_WINDOW_BORDER_TOP + sprTitleBar->sizeY);
		return y;
	}

	s32 Window::getContainerWidth() {
		if (!docked) return (maximized ? width : width - GUI_WINDOW_BORDER_LEFT - GUI_WINDOW_BORDER_RIGHT);
		return width;
	}

	s32 Window::getContainerHeight() {
		if (!docked) return (maximized ? height - sprTitleBar->sizeY: height - GUI_WINDOW_BORDER_TOP - GUI_WINDOW_BORDER_BOTTOM - sprTitleBar->sizeY);
		return height;
	}

	s32 Window::getAvailableTitleSpace() {
		return (width - (sprClose->sizeX * showButtonClose) - (sprMaximize->sizeX * showButtonMaximize) - (GUI_WINDOW_BUTTON_HOR_PADDING * (showButtonClose || showButtonMaximize)) - (GUI_WINDOW_BUTTON_SPACING * (showButtonClose && showButtonMaximize)));
	}

	void Window::updateTitleDraw() {

		UTF8String fullTitle;
		if (saveState) fullTitle += '*';
		fullTitle += title;

		s32 titleX = GUI_WINDOW_TITLE_HOR_SPACING;
		s32 titleMinWidth = titleX + renderer->getStringWidth(fntTitle, fullTitle) + GUI_WINDOW_TITLE_HOR_SPACING;
		s32 titleSpace = getAvailableTitleSpace();

		titleDraw = fullTitle;

		if (titleSpace - titleMinWidth < 0) {

			titleDraw += "...";
			titleMinWidth = titleX + renderer->getStringWidth(fntTitle, titleDraw) + GUI_WINDOW_TITLE_HOR_SPACING;

			if (!saveState || titleDraw.size() > 4) {

				while (titleSpace - titleMinWidth < 0) {

					titleDraw.erase(titleDraw.begin() + titleDraw.size() - 4);
					titleMinWidth = titleX + renderer->getStringWidth(fntTitle, titleDraw) + GUI_WINDOW_TITLE_HOR_SPACING;

					if (titleDraw.length() <= 3 + (size_t)saveState) break;

				}

			}

		}

	}

	//Grid.
	void Window::setGridColumnRowCount(s32 _numColumns, s32 _numRows) {

		if (_numColumns <= 0) {
			_numColumns = 1;
			ZIXEL_WARN("Error in Window::setGridColumnRowCount. Column count can't be less than 1.");
		}

		if (_numRows <= 0) {
			_numRows = 1;
			ZIXEL_WARN("Error in Window::setGridColumnRowCount. Row count can't be less than 1.");
		}

		gridColumnCount = _numColumns;
		gridRowCount = _numRows;

		if (gridLayout) {

			for (Widget* child : widgetList) {

				if (child->gridColumn >= gridColumnCount) child->gridColumn = 0;
				if (child->gridRow >= gridRowCount) child->gridRow = 0;

			}

			updateTransform(true);

		}

	}

	void Window::setGridLayout(bool _enable) {

		if (gridLayout != _enable) {

			gridLayout = _enable;
			updateTransform(true);

		}

	}

	std::vector<Widget*>& Window::getChildren() {
		return widgetList;
	}

	void Window::setOnFocus(std::function<void(Window*)> _onFocus) {
		onFocus = _onFocus;
	}

	void Window::setOnPreClose(std::function<bool(Window*)> _onPreClose) {
		onPreClose = _onPreClose;
	}

	void Window::setOnClose(std::function<void(Window*, bool, DockContainer*, bool&)> _onClose) {
		onClose = _onClose;
	}

	void Window::update(f32 dt) {

		s32 mouseX = gui->mouseX;
		s32 mouseY = gui->mouseY;

		mouseOverButtonClose = false;
		mouseOverButtonMaximize = false;

		if (canInteract()) {

			if (gui->windowMouseOver != this) {
				showTooltip = true;
			}

			if ((gui->windowSelected == nullptr || gui->windowSelected == this) && gui->windowMouseOver == this) {

				if (showButtonClose || showButtonMaximize) {

					s32 titleHeight = sprTitleBar->sizeY;

					s32 buttonX = x + width - sprClose->sizeX - GUI_WINDOW_BUTTON_HOR_PADDING;

					if (showButtonClose) {

						mouseOverButtonClose = Math::pointInRect(mouseX, mouseY, buttonX, y + (titleHeight / 2) - (sprClose->sizeY / 2), sprClose->sizeX, sprClose->sizeY);
						buttonX -= sprClose->sizeX + (sprMaximize->sizeX - sprClose->sizeX) + GUI_WINDOW_BUTTON_SPACING;

					}

					if (!mouseOverButtonClose && showButtonMaximize && canResize) {
						mouseOverButtonMaximize = Math::pointInRect(mouseX, mouseY, buttonX, y + (titleHeight / 2) - (sprMaximize->sizeY / 2), sprMaximize->sizeX, sprMaximize->sizeY);
					}

				}

			}

			if (gui->windowSelected == nullptr && gui->windowMouseOver == this) {

				bool mouseOverTopLeft = false, mouseOverTopRight = false, mouseOverBottomLeft = false, mouseOverBottomRight = false;
				bool mouseOverTop = false, mouseOverBottom = false, mouseOverLeft = false, mouseOverRight = false;

				if (!maximized && canResize) {

					s32 minGrabHeight = Math::minInt(GUI_WINDOW_RESIZE_GRAB_RANGE, sprTitleBar->sizeY);

					mouseOverTopLeft = Math::pointInRect(mouseX, mouseY, x - GUI_WINDOW_RESIZE_GRAB_RANGE, y, GUI_WINDOW_RESIZE_GRAB_RANGE * 2, minGrabHeight);
					mouseOverTopRight = Math::pointInRect(mouseX, mouseY, x + width - GUI_WINDOW_RESIZE_GRAB_RANGE, y, GUI_WINDOW_RESIZE_GRAB_RANGE * 2, minGrabHeight);
					mouseOverBottomLeft = Math::pointInRect(mouseX, mouseY, x - GUI_WINDOW_RESIZE_GRAB_RANGE, y + height, GUI_WINDOW_RESIZE_GRAB_RANGE, GUI_WINDOW_RESIZE_GRAB_RANGE);
					mouseOverBottomRight = Math::pointInRect(mouseX, mouseY, x + width, y + height, GUI_WINDOW_RESIZE_GRAB_RANGE, GUI_WINDOW_RESIZE_GRAB_RANGE);

					mouseOverTop = Math::pointInRect(mouseX, mouseY, x + GUI_WINDOW_RESIZE_GRAB_RANGE, y, width - (GUI_WINDOW_RESIZE_GRAB_RANGE * 2), minGrabHeight);
					mouseOverBottom = Math::pointInRect(mouseX, mouseY, x, y + height, width, GUI_WINDOW_RESIZE_GRAB_RANGE);
					mouseOverLeft = Math::pointInRect(mouseX, mouseY, x - GUI_WINDOW_RESIZE_GRAB_RANGE, y + minGrabHeight, GUI_WINDOW_RESIZE_GRAB_RANGE, height - minGrabHeight);
					mouseOverRight = Math::pointInRect(mouseX, mouseY, x + width, y + minGrabHeight, GUI_WINDOW_RESIZE_GRAB_RANGE, height - minGrabHeight);

				}

				bool mouseOverMove = (canMove && Math::pointInRect(mouseX, mouseY, x, y, width, sprTitleBar->sizeY));
				
				if (mouseOverTopLeft || mouseOverTopRight || mouseOverBottomLeft || mouseOverBottomRight || mouseOverTop || mouseOverBottom || mouseOverLeft || mouseOverRight || mouseOverButtonClose || mouseOverButtonMaximize) {
					showTooltip = true;
				}

				bool mouseOverTitle = isMouseOverTitle();
				if (!mouseOverTitle) showTooltip = true;

				if (!tooltip.empty() && !mouseOverTopLeft && !mouseOverTopRight && !mouseOverBottomLeft && !mouseOverBottomRight && !mouseOverTop && !mouseOverBottom && !mouseOverLeft && !mouseOverRight) {

					if (mouseOverTitle && showTooltip) {
						gui->setTooltip(this, tooltip);
					}

				}

				if (!mouseOverButtonClose && !mouseOverButtonMaximize) {

					if (mouseOverTop || mouseOverBottom) gui->setMouseCursor(CURSOR_SIZE_NS);
					else if (mouseOverLeft || mouseOverRight) gui->setMouseCursor(CURSOR_SIZE_WE);
					else if (mouseOverTopLeft || mouseOverBottomRight) gui->setMouseCursor(CURSOR_SIZE_NWSE);
					else if (mouseOverTopRight || mouseOverBottomLeft) gui->setMouseCursor(CURSOR_SIZE_NESW);

				}

				if (gui->isMousePressed(MOUSE_LEFT)) {

					if (mouseOverButtonClose) {

						gui->windowSelected = this;
						moveDir = WindowMoveDir::Close;

					}
					else if (mouseOverButtonMaximize) {

						gui->windowSelected = this;
						moveDir = WindowMoveDir::Maximize;

					}
					else if ((mouseOverTopLeft || mouseOverTopRight || mouseOverBottomLeft || mouseOverBottomRight || mouseOverTop || mouseOverBottom || mouseOverLeft || mouseOverRight)) {

						gui->windowSelected = this;

						xStart = x;
						yStart = y;

						widthStart = width;
						heightStart  = height;

						mouseXStart = mouseX;
						mouseYStart = mouseY;

						if (mouseOverTopLeft) moveDir = WindowMoveDir::TopLeft;
						else if (mouseOverTopRight) moveDir = WindowMoveDir::TopRight;
						else if (mouseOverBottomLeft) moveDir = WindowMoveDir::BottomLeft;
						else if (mouseOverBottomRight) moveDir = WindowMoveDir::BottomRight;
						else if (mouseOverTop) moveDir = WindowMoveDir::Top;
						else if (mouseOverBottom) moveDir = WindowMoveDir::Bottom;
						else if (mouseOverLeft) moveDir = WindowMoveDir::Left;
						else if (mouseOverRight) moveDir = WindowMoveDir::Right;

					}
					else if (mouseOverMove) {
						
						showTooltip = false;

						bool prevMaximized = maximized;

						if (canResize) {

							f64 currentTime = renderer->getCurrentTime();

							if (doubleClickTime < 0 || (currentTime - doubleClickTime) > GUI_WINDOW_DOUBLE_CLICK_TIME) {

								doubleClickTime = currentTime;

							}
							else if ((currentTime - doubleClickTime) <= GUI_WINDOW_DOUBLE_CLICK_TIME) {

								doubleClickTime = -1;
								maximize();

							}

						}
						else {
							doubleClickTime = -1;
						}

						if (prevMaximized == maximized) {

							gui->windowSelected = this;

							moveDir = WindowMoveDir::Move;

							xStart = x;
							yStart = y;

							mouseXStart = mouseX;
							mouseYStart = mouseY;

							if (dockArea != nullptr) dockArea->destroyEmptyContainerWhenUndocking = false;

						}

					}

				}

			}
			else if (gui->windowSelected == this) {

				s32 widthPrev = width, heightPrev = height;

				if (moveDir == WindowMoveDir::TopLeft) {

					x = Math::clampInt(xStart + (mouseX - mouseXStart), xStart - 9999 + widthStart, xStart + widthStart - minWidth);
					y = Math::clampInt(yStart + (mouseY - mouseYStart), 0, yStart + heightStart - minHeight);

					s32 dX = Math::minInt(0, x);
					s32 dY = Math::minInt(0, y);

					x -= dX;
					y -= dY;

					width = Math::clampInt(widthStart - (mouseX - mouseXStart), minWidth, 9999);
					height = Math::clampInt(heightStart - (mouseY - mouseYStart), minHeight, (yStart + heightStart));

					width += dX;
					height += dY;

					gui->setMouseCursor(CURSOR_SIZE_NWSE);

				}
				else if (moveDir == WindowMoveDir::TopRight) {

					y = Math::clampInt(yStart + (mouseY - mouseYStart), 0, yStart + heightStart - minHeight);

					s32 dY = Math::minInt(0, y);
					y -= dY;

					width = Math::clampInt(widthStart + (mouseX - mouseXStart), minWidth, 9999);
					height = Math::clampInt(heightStart - (mouseY - mouseYStart), minHeight, (yStart + heightStart));

					height += dY;

					s32 dX = Math::maxInt(0, (x + width - 1) - (renderer->windowWidth - 1));
					width -= dX;

					gui->setMouseCursor(CURSOR_SIZE_NESW);

				}
				else if (moveDir == WindowMoveDir::BottomLeft) {

					x = Math::clampInt(xStart + (mouseX - mouseXStart), xStart - 9999 + widthStart, xStart + widthStart - minWidth);

					s32 dX = Math::minInt(0, x);
					x -= dX;

					width = Math::clampInt(widthStart - (mouseX - mouseXStart), minWidth, 9999);
					height = Math::clampInt(heightStart + (mouseY - mouseYStart), minHeight, 9999);

					width += dX;

					s32 dY = Math::maxInt(0, (y + height - 1) - (renderer->windowHeight - 1));
					height -= dY;

					gui->setMouseCursor(CURSOR_SIZE_NESW);

				}
				else if (moveDir == WindowMoveDir::BottomRight) {

					width = Math::clampInt(widthStart + (mouseX - mouseXStart), minWidth, 9999);
					height = Math::clampInt(heightStart + (mouseY - mouseYStart), minHeight, 9999);

					s32 dX = Math::maxInt(0, (x + width - 1) - (renderer->windowWidth - 1));
					s32 dY = Math::maxInt(0, (y + height - 1) - (renderer->windowHeight - 1));

					width -= dX;
					height -= dY;

					gui->setMouseCursor(CURSOR_SIZE_NWSE);

				}
				else if (moveDir == WindowMoveDir::Top) {

					y = Math::clampInt(yStart + (mouseY - mouseYStart), 0, yStart + heightStart - minHeight);

					s32 dY = Math::minInt(0, y);
					y -= dY;

					height = Math::clampInt(heightStart - (mouseY - mouseYStart), minHeight, (yStart + heightStart));
					height += dY;

					gui->setMouseCursor(CURSOR_SIZE_NS);

				}
				else if (moveDir == WindowMoveDir::Bottom) {

					height = Math::clampInt(heightStart + (mouseY - mouseYStart), minHeight, 9999);

					s32 dY = Math::maxInt(0, (y + height - 1) - (renderer->windowHeight - 1));
					height -= dY;

					gui->setMouseCursor(CURSOR_SIZE_NS);

				}
				else if (moveDir == WindowMoveDir::Left) {

					x = Math::clampInt(xStart + (mouseX - mouseXStart), xStart - 9999 + widthStart, xStart + widthStart - minWidth);

					s32 dX = Math::minInt(0, x);
					x -= dX;

					width = Math::clampInt(widthStart - (mouseX - mouseXStart), minWidth, 9999);
					width += dX;

					gui->setMouseCursor(CURSOR_SIZE_WE);

				}
				else if (moveDir == WindowMoveDir::Right) {

					width = Math::clampInt(widthStart + (mouseX - mouseXStart), minWidth, 9999);

					s32 dX = Math::maxInt(0, (x + width - 1) - (renderer->windowWidth - 1));
					width -= dX;

					gui->setMouseCursor(CURSOR_SIZE_WE);

				}
				else if (moveDir == WindowMoveDir::Move) {

					s32 xPrev = x, yPrev = y;

					if (!maximized) {

						s32 displayWidth, displayHeight;
						renderer->getWindowSize(displayWidth, displayHeight);

						x = Math::clampInt(xStart + (mouseX - mouseXStart), -width + GUI_WINDOW_CLAMP_BORDER, displayWidth - GUI_WINDOW_CLAMP_BORDER);
						y = Math::clampInt(yStart + (mouseY - mouseYStart), 0, displayHeight - GUI_WINDOW_CLAMP_BORDER);

						if (x != xPrev || y != yPrev) {

							doubleClickTime = -1;
							for (s32 i = (s32)widgetList.size() - 1; i >= 0; --i) {
								widgetList[i]->updateTransform(true);
							}

							//Check docking.
							if (dockArea != nullptr && ((Widget*)dockArea)->visibleGlobal && !gui->isKeyDown(GUI_CANCEL_DOCKING_KEY)) {
								
								if (gui->isMouseOverWidget(dockArea)) {
									
									if (dockArea->containerList.size() > 0) {
										
										for (DockContainer* container : dockArea->containerList) {

											s32 tabHeight = dockArea->sprDockTab->sizeY;

											//Dock to tab.
											if (Math::pointInRect(mouseX, mouseY, dockArea->x + container->x, dockArea->y + container->y, container->width, tabHeight)) {

												gui->windowSelected = nullptr;
												gui->setWidgetSelected(dockArea);

												dockTo(container);
												container->tabSelected = dockTab;

												dockArea->tabMove = dockTab;
												//dockArea->tabMoveX = (mouseX - (mouseXStart - xStart)) - (dockArea->x + container->x);
												dockArea->tabMoveX = (mouseX - (dockTab->width / 2)) - (dockArea->x + container->x);
												dockArea->tabMoveXStart = dockArea->tabMoveX;
												dockArea->tabMoveMouseXStart = mouseX;
												dockArea->tabMoveMouseYStart = mouseY;

												dockArea->checkTabOrder();

												break;

											}

											//Dock to container.
											else {

												s32 dockWidth = Math::minInt((container->width / GUI_DOCK_SIZE_DIVIDER), width);
												s32 dockHeight = Math::minInt(((container->height - tabHeight) / GUI_DOCK_SIZE_DIVIDER), height);

												if (Math::pointInRect(mouseX, mouseY, dockArea->x + container->x, dockArea->y + container->y + tabHeight, dockWidth, container->height - tabHeight)
												 || Math::pointInRect(mouseX, mouseY, dockArea->x + container->x, dockArea->y + container->y + tabHeight, container->width, dockHeight)
												 || Math::pointInRect(mouseX, mouseY, dockArea->x + container->x + container->width - dockWidth, dockArea->y + container->y + tabHeight, dockWidth, container->height - tabHeight)
												 || Math::pointInRect(mouseX, mouseY, dockArea->x + container->x, dockArea->y + container->y + container->height - dockHeight, container->width, dockHeight)) {

													gui->windowSelected = nullptr;
													gui->setWidgetSelected(dockArea);

													__close(true, false, false, false, false);

													gui->dockTabDragArea = dockArea;
													gui->dockTabDrag = new DockTab(gui, container, this);
													gui->dockTabDragMouseXOffset = (mouseXStart - xStart);
													gui->dockTabDragMouseYOffset = (mouseYStart - yStart);
													gui->dockTabDragContainerWidth = width;
													gui->dockTabDragContainerHeight = height;
													gui->dockTabDragWidth = Math::minInt(gui->dockTabDrag->getDefaultWidth(), width);
													gui->dockTabDragTitle = gui->dockTabDrag->getTitleShowFromWidth(gui->dockTabDragWidth);
													gui->dockTabDragShowClose = showButtonClose;

													dockArea->dockToContainer = nullptr;
													dockArea->dockToSide = DockSplitSide::None;

													dockArea->checkDockTo();

													break;

												}

											}

										}
	
									}

									//No container already exists.
									else {

										s32 dockWidth = Math::minInt((dockArea->widthDraw / GUI_DOCK_SIZE_DIVIDER), width);
										s32 dockHeight = Math::minInt((dockArea->heightDraw / GUI_DOCK_SIZE_DIVIDER), height);

										if (Math::pointInRect(mouseX, mouseY, dockArea->x, dockArea->y, dockWidth, dockArea->heightDraw)
										 || Math::pointInRect(mouseX, mouseY, dockArea->x, dockArea->y, dockArea->widthDraw, dockHeight)
										 || Math::pointInRect(mouseX, mouseY, dockArea->x + dockArea->widthDraw - dockWidth, dockArea->y, dockWidth, dockArea->heightDraw)
										 || Math::pointInRect(mouseX, mouseY, dockArea->x, dockArea->y + dockArea->heightDraw - dockHeight, dockArea->widthDraw, dockHeight)) {

											gui->windowSelected = nullptr;
											gui->setWidgetSelected(dockArea);

											__close(true, false, false);

											gui->dockTabDragArea = dockArea;
											gui->dockTabDrag = new DockTab(gui, nullptr, this);
											gui->dockTabDrag->dockArea = dockArea;
											gui->dockTabDragMouseXOffset = (mouseXStart - xStart);
											gui->dockTabDragMouseYOffset = (mouseYStart - yStart);
											gui->dockTabDragContainerWidth = width;
											gui->dockTabDragContainerHeight = height;
											gui->dockTabDragWidth = Math::minInt(gui->dockTabDrag->getDefaultWidth(), width);
											gui->dockTabDragTitle = gui->dockTabDrag->getTitleShowFromWidth(gui->dockTabDragWidth);
											gui->dockTabDragShowClose = showButtonClose;

											dockArea->dockToContainer = nullptr;
											dockArea->dockToSide = DockSplitSide::None;

											dockArea->checkDockTo();

										}

									}

								}

							}

						}

					}
					else {

						if (Math::distance2DInt(mouseXStart, mouseYStart, mouseX, mouseY) >= GUI_WINDOW_MAXIMIZE_DRAG_RANGE) {

							maximized = false;

							f32 dX = Math::clampFloat(((f32)mouseXStart - (f32)xStart) / (f32)width, 0.0f, 1.0f);
							
							s32 widthNew = widthPrevState;
							s32 heightNew = heightPrevState;

							s32 xNew = (mouseX - (s32)roundf((f32)widthNew * dX));
							s32 yNew = mouseY - (mouseYStart - yStart);

							setRect(xNew, yNew, widthNew, heightNew);

							xStart = x;
							yStart = y;

							mouseXStart = mouseX;
							mouseYStart = mouseY;

						}

					}

				}

				xFloat = (f32)x;
				yFloat = (f32)y;

				if (width != widthPrev || height != heightPrev) {

					updateTitleDraw();
					updateTransform(true);

				}

				if (!gui->isMouseDown(MOUSE_LEFT)) {

					gui->windowSelected = nullptr;
					if (!isMouseOverTitle()) showTooltip = true;

					if (moveDir == WindowMoveDir::Close && mouseOverButtonClose) close();
					else if (moveDir == WindowMoveDir::Maximize && mouseOverButtonMaximize) maximize();

				}

			}

		}
		else {

			if (gui->windowSelected == this) gui->windowSelected = nullptr;

			mouseOverButtonClose = false;
			mouseOverButtonMaximize = false;

			doubleClickTime = -1;

			showTooltip = true;
		}

	}

	void Window::render() {

		bool focused = isFocused();
		s32 titleHeight = sprTitleBar->sizeY;

		s32 containerX = getContainerX();
		s32 containerY = getContainerY();
		s32 containerWidth = getContainerWidth();
		s32 containerHeight = getContainerHeight();

		//Draw container.
		renderer->render9P(sprContainer, maximized, x, y + titleHeight, width, height - titleHeight);

		//Render children widgets.
		renderer->cutStart(containerX, containerY, containerWidth - (sprScrollVer->sizeX * scrollVerShow), containerHeight - (sprScrollHor->sizeY * scrollHorShow));

		for (Widget* child : widgetList) {

			if (child->visibleGlobal) {
				child->renderMain();
			}
		}

		renderer->cutEnd();

		//Draw scrollbars.
		if (scrollVerShow || scrollHorShow) {

			renderer->cutStart(containerX, containerY, containerWidth, containerHeight);

			if (scrollVerShow) {

				s32 scrollX = Math::maxInt(containerX, containerX + containerWidth - sprScrollVer->sizeX);

				renderer->render3PVer(sprScrollVer, 0, scrollX, containerY, scrollVerLen);

				renderer->cutStart(scrollX, containerY, sprScrollVer->sizeX, scrollVerLen);
				renderer->render3PVer(sprScrollVer, 1 + scrollVerInd, scrollX, containerY + scrollVerBarPos, scrollVerBarLen);
				renderer->cutEnd();

			}

			if (scrollHorShow) {

				s32 scrollY = Math::maxInt(containerY, containerY + containerHeight - sprScrollHor->sizeY);

				renderer->render3PHor(sprScrollHor, 0, containerX, scrollY, scrollHorLen);

				renderer->cutStart(containerX, scrollY, containerWidth, sprScrollHor->sizeY);
				renderer->render3PHor(sprScrollHor, 1 + scrollHorInd, containerX + scrollHorBarPos, scrollY, scrollHorBarLen);
				renderer->cutEnd();

			}

			if (scrollVerShow && scrollHorShow) renderer->render9P(sprScrollCorner, 0, containerX + containerWidth - sprScrollVer->sizeX, containerY + containerHeight - sprScrollHor->sizeY, sprScrollVer->sizeX, sprScrollHor->sizeY);

			renderer->cutEnd();

		}

		//Draw title bar.
		renderer->render3PHor(sprTitleBar, maximized, x, y, width);

		Color4f titleCol;
		if (focused) titleCol = GUI_WINDOW_TITLE_COL_FOCUSED; else titleCol = GUI_WINDOW_TITLE_COL_UNFOCUSED;

		renderer->cutStart(x + GUI_WINDOW_TITLE_HOR_SPACING, y, getAvailableTitleSpace() - (GUI_WINDOW_TITLE_HOR_SPACING * 2), titleHeight); //@TODO: Maybe we should only call getAvailableTitleSpace when the window size or title text actually changes. Have fun future me :)
		renderer->renderText(fntTitle, titleDraw, x + GUI_WINDOW_TITLE_HOR_SPACING, y + (titleHeight / 2) + GUI_WINDOW_TITLE_VER_SPACING, TextAlign::Left, TextAlign::Middle, titleCol);
		renderer->cutEnd();

		if (showButtonClose || showButtonMaximize) {

			s32 buttonX = x + width - sprClose->sizeX - GUI_WINDOW_BUTTON_HOR_PADDING;

			renderer->cutStart(x, y, width, titleHeight);

			if (showButtonClose) {

				renderer->renderSprite(sprClose, (gui->windowSelected == this && moveDir == WindowMoveDir::Close) ? (mouseOverButtonClose ? 2 : 0) : ((mouseOverButtonClose && gui->windowSelected == nullptr) ? 1 : (3 * !focused)), buttonX, y + (titleHeight / 2) - (sprClose->sizeY / 2));
				buttonX -= sprClose->sizeX + (sprMaximize->sizeX - sprClose->sizeX) + GUI_WINDOW_BUTTON_SPACING;

			}

			if (showButtonMaximize) {
				renderer->renderSprite(sprMaximize, (5 * maximized) + (!canResize ? 3 : ((gui->windowSelected == this && moveDir == WindowMoveDir::Maximize) ? (mouseOverButtonMaximize ? 2 : 0) : ((mouseOverButtonMaximize && gui->windowSelected == nullptr) ? 1 : (4 * !focused)))), buttonX, y + (titleHeight / 2) - (sprMaximize->sizeY / 2));
			}

			renderer->cutEnd();

		}

	}

}