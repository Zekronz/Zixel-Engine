#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/GUI/DockArea.h"
#include "Engine/GUI/DockSplit.h"
#include "Engine/GUI/DockContainer.h"
#include "Engine/GUI/DockTab.h"
#include "Engine/GUI/Window.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/GUIMacros.h"

namespace Zixel {

	DockArea::DockArea(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::DockArea;

		renderFocus = false;
		cutContent = true;

		sprDockArea = renderer->getTextureAtlasSprite(GUI_DOCK_TAB_SPR_AREA);
		sprDockSplitVer = renderer->getTextureAtlasSprite(GUI_DOCK_TAB_SPR_SPLIT_VER);
		sprDockSplitHor = renderer->getTextureAtlasSprite(GUI_DOCK_TAB_SPR_SPLIT_HOR);
		sprDockContainer = renderer->getTextureAtlasSprite(GUI_DOCK_TAB_SPR_CONTAINER);
		sprDockTab = renderer->getTextureAtlasSprite(GUI_DOCK_TAB_SPR_TAB);
		sprDockTabClose = renderer->getTextureAtlasSprite(GUI_DOCK_TAB_SPR_TAB_CLOSE);
		sprDockTabLine = renderer->getTextureAtlasSprite(GUI_DOCK_TAB_SPR_LINE);
		sprDockPreview = renderer->getTextureAtlasSprite(GUI_DOCK_TAB_SPR_PREVIEW);

		fntDockTab = renderer->getTextureAtlasFont(GUI_DOCK_TAB_FONT);

		resizeWidth = widthDraw;
		resizeHeight = heightDraw;

	}

	DockArea::~DockArea() {

		for (s32 i = (s32)containerList.size() - 1; i >= 0; --i) {
			destroyContainer(containerList[i], false);
		}

		for (s32 i = (s32)splitList.size() - 1; i >= 0; --i) {
			delete splitList[i];
		}

	}

	DockContainer* DockArea::addContainer(DockContainer* _parent, DockSplitSide _splitSide, f32 _width, f32 _height, bool _sizeIsPercentage) {

		s32 containerX = 0, containerY = 0, containerWidth = widthDraw, containerHeight = heightDraw;

		DockSplit* splitLeft = nullptr;
		DockSplit* splitRight = nullptr;
		DockSplit* splitUp = nullptr;
		DockSplit* splitDown = nullptr;

		DockContainer* container = new DockContainer(this);

		if (_parent != nullptr) {

			s32 sprWidth = sprDockSplitVer->sizeX;
			s32 sprHeight = sprDockSplitHor->sizeY;
			s32 sprHalfWidth = (sprDockSplitVer->sizeX / 2);
			s32 sprHalfHeight = (sprDockSplitHor->sizeY / 2);

			containerX = _parent->x;
			containerY = _parent->y;
			containerWidth = _parent->width;
			containerHeight = _parent->height;

			if (_sizeIsPercentage) {
				if (_width >= 0.0f) _width = Math::maxFloat(roundf((f32)containerWidth * _width), (f32)GUI_DOCK_CONTAINER_MIN_WIDTH);
				if (_height >= 0.0f) _height = Math::maxFloat(roundf((f32)containerHeight * _height), (f32)GUI_DOCK_CONTAINER_MIN_HEIGHT);
			}

			DockSplit* split = new DockSplit();

			if (_splitSide == DockSplitSide::Left) {

				if (_parent->splitUp != nullptr) _parent->splitUp->splits2.push_back(split);
				if (_parent->splitDown != nullptr) _parent->splitDown->splits1.push_back(split);

				split->containers1.push_back(container);
				
				if (_parent->splitUp != nullptr) _parent->splitUp->containers2.push_back(container);
				if (_parent->splitDown != nullptr) _parent->splitDown->containers1.push_back(container);

				if (_parent->splitLeft != nullptr) {

					auto it = std::find(_parent->splitLeft->containers2.begin(), _parent->splitLeft->containers2.end(), _parent);
					if (it != _parent->splitLeft->containers2.end()) _parent->splitLeft->containers2.erase(it);

					_parent->splitLeft->containers2.push_back(container);

				}

				split->containers2.push_back(_parent);

				containerWidth = (_width >= 0.0f) ? (_sizeIsPercentage ? (s32)_width - sprHalfWidth : (s32)_width) : (_parent->width / 2) - sprHalfWidth;
				_parent->width -= (containerWidth + sprWidth);
				_parent->x += (containerWidth + sprWidth);

				split->dir = 1;
				split->x = (f32)(containerX + containerWidth);
				split->y = (f32)containerY;
				split->length = (f32)containerHeight;

				splitRight = split;
				splitLeft = _parent->splitLeft;
				splitUp = _parent->splitUp;
				splitDown = _parent->splitDown;
				_parent->splitLeft = split;

			}
			else if (_splitSide == DockSplitSide::Right) {

				if (_parent->splitUp != nullptr) _parent->splitUp->splits2.push_back(split);
				if (_parent->splitDown != nullptr) _parent->splitDown->splits1.push_back(split);

				split->containers2.push_back(container);

				if (_parent->splitUp != nullptr) _parent->splitUp->containers2.push_back(container);
				if (_parent->splitDown != nullptr) _parent->splitDown->containers1.push_back(container);

				if (_parent->splitRight != nullptr) {

					auto it = std::find(_parent->splitRight->containers1.begin(), _parent->splitRight->containers1.end(), _parent);
					if (it != _parent->splitRight->containers1.end()) _parent->splitRight->containers1.erase(it);

					_parent->splitRight->containers1.push_back(container);

				}

				split->containers1.push_back(_parent);

				containerWidth = (_width >= 0.0f) ? (_sizeIsPercentage ? (s32)_width - (sprWidth - sprHalfWidth) : (s32)_width) : ((_parent->width / 2) - sprHalfWidth) - sprWidth;
				_parent->width -= (containerWidth + sprWidth);
				containerX += (_parent->width + sprWidth);

				split->dir = 1;
				split->x = (f32)(_parent->x + _parent->width);
				split->y = (f32)containerY;
				split->length = (f32)containerHeight;

				splitLeft = split;
				splitRight = _parent->splitRight;
				splitUp = _parent->splitUp;
				splitDown = _parent->splitDown;
				_parent->splitRight = split;

			}
			else if (_splitSide == DockSplitSide::Up) {

				if (_parent->splitLeft != nullptr) _parent->splitLeft->splits2.push_back(split);
				if (_parent->splitRight != nullptr) _parent->splitRight->splits1.push_back(split);

				split->containers1.push_back(container);

				if (_parent->splitLeft != nullptr) _parent->splitLeft->containers2.push_back(container);
				if (_parent->splitRight != nullptr) _parent->splitRight->containers1.push_back(container);

				if (_parent->splitUp != nullptr) {

					auto it = std::find(_parent->splitUp->containers2.begin(), _parent->splitUp->containers2.end(), _parent);
					if (it != _parent->splitUp->containers2.end()) _parent->splitUp->containers2.erase(it);

					_parent->splitUp->containers2.push_back(container);

				}

				split->containers2.push_back(_parent);

				containerHeight = (_height >= 0.0f) ? (_sizeIsPercentage ? (s32)_height - sprHalfHeight : (s32)_height) : (_parent->height / 2) - sprHalfHeight;
				_parent->height -= (containerHeight + sprHeight);
				_parent->y += (containerHeight + sprHeight);

				split->dir = 0;
				split->x = (f32)containerX;
				split->y = (f32)(containerY + containerHeight);
				split->length = (f32)containerWidth;

				splitDown = split;
				splitUp = _parent->splitUp;
				splitLeft = _parent->splitLeft;
				splitRight = _parent->splitRight;
				_parent->splitUp = split;

			}
			else if (_splitSide == DockSplitSide::Down) {

				if (_parent->splitLeft != nullptr) _parent->splitLeft->splits2.push_back(split);
				if (_parent->splitRight != nullptr) _parent->splitRight->splits1.push_back(split);

				split->containers2.push_back(container);

				if (_parent->splitLeft != nullptr) _parent->splitLeft->containers2.push_back(container);
				if (_parent->splitRight != nullptr) _parent->splitRight->containers1.push_back(container);

				if (_parent->splitDown != nullptr) {

					auto it = std::find(_parent->splitDown->containers1.begin(), _parent->splitDown->containers1.end(), _parent);
					if (it != _parent->splitDown->containers1.end()) _parent->splitDown->containers1.erase(it);

					_parent->splitDown->containers1.push_back(container);

				}

				split->containers1.push_back(_parent);

				containerHeight = (_height >= 0.0f) ? (_sizeIsPercentage ? (s32)_height - (sprHeight - sprHalfHeight) : (s32)_height) : ((_parent->height / 2) - sprHalfHeight) - sprHeight;
				_parent->height -= (containerHeight + sprHeight);
				containerY += (_parent->height + sprHeight);

				split->dir = 0;
				split->x = (f32)containerX;
				split->y = (f32)(_parent->y + _parent->height);
				split->length = (f32)containerWidth;

				splitUp = split;
				splitDown = _parent->splitDown;
				splitLeft = _parent->splitLeft;
				splitRight = _parent->splitRight;
				_parent->splitDown = split;

			}

			split->xRound = (s32)split->x;
			split->yRound = (s32)split->y;
			split->lengthRound = (s32)split->lengthRound;

			splitList.push_back(split);
			_parent->children.push_back(container);

			for (DockTab* tab : _parent->tabList) {
				tab->updateWindowRect();
			}

			_parent->updateTabs();

		}

		container->x = containerX;
		container->y = containerY;
		container->width = containerWidth;
		container->height = containerHeight;
		container->splitLeft = splitLeft;
		container->splitRight = splitRight;
		container->splitUp = splitUp;
		container->splitDown = splitDown;

		containerList.push_back(container);

		fixHorizontal(false);
		fixVertical(false);
		realignSplits();
		updateAllContainerSizes();

		return container;

	}

	void DockArea::destroyContainer(DockContainer* _container, bool _redock) {

		auto containerIt = std::find(containerList.begin(), containerList.end(), _container);
		if (containerIt == containerList.end()) {
			ZIXEL_WARN("Error in DockArea::destroyContainer. Trying to destroy container that doesn't exist in current dock area.");
			return;
		}

		for (s32 i = (s32)_container->tabList.size() - 1; i >= 0; --i) {

			DockTab* tab = _container->tabList[i];
			Window* window = tab->window;

			window->setDocked(false);
			window->setDockTab(nullptr);
			window->setOpened(false);
			window->setMaximized(false);

			delete tab;

		}

		if (_redock) {

			std::vector<DockSplit*> candidates;

			if (_container->splitLeft != nullptr) {

				auto it = std::find(_container->splitLeft->containers2.begin(), _container->splitLeft->containers2.end(), _container);
				if (it != _container->splitLeft->containers2.end()) _container->splitLeft->containers2.erase(it);

				if (_container->splitLeft->yRound == _container->y && _container->splitLeft->lengthRound == _container->height) {
					candidates.push_back(_container->splitLeft);
				}

			}

			if (_container->splitRight != nullptr) {

				auto it = std::find(_container->splitRight->containers1.begin(), _container->splitRight->containers1.end(), _container);
				if (it != _container->splitRight->containers1.end()) _container->splitRight->containers1.erase(it);

				if (_container->splitRight->yRound == _container->y && _container->splitRight->lengthRound == _container->height) {
					candidates.push_back(_container->splitRight);
				}

			}

			if (_container->splitUp != nullptr) {

				auto it = std::find(_container->splitUp->containers2.begin(), _container->splitUp->containers2.end(), _container);
				if (it != _container->splitUp->containers2.end()) _container->splitUp->containers2.erase(it);

				if (_container->splitUp->xRound == _container->x && _container->splitUp->lengthRound == _container->width) {
					candidates.push_back(_container->splitUp);
				}

			}

			if (_container->splitDown != nullptr) {

				auto it = std::find(_container->splitDown->containers1.begin(), _container->splitDown->containers1.end(), _container);
				if (it != _container->splitDown->containers1.end()) _container->splitDown->containers1.erase(it);

				if (_container->splitDown->xRound == _container->x && _container->splitDown->lengthRound == _container->width) {
					candidates.push_back(_container->splitDown);
				}

			}

			if (candidates.size() > 0) {

				DockSplit* p = nullptr;
				for(s32 i = (s32)candidates.size() - 1; i >= 0; --i){

					DockSplit* split = candidates[i];

					bool shouldBreak = false;
					if (_container->splitLeft == split || _container->splitUp == split) {

						for (s32 j = (s32)split->containers1.size() - 1; j >= 0; --j) {

							if (_container->parent == split->containers1[j]) {

								p = split;
								shouldBreak = true;
								break;

							}

						}

					}
					else {

						for (s32 j = (s32)split->containers2.size() - 1; j >= 0; --j) {

							if (_container->parent == split->containers2[j]) {

								p = split;
								shouldBreak = true;
								break;

							}

						}

					}

					if (shouldBreak) break;

				}

				if (p == nullptr) p = candidates[0];

				DockSplit* split = nullptr;

				if (p == _container->splitLeft) {

					split = _container->splitLeft;

					for (s32 i = (s32)split->containers1.size() - 1; i >= 0; --i) {

						DockContainer* temp = split->containers1[i];

						temp->splitRight = _container->splitRight;

						if (_container->splitRight != nullptr) {

							_container->splitRight->containers1.push_back(temp);
							temp->width += _container->splitRight->xRound - (temp->x + temp->width - 1) - 1;

						}
						else temp->width += widthDraw - (temp->x + temp->width - 1) - 1;

						for (s32 j = (s32)temp->tabList.size() - 1; j >= 0; --j)
							temp->tabList[j]->updateWindowRect();

						temp->updateTabs();

					}

					for (s32 i = (s32)split->splits1.size() - 1; i >= 0; --i) {

						DockSplit* temp = split->splits1[i];

						if (_container->splitRight != nullptr) {

							_container->splitRight->splits1.push_back(temp);
							temp->length += _container->splitRight->x - (temp->x + temp->length - 1.0f) - 1.0f;

						}
						else temp->length += (f32)widthDraw - (temp->x + temp->length - 1.0f) - 1.0f;

						temp->lengthRound = (s32)roundf(temp->length);

					}

				}
				else if (p == _container->splitRight) {

					split = _container->splitRight;

					for (s32 i = (s32)split->containers2.size() - 1; i >= 0; --i) {

						DockContainer* temp = split->containers2[i];

						temp->splitLeft = _container->splitLeft;

						if (_container->splitLeft != nullptr) {

							_container->splitLeft->containers2.push_back(temp);

							s32 d = temp->x;
							temp->x = _container->splitLeft->xRound + sprDockSplitVer->sizeX;
							temp->width += (d - temp->x);

						}
						else {

							s32 d = temp->x;
							temp->x = 0;
							temp->width += (d - temp->x);

						}

						for (s32 j = (s32)temp->tabList.size() - 1; j >= 0; --j)
							temp->tabList[j]->updateWindowRect();

						temp->updateTabs();

					}

					for (s32 i = (s32)split->splits2.size() - 1; i >= 0; --i) {

						DockSplit* temp = split->splits2[i];

						if (_container->splitLeft != nullptr) {

							_container->splitLeft->splits2.push_back(temp);

							f32 d = temp->x;
							temp->x = _container->splitLeft->x + (f32)sprDockSplitVer->sizeX;
							temp->length += (d - temp->x);

						}
						else {

							f32 d = temp->x;
							temp->x = 0.0f;
							temp->length += (d - temp->x);

						}

						temp->xRound = (s32)roundf(temp->x);
						temp->lengthRound = (s32)roundf(temp->length);

					}

				}
				else if (p == _container->splitUp) {

					split = _container->splitUp;

					for (s32 i = (s32)split->containers1.size() - 1; i >= 0; --i) {

						DockContainer* temp = split->containers1[i];

						temp->splitDown = _container->splitDown;

						if (_container->splitDown != nullptr) {

							_container->splitDown->containers1.push_back(temp);
							temp->height += _container->splitDown->yRound - (temp->y + temp->height - 1) - 1;

						}
						else temp->height += heightDraw - (temp->y + temp->height - 1) - 1;

						for (s32 j = (s32)temp->tabList.size() - 1; j >= 0; --j)
							temp->tabList[j]->updateWindowRect();

						temp->updateTabs();

					}

					for (s32 i = (s32)split->splits1.size() - 1; i >= 0; --i) {

						DockSplit* temp = split->splits1[i];

						if (_container->splitDown != nullptr) {

							_container->splitDown->splits1.push_back(temp);
							temp->length += _container->splitDown->y - (temp->y + temp->length - 1.0f) - 1.0f;

						}
						else temp->length += (f32)heightDraw - (temp->y + temp->length - 1.0f) - 1.0f;

						temp->lengthRound = (s32)roundf(temp->length);

					}

				}
				else if (p == _container->splitDown) {

					split = _container->splitDown;

					for (s32 i = (s32)split->containers2.size() - 1; i >= 0; --i) {

						DockContainer* temp = split->containers2[i];

						temp->splitUp = _container->splitUp;

						if (_container->splitUp != nullptr) {

							_container->splitUp->containers2.push_back(temp);

							s32 d = temp->y;
							temp->y = _container->splitUp->yRound + sprDockSplitHor->sizeY;
							temp->height += (d - temp->y);

						}
						else {

							s32 d = temp->y;
							temp->y = 0;
							temp->height += (d - temp->y);

						}

						for (s32 j = (s32)temp->tabList.size() - 1; j >= 0; --j)
							temp->tabList[j]->updateWindowRect();

						temp->updateTabs();

					}

					for (s32 i = (s32)split->splits2.size() - 1; i >= 0; --i) {

						DockSplit* temp = split->splits2[i];

						if (_container->splitUp != nullptr) {

							_container->splitUp->splits2.push_back(temp);

							f32 d = temp->y;
							temp->y = _container->splitUp->y + (f32)sprDockSplitHor->sizeY;
							temp->length += (d - temp->y);

						}
						else {

							f32 d = temp->y;
							temp->y = 0.0f;
							temp->length += (d - temp->y);

						}

						temp->yRound = (s32)roundf(temp->y);
						temp->lengthRound = (s32)roundf(temp->length);

					}

				}

				if (split != nullptr) {

					for (s32 i = (s32)splitList.size() - 1; i >= 0; --i) {

						DockSplit* temp = splitList[i];
						if (temp == split || temp->dir == split->dir) continue;

						auto it = std::find(temp->splits1.begin(), temp->splits1.end(), split);
						if (it != temp->splits1.end()) temp->splits1.erase(it);

						it = std::find(temp->splits2.begin(), temp->splits2.end(), split);
						if (it != temp->splits2.end()) temp->splits2.erase(it);

					}

					auto it = std::find(splitList.begin(), splitList.end(), split);
					if (it != splitList.end()) splitList.erase(it);

					delete split;

				}

			}

		}

		delete _container;
		containerList.erase(containerIt);
	}

	void DockArea::updateAllContainerSizes() {

		for (DockContainer* container : containerList) {

			s32 prevX = container->x, prevY = container->y, prevWidth = container->width, prevHeight = container->height;

			container->x = (container->splitLeft != nullptr) ? container->splitLeft->xRound + sprDockSplitVer->sizeX : 0;
			container->y = (container->splitUp != nullptr) ? container->splitUp->yRound + sprDockSplitHor->sizeY : 0;
			container->width = (container->splitRight != nullptr) ? container->splitRight->xRound - container->x : Math::maxInt(widthDraw - container->x, container->minWidth);
			container->height = (container->splitDown != nullptr) ? container->splitDown->yRound - container->y : Math::maxInt(heightDraw - container->y, container->minHeight);
			
			if (container->x != prevX || container->y != prevY || container->width != prevWidth || container->height != prevHeight) {

				for (DockTab* tab : container->tabList) {
					tab->updateWindowRect();
				}

				container->updateTabs();

			}

		}

	}

	void DockArea::realignSplits() {

		for(DockSplit* split : splitList) {

			DockSplit* splitLeft = nullptr;
			DockSplit* splitRight = nullptr;
			DockSplit* splitUp = nullptr;
			DockSplit* splitDown = nullptr;

			for(DockSplit* temp : splitList) {

				if (temp == split) continue;

				if (split->dir == 0) {

					auto it = std::find(temp->splits2.begin(), temp->splits2.end(), split);
					if (it != temp->splits2.end()) splitLeft = temp;

					it = std::find(temp->splits1.begin(), temp->splits1.end(), split);
					if (it != temp->splits1.end()) splitRight = temp;

					if (splitLeft != nullptr && splitRight != nullptr) break;

				}
				else {

					auto it = std::find(temp->splits2.begin(), temp->splits2.end(), split);
					if (it != temp->splits2.end()) splitUp = temp;

					it = std::find(temp->splits1.begin(), temp->splits1.end(), split);
					if (it != temp->splits1.end()) splitDown = temp;

					if (splitUp != nullptr && splitDown != nullptr) break;

				}

			}

			if (split->dir == 0) {

				split->x = (splitLeft != nullptr) ? (splitLeft->x + (f32)sprDockSplitVer->sizeX) : 0.0f;
				split->length = (splitRight != nullptr) ? (splitRight->x - split->x) : Math::maxFloat(((f32)widthDraw - split->x), 1.0f);

				split->xRound = (splitLeft != nullptr) ? (splitLeft->xRound + sprDockSplitVer->sizeX) : 0;
				split->lengthRound = (splitRight != nullptr) ? (splitRight->xRound - split->xRound) : Math::maxInt((widthDraw - split->xRound), 1);

			}
			else {

				split->y = (splitUp != nullptr) ? (splitUp->y + (f32)sprDockSplitHor->sizeY) : 0.0f;
				split->length = (splitDown != nullptr) ? (splitDown->y - split->y) : Math::maxFloat(((f32)heightDraw - split->y), 1.0f);

				split->yRound = (splitUp != nullptr) ? (splitUp->yRound + sprDockSplitHor->sizeY) : 0;
				split->lengthRound = (splitDown != nullptr) ? (splitDown->yRound - split->yRound) : Math::maxInt((heightDraw - split->yRound), 1);

			}

		}

	}

	void DockArea::fixHorizontal(bool _realignAndUpdateContainerSizes) {

		std::vector<DockContainer*> leftMost;
		std::vector<DockContainer*> rightMost;

		for (DockContainer* container : containerList) {

			if (container->splitLeft == nullptr && container->splitRight != nullptr) leftMost.push_back(container);
			if (container->splitLeft != nullptr && container->splitRight == nullptr) rightMost.push_back(container);

		}

		for (DockContainer* container : rightMost) {
			pushSplitLeft(container->splitLeft);
		}
		
		for (DockContainer* container : leftMost) {
			clampSplitFromLeft(container->splitRight);
		}

		if (_realignAndUpdateContainerSizes) {

			realignSplits();
			updateAllContainerSizes();

		}

	}

	void DockArea::fixVertical(bool _realignAndUpdateContainerSizes) {

		std::vector<DockContainer*> upMost;
		std::vector<DockContainer*> downMost;

		for (DockContainer* container : containerList) {

			if (container->splitUp == nullptr && container->splitDown != nullptr) upMost.push_back(container);
			if (container->splitUp != nullptr && container->splitDown == nullptr) downMost.push_back(container);

		}

		for (DockContainer* container : downMost) {
			pushSplitUp(container->splitUp);
		}

		for (DockContainer* container : upMost) {
			clampSplitFromUp(container->splitDown);
		}

		if (_realignAndUpdateContainerSizes) {

			realignSplits();
			updateAllContainerSizes();

		}

	}

	void DockArea::getLeftMostContainers(std::vector<DockContainer*>& _containers) {
		_containers.clear();

		for (DockContainer* container : containerList) {
			if (container->splitLeft == nullptr && container->splitRight != nullptr) {
				_containers.push_back(container);
			}
		}
	}

	void DockArea::getUpMostContainers(std::vector<DockContainer*>& _containers) {
		_containers.clear();

		for (DockContainer* container : containerList) {
			if (container->splitUp == nullptr && container->splitDown != nullptr) {
				_containers.push_back(container);
			}
		}
	}

	void DockArea::getRightMostContainers(std::vector<DockContainer*>& _containers) {
		_containers.clear();

		for (DockContainer* container : containerList) {
			if (container->splitRight == nullptr && container->splitLeft != nullptr) {
				_containers.push_back(container);
			}
		}
	}

	void DockArea::getDownMostContainers(std::vector<DockContainer*>& _containers) {
		_containers.clear();

		for (DockContainer* container : containerList) {
			if (container->splitDown == nullptr && container->splitUp != nullptr) {
				_containers.push_back(container);
			}
		}
	}

	void DockArea::pushSplitLeft(DockSplit* _split) {

		f32 d = 0.0f;

		for(DockContainer* container : _split->containers2) {

			f32 widthRight = (container->splitRight != nullptr) ? (container->splitRight->x - _split->x - (f32)sprDockSplitVer->sizeX) : ((f32)widthDraw - _split->x - (f32)sprDockSplitVer->sizeX);
			d = Math::maxFloat(0.0f, d, (f32)container->minWidth - widthRight);

		}

		if (d > 0.0f) {

			_split->x -= d;
			_split->xRound = (s32)roundf(_split->x);

		}

		for (DockContainer* container : _split->containers1) {
			if (container->splitLeft != nullptr) {
				pushSplitLeft(container->splitLeft);
			}
		}

	}

	void DockArea::pushSplitRight(DockSplit* _split) {

		f32 d = 0.0f;

		for (DockContainer* container : _split->containers1) {

			f32 widthLeft = (container->splitLeft != nullptr) ? (_split->x - container->splitLeft->x - (f32)sprDockSplitVer->sizeX) : _split->x;
			d = Math::maxFloat(0.0f, d, (f32)container->minWidth - widthLeft);

		}

		if (d > 0.0f) {

			_split->x += d;
			_split->xRound = (s32)roundf(_split->x);

		}

		for (DockContainer* container : _split->containers2) {
			if (container->splitRight != nullptr) {
				pushSplitRight(container->splitRight);
			}
		}

	}

	void DockArea::pushSplitUp(DockSplit* _split) {

		f32 d = 0.0f;

		for (DockContainer* container : _split->containers2) {

			f32 heightDown = (container->splitDown != nullptr) ? (container->splitDown->y - _split->y - (f32)sprDockSplitHor->sizeY) : ((f32)heightDraw - _split->y - (f32)sprDockSplitHor->sizeY);
			d = Math::maxFloat(0.0f, d, (f32)container->minHeight - heightDown);

		}

		if (d > 0.0f) {

			_split->y -= d;
			_split->yRound = (s32)roundf(_split->y);

		}

		for (DockContainer* container : _split->containers1) {
			if (container->splitUp != nullptr) {
				pushSplitUp(container->splitUp);
			}
		}

	}

	void DockArea::pushSplitDown(DockSplit* _split) {

		f32 d = 0.0f;

		for (DockContainer* container : _split->containers1) {

			f32 heightUp = (container->splitUp != nullptr) ? (_split->y - container->splitUp->y - (f32)sprDockSplitHor->sizeY) : _split->y;
			d = Math::maxFloat(0.0f, d, (f32)container->minHeight - heightUp);

		}

		if (d > 0.0f) {

			_split->y += d;
			_split->yRound = (s32)roundf(_split->y);

		}

		for (DockContainer* container : _split->containers2) {
			if (container->splitDown != nullptr) {
				pushSplitRight(container->splitDown);
			}
		}

	}

	void DockArea::clampSplitFromLeft(DockSplit* _split) {

		for(DockContainer* container : _split->containers1) {

			DockSplit* splitLeft = container->splitLeft;

			f32 w = (splitLeft != nullptr) ? (_split->x - splitLeft->x - (f32)sprDockSplitVer->sizeX) : _split->x;
			if (w < (f32)container->minWidth) {

				_split->x = (splitLeft != nullptr) ? (splitLeft->x + (f32)sprDockSplitVer->sizeX + (f32)container->minWidth) : (f32)container->minWidth;
				_split->xRound = (s32)roundf(_split->x);

			}

		}

		for (DockContainer* container : _split->containers2) {
			if (container->splitRight != nullptr) {
				clampSplitFromLeft(container->splitRight);
			}
		}

	}

	void DockArea::clampSplitFromRight(DockSplit* _split) {

		for (DockContainer* container : _split->containers2) {

			DockSplit* splitRight = container->splitRight;

			f32 w = (splitRight != nullptr) ? (splitRight->x - _split->x - (f32)sprDockSplitVer->sizeX) : (f32)widthDraw - _split->x - (f32)sprDockSplitVer->sizeX;
			if (w < (f32)container->minWidth) {

				_split->x = (splitRight != nullptr) ? (splitRight->x - (f32)container->minWidth - (f32)sprDockSplitVer->sizeX) : ((f32)widthDraw - (f32)container->minWidth - (f32)sprDockSplitVer->sizeX);
				_split->xRound = (s32)roundf(_split->x);

			}

		}

		for (DockContainer* container : _split->containers1) {
			if (container->splitLeft != nullptr) {
				clampSplitFromRight(container->splitLeft);
			}
		}

	}

	void DockArea::clampSplitFromUp(DockSplit* _split) {

		for (DockContainer* container : _split->containers1) {

			DockSplit* splitUp = container->splitUp;

			f32 h = (splitUp != nullptr) ? (_split->y - splitUp->y - (f32)sprDockSplitHor->sizeY) : _split->y;
			if (h < (f32)container->minHeight) {

				_split->y = (splitUp != nullptr) ? (splitUp->y + (f32)sprDockSplitHor->sizeY + (f32)container->minHeight) : (f32)container->minHeight;
				_split->yRound = (s32)roundf(_split->y);

			}

		}

		for (DockContainer* container : _split->containers2) {
			if (container->splitDown != nullptr) {
				clampSplitFromUp(container->splitDown);
			}
		}

	}

	void DockArea::clampSplitFromDown(DockSplit* _split) {

		for (DockContainer* container : _split->containers2) {

			DockSplit* splitDown = container->splitDown;

			f32 h = (splitDown != nullptr) ? (splitDown->y - _split->y - (f32)sprDockSplitHor->sizeY) : (f32)heightDraw - _split->y - (f32)sprDockSplitHor->sizeY;
			if (h < (f32)container->minHeight) {

				_split->y = (splitDown != nullptr) ? (splitDown->y - (f32)container->minHeight - (f32)sprDockSplitHor->sizeY) : ((f32)heightDraw - (f32)container->minHeight - (f32)sprDockSplitHor->sizeY);
				_split->yRound = (s32)roundf(_split->y);

			}

		}

		for (DockContainer* container : _split->containers1) {
			if (container->splitUp != nullptr) {
				clampSplitFromDown(container->splitUp);
			}
		}

	}

	s32 DockArea::getMinAreaWidthContainer(DockContainer* _container, s32 _width) {

		s32 newWidth = _width;

		if (_container->splitRight != nullptr) {

			for (DockContainer* temp : _container->splitRight->containers2) {
				s32 tempWidth = getMinAreaWidthContainer(temp, _width + temp->minWidth + sprDockSplitVer->sizeX);
				if (tempWidth > newWidth) newWidth = tempWidth;
			}

		}

		return newWidth;

	}

	s32 DockArea::getMinAreaHeightContainer(DockContainer* _container, s32 _height) {

		s32 newHeight = _height;

		if (_container->splitDown != nullptr) {

			for (DockContainer* temp : _container->splitDown->containers2) {
				s32 tempHeight = getMinAreaHeightContainer(temp, _height + temp->minHeight + sprDockSplitHor->sizeY);
				if (tempHeight > newHeight) newHeight = tempHeight;
			}

		}

		return newHeight;

	}

	s32 DockArea::getMinAreaWidth() {

		s32 minAreaWidth = 0;

		std::vector<DockContainer*> leftMost;
		getLeftMostContainers(leftMost);

		if (containerList.size() > 0 && leftMost.size() <= 0) {

			for (DockContainer* container : containerList) {
				if (minAreaWidth < container->minWidth) minAreaWidth = container->minWidth;
			}

		}
		else {

			for (DockContainer* container : leftMost) {
				s32 tempWidth = getMinAreaWidthContainer(container, container->minWidth);
				if (tempWidth > minAreaWidth) minAreaWidth = tempWidth;
			}

		}

		return minAreaWidth;

	}

	s32 DockArea::getMinAreaHeight() {

		s32 minAreaHeight = 0;

		std::vector<DockContainer*> upMost;
		getUpMostContainers(upMost);

		if (containerList.size() > 0 && upMost.size() <= 0) {

			for (DockContainer* container : containerList) {
				if (minAreaHeight < container->minHeight) minAreaHeight = container->minHeight;
			}

		}
		else {

			for (DockContainer* container : upMost) {
				s32 tempHeight = getMinAreaHeightContainer(container, container->minHeight);
				if (tempHeight > minAreaHeight) minAreaHeight = tempHeight;
			}

		}

		return minAreaHeight;

	}

	s32 DockArea::getMinAreaWidthFromLeftToRight(DockSplit* _split, s32 _width) {

		s32 newWidth = _width;

		for (DockContainer* container : _split->containers2) {

			DockSplit* splitRight = container->splitRight;

			if (splitRight != nullptr) {

				s32 tempWidth = getMinAreaWidthFromLeftToRight(splitRight, _width + container->minWidth + sprDockSplitVer->sizeX);
				if (tempWidth > newWidth) newWidth = tempWidth;

			}
			else {

				s32 tempWidth = _width + container->minWidth + sprDockSplitVer->sizeX;
				if (tempWidth > newWidth) newWidth = tempWidth;

			}

		}

		return newWidth;

	}

	s32 DockArea::getMinAreaHeightFromUpToDown(DockSplit* _split, s32 _height) {

		s32 newHeight = _height;

		for (DockContainer* container : _split->containers2) {

			DockSplit* splitDown = container->splitDown;

			if (splitDown != nullptr) {

				s32 tempHeight = getMinAreaHeightFromUpToDown(splitDown, _height + container->minHeight + sprDockSplitHor->sizeY);
				if (tempHeight > newHeight) newHeight = tempHeight;

			}
			else {

				s32 tempHeight = _height + container->minHeight + sprDockSplitHor->sizeY;
				if (tempHeight > newHeight) newHeight = tempHeight;

			}

		}

		return newHeight;

	}

	s32 DockArea::getMinAreaWidthFromRightToLeft(DockSplit* _split, s32 _width) {

		s32 newWidth = _width;

		for (DockContainer* container : _split->containers1) {

			DockSplit* splitLeft = container->splitLeft;

			if (splitLeft != nullptr) {

				s32 tempWidth = getMinAreaWidthFromRightToLeft(splitLeft, _width + container->minWidth + sprDockSplitVer->sizeX);
				if (tempWidth > newWidth) newWidth = tempWidth;

			}
			else {

				s32 tempWidth = _width + container->minWidth + sprDockSplitVer->sizeX;
				if (tempWidth > newWidth) newWidth = tempWidth;

			}

		}

		return newWidth;

	}

	s32 DockArea::getMinAreaHeightFromDownToUp(DockSplit* _split, s32 _height) {

		s32 newHeight = _height;

		for (DockContainer* container : _split->containers1) {

			DockSplit* splitUp = container->splitUp;

			if (splitUp != nullptr) {

				s32 tempHeight = getMinAreaHeightFromDownToUp(splitUp, _height + container->minHeight + sprDockSplitHor->sizeY);
				if (tempHeight > newHeight) newHeight = tempHeight;

			}
			else {

				s32 tempHeight = _height + container->minHeight + sprDockSplitHor->sizeY;
				if (tempHeight > newHeight) newHeight = tempHeight;

			}

		}

		return newHeight;

	}

	void DockArea::handleLeftFixedSize(DockContainer* _container) {

		s32 xPos = (_container->splitLeft != nullptr) ? _container->splitLeft->xRound + sprDockSplitVer->sizeX : 0;

		_container->splitRight->xRound = xPos + _container->width;
		_container->splitRight->x = (f32)_container->splitRight->xRound;

		bool hasFixed = false;
		bool hasResize = false;

		for (DockContainer* container : _container->splitRight->containers2) {

			if (container->splitRight == nullptr) continue;

			/*bool foundFixed = false;

			for (DockTab* tab : container->tabList) {

				if (tab->window->dockFixedWidth) {

					foundFixed = true;
					break;

				}

			}*/

			if (/*foundFixed*/ container->tabSelected != nullptr && container->tabSelected->window->dockFixedWidth) hasFixed = true;
			else hasResize = true;

		}

		for (DockContainer* container : _container->splitRight->containers2) {

			if (container->splitRight == nullptr) continue;

			/*bool foundFixed = false;

			for (DockTab* tab : container->tabList) {

				if (tab->window->dockFixedWidth) {

					foundFixed = true;
					break;

				}

			}*/

			if ((/*foundFixed*/ (container->tabSelected != nullptr && container->tabSelected->window->dockFixedWidth) || (hasFixed && hasResize)) && shouldHandleLeftFixedSize(container)) handleLeftFixedSize(container);

		}

	}

	void DockArea::handleUpFixedSize(DockContainer* _container) {

		s32 yPos = (_container->splitUp != nullptr) ? _container->splitUp->yRound + sprDockSplitHor->sizeY : 0;

		_container->splitDown->yRound = yPos + _container->height;
		_container->splitDown->y = (f32)_container->splitDown->yRound;

		bool hasFixed = false;
		bool hasResize = false;

		for (DockContainer* container : _container->splitDown->containers2) {

			if (container->splitDown == nullptr) continue;

			/*bool foundFixed = false;

			for (DockTab* tab : container->tabList) {

				if (tab->window->dockFixedHeight) {

					foundFixed = true;
					break;

				}

			}*/

			if (/*foundFixed*/ container->tabSelected != nullptr && container->tabSelected->window->dockFixedHeight) hasFixed = true;
			else hasResize = true;

		}

		for (DockContainer* container : _container->splitDown->containers2) {

			if (container->splitDown == nullptr) continue;

			/*bool foundFixed = false;

			for (DockTab* tab : container->tabList) {

				if (tab->window->dockFixedHeight) {

					foundFixed = true;
					break;

				}

			}*/

			if ((/*foundFixed*/ (container->tabSelected != nullptr && container->tabSelected->window->dockFixedHeight) || (hasFixed && hasResize)) && shouldHandleUpFixedSize(container)) handleUpFixedSize(container);

		}

	}

	void DockArea::handleRightFixedSize(DockContainer* _container) {

		s32 xPos = (_container->splitRight != nullptr) ? _container->splitRight->xRound : widthDraw;

		_container->splitLeft->xRound = xPos - _container->width - sprDockSplitVer->sizeX;
		_container->splitLeft->x = (f32)_container->splitLeft->xRound;

		bool hasFixed = false;
		bool hasResize = false;

		for (DockContainer* container : _container->splitLeft->containers1) {

			if (container->splitLeft == nullptr) continue;

			/*bool foundFixed = false;

			for (DockTab* tab : container->tabList) {

				if (tab->window->dockFixedWidth) {

					foundFixed = true;
					break;

				}

			}*/

			if (/*foundFixed*/ container->tabSelected != nullptr && container->tabSelected->window->dockFixedWidth) hasFixed = true;
			else hasResize = true;

		}

		for (DockContainer* container : _container->splitLeft->containers1) {

			if (container->splitLeft == nullptr) continue;
			
			/*bool foundFixed = false;

			for (DockTab* tab : container->tabList) {

				if (tab->window->dockFixedWidth) {

					foundFixed = true;
					break;

				}

			}*/

			if ((/*foundFixed*/ (container->tabSelected != nullptr && container->tabSelected->window->dockFixedWidth) || (hasFixed && hasResize)) && shouldHandleRightFixedSize(container)) handleRightFixedSize(container);

		}

	}

	void DockArea::handleDownFixedSize(DockContainer* _container) {
		
		s32 yPos = (_container->splitDown != nullptr) ? _container->splitDown->yRound : heightDraw;

		_container->splitUp->yRound = yPos - _container->height - sprDockSplitHor->sizeY;
		_container->splitUp->y = (f32)_container->splitUp->yRound;

		bool hasFixed = false;
		bool hasResize = false;

		for (DockContainer* container : _container->splitUp->containers1) {

			if (container->splitUp == nullptr) continue;

			/*bool foundFixed = false;

			for (DockTab* tab : container->tabList) {

				if (tab->window->dockFixedHeight) {

					foundFixed = true;
					break;

				}

			}*/

			if (/*foundFixed*/ container->tabSelected != nullptr && container->tabSelected->window->dockFixedHeight) hasFixed = true;
			else hasResize = true;

		}

		for (DockContainer* container : _container->splitUp->containers1) {

			if (container->splitUp == nullptr) continue;

			/*bool foundFixed = false;

			for (DockTab* tab : container->tabList) {

				if (tab->window->dockFixedHeight) {

					foundFixed = true;
					break;

				}

			}*/

			if ((/*foundFixed*/ (container->tabSelected != nullptr && container->tabSelected->window->dockFixedHeight) || (hasFixed && hasResize)) && shouldHandleDownFixedSize(container)) handleDownFixedSize(container);

		}

	}

	bool DockArea::shouldHandleLeftFixedSize(DockContainer* _container) {
		
		bool validPath = false;

		for (DockContainer* container : _container->splitRight->containers2) {

			/*bool hasFixed = false;

			for (DockTab* tab : container->tabList) {

				if (tab->window->dockFixedWidth) {

					hasFixed = true;
					break;

				}

			}*/

			if (/*hasFixed*/ container->tabSelected != nullptr && container->tabSelected->window->dockFixedWidth) {
				if (container->splitRight != nullptr && shouldHandleLeftFixedSize(container)) validPath = true;
			}
			else {
				validPath = true;
			}

		}

		return validPath;

	}

	bool DockArea::shouldHandleUpFixedSize(DockContainer* _container) {
		
		bool validPath = false;

		for (DockContainer* container : _container->splitDown->containers2) {

			/*bool hasFixed = false;

			for (DockTab* tab : container->tabList) {

				if (tab->window->dockFixedHeight) {

					hasFixed = true;
					break;

				}

			}*/

			if (/*hasFixed*/ container->tabSelected != nullptr && container->tabSelected->window->dockFixedHeight) {
				if (container->splitDown != nullptr && shouldHandleUpFixedSize(container)) validPath = true;
			}
			else {
				validPath = true;
			}

		}

		return validPath;

	}

	bool DockArea::shouldHandleRightFixedSize(DockContainer* _container) {

		bool validPath = false;

		for (DockContainer* container : _container->splitLeft->containers1) {

			/*bool hasFixed = false;

			for (DockTab* tab : container->tabList) {

				if (tab->window->dockFixedWidth) {

					hasFixed = true;
					break;

				}

			}*/

			if (/*hasFixed*/ container->tabSelected != nullptr && container->tabSelected->window->dockFixedWidth) {
				if (container->splitLeft != nullptr && shouldHandleRightFixedSize(container)) validPath = true;
			}
			else {
				validPath = true;
			}

		}

		return validPath;

	}

	bool DockArea::shouldHandleDownFixedSize(DockContainer* _container) {
		
		bool validPath = false;

		for (DockContainer* container : _container->splitUp->containers1) {

			/*bool hasFixed = false;

			for (DockTab* tab : container->tabList) {

				if (tab->window->dockFixedHeight) {

					hasFixed = true;
					break;

				}

			}*/

			if (/*hasFixed*/ container->tabSelected != nullptr && container->tabSelected->window->dockFixedHeight) {
				if (container->splitUp != nullptr && shouldHandleDownFixedSize(container)) validPath = true;
			}
			else {
				validPath = true;
			}

		}

		return validPath;

	}

	void DockArea::onResize(s32 prevWidth, s32 prevHeight) {

		s32 displayWidth, displayHeight;
		renderer->getWindowSize(displayWidth, displayHeight);

		if (widthDraw > 0 && heightDraw > 0 && displayWidth > 0 && displayHeight > 0) {

			if (containerList.size() <= 0) return;

			std::vector<DockContainer*> leftMost;
			std::vector<DockContainer*> upMost;
			std::vector<DockContainer*> rightMost;
			std::vector<DockContainer*> downMost;

			for (DockContainer* container : containerList) {

				if (container->splitLeft == nullptr && container->splitRight != nullptr) leftMost.push_back(container);
				if (container->splitUp == nullptr && container->splitDown != nullptr) upMost.push_back(container);
				if (container->splitRight == nullptr && container->splitLeft != nullptr) rightMost.push_back(container);
				if (container->splitDown == nullptr && container->splitUp != nullptr) downMost.push_back(container);

			}

			//Get min area size.
			s32 minAreaWidth = getMinAreaWidth();
			s32 minAreaHeight = getMinAreaHeight();

			//Scale splits.
			f32 xs = (f32)Math::maxInt(widthDraw, minAreaWidth) / (f32)Math::maxInt(resizeWidth, minAreaWidth);
			f32 ys = (f32)Math::maxInt(heightDraw, minAreaHeight) / (f32)Math::maxInt(resizeHeight, minAreaHeight);

			resizeWidth = widthDraw;
			resizeHeight = heightDraw;

			for (DockSplit* split : splitList) {

				if (split->dir == 0) {

					split->x *= xs;
					split->length *= xs;
					split->y = ((split->y + ((f32)sprDockSplitHor->sizeY * 0.5f)) * ys) - ((f32)sprDockSplitHor->sizeY * 0.5f);

				}
				else {

					split->y *= ys;
					split->length *= ys;
					split->x = ((split->x + ((f32)sprDockSplitVer->sizeX * 0.5f)) * xs) - ((f32)sprDockSplitVer->sizeX * 0.5f);

				}

				split->xRound = (s32)roundf(split->x);
				split->yRound = (s32)roundf(split->y);
				split->lengthRound = (s32)roundf(split->length);

			}

			//Re-align.
			realignSplits();

			//Handle fixed size containers.
			for (DockContainer* container : leftMost) {

				/*bool foundFixed = false;
				
				for (DockTab* tab : container->tabList) {

					if (tab->window->dockFixedWidth) {

						foundFixed = true;
						break;

					}

				}*/

				if (/*foundFixed*/ container->tabSelected != nullptr && container->tabSelected->window->dockFixedWidth && shouldHandleLeftFixedSize(container)) handleLeftFixedSize(container);

			}

			for (DockContainer* container : upMost) {

				/*bool foundFixed = false;

				for (DockTab* tab : container->tabList) {

					if (tab->window->dockFixedHeight) {

						foundFixed = true;
						break;

					}

				}*/

				if (/*foundFixed*/ container->tabSelected != nullptr && container->tabSelected->window->dockFixedHeight && shouldHandleUpFixedSize(container)) handleUpFixedSize(container);

			}

			for (DockContainer* container : rightMost) {
				
				/*bool foundFixed = false;

				for (DockTab* tab : container->tabList) {
					
					if (tab->window->dockFixedWidth) {

						foundFixed = true;
						break;

					}

				}*/
				
				if (/*foundFixed*/ container->tabSelected != nullptr && container->tabSelected->window->dockFixedWidth && shouldHandleRightFixedSize(container)) handleRightFixedSize(container);

			}

			for (DockContainer* container : downMost) {

				/*bool foundFixed = false;

				for (DockTab* tab : container->tabList) {

					if (tab->window->dockFixedHeight) {

						foundFixed = true;
						break;

					}

				}*/
				
				if (/*foundFixed*/ container->tabSelected != nullptr && container->tabSelected->window->dockFixedHeight && shouldHandleDownFixedSize(container)) handleDownFixedSize(container);

			}

			//Handle splits that are smaller than the minimum container size.
			for (DockContainer* container : rightMost) {
				pushSplitLeft(container->splitLeft);
			}

			for (DockContainer* container : leftMost) {
				clampSplitFromLeft(container->splitRight);
			}

			for (DockContainer* container : downMost) {
				pushSplitUp(container->splitUp);
			}

			for (DockContainer* container : upMost) {
				clampSplitFromUp(container->splitDown);
			}

			//Re-align.
			realignSplits();

			//Update container sizes.
			updateAllContainerSizes();

		}

	}

	void DockArea::checkDockTo() {

		dockToContainer = nullptr;

		if (gui->isKeyDown(GUI_CANCEL_DOCKING_KEY)) {

			dockToSide = DockSplitSide::None;
			return;

		}

		s32 mouseX = gui->mouseX;
		s32 mouseY = gui->mouseY;

		s32 tabHeight = sprDockTab->sizeY;

		if (containerList.size() > 0) {

			for (DockContainer* container : containerList) {

				if (Math::pointInRect(mouseX, mouseY, x + container->x, y + container->y + tabHeight, container->width, container->height - tabHeight)) {

					dockToContainer = container;

					s32 dockWidth = Math::minInt((container->width / GUI_DOCK_SIZE_DIVIDER), gui->dockTabDragContainerWidth);
					s32 dockHeight = Math::minInt(((container->height - tabHeight) / GUI_DOCK_SIZE_DIVIDER), gui->dockTabDragContainerHeight);

					bool up = Math::pointInRect(mouseX, mouseY, x + container->x, y + container->y + tabHeight, container->width, dockHeight);
					bool down = Math::pointInRect(mouseX, mouseY, x + container->x, y + container->y + container->height - dockHeight, container->width, dockHeight);
					bool left = Math::pointInRect(mouseX, mouseY, x + container->x, y + container->y + tabHeight, dockWidth, container->height - tabHeight);
					bool right = Math::pointInRect(mouseX, mouseY, x + container->x + container->width - dockWidth, y + container->y, dockWidth, container->height - tabHeight);

					if ((dockToSide == DockSplitSide::Up && up)
					 || (dockToSide == DockSplitSide::Down && down)
					 || (dockToSide == DockSplitSide::Left && left)
					 || (dockToSide == DockSplitSide::Right && right)) break;

					if (up) dockToSide = DockSplitSide::Up;
					else if (down) dockToSide = DockSplitSide::Down;
					else if (left) dockToSide = DockSplitSide::Left;
					else if (right) dockToSide = DockSplitSide::Right;
					else dockToSide = DockSplitSide::None;

					break;

				}
				else if (Math::pointInRect(mouseX, mouseY, x + container->x, y + container->y, container->width, tabHeight)) {

					//@Consider: Don't really like how all of this is hard-coded.
					
					tabMove = gui->dockTabDrag;
					container->appendTab(tabMove);
					
					gui->dockTabDrag = nullptr;
					gui->dockTabDragArea = nullptr;

					if ((container->minWidth < tabMove->window->minWidth && tabMove->window->hasSetMinWidth) || (container->minHeight < tabMove->window->minHeight && tabMove->window->hasSetMinHeight)) {

						if (container->minWidth < tabMove->window->minWidth && tabMove->window->hasSetMinWidth) {

							container->minWidth = tabMove->window->minWidth;
							fixHorizontal(false);

						}

						if (container->minHeight < tabMove->window->minHeight && tabMove->window->hasSetMinHeight) {

							container->minHeight = tabMove->window->minHeight;
							fixVertical(false);

						}

						realignSplits();
						updateAllContainerSizes();

					}

					//tabMoveX = (mouseX - gui->dockTabDragMouseXOffset) - (x + container->x);
					tabMoveX = (mouseX - (tabMove->width / 2)) - (x + container->x);
					tabMoveXStart = tabMoveX;
					tabMoveMouseXStart = mouseX;
					tabMoveMouseYStart = mouseY;

					checkTabOrder();

					break;

				}

			}

		}
		else {

			if (Math::pointInRect(mouseX, mouseY, x, y, widthDraw, heightDraw)) {

				dockToContainer = nullptr;
				dockToContainerEmpty = true;

				s32 dockWidth = Math::minInt((widthDraw / GUI_DOCK_SIZE_DIVIDER), gui->dockTabDragContainerWidth);
				s32 dockHeight = Math::minInt((heightDraw / GUI_DOCK_SIZE_DIVIDER), gui->dockTabDragContainerHeight);

				bool up = Math::pointInRect(mouseX, mouseY, x, y, widthDraw, dockHeight);
				bool down = Math::pointInRect(mouseX, mouseY, x, y + heightDraw - dockHeight, widthDraw, dockHeight);
				bool left = Math::pointInRect(mouseX, mouseY, x, y, dockWidth, heightDraw);
				bool right = Math::pointInRect(mouseX, mouseY, x + widthDraw - dockWidth, y, dockWidth, heightDraw);
				
				if ((dockToSide == DockSplitSide::Up && up)
				 || (dockToSide == DockSplitSide::Down && down)
				 || (dockToSide == DockSplitSide::Left && left)
				 || (dockToSide == DockSplitSide::Right && right)) return;

				if (up) dockToSide = DockSplitSide::Up;
				else if (down) dockToSide = DockSplitSide::Down;
				else if (left) dockToSide = DockSplitSide::Left;
				else if (right) dockToSide = DockSplitSide::Right;
				else dockToSide = DockSplitSide::None;

			}

		}

	}

	void DockArea::checkTabOrder() {

		s32 tabCount = (s32)tabMove->container->tabList.size();

		if (tabCount > 1) {

			auto it = std::find(tabMove->container->tabList.begin(), tabMove->container->tabList.end(), tabMove);

			s32 ind = (s32)(it - tabMove->container->tabList.begin());
			s32 indTo = -1;

			if (ind < tabCount - 1) {

				for (s32 i = ind + 1; i < tabCount; ++i) {

					DockTab* temp = tabMove->container->tabList[i];
					if ((tabMoveX + tabMove->width - 1) >= temp->x + (s32)((f32)temp->width * 0.55f)) indTo = i;

				}

			}

			if (ind > 0) {

				for (s32 i = ind - 1; i >= 0; --i) {

					DockTab* temp = tabMove->container->tabList[i];
					if (tabMoveX <= temp->x + (temp->width * 0.45)) indTo = i;

				}

			}

			if (indTo >= 0) {

				tabMove->container->tabList.erase(it);
				tabMove->container->tabList.insert(tabMove->container->tabList.begin() + indTo, tabMove);

				tabMove->container->updateTabs();

			}

		}

	}

	void DockArea::update(f32 dt) {

		if (updateResizeSize) {

			resizeWidth = widthDraw;
			resizeHeight = heightDraw;

			updateResizeSize = false;
		}

		if (canInteract()) {
			
			s32 mouseX = gui->mouseX;
			s32 mouseY = gui->mouseY;

			if (gui->widgetSelected != this) {

				splitHovered = nullptr;
				tabHovered = nullptr;

				if (gui->getWidgetMouseOver() == this) {

					bool mouseOverSplit = false;

					for (DockSplit* split : splitList) {

						s32 sprWidth = sprDockSplitVer->sizeX;
						s32 sprHeight = sprDockSplitHor->sizeY;

						if (split->dir == 0 && Math::pointInRect(mouseX, mouseY, x + split->xRound, y + split->yRound + (sprHeight / 2) - (GUI_DOCK_GRAB_RANGE / 2), split->lengthRound, (tabMouseOver != nullptr) ? (-(sprHeight / 2) + sprHeight + (GUI_DOCK_GRAB_RANGE / 2)) : GUI_DOCK_GRAB_RANGE)) {

							mouseOverSplit = true;

							splitHovered = split;
							gui->setMouseCursor(CURSOR_SIZE_NS);

							if (gui->isMousePressed(MOUSE_LEFT)) {

								gui->setWidgetSelected(this);

								splitPosStart = split->y;
								splitMouseStart = mouseY;

								for (DockSplit* splitUp : splitHovered->splits1) {
									splitUp->lengthStart = splitUp->length;
								}

								for (DockSplit* splitDown : splitHovered->splits2) {
									splitDown->posStart = splitDown->y;
									splitDown->lengthStart = splitDown->length;
								}

								for (DockContainer* containerUp : splitHovered->containers1) {
									containerUp->heightStart = containerUp->height;
								}

								for (DockContainer* containerDown : splitHovered->containers2) {
									containerDown->yStart = containerDown->y;
									containerDown->heightStart = containerDown->height;
								}

							}

							break;

						}
						else if (split->dir != 0 && Math::pointInRect(mouseX, mouseY, x + split->xRound + (sprWidth / 2) - (GUI_DOCK_GRAB_RANGE / 2), y + split->yRound, GUI_DOCK_GRAB_RANGE, split->lengthRound)) {

							mouseOverSplit = true;

							splitHovered = split;
							gui->setMouseCursor(CURSOR_SIZE_WE);

							if (gui->isMousePressed(MOUSE_LEFT)) {

								gui->setWidgetSelected(this);

								splitPosStart = split->x;
								splitMouseStart = mouseX;

								for (DockSplit* splitLeft : splitHovered->splits1) {
									splitLeft->lengthStart = splitLeft->length;
								}

								for (DockSplit* splitRight : splitHovered->splits2) {
									splitRight->posStart = splitRight->x;
									splitRight->lengthStart = splitRight->length;
								}

								for (DockContainer* containerLeft : splitHovered->containers1) {
									containerLeft->widthStart = containerLeft->width;
								}

								for (DockContainer* containerRight : splitHovered->containers2) {
									containerRight->xStart = containerRight->x;
									containerRight->widthStart = containerRight->width;
								}

							}

							break;

						}

					}

					if (!mouseOverSplit && tabMouseOver != nullptr) {

						if (tabMouseOver != tooltipTab) {
							tooltipTab = nullptr;
							showTooltip = true;
						}

						bool mouseOverClose = Math::pointInRect(mouseX, mouseY, x + tabMouseOver->container->x + tabMouseOver->x + tabMouseOver->width - sprDockTabClose->sizeX - GUI_DOCK_TAB_CLOSE_HOR_SPACING, y + tabMouseOver->container->y + (sprDockTab->sizeY / 2) - (sprDockTabClose->sizeY / 2), sprDockTabClose->sizeX, sprDockTabClose->sizeY);

						if (mouseOverClose) gui->setTooltip(this, tooltipClose);
						else if (showTooltip) gui->setTooltip(this, tabMouseOver->window->tooltip);

						if (gui->isMousePressed(MOUSE_LEFT)) {

							gui->setWidgetSelected(this);

							showTooltip = false;
							tooltipTab = tabMouseOver;

							tabMouseOver->container->selectTab(tabMouseOver);

							if (mouseOverClose) {
								tabClose = tabMouseOver;
							}
							else {

								tabMove = tabMouseOver;
								tabMoveX = tabMove->x;
								tabMoveXStart = tabMove->x;
								tabMoveMouseXStart = mouseX;
								tabMoveMouseYStart = mouseY;

								destroyEmptyContainerWhenUndocking = true;

							}

						}
						else {
							tabHovered = tabMouseOver;
						}

					}
					else {
						showTooltip = true;
					}

				}
				else {
					showTooltip = true;
				}

			}
			else {
			
				if (gui->dockTabDragArea == this && gui->dockTabDrag != nullptr) {
					checkDockTo();
				}
				else if (tabClose != nullptr) { //Keep this to not cause any errors.

				}
				else if (tabMove != nullptr) {

					tabMoveX = tabMoveXStart + (mouseX - tabMoveMouseXStart);

					checkTabOrder();

					//Undocking.
					if (mouseX < x + tabMove->container->x - GUI_DOCK_TAB_UNDOCK_THRESHOLD || mouseX >= x + tabMove->container->x + tabMove->container->width + GUI_DOCK_TAB_UNDOCK_THRESHOLD
					 || mouseY < y + tabMove->container->y - GUI_DOCK_TAB_UNDOCK_THRESHOLD || mouseY >= y + tabMove->container->y + sprDockTab->sizeY + GUI_DOCK_TAB_UNDOCK_THRESHOLD) {
						
						s32 containerX = tabMove->container->x;
						s32 containerY = tabMove->container->y;
						s32 containerWidth = tabMove->container->width;
						s32 containerHeight = tabMove->container->height;

						gui->dockTabDrag = tabMove;
						gui->dockTabDragArea = this;
						gui->dockTabDragMouseXOffset = mouseX - (x + containerX + tabMoveX);
						gui->dockTabDragMouseYOffset = tabMoveMouseYStart - (y + containerY);

						tabMove->window->__close(false, true, destroyEmptyContainerWhenUndocking, false, false);
						destroyEmptyContainerWhenUndocking = false;

						if (!isDocking) {

							isDocking = true;
						
							gui->dockTabDragContainerWidth = containerWidth;
							gui->dockTabDragContainerHeight = containerHeight;
							gui->dockTabDragWidth = Math::minInt(tabMove->getDefaultWidth(), containerWidth);
							gui->dockTabDragTitle = tabMove->getTitleShowFromWidth(gui->dockTabDragWidth);
							gui->dockTabDragShowClose = tabMove->window->showButtonClose;

						}

						dockToContainer = nullptr;
						dockToSide = DockSplitSide::None;

						tabMove = nullptr;

						checkDockTo();

					}

				}
				else if (splitHovered->dir == 0) {

					gui->setMouseCursor(CURSOR_SIZE_NS);

					s32 mouseDelta = (mouseY - splitMouseStart);

					if (mouseDelta != 0) {

						s32 minAreaHeight = getMinAreaHeight();
						if (heightDraw > minAreaHeight) {

							splitHovered->y = splitPosStart + (f32)mouseDelta;
							splitHovered->yRound = (s32)roundf(splitHovered->y);

							if (mouseDelta < 0) {

								minAreaHeight = getMinAreaHeightFromDownToUp(splitHovered, 0);
								if (splitHovered->y < (f32)minAreaHeight - (f32)sprDockSplitHor->sizeY) {

									splitHovered->y = (f32)minAreaHeight - (f32)sprDockSplitHor->sizeY;
									splitHovered->yRound = (s32)roundf(splitHovered->y);

								}

								std::vector<DockContainer*> upMost;
								getUpMostContainers(upMost);

								for (DockContainer* container : splitHovered->containers1) {
									DockSplit* splitUp = container->splitUp;
									if (splitUp != nullptr) pushSplitUp(splitUp);
								}

								for (DockContainer* container : upMost) {
									clampSplitFromUp(container->splitDown);
								}

							}
							else {

								minAreaHeight = getMinAreaHeightFromUpToDown(splitHovered, 0);
								if (splitHovered->y > (f32)heightDraw - (f32)minAreaHeight) {

									splitHovered->y = (f32)heightDraw - (f32)minAreaHeight;
									splitHovered->yRound = (s32)roundf(splitHovered->y);

								}

								std::vector<DockContainer*> downMost;
								getDownMostContainers(downMost);

								for (DockContainer* container : splitHovered->containers2) {
									DockSplit* splitDown = container->splitDown;
									if (splitDown != nullptr) pushSplitDown(splitDown);
								}

								for (DockContainer* container : downMost) {
									clampSplitFromDown(container->splitUp);
								}

							}

							realignSplits();
							updateAllContainerSizes();

						}

					}

				}
				else {

					gui->setMouseCursor(CURSOR_SIZE_WE);

					s32 mouseDelta = (mouseX - splitMouseStart);

					if (mouseDelta != 0) {

						s32 minAreaWidth = getMinAreaWidth();
						if (widthDraw > minAreaWidth) {

							splitHovered->x = splitPosStart + (f32)mouseDelta;
							splitHovered->xRound = (s32)roundf(splitHovered->x);

							if (mouseDelta < 0) {

								minAreaWidth = getMinAreaWidthFromRightToLeft(splitHovered, 0);
								if (splitHovered->x < (f32)minAreaWidth - (f32)sprDockSplitVer->sizeX) {

									splitHovered->x = (f32)minAreaWidth - (f32)sprDockSplitVer->sizeX;
									splitHovered->xRound = (s32)roundf(splitHovered->x);

								}

								std::vector<DockContainer*> leftMost;
								getLeftMostContainers(leftMost);

								for (DockContainer* container : splitHovered->containers1) {
									DockSplit* splitLeft = container->splitLeft;
									if (splitLeft != nullptr) pushSplitLeft(splitLeft);
								}

								for (DockContainer* container : leftMost) {
									clampSplitFromLeft(container->splitRight);
								}

							}
							else {

								minAreaWidth = getMinAreaWidthFromLeftToRight(splitHovered, 0);
								if (splitHovered->x > (f32)widthDraw - (f32)minAreaWidth) {

									splitHovered->x = (f32)widthDraw - (f32)minAreaWidth;
									splitHovered->xRound = (s32)roundf(splitHovered->x);

								}

								std::vector<DockContainer*> rightMost;
								getRightMostContainers(rightMost);

								for (DockContainer* container : splitHovered->containers2) {
									DockSplit* splitRight = container->splitRight;
									if (splitRight != nullptr) pushSplitRight(splitRight);
								}

								for (DockContainer* container : rightMost) {
									clampSplitFromRight(container->splitLeft);
								}

							}

							realignSplits();
							updateAllContainerSizes();

						}

					}

				}

				if (!gui->isMouseDown(MOUSE_LEFT)) {

					gui->deselectWidget(this);

					isDocking = false;

					if (gui->dockTabDragArea == this && gui->dockTabDrag != nullptr) {

						showTooltip = true;

						bool createFloatingWindow = false;

						if (dockToContainer != nullptr || dockToContainerEmpty) {

							if (dockToSide != DockSplitSide::None) {

								s32 dockWidth = -1, dockHeight = -1;
								if (dockToContainer != nullptr) {

									if (dockToSide == DockSplitSide::Left || dockToSide == DockSplitSide::Right) {
										dockWidth = Math::minInt(gui->dockTabDragContainerWidth, (dockToContainer->width / 2));
									}

									else if (dockToSide == DockSplitSide::Up || dockToSide == DockSplitSide::Down) {
										dockHeight = Math::minInt(gui->dockTabDragContainerHeight, (dockToContainer->height / 2));
									}

								}

								DockContainer* container = addContainer((dockToContainer != nullptr) ? dockToContainer : nullptr, dockToSide, (f32)dockWidth, (f32)dockHeight, false);

								//@Consider: Don't really like how all of this is hard-coded.
								DockTab* tab = gui->dockTabDrag;
								tab->container = container;
								tab->window->dockTab = tab;
								tab->window->opened = true;
								tab->window->maximized = false;
								tab->window->docked = true;

								for (Widget* child : tab->window->widgetList) {
									child->setVisibleGlobal(true);
								}

								container->tabList.push_back(tab);
								container->selectTab(tab);

								if ((container->minWidth < tab->window->minWidth && tab->window->hasSetMinWidth) || (container->minHeight < tab->window->minHeight && tab->window->hasSetMinHeight)) {

									if (container->minWidth < tab->window->minWidth && tab->window->hasSetMinWidth) {

										container->minWidth = tab->window->minWidth;
										fixHorizontal(false);

									}

									if (container->minHeight < tab->window->minHeight && tab->window->hasSetMinHeight) {

										container->minHeight = tab->window->minHeight;
										fixVertical(false);

									}

									realignSplits();
									updateAllContainerSizes();

								}

								container->updateTabs();

								for (DockTab* tab : container->tabList) {
									tab->updateWindowRect();
								}

								if (dockToContainer != nullptr) {

									dockToContainer->updateTabs();

									for (DockTab* tab : dockToContainer->tabList) {
										tab->updateWindowRect();
									}

								}

							}
							else {
								createFloatingWindow = true;
							}

						}
						else {
							createFloatingWindow = true;
						}

						if (createFloatingWindow) {

							gui->dockTabDrag->window->open(mouseX - gui->dockTabDragMouseXOffset, mouseY - gui->dockTabDragMouseYOffset, gui->dockTabDragContainerWidth, gui->dockTabDragContainerHeight);
							delete gui->dockTabDrag;

						}

						gui->dockTabDrag = nullptr;
						gui->dockTabDragArea = nullptr;

					}
					else if (tabClose != nullptr) {

						if (Math::pointInRect(mouseX, mouseY, x + tabClose->container->x + tabClose->x + tabClose->width - sprDockTabClose->sizeX - GUI_DOCK_TAB_CLOSE_HOR_SPACING, y + tabClose->container->y + (sprDockTab->sizeY / 2) - (sprDockTabClose->sizeY / 2), sprDockTabClose->sizeX, sprDockTabClose->sizeY)) {
							tabClose->window->close();
						}

						tabClose = nullptr;
						showTooltip = true;

					}
					else if (tabMove != nullptr) {

						tabMove = nullptr;

						if (tabMouseOver != tooltipTab) {
							tooltipTab = nullptr;
							showTooltip = true;
						}

					}
					else {

						showTooltip = true;

						s32 sprWidth = sprDockSplitVer->sizeX;
						s32 sprHeight = sprDockSplitHor->sizeY;

						if (splitHovered->dir == 0) {

							if (!Math::pointInRect(mouseX, mouseY, x + splitHovered->xRound, y + splitHovered->yRound + (sprHeight / 2) - (GUI_DOCK_GRAB_RANGE / 2), splitHovered->lengthRound, GUI_DOCK_GRAB_RANGE)) {
								splitHovered = nullptr;
							}

						}
						else {

							if (!Math::pointInRect(mouseX, mouseY, x + splitHovered->xRound + (sprWidth / 2) - (GUI_DOCK_GRAB_RANGE / 2), y + splitHovered->yRound, GUI_DOCK_GRAB_RANGE, splitHovered->lengthRound)) {
								splitHovered = nullptr;
							}

						}

					}

				}

			}

		}
		else {

			gui->deselectWidget(this);

			splitHovered = nullptr;
			tabHovered = nullptr;
			tabMouseOver = nullptr;
			tabMove = nullptr;
			tabClose = nullptr;
			dockToContainer = nullptr;
			dockToContainerEmpty = false;
			isDocking = false;
			showTooltip = true;
			tooltipTab = nullptr;

			if (gui->dockTabDragArea == this && gui->dockTabDrag != nullptr) {

				delete gui->dockTabDrag;
				gui->dockTabDrag = nullptr;
				gui->dockTabDragArea = nullptr;
			}
		}

	}

	void DockArea::render() {
		
		//Render dock area background.
		renderer->render9P(sprDockArea, 0, x, y, widthDraw, heightDraw);

		for (DockContainer* container : containerList) {

			s32 containerX = x + container->x;
			s32 containerY = y + container->y;
			s32 containerWidth = container->width;
			s32 containerHeight = container->height;

			//Render container background.
			renderer->render9P(sprDockContainer, 0, containerX, containerY, containerWidth, containerHeight);

			//Render tab line.
			renderer->renderSpriteStretched(sprDockTabLine, 0, containerX, containerY + sprDockTab->sizeY - sprDockTabLine->sizeY, containerWidth, sprDockTabLine->sizeY);

			s32 tabCount = (s32)container->tabList.size();
			
			if (tabCount > 0) {	

				//Render window widgets.
				Window* window = container->tabSelected->window;

				renderer->cutStart(window->x, window->y, window->width - (window->sprScrollVer->sizeX * window->scrollVerShow), window->height - (window->sprScrollHor->sizeY * window->scrollHorShow));

				for (Widget* child : window->widgetList) {

					if (child->visibleGlobal) {
						child->renderMain();
					}

				}

				renderer->cutEnd();

				//Render tabs.
				renderer->cutStart(containerX, containerY, containerWidth, containerHeight);

				//Render all tabs except the selected one.
				for (DockTab* tab : container->tabList) {

					if (tab == container->tabSelected) continue;

					s32 tabX = containerX + ((tabMove == tab) ? tabMoveX : tab->x);

					renderer->render3PHor(sprDockTab, 0, tabX, containerY, tab->width);

					Color4f titleCol;
					if (tabHovered == tab) titleCol = GUI_DOCK_TAB_TITLE_COL_FOCUSED; else titleCol = GUI_DOCK_TAB_TITLE_COL_UNFOCUSED;

					renderer->cutStart(tabX, containerY, tab->width, sprDockTab->sizeY);

					if (tab->window->showButtonClose) {

						s32 diff = tab->window->showButtonClose ? Math::maxInt(0, ((GUI_DOCK_TAB_TEXT_HOR_SPACING * 2) + renderer->getStringWidth(fntDockTab, tab->titleShow) - 1) - (tab->width - ((sprDockTabClose->sizeX + GUI_DOCK_TAB_CLOSE_HOR_SPACING) * (s32)tab->window->showButtonClose))) : 0;

						renderer->cutStart(tabX, containerY, tab->width - sprDockTabClose->sizeX - GUI_DOCK_TAB_CLOSE_HOR_SPACING, sprDockTab->sizeY);
						renderer->renderText(fntDockTab, tab->titleShow, Math::maxInt(tabX + 1, tabX + GUI_DOCK_TAB_TEXT_HOR_SPACING - diff), containerY + (sprDockTab->sizeY / 2) + GUI_DOCK_TAB_TEXT_VER_SPACING, TextAlign::Left, TextAlign::Middle, titleCol);
						renderer->cutEnd();

					}
					else {
						renderer->renderText(fntDockTab, tab->titleShow, tabX + (tab->width / 2), containerY + (sprDockTab->sizeY / 2) + GUI_DOCK_TAB_TEXT_VER_SPACING, TextAlign::Center, TextAlign::Middle, titleCol);
					}

					if (tab->window->showButtonClose) renderer->renderSprite(sprDockTabClose, (tabHovered == tab), tabX + tab->width - sprDockTabClose->sizeX - GUI_DOCK_TAB_CLOSE_HOR_SPACING, containerY + (sprDockTab->sizeY / 2) - (sprDockTabClose->sizeY / 2));

					renderer->cutEnd();

				}

				//Render selected tab above the others.
				DockTab* tab = container->tabSelected;

				s32 tabX = containerX + ((tabMove == tab) ? tabMoveX : tab->x);

				renderer->render3PHor(sprDockTab, 1, tabX, containerY, tab->width);

				renderer->cutStart(tabX, containerY, tab->width, sprDockTab->sizeY);

				if (tab->window->showButtonClose) {

					s32 diff = tab->window->showButtonClose ? Math::maxInt(0, ((GUI_DOCK_TAB_TEXT_HOR_SPACING * 2) + renderer->getStringWidth(fntDockTab, tab->titleShow) - 1) - (tab->width - ((sprDockTabClose->sizeX + GUI_DOCK_TAB_CLOSE_HOR_SPACING) * (s32)tab->window->showButtonClose))) : 0;

					renderer->cutStart(tabX, containerY, tab->width - sprDockTabClose->sizeX - GUI_DOCK_TAB_CLOSE_HOR_SPACING, sprDockTab->sizeY);
					renderer->renderText(fntDockTab, tab->titleShow, Math::maxInt(tabX + 1, tabX + GUI_DOCK_TAB_TEXT_HOR_SPACING - diff), containerY + (sprDockTab->sizeY / 2) + GUI_DOCK_TAB_TEXT_VER_SPACING, TextAlign::Left, TextAlign::Middle, GUI_DOCK_TAB_TITLE_COL_FOCUSED);
					renderer->cutEnd();

				}
				else {
					renderer->renderText(fntDockTab, tab->titleShow, tabX + (tab->width / 2), containerY + (sprDockTab->sizeY / 2) + GUI_DOCK_TAB_TEXT_VER_SPACING, TextAlign::Center, TextAlign::Middle, GUI_DOCK_TAB_TITLE_COL_FOCUSED);
				}

				if (tab->window->showButtonClose) renderer->renderSprite(sprDockTabClose, 1, tabX + tab->width - sprDockTabClose->sizeX - GUI_DOCK_TAB_CLOSE_HOR_SPACING, containerY + (sprDockTab->sizeY / 2) - (sprDockTabClose->sizeY / 2));

				renderer->cutEnd();

				renderer->cutEnd();

			}

		}

		for (DockSplit* split : splitList) {

			if (split->dir == 0) {
				renderer->render3PHor(sprDockSplitHor, (splitHovered == split), x + split->xRound, y + split->yRound, split->lengthRound);
			}
			else {
				renderer->render3PVer(sprDockSplitVer, (splitHovered == split), x + split->xRound, y + split->yRound, split->lengthRound);
			}

		}

		if (gui->dockTabDragArea == this && gui->dockTabDrag != nullptr) {

			if (dockToSide != DockSplitSide::None) {

				//@TODO: Replace dock preview sprite with shader?
				if (dockToContainer != nullptr) {

					s32 dockWidth = Math::minInt(gui->dockTabDragContainerWidth, (dockToContainer->width / 2));
					s32 dockHeight = Math::minInt(gui->dockTabDragContainerHeight, (dockToContainer->height / 2));

					if (dockToSide == DockSplitSide::Up) renderer->render9PRepeat(sprDockPreview, 0, x + dockToContainer->x, y + dockToContainer->y, dockToContainer->width, dockHeight);
					else if (dockToSide == DockSplitSide::Down) renderer->render9PRepeat(sprDockPreview, 0, x + dockToContainer->x, y + dockToContainer->y + dockToContainer->height - dockHeight, dockToContainer->width, dockHeight);
					else if (dockToSide == DockSplitSide::Left) renderer->render9PRepeat(sprDockPreview, 0, x + dockToContainer->x, y + dockToContainer->y, dockWidth, dockToContainer->height);
					else if (dockToSide == DockSplitSide::Right) renderer->render9PRepeat(sprDockPreview, 0, x + dockToContainer->x + dockToContainer->width - dockWidth, y + dockToContainer->y, dockWidth, dockToContainer->height);

				}
				else if (dockToContainerEmpty) {

					s32 dockWidth = Math::minInt(gui->dockTabDragContainerWidth, (widthDraw / 2));
					s32 dockHeight = Math::minInt(gui->dockTabDragContainerHeight, (heightDraw / 2));

					if (dockToSide == DockSplitSide::Up) renderer->render9PRepeat(sprDockPreview, 0, x, y, widthDraw, dockHeight);
					else if (dockToSide == DockSplitSide::Down) renderer->render9PRepeat(sprDockPreview, 0, x, y + heightDraw - dockHeight, widthDraw, dockHeight);
					else if (dockToSide == DockSplitSide::Left) renderer->render9PRepeat(sprDockPreview, 0, x, y, dockWidth, heightDraw);
					else if (dockToSide == DockSplitSide::Right) renderer->render9PRepeat(sprDockPreview, 0, x + widthDraw - dockWidth, y, dockWidth, heightDraw);

				}

			}

		}

	}

}