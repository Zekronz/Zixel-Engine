/*
    TreeView.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <functional>
#include "Engine/GUI/Widget.h"

#define GUI_TREE_VIEW_ITEM_ADD_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1)
#define GUI_TREE_VIEW_ITEM_DESTROY_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1)
#define GUI_TREE_VIEW_ITEM_SELECT_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define GUI_TREE_VIEW_ITEM_MOVE_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2)
#define GUI_TREE_VIEW_ITEM_OPEN_CLOSE_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2)
#define GUI_TREE_VIEW_ITEM_NAME_CHANGE_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2)

#define GUI_TREE_VIEW_SPR_BACKGROUND "treeViewBackground"
#define GUI_TREE_VIEW_SPR_ARROW "treeViewArrow"
#define GUI_TREE_VIEW_SPR_DRAG_DIR "treeViewDragDir"
#define GUI_TREE_VIEW_FONT "robotoRegular12"
#define GUI_TREE_VIEW_TEXT_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_TREE_VIEW_LINE_COL { 0.404f, 0.408f, 0.447f }
#define GUI_TREE_VIEW_ITEM_SELECTED_COL { 0.192f, 0.196f, 0.247f }
#define GUI_TREE_VIEW_ITEM_DRAG_COL { 0.255f, 0.263f, 0.329f }
#define GUI_TREE_VIEW_ITEM_HEIGHT 20
#define GUI_TREE_VIEW_ITEM_HOR_SPACING 7
#define GUI_TREE_VIEW_ITEM_VER_SPACING 1
#define GUI_TREE_VIEW_ARROW_HOR_SPACING 4
#define GUI_TREE_VIEW_ICON_HOR_SPACING 4
#define GUI_TREE_VIEW_ICON_VER_OFFSET -1
#define GUI_TREE_VIEW_TEXT_HOR_SPACING 0
#define GUI_TREE_VIEW_TEXT_VER_SPACING -1
#define GUI_TREE_VIEW_LINE_SPACING 10
#define GUI_TREE_VIEW_TAB_WIDTH 22
#define GUI_TREE_VIEW_CUTOFF_TOP 0
#define GUI_TREE_VIEW_CUTOFF_LEFT 0
#define GUI_TREE_VIEW_CUTOFF_BOTTOM 0
#define GUI_TREE_VIEW_CUTOFF_RIGHT 0
#define GUI_TREE_VIEW_DRAG_START_DISTANCE 5
#define GUI_TREE_VIEW_DRAG_SCROLL_RANGE 24
#define GUI_TREE_VIEW_DRAG_SCROLL_SPEED 60.0f
#define GUI_TREE_VIEW_DOUBLE_CLICK_TIME 300
#define GUI_TREE_VIEW_EDIT_MIN_WIDTH 32
#define GUI_TREE_VIEW_EDIT_HEIGHT 17

namespace Zixel {

	struct Font;
	struct GUI;
	struct TextEdit;
	struct DropDownMenu;
	struct Theme;
	struct TreeViewTheme;

	struct TreeViewStructure { //@TODO: Maybe generalize this struct so we can use it for other things as well. Canvas actions use this struct a lot and it's kinda unneccessary to include this file every time.

		std::vector<s32> positionList;

		void addPosition(s32 _position);
		void insertPosition(s32 _position, s32 _index);
		bool isEmpty();
		void print();

	};

	struct TreeView;

	struct TreeViewItem {

		TreeView* treeView = nullptr;
		TreeViewItem* parent = nullptr;
		std::vector<TreeViewItem*> itemList;

		s32 filterType = -1; //What type an item is.
		s32 filterTypeExclusive = false; //If true, the item can only be dropped into other items (parent cannot be nullptr).
		std::vector<s32> filterList; //The types of children item this item can hold.
		bool allowChildren = true;

		std::string name = "";
		bool opened = false;

		Sprite* sprIcon = nullptr;
		s32 iconSub = 0;

		DropDownMenu* menu = nullptr;

		bool hasParent(TreeViewItem* _item);
		bool checkFilter(TreeViewItem* _item);

		void setFilterType(s32 _filterType, bool _exclusive = true);
		void addFilterMask(s32 _filterType);
		void setAllowChildren(bool _allow);

		void setMenu(DropDownMenu* _menu);

		TreeViewStructure getStructure();

		void openAllParents(bool _recalculateTotalSize = true, bool _callOnOpenClose = true);
		void open(bool _recalculateTotalSize = true, bool _callOnOpenClose = true);
		void close(bool _recalculateTotalSize = true, bool _callOnOpenClose = true);
		void setOpened(bool _opened, bool _recalculateTotalSize = true, bool _callOnOpenClose = true);

		void setName(std::string _name);

		s32 getX();
		void getY(bool& _foundTarget, s32& _y, bool _isRoot, TreeViewItem* _target);
		s32 getY();

	};

	struct TreeView : public Widget {

		TreeViewTheme* treeViewTheme;

		Sprite* sprTreeViewBackground;
		Sprite* sprTreeViewArrow;
		Sprite* sprTreeViewDragDir;

		Font* fntText;

		TextEdit* editName = nullptr;

		std::vector<TreeViewItem*> itemList;
		TreeViewItem* itemHovered = nullptr;
		TreeViewItem* itemSelected = nullptr;
		TreeViewItem* itemEdit = nullptr;

		bool itemDrag = false;
		s32 itemDragMouseY = 0;
		u8 itemDragDir = 0;
		bool itemDragCanDrop = false;
		u8 itemMouse = 0;

		s32 itemsWidth = 0;
		s32 itemsHeight = 0;

		bool arrowHovered = false;

		TreeViewItem* doubleClickItem = nullptr;
		f64 doubleClickTime = 0;
		u8 doubleClickMouse = 0;

		DropDownMenu* defaultItemMenu = nullptr;

		bool shouldDeselect = false;
		bool breakOutOfLoop = false;
		bool hasInteracted = false;

		std::function<void(TreeViewItem*)> onItemAdd;
		std::function<void(TreeViewItem*)> onItemDestroy;
		std::function<void(TreeViewItem*, bool, bool)> onItemSelect;
		std::function<void(TreeViewItem*, TreeViewStructure)> onItemMove;
		std::function<void(TreeViewItem*, bool)> onItemOpenClose;
		std::function<void(TreeViewItem*, std::string)> onItemNameChange;

		virtual bool onItemUpdate(TreeViewItem* _item, s32 _itemX, s32 _itemY);
		virtual void onItemDraw(TreeViewItem* _item, s32 _itemX, s32 _itemY);

		void update(f32 dt) override;
		void render() override;

		void getMaxSize(std::vector<TreeViewItem*>& _itemList, s32& _width, s32& _height);
		void calculateMaxSize();
		void recalculateTotalSize();

		void updateItems(std::vector<TreeViewItem*>& _itemList, s32 _x, s32& _y, f32 dt);
		void renderItems(std::vector<TreeViewItem*>& _itemList, s32 _x, s32& _y);

		TreeView(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);
		~TreeView();

		TreeViewItem* insertItem(std::string _name, TreeViewItem* _parent = nullptr, s32 _position = -1, Sprite* _icon = nullptr, s32 _iconSub = 0);
		TreeViewItem* insertItem(std::string _name, TreeViewStructure& _structure, Sprite* _icon = nullptr, s32 _iconSub = 0);
		TreeViewItem* destroyItem(TreeViewItem* _item, bool _recalculateTotalSize = true, bool _callOnSelect = true, bool _callOnDestroy = true);
		void selectItem(TreeViewItem* _item, bool _wasItemAdded = false, bool _wasItemDeleted = false, bool _callOnSelect = true, bool _forceCall = false);
		void moveItem(TreeViewItem* _item, TreeViewStructure& _structure, bool _recalculateTotalSize = true, bool _callOnMove = true);
		TreeViewItem* getItemFromStructure(TreeViewStructure& _structure);
		s32 getItemCount(s32 _filterType = -1, TreeViewItem* _rootItem = nullptr);
		void scrollToItem(TreeViewItem* _item);

		void startEditing(TreeViewItem* _item);
		void stopEditing();
		void onEditNameConfirm(TextEdit* _textEdit);
		void onEditNameReturn(TextEdit* _textEdit);

		void setDefaultItemMenu(DropDownMenu* _menu);

		void setOnItemAdd(std::function<void(TreeViewItem*)> _onItemAdd);
		void setOnItemDestroy(std::function<void(TreeViewItem*)> _onItemDestroy);
		void setOnItemSelect(std::function<void(TreeViewItem*, bool, bool)> _onItemSelect);
		void setOnItemMove(std::function<void(TreeViewItem*, TreeViewStructure)> _onItemMove);
		void setOnItemOpenClose(std::function<void(TreeViewItem*, bool)> _onItemOpenClose);
		void setOnItemNameChange(std::function<void(TreeViewItem*, std::string)> _onItemNameChange);
		
	};

}