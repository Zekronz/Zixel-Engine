#pragma once

#include <vector>
#include <string>
#include <functional>

#define GUI_DROP_DOWN_MENU_ITEM_PRESS_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2)
#define GUI_DROP_DOWN_MENU_OPEN_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1)
#define GUI_DROP_DOWN_MENU_CLOSE_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1)

#define GUI_DROP_DOWN_MENU_SPR_MENU "dropDownMenu"
#define GUI_DROP_DOWN_MENU_SPR_ARROW "dropDownMenuArrow"
#define GUI_DROP_DOWN_MENU_SPR_SEPARATOR "dropDownMenuSeparator"
#define GUI_DROP_DOWN_MENU_SPR_CHECKBOX "checkbox"
#define GUI_DROP_DOWN_MENU_SPR_CHECKBOX_ICON "checkboxIcon"
#define GUI_DROP_DOWN_MENU_FONT "robotoRegular12"
#define GUI_DROP_DOWN_MENU_TEXT_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_DROP_DOWN_MENU_TEXT_COL_DISABLED { 0.353f, 0.353f, 0.439f, 1.0f }
#define GUI_DROP_DOWN_MENU_TEXT_SPACING 9
#define GUI_DROP_DOWN_MENU_ITEM_HEIGHT 24
#define GUI_DROP_DOWN_MENU_ICON_SPACING 9
#define GUI_DROP_DOWN_MENU_ICON_ALPHA_ENABLED 1.0f
#define GUI_DROP_DOWN_MENU_ICON_ALPHA_DISABLED 0.3f
#define GUI_DROP_DOWN_MENU_ARROW_SPACING 6
#define GUI_DROP_DOWN_MENU_CHECKBOX_SIZE 17
#define GUI_DROP_DOWN_MENU_CHECKBOX_SPACING 9
#define GUI_DROP_DOWN_MENU_CHECKBOX_ICON_HOR_OFFSET 0
#define GUI_DROP_DOWN_MENU_CHECKBOX_ICON_VER_OFFSET 0
#define GUI_DROP_DOWN_MENU_SEPARATOR_HOR_SPACING 9
#define GUI_DROP_DOWN_MENU_SEPARATOR_VER_SPACING 3
#define GUI_DROP_DOWN_MENU_PAGE_SPACING -2
#define GUI_DROP_DOWN_MENU_OPEN_TIME 0.25f
#define GUI_DROP_DOWN_MENU_KEY_PRESS_START_TIME 0.5f
#define GUI_DROP_DOWN_MENU_KEY_PRESS_TIME 0.055f
#define GUI_DROP_DOWN_MENU_INTERACT_TIME 0.1f

namespace Zixel {

	struct Sprite;
	struct Font;

	struct DropDownMenuPage;
	struct DropDownMenu;

	enum class DropDownMenuItemType : u8 {
		Item,
		Separator,
	};

	struct DropDownMenuItem {

		DropDownMenuPage* parentPage = nullptr;
		DropDownMenuPage* childPage = nullptr;
		s32 pageInd = 0;

		DropDownMenuItem* parentItem = nullptr;

		DropDownMenuItemType type = DropDownMenuItemType::Item;
		std::string name;
		std::string shortcut;

		Sprite* icon = nullptr;
		s32 iconSub = 0;

		bool checkable = false;
		bool checked = false;

		bool enabled = true;

		std::function<void(DropDownMenu*, DropDownMenuItem*)> callback;
		bool hasCallback = false;

		s32 yOffset = 0;
		bool opened = false;

		void setName(std::string _name);
		void setCheckable(bool _checkable);
		void setChecked(bool _checked);
		bool isChecked();
		void setEnabled(bool _enabled);
		void setCallback(std::function<void(DropDownMenu*, DropDownMenuItem*)> _callback);

		void close(bool _updateChildren = true);
		s32 getIndex();

	};

	struct DropDownMenuPage {

		DropDownMenu* menu = nullptr;
		DropDownMenuPage* parent = nullptr;

		std::vector<DropDownMenuPage*> children;
		std::vector<DropDownMenuItem*> items;

		s32 x = 0;
		s32 y = 0;
		s32 width = 0;
		s32 maxHeight = 0;
		std::vector<s32> height;

		s32 maxIconWidth = 0;
		bool hasCheckbox = false;
		bool hasArrow = false;

	};

	struct GUI;
	struct Renderer;
	struct MenuBar;

	struct DropDownMenu {

		GUI* gui;
		Renderer* renderer;

		Font* fntText;

		Sprite* sprDropDownMenu;
		Sprite* sprDropDownMenuArrow;
		Sprite* sprDropDownMenuSeparator;
		Sprite* sprDropDownMenuCheckbox;
		Sprite* sprDropDownMenuCheckboxIcon;

		s32 x = 0;
		s32 y = 0;
		
		s32 mouseXStart = 0;
		s32 mouseYStart = 0;
		bool mouseStartMoved = false;

		f32 timer = 0.0f;
		bool timerTriggered = false;

		f32 interactTimer = 0.0f;

		DropDownMenuPage* page = nullptr;
		DropDownMenuPage* lastHoveredPage = nullptr;

		DropDownMenuItem* itemHover = nullptr;
		DropDownMenuItem* itemLastHovered = nullptr;
		DropDownMenuItem* itemShouldClose = nullptr;
		DropDownMenuItem* itemPressed = nullptr;

		bool keyboardMode = false;
		bool openedFromKeyboard = false;
		DropDownMenuItem* itemHoveredKeyboard = nullptr;

		u16 key = 0;
		bool keyStart = false;
		bool keyPress = false;
		bool keyShift = false;
		f32 keyTimerStart = 0.0f;
		f32 keyTimer = 0.0f;

		bool mouseOverPage = false;

		MenuBar* menuBar = nullptr;

		std::function<void(DropDownMenu*)> onOpen;
		std::function<void(DropDownMenu*)> onClose;

		DropDownMenuItem* findFirstInteractableItem(DropDownMenuPage* _page);

		bool doKeyDown();
		bool doKeyUp();
		bool doKeyLeft();
		bool doKeyRight();
		bool doKeyTab();

		void goToNextMenuBarButton(bool _selectFirstItemOnPage = true);
		void goToPreviousMenuBarButton(bool _selectFirstItemOnPage = true);

		DropDownMenu(GUI* _gui);
		~DropDownMenu();

		void open(s32 _x, s32 _y, MenuBar* _menuBar = nullptr);
		void close(bool _closeMenuBar = true);

		void setPosition(s32 _x, s32 _y);

		DropDownMenuItem* addItem(std::string _name, DropDownMenuItem* _parent = nullptr, std::string _shortcut = "", Sprite* _icon = nullptr, s32 _iconSub = 0, bool _enabled = true);
		DropDownMenuItem* addSeparator(DropDownMenuItem* _parent = nullptr);
		DropDownMenuItem* getLeafItem(DropDownMenuPage* _page = nullptr);
		void deleteItem(DropDownMenuItem* _item, bool _updatePagePos = true);

		DropDownMenuPage* addPage(DropDownMenuPage* _parent = nullptr);
		DropDownMenuPage* getLeafPage(DropDownMenuPage* _parent = nullptr);
		void deletePage(DropDownMenuPage* _page);
		void updatePagePos(DropDownMenuPage* _page, bool _hasSetX = false, bool _hasSetY = false, s32 _x = 0, s32 _y = 0);
		void updatePage(DropDownMenuPage* _page, f32 dt);
		void renderPage(DropDownMenuPage* _page);
		void resetPage(DropDownMenuPage* _page);

		void setOnOpen(std::function<void(DropDownMenu*)> _callback);
		void setOnClose(std::function<void(DropDownMenu*)> _callback);

		void update(f32 dt);
		void render();

	};

}