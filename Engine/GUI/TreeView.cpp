#include "Engine/ZixelPCH.h"
#include "Engine/KeyCodes.h"
#include "Engine/Renderer.h"
#include "Engine/TextureAtlas.h"
#include "Engine/Math.h"
#include "Engine/GUI/TreeView.h"
#include "Engine/GUI/LineEdit.h"
#include "Engine/GUI/DropDownMenu.h"
#include "Engine/GUI/Theme.h"
#include "Engine/GUI/GUI.h"

namespace Zixel {

	TreeView::TreeView(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme) : Widget(_gui, _parent, _window) {
		
		type = WidgetType::TreeView;
		treeViewTheme = (_theme != nullptr) ? (TreeViewTheme*)_theme : (TreeViewTheme*)gui->getDefaultTheme(type);

		minWidth = 30;
		minHeight = 30;

		cutContent = true;

		sprTreeViewBackground = renderer->getTextureAtlasSprite(treeViewTheme->sprTreeViewBackground);
		sprTreeViewArrow = renderer->getTextureAtlasSprite(treeViewTheme->sprTreeViewArrow);
		sprTreeViewDragDir = renderer->getTextureAtlasSprite(treeViewTheme->sprTreeViewDragDir);

		fntText = renderer->getTextureAtlasFont(treeViewTheme->font);

		editName = gui->createWidget<LineEdit>(this, "visible = false; includeWhenCalculatingParentContentWidth = false; maxLineCharCount = 255; height = " + std::to_string(treeViewTheme->editHeight) + ";");
		editName->setFont(treeViewTheme->font);
		editName->setOnConfirm(GUI_TEXT_EDIT_CONFIRM_CALLBACK(TreeView::onEditNameConfirm));
		editName->setOnReturn(GUI_TEXT_EDIT_RETURN_CALLBACK(TreeView::onEditNameReturn));
		//editName->setTextFilter("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_ ");

	}

	TreeView::~TreeView() {
		
		for (s32 i = (s32)itemList.size() - 1; i >= 0; --i) {
			destroyItem(itemList[i], false, false);
		}

	}

	bool TreeView::onItemUpdate(TreeViewItem* _item, s32 _itemX, s32 _itemY) { return false; }
	void TreeView::onItemDraw(TreeViewItem* _item, s32 _itemX, s32 _itemY) { }

	void TreeView::getMaxSize(std::vector<TreeViewItem*>& _itemList, s32& _width, s32& _height) {

		s32 newWidth = _width;

		for (TreeViewItem* item : _itemList) {

			if (_height == 0) {
				_height = treeViewTheme->itemVerSpacing;
			}

			bool hasChildren = (item->itemList.size() > 0);

			s32 tempWidth = newWidth + (treeViewTheme->itemHorSpacing * 2);
			if (item->name != "") tempWidth += (renderer->getStringWidth(fntText, item->name) + treeViewTheme->textHorSpacing);
			if (hasChildren) tempWidth += (sprTreeViewArrow->sizeX + treeViewTheme->arrowHorSpacing);
			if (item->sprIcon != nullptr) tempWidth += (item->sprIcon->sizeX + treeViewTheme->iconHorSpacing);

			if (tempWidth > _width) {
				_width = tempWidth;
			}

			_height += treeViewTheme->itemHeight;

			if (hasChildren && item->opened) {

				s32 prev = _width;
				_width = newWidth + treeViewTheme->tabWidth;

				getMaxSize(item->itemList, _width, _height);

				if (prev > _width) {
					_width = prev;
				}

			}

		}

	}

	void TreeView::calculateMaxSize() {

		itemsWidth = 0;
		itemsHeight = 0;

		getMaxSize(itemList, itemsWidth, itemsHeight);

		if (insideWidth != itemsWidth || insideHeight != itemsHeight) {
			setInsideSize(itemsWidth, itemsHeight);
		}

	}

	void TreeView::recalculateTotalSize() {

		calculateMaxSize();
		setScrollOffset(scrollHorOffset, scrollVerOffset);

	}

	TreeViewItem* TreeView::insertItem(std::string _name, TreeViewItem* _parent, s32 _position, Sprite* _icon, s32 _iconSub) {

		stopEditing();
		
		TreeViewItem* item = new TreeViewItem();

		if (itemSelected == nullptr) {
			itemSelected = item;
		}
		
		item->treeView = this;
		item->opened = true;
		item->sprIcon = _icon;
		item->iconSub = _iconSub;

		if (defaultItemMenu != nullptr) item->menu = defaultItemMenu;

		if (_parent == nullptr) {

			if (_position < 0) itemList.push_back(item);
			else itemList.insert(itemList.begin() + Math::minInt(_position, (s32)itemList.size()), item);

		}
		else {

			if (_position < 0) _parent->itemList.push_back(item);
			else _parent->itemList.insert(_parent->itemList.begin() + Math::minInt(_position, (s32)_parent->itemList.size()), item);

		}

		item->parent = _parent;
		item->name = _name;

		calculateMaxSize();

		if (onItemAdd) {
			onItemAdd(item);
		}

		return item;

	}

	TreeViewItem* TreeView::insertItem(std::string _name, TreeViewStructure& _structure, Sprite* _icon, s32 _iconSub) {

		TreeViewItem* parentItem = nullptr;
		s32 pos = -1;
		bool error = false;

		std::vector<TreeViewItem*>* tempList = &itemList;
		if (_structure.positionList.size() > 0) {

			for (s32 i = 0; i < _structure.positionList.size() - 1; ++i) {

				s32 tempPos = _structure.positionList[i];
				if (tempPos < 0 || tempPos >= tempList->size()) {

					error = true;
					break;

				}

				parentItem = (*tempList)[tempPos];
				tempList = &(parentItem->itemList);

			}

			if (!error) {

				s32 tempPos = _structure.positionList[_structure.positionList.size() - 1];

				if (tempPos >= 0 && tempPos < tempList->size()) {
					pos = tempPos;
				}

			}

		}

		return insertItem(_name, parentItem, pos, _icon, _iconSub);

	}

	TreeViewItem* TreeView::destroyItem(TreeViewItem* _item, bool _recalculateTotalSize, bool _callOnSelect, bool _callOnDestroy) {

		for (s32 i = (s32)_item->itemList.size() - 1; i >= 0; --i) {
			destroyItem(_item->itemList[i], false, /*true*/false, _callOnDestroy);
		}

		if (itemHovered == _item) {

			itemHovered = nullptr;
			arrowHovered = false;

		}

		if (itemSelected == _item) {
			itemSelected = nullptr;
		}

		/*if (itemEdit == _item) */ stopEditing(); //Maybe we should just update the edit position instead.

		TreeViewItem* itemToSelect = nullptr;

		if (_item->parent == nullptr) {

			const auto& it = std::find(itemList.begin(), itemList.end(), _item);

			if (it != itemList.end()) {

				s32 ind = (s32)(it - itemList.begin());
				itemList.erase(it);

				if (ind >= itemList.size()) --ind;
				if (ind >= 0) itemToSelect = itemList[ind];// selectItem(itemList[ind], false, true, _callOnSelect);
				else itemSelected = nullptr;

			}

		}
		else {

			const auto& it = std::find(_item->parent->itemList.begin(), _item->parent->itemList.end(), _item);

			if (it != _item->parent->itemList.end()) {

				s32 ind = (s32)(it - _item->parent->itemList.begin());
				_item->parent->itemList.erase(it);

				if (ind >= _item->parent->itemList.size()) --ind;
				if (ind >= 0) itemToSelect = _item->parent->itemList[ind];// selectItem(_item->parent->itemList[ind], false, true, _callOnSelect);
				else itemToSelect = _item->parent;//selectItem(_item->parent, false, true, _callOnSelect);

			}

		}

		if (_callOnDestroy && onItemDestroy) {
			onItemDestroy(_item);
		}

		if (itemToSelect != nullptr) {
			selectItem(itemToSelect, false, true, _callOnSelect);
		}
		
		delete _item;

		if (_recalculateTotalSize) {
			calculateMaxSize();
		}

		return itemSelected;

	}

	void TreeView::selectItem(TreeViewItem* _item, bool _wasItemAdded, bool _wasItemDeleted, bool _callOnSelect, bool _forceCall) {

		bool call = (itemSelected != _item || _forceCall);

		if (itemSelected != _item) itemSelected = _item;

		if (onItemSelect && _callOnSelect && call) {
			onItemSelect(_item, _wasItemAdded, _wasItemDeleted);
		}

	}

	void TreeView::moveItem(TreeViewItem* _item, TreeViewStructure& _structure, bool _recalculateTotalSize, bool _callOnMove) {

		std::vector<TreeViewItem*>& parentList = (_item->parent != nullptr) ? _item->parent->itemList : itemList;

		const auto& it = std::find(parentList.begin(), parentList.end(), _item);

		if (it == parentList.end()) {

			ZIXEL_WARN("Error in TreeView::moveItem. Item '{}' is not part of TreeView.", _item->name);
			return;

		}

		TreeViewItem* prevParent = _item->parent;
		size_t prevPos = (prevParent == nullptr) ? (std::find(itemList.begin(), itemList.end(), _item) - itemList.begin()) : (std::find(prevParent->itemList.begin(), prevParent->itemList.end(), _item) - prevParent->itemList.begin());

		parentList.erase(it);

		TreeViewItem* itemParent = nullptr;
		s32 itemPosition = -1;

		if (_structure.positionList.size() > 0) {

			itemPosition = _structure.positionList[_structure.positionList.size() - 1];

			for (s32 i = 0; i < _structure.positionList.size() - 1; ++i) {

				s32 ind = _structure.positionList[i];

				if (ind < 0) {

					ZIXEL_WARN("Error in TreeView::moveItem. Position index ({}) is less than zero.", ind);
					break;

				}

				if (itemParent == nullptr) {

					if (ind >= itemList.size()) {

						ZIXEL_WARN("Error in TreeView::moveItem. Position index ({}) is out of bounds. Valid range: 0-{}", ind, (s32)itemList.size() - 1);
						break;

					}

					itemParent = itemList[ind];

				}
				else {

					if (ind >= itemParent->itemList.size()) {

						ZIXEL_WARN("Error in TreeView::moveItem. Position index ({}) is out of bounds. Valid range: 0-{}", ind, (s32)itemParent->itemList.size() - 1);
						break;

					}

					itemParent = itemParent->itemList[ind];

				}

			}

		}

		std::vector<TreeViewItem*>& newParentList = (itemParent != nullptr) ? itemParent->itemList : itemList;

		_item->parent = itemParent;

		if (itemPosition < 0) {
			newParentList.push_back(_item);
		}
		else {

			if (itemPosition > newParentList.size()) {

				ZIXEL_WARN("Error in TreeView::moveItem. Position index ({}) is out of bounds. Valid range: 0-{}", itemPosition, newParentList.size());
				newParentList.push_back(_item);

			}
			else {
				newParentList.insert(newParentList.begin() + itemPosition, _item);
			}

		}

		if (_recalculateTotalSize) {
			
			calculateMaxSize();
			setScrollOffset(scrollHorOffset, scrollVerOffset);

		}

		size_t newPos = (_item->parent == nullptr) ? (std::find(itemList.begin(), itemList.end(), _item) - itemList.begin()) : (std::find(_item->parent->itemList.begin(), _item->parent->itemList.end(), _item) - _item->parent->itemList.begin());

		if (prevParent != _item->parent || prevPos != newPos) {

			scrollToItem(_item);

			if (_callOnMove && onItemMove) {
				onItemMove(_item, _item->getStructure());
			}

		}

	}

	TreeViewItem* TreeView::getItemFromStructure(TreeViewStructure& _structure) {

		TreeViewItem* item = nullptr;

		for (s32 ind : _structure.positionList) {

			if (ind < 0) {

				ZIXEL_WARN("Error in TreeView::getItemFromStructure. Position index ({}) is less than zero.", ind);
				return nullptr;

			}

			if (item == nullptr) {

				if (ind >= itemList.size()) {

					ZIXEL_WARN("Error in TreeView::getItemFromStructure. Position index ({}) is out of bounds. Valid range: 0-{}", ind, (s32)itemList.size() - 1);
					return nullptr;

				}

				item = itemList[ind];

			}
			else {

				if (ind >= item->itemList.size()) {

					ZIXEL_WARN("Error in TreeView::getItemFromStructure. Position index ({}) is out of bounds. Valid range: 0-{}", ind, (s32)item->itemList.size() - 1);
					return nullptr;

				}

				item = item->itemList[ind];

			}

		}

		return item;

	}

	s32 TreeView::getItemCount(s32 _filterType, TreeViewItem* _rootItem) {

		s32 count = 0;

		std::vector<TreeViewItem*>& list = (_rootItem != nullptr) ? _rootItem->itemList : itemList;

		for (TreeViewItem* item : list) {

			if (_filterType == -1 || item->filterType == _filterType) {
				++count;
			}

			count += getItemCount(_filterType, item);

		}

		return count;

	}

	void TreeView::scrollToItem(TreeViewItem* _item) {

		s32 itemY = _item->getY();

		s32 scrollVer = (s32)roundf(scrollVerOffset);

		if (itemY - scrollVer < 0) { //Outside top.
			setScrollOffset(scrollHorOffset, (f32)(scrollVer + (itemY - scrollVer)));
		}
		else if (itemY - scrollVer + treeViewTheme->itemHeight - 1 >= viewportHeight) { //Outside bottom.
			setScrollOffset(scrollHorOffset, (f32)((itemY + treeViewTheme->itemHeight) - viewportHeight));
		}

	}

	void TreeView::startEditing(TreeViewItem* _item) {

		if (itemEdit != nullptr) stopEditing();

		itemEdit = _item;

		s32 itemX = _item->getX() + treeViewTheme->itemHorSpacing + treeViewTheme->textHorSpacing;
		if (_item->itemList.size() > 0) itemX += (sprTreeViewArrow->sizeX + treeViewTheme->arrowHorSpacing);

		s32 itemY = _item->getY() + ((treeViewTheme->itemHeight - editName->height) / 2) + treeViewTheme->textVerSpacing;

		s32 editWidth = Math::maxInt(Math::maxInt(viewportWidth, contentWidth) - itemX, treeViewTheme->editMinWidth);
		
		gui->setWidgetSelected(nullptr);
		gui->setWidgetFocused(editName, true, true);
		editName->setVisible(true);
		editName->setPadding(itemX, itemY, 0, 0);
		editName->setWidth(editWidth);
		editName->setText(_item->name, GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, false);
		editName->selectAll();
		editName->scrollToChar = 0; //Force scroll to the left.

	}

	void TreeView::stopEditing() {

		if (itemEdit == nullptr) return;

		std::string newName;
		editName->getLineText(newName, 0);

		StringHelper::clean(newName);
		if (!newName.empty() && newName != itemEdit->name) {

			std::string oldName = itemEdit->name;

			itemEdit->name = newName;
			recalculateTotalSize();
			
			if (onItemNameChange) onItemNameChange(itemEdit, oldName);

		}

		itemEdit = nullptr;
		editName->setVisible(false);

	}

	void TreeView::onEditNameConfirm(TextEdit* _textEdit) {
		stopEditing();
	}

	void TreeView::onEditNameReturn(TextEdit* _textEdit) {
		gui->setWidgetFocused(this);
	}

	void TreeView::setDefaultItemMenu(DropDownMenu* _menu) {
		defaultItemMenu = _menu;
	}

	void TreeView::updateItems(std::vector<TreeViewItem*>& _itemList, s32 _x, s32& _y, f32 dt) {

		for (TreeViewItem* item : _itemList) {

			bool hasChildren = (item->itemList.size() > 0);

			if (Math::pointInRect(gui->mouseX, gui->mouseY, x, y + _y, viewportWidth, treeViewTheme->itemHeight)) {
				itemHovered = item;
			}

			if (gui->widgetSelected != this) {

				if (itemHovered == item && item != nullptr) { //For some reason VS complains that 'item' could potentially be NULL, so I'm just check for a null pointer here. (Even tho it can never happend and VS is just the worst piece of software I've ever used.)

					arrowHovered = false;

					if (hasChildren) {
						arrowHovered = Math::pointInRect(gui->mouseX, gui->mouseY, x + _x + treeViewTheme->itemHorSpacing, y + _y + (treeViewTheme->itemHeight / 2) - (sprTreeViewArrow->sizeY / 2), sprTreeViewArrow->sizeX, sprTreeViewArrow->sizeY);
					}

					if ((gui->isMousePressed(MOUSE_LEFT) || gui->isMousePressed(MOUSE_RIGHT)) && !hasInteracted) {

						hasInteracted = true;

						itemMouse = (gui->isMousePressed(MOUSE_LEFT) ? MOUSE_LEFT : MOUSE_RIGHT);

						if (arrowHovered) {

							gui->setWidgetFocused(this);

							//item->opened = !(item->opened);
							item->setOpened(!item->opened, false, true);

							itemHovered = nullptr;
							arrowHovered = false;

							calculateMaxSize();

							//selectItem(item);

						}
						else {

							gui->setWidgetFocused(this);

							bool select = !onItemUpdate(item, x + _x, y + _y);

							if (select) {

								gui->setWidgetSelected(this);
								selectItem(item);

							}

							itemDrag = false;
							itemDragMouseY = gui->mouseY;

							if (select) {

								if (doubleClickItem != item || (renderer->getCurrentTime() - doubleClickTime > treeViewTheme->doubleClickTime) || (itemMouse == MOUSE_RIGHT && item->menu != nullptr) || (doubleClickMouse != itemMouse)){

									doubleClickItem = item;
									doubleClickTime = renderer->getCurrentTime();
									doubleClickMouse = itemMouse;

								}
								else if (doubleClickItem == item && (renderer->getCurrentTime() - doubleClickTime <= treeViewTheme->doubleClickTime)) {

									bool shouldStartEdit = (gui->mouseX >= x + _x + treeViewTheme->itemHorSpacing + treeViewTheme->textHorSpacing);

									if (shouldStartEdit) startEditing(item);
									else if (hasChildren) {

										//item->opened = !(item->opened);
										item->setOpened(!item->opened, false, true);
										calculateMaxSize();

									}

								}

							}
							else {
								doubleClickItem = nullptr;
							}

						}

					}

				}

			}
			else {

				if (!itemDrag) {

					if (Math::absInt(gui->mouseY - itemDragMouseY) >= treeViewTheme->dragStartDistance) {

						itemDrag = true;
						itemDragDir = (gui->mouseY >= y + _y + (treeViewTheme->itemHeight / 2));

						if (gui->mouseY < y + _y + (treeViewTheme->itemHeight / 3)) itemDragDir = 0;
						else if (gui->mouseY >= y + _y + treeViewTheme->itemHeight - (treeViewTheme->itemHeight / 3)) itemDragDir = 1;
						else itemDragDir = 2;

						if (itemDragDir == 2) {

							itemDragCanDrop = (itemSelected->checkFilter(item) && !item->hasParent(itemSelected));

							if (!itemDragCanDrop) {

								if (gui->mouseY < y + _y + (treeViewTheme->itemHeight / 2)) {
									itemDragDir = 0;
								}
								else {
									itemDragDir = 1;
								}

								itemDragCanDrop = (itemSelected->checkFilter(item->parent) && !item->hasParent(itemSelected));

							}

						}
						else {

							itemDragCanDrop = (itemSelected->checkFilter(item->parent) && !item->hasParent(itemSelected));

							if (!itemDragCanDrop) {

								itemDragDir = 2;
								itemDragCanDrop = (itemSelected->checkFilter(item) && !item->hasParent(itemSelected));

							}

						}

					}

				}
				else {
					
					if (itemHovered != itemSelected && item == itemHovered) {

						if (gui->mouseY < y + _y + (treeViewTheme->itemHeight / 3)) itemDragDir = 0;
						else if (gui->mouseY >= y + _y + treeViewTheme->itemHeight - (treeViewTheme->itemHeight / 3)) itemDragDir = 1;
						else itemDragDir = 2;

					}

					if (gui->mouseY < y + treeViewTheme->dragScrollRange) {

						f32 p = Math::clampFloat(1.0f - ((f32)(gui->mouseY - y - 1) / (f32)(treeViewTheme->dragScrollRange)), 0.0f, 1.0f);
						setScrollOffset(scrollHorOffset, scrollVerOffset - (treeViewTheme->dragScrollSpeed * p * dt));

					}
					else if (gui->mouseY >= y + viewportHeight - treeViewTheme->dragScrollRange) {

						f32 p = Math::clampFloat((f32)(gui->mouseY - (y + viewportHeight - treeViewTheme->dragScrollRange) + 1) / (f32)(treeViewTheme->dragScrollRange), 0.0f, 1.0f);
						setScrollOffset(scrollHorOffset, scrollVerOffset + (treeViewTheme->dragScrollSpeed * p * dt));

					}

					if (item == itemHovered) {

						if (itemDragDir == 2) {

							itemDragCanDrop = (itemSelected->checkFilter(item) && !item->hasParent(itemSelected));

							if (!itemDragCanDrop) {

								if (gui->mouseY < y + _y + (treeViewTheme->itemHeight / 2)) {
									itemDragDir = 0;
								}
								else {
									itemDragDir = 1;
								}

								itemDragCanDrop = (itemSelected->checkFilter(item->parent) && !item->hasParent(itemSelected));

							}

						}
						else {

							itemDragCanDrop = (itemSelected->checkFilter(item->parent) && !item->hasParent(itemSelected));

							if (!itemDragCanDrop) {

								itemDragDir = 2;
								itemDragCanDrop = (itemSelected->checkFilter(item) && !item->hasParent(itemSelected));

							}

						}

					}

				}

				if (!gui->isMouseDown(itemMouse)) {

					shouldDeselect = true;

					if (item == itemHovered) {

						if (itemDrag) {

							itemDrag = false;

							if (item != itemSelected && itemDragCanDrop) {

								TreeViewStructure structure = itemSelected->getStructure();

								s32 ind;
								if (itemSelected->parent == nullptr) {

									const auto& it = std::find(itemList.begin(), itemList.end(), itemSelected);
									ind = (s32)(it - itemList.begin());
									itemList.erase(it);

								}
								else {

									const auto& it = std::find(itemSelected->parent->itemList.begin(), itemSelected->parent->itemList.end(), itemSelected);
									ind = (s32)(it - itemSelected->parent->itemList.begin());
									itemSelected->parent->itemList.erase(it);

								}

								TreeViewItem* prevParent = itemSelected->parent;
								s32 prevInd = ind;

								if (itemDragDir != 2) {

									itemSelected->parent = item->parent;

									if (item->parent == nullptr) {

										const auto& it = std::find(itemList.begin(), itemList.end(), item);
										itemList.insert(it + itemDragDir, itemSelected);

									}
									else {

										const auto& it = std::find(item->parent->itemList.begin(), item->parent->itemList.end(), item);
										item->parent->itemList.insert(it + itemDragDir, itemSelected);

									}

								}
								else {

									itemSelected->parent = item;
									item->itemList.push_back(itemSelected);

									item->opened = true;

								}

								calculateMaxSize();

								if (itemSelected->parent == nullptr) {

									const auto& it = std::find(itemList.begin(), itemList.end(), itemSelected);
									ind = (s32)(it - itemList.begin());

								}
								else {

									const auto& it = std::find(itemSelected->parent->itemList.begin(), itemSelected->parent->itemList.end(), itemSelected);
									ind = (s32)(it - itemSelected->parent->itemList.begin());

								}

								if (prevParent != itemSelected->parent || prevInd != ind) {

									scrollToItem(itemSelected);

									if (onItemMove) {
										onItemMove(itemSelected, structure);
									}

									breakOutOfLoop = true;
									break;

								}

							}

						}
						else {

							if (item == itemSelected && itemMouse == MOUSE_RIGHT && item->menu != nullptr) {
								item->menu->open(gui->mouseX, gui->mouseY + 1);
							}

						}

					}

				}

			}

			_y += treeViewTheme->itemHeight;

			if (hasChildren && item->opened) {

				updateItems(item->itemList, _x + treeViewTheme->tabWidth, _y, dt);

				if (breakOutOfLoop) {
					break;
				}

			}

		}

	}

	void TreeView::update(f32 dt) {

		if (canInteract()) {
			
			if (gui->widgetSelected != this) {

				if (gui->getWidgetMouseOver() == this) {

					itemHovered = nullptr;
					arrowHovered = false;
					hasInteracted = false;

					s32 itemX = -(s32)roundf(scrollHorOffset), itemY = treeViewTheme->itemVerSpacing - (s32)roundf(scrollVerOffset);
					updateItems(itemList, itemX, itemY, dt);

					breakOutOfLoop = false;

                    if(!itemHovered && (gui->isMousePressed(MOUSE_LEFT) || gui->isMousePressed(MOUSE_RIGHT))){
                        gui->setWidgetFocused(this);
                    }

				}

			}
			else {

				itemHovered = nullptr;
				hasInteracted = false;
				
				s32 itemX = -(s32)roundf(scrollHorOffset), itemY = treeViewTheme->itemVerSpacing - (s32)roundf(scrollVerOffset);
				updateItems(itemList, itemX, itemY, dt);

				breakOutOfLoop = false;

				if (shouldDeselect) {

					gui->deselectWidget(this);
					shouldDeselect = false;

					itemDrag = false;

				}

			}

		}
		else {

			gui->deselectWidget(this);

			if (gui->widgetSelected != editName) stopEditing(); //This prevents the text edit from deselecting when we try to click on it. Not sure if this is the best solution, but I think it's fine in any real-case scenario.

			itemHovered = nullptr;
			itemDrag = false;
			arrowHovered = false;
			doubleClickItem = nullptr;
			shouldDeselect = false;
			breakOutOfLoop = false;
			hasInteracted = false;

		}

	}

	void TreeView::renderItems(std::vector<TreeViewItem*>& _itemList, s32 _x, s32& _y) {

		s32 lineYStart = _y;
		s32 lineYEnd = _y;
		s32 lineX = x + _x - treeViewTheme->tabWidth + treeViewTheme->itemHorSpacing + (sprTreeViewArrow->sizeX / 2);

		for (TreeViewItem* item : _itemList) {

			bool hasChildren = (item->itemList.size() > 0);

			if (itemSelected == item || (itemDrag && itemHovered == item && itemDragCanDrop)) {

				Color4f col;
				if (itemSelected != item && itemDrag) col = treeViewTheme->colItemDrag; else col = treeViewTheme->colItemSelected;

				renderer->renderRect(x, y + _y, viewportWidth, treeViewTheme->itemHeight, col);

				if (itemSelected != item && itemDrag) {

					if(itemDragDir == 0) renderer->renderSpriteStretched(sprTreeViewDragDir, 0, x, y + _y + sprTreeViewDragDir->sizeY, viewportWidth, -sprTreeViewDragDir->sizeY);
					else if(itemDragDir == 1) renderer->renderSpriteStretched(sprTreeViewDragDir, 0, x, y + _y + treeViewTheme->itemHeight - sprTreeViewDragDir->sizeY, viewportWidth, sprTreeViewDragDir->sizeY);

				}

			}

			if (_itemList != itemList) {

				renderer->renderRect(lineX, y + _y + (treeViewTheme->itemHeight / 2), treeViewTheme->tabWidth - treeViewTheme->lineSpacing, 1, treeViewTheme->colLine);

				if (item == _itemList[_itemList.size() - 1]) {
					lineYEnd = _y + treeViewTheme->itemHeight;
				}

			}

			s32 itemX = x + _x + treeViewTheme->itemHorSpacing;

			if (hasChildren) {

				renderer->renderSprite(sprTreeViewArrow, (2 * item->opened) + (arrowHovered * (itemHovered == item)), itemX, y + _y + (treeViewTheme->itemHeight / 2) - (sprTreeViewArrow->sizeY / 2));
				itemX += (sprTreeViewArrow->sizeX + treeViewTheme->arrowHorSpacing);

			}

			if (item->sprIcon != nullptr) {

				renderer->renderSprite(item->sprIcon, item->iconSub, itemX, y + _y + (treeViewTheme->itemHeight / 2) - (item->sprIcon->sizeY / 2) + treeViewTheme->iconVerOffset);
				itemX += (item->sprIcon->sizeX + treeViewTheme->iconHorSpacing);

			}

			if (item->name != "" && itemEdit != item) {
				renderer->renderText(fntText, item->name, itemX + treeViewTheme->textHorSpacing, y + _y + (treeViewTheme->itemHeight / 2) + treeViewTheme->textVerSpacing, TextAlign::Left, TextAlign::Middle, treeViewTheme->colText);
			}

			onItemDraw(item, itemX, y + _y);

			_y += treeViewTheme->itemHeight;

			if (hasChildren && item->opened) {
				renderItems(item->itemList, _x + treeViewTheme->tabWidth, _y);
			}

		}

		if (_itemList != itemList) {

			s32 lineHeight = lineYEnd - lineYStart - treeViewTheme->itemHeight + (treeViewTheme->itemHeight / 2);
			renderer->renderRect(lineX, y + lineYStart, 1, lineHeight, treeViewTheme->colLine);

		}

	}

	void TreeView::render() {
		
		renderer->render9P(treeViewTheme->sprTreeViewBackground, 0, x, y, viewportWidth, viewportHeight);

		renderer->cutStart(x + treeViewTheme->cutoffLeft, y + treeViewTheme->cutoffTop, viewportWidth - (treeViewTheme->cutoffLeft + treeViewTheme->cutoffRight), viewportHeight - (treeViewTheme->cutoffTop + treeViewTheme->cutoffBottom));
		
		s32 itemX = -(s32)roundf(scrollHorOffset), itemY = treeViewTheme->itemVerSpacing - (s32)roundf(scrollVerOffset);
		renderItems(itemList, itemX, itemY);

		renderer->cutEnd();

	}

	void TreeView::setOnItemAdd(std::function<void(TreeViewItem*)> _onItemAdd) {
		onItemAdd = _onItemAdd;
	}

	void TreeView::setOnItemDestroy(std::function<void(TreeViewItem*)> _onItemDestroy) {
		onItemDestroy = _onItemDestroy;
	}

	void TreeView::setOnItemSelect(std::function<void(TreeViewItem*, bool, bool)> _onItemSelect) {
		onItemSelect = _onItemSelect;
	}

	void TreeView::setOnItemMove(std::function<void(TreeViewItem*, TreeViewStructure)> _onItemMove) {
		onItemMove = _onItemMove;
	}

	void TreeView::setOnItemOpenClose(std::function<void(TreeViewItem*, bool)> _onItemOpenClose) {
		onItemOpenClose = _onItemOpenClose;
	}

	void TreeView::setOnItemNameChange(std::function<void(TreeViewItem*, std::string)> _onItemNameChange) {
		onItemNameChange = _onItemNameChange;
	}

	bool TreeViewItem::hasParent(TreeViewItem* _item) {

		if (parent == nullptr) {
			return (_item == nullptr);
		}

		if (parent == _item) {
			return true;
		}

		return parent->hasParent(_item);

	}

	bool TreeViewItem::checkFilter(TreeViewItem* _item) {

		if (_item == nullptr) {
			return (!filterTypeExclusive);
		}

		if (!_item->allowChildren) {
			return false;
		}

		if (filterType == _item->filterType) {
			return true; //Is this fine?
		}

		for (const s32& type : _item->filterList) {

			if (type == filterType) {
				return true;
			}

		}

		return false;

	}

	void TreeViewItem::setFilterType(s32 _filterType, bool _exclusive) {

		filterType = _filterType;
		filterTypeExclusive = _exclusive;

	}

	void TreeViewItem::addFilterMask(s32 _filterType) {
		filterList.push_back(_filterType);
	}

	void TreeViewItem::setAllowChildren(bool _allow) {
		allowChildren = _allow;
	}

	void TreeViewItem::setMenu(DropDownMenu* _menu) {
		menu = _menu;
	}

	TreeViewStructure TreeViewItem::getStructure() {

		TreeViewStructure structure;

		s32 ind;
		if (parent == nullptr) {
			ind = (s32)(std::find(treeView->itemList.begin(), treeView->itemList.end(), this) - treeView->itemList.begin());
		}
		else {
			ind = (s32)(std::find(parent->itemList.begin(), parent->itemList.end(), this) - parent->itemList.begin());
		}

		structure.addPosition(ind);

		TreeViewItem* temp = parent;
		while (temp != nullptr) {

			if (temp->parent == nullptr) {
				ind = (s32)(std::find(treeView->itemList.begin(), treeView->itemList.end(), temp) - treeView->itemList.begin());
			}
			else {
				ind = (s32)(std::find(temp->parent->itemList.begin(), temp->parent->itemList.end(), temp) - temp->parent->itemList.begin());
			}

			structure.insertPosition(ind, 0);

			temp = temp->parent;

		}
		
		return structure;

	}

	void TreeViewItem::openAllParents(bool _recalculateTotalSize, bool _callOnOpenClose) {

		if (parent != nullptr) {

			if (!parent->opened) {

				parent->opened = true;

				if (_callOnOpenClose && treeView->onItemOpenClose) {
					treeView->onItemOpenClose(parent, true);
				}

			}

			parent->openAllParents();

		}
		else {

			if (_recalculateTotalSize) {
				treeView->recalculateTotalSize();
			}

		}

	}

	void TreeViewItem::open(bool _recalculateTotalSize, bool _callOnOpenClose) {

		bool prev = opened;

		opened = false;

		if (allowChildren && itemList.size() > 0) {

			opened = true;

			if (_recalculateTotalSize) {
				treeView->recalculateTotalSize();
			}

		}

		if (prev != opened && _callOnOpenClose && treeView->onItemOpenClose) {
			treeView->onItemOpenClose(this, opened);
		}

	}

	void TreeViewItem::close(bool _recalculateTotalSize, bool _callOnOpenClose) {

		if (opened) {

			opened = false;

			if (_recalculateTotalSize) {
				treeView->recalculateTotalSize();
			}

			if (_callOnOpenClose && treeView->onItemOpenClose) {
				treeView->onItemOpenClose(this, false);
			}

		}

	}

	void TreeViewItem::setOpened(bool _opened, bool _recalculateTotalSize, bool _callOnOpenClose) {

		if (_opened && !opened) open(_recalculateTotalSize, _callOnOpenClose);
		else if (!_opened && opened) close(_recalculateTotalSize, _callOnOpenClose);

	}

	void TreeViewItem::setName(std::string _name) {

		if (_name == name) return;

		name = _name;
		treeView->recalculateTotalSize();

	}

	s32 TreeViewItem::getX() {

		if (parent == nullptr) return 0;
		return parent->getX() + treeView->treeViewTheme->tabWidth;

	}

	void TreeViewItem::getY(bool& _foundTarget, s32& _y, bool _isRoot, TreeViewItem* _target) {

		std::vector<TreeViewItem*>& curList = _isRoot ? treeView->itemList : itemList;

		for (TreeViewItem* item : curList) {

			if (item == _target) {

				_foundTarget = true;
				return;

			}

			_y += treeView->treeViewTheme->itemHeight;

			if (item->itemList.size() > 0 && item->opened) {

				item->getY(_foundTarget, _y, false, _target);

				if (_foundTarget) {
					return;
				}

			}

		}

	}

	s32 TreeViewItem::getY() {

		bool foundTarget = false;
		s32 itemY = treeView->treeViewTheme->itemVerSpacing;

		getY(foundTarget, itemY, true, this);
		
		if (!foundTarget) {
			return -1;
		}

		return itemY;

	}

	void TreeViewStructure::addPosition(s32 _position) {
		positionList.push_back(_position);
	}

	void TreeViewStructure::insertPosition(s32 _position, s32 _index) {
		positionList.insert(positionList.begin() + Math::clampInt(_index, 0, (s32)positionList.size()), _position);
	}

	bool TreeViewStructure::isEmpty() {
		return (positionList.size() <= 0);
	}

	void TreeViewStructure::print() {

		std::string output;
		for (s32 i = 0; i < positionList.size(); ++i) {

			output += std::to_string(positionList[i]);
			if (i < positionList.size() - 1) output += ", ";

		}

		ZIXEL_TRACE("TreeViewStructure: {}", output);

	}

}