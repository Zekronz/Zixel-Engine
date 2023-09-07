#pragma once

#include <unordered_map>
#include <vector>
#include <NFDExtended/nfd.h>

#include "Engine/StringHelper.h"
#include "Engine/Cursor.h"
#include "Engine/GUI/WidgetType.h"

namespace Zixel {

	struct Renderer;
	struct Window;
	struct Widget;
	struct DockArea;
	struct DockTab;
	struct DropDownMenu;
	struct MenuBar;
	struct TextEdit;

	struct Theme;
	struct PanelTheme;
	struct TreeViewTheme;

	struct Sprite;
	struct Font;

	struct GlobalShortcut {

		u16 keyCode = 0;
		u8 modifiers = 0;

		bool repeatable = false;

		std::function<bool()> onCallback;
		std::function<bool()> onStart;
		std::function<void()> onEnd;

	};

	struct TabGroup {

		Window* window = nullptr;
		std::vector<Widget*> widgetList;

		void addWidget(Widget* _widget);

	};

	struct GUI {

		bool initialized = false;

		Renderer* renderer = nullptr;

		u8 keyDown[48] = { 0 };
		u8 keyPress[48] = { 0 };
		u8 keyRelease[48] = { 0 };
		u8 keyClearState[48] = { 0 };

		s32 keyDownCounter = 0;
		s32 keyPressCounter = 0;
		s32 keyReleaseCounter = 0;

		u16 lastKey = 0;
		char lastChar = 0;

		s32 mouseX = 0;
		s32 mouseY = 0;

		u8 mouseDown = 0x00;
		u8 mousePress = 0x00;
		u8 mouseRelease = 0x00;
		u8 mouseClearState = 0x00;
		s8 mouseScrollDir = 0;

		//Native File Dialog Extended.
		bool fileDialogAvailable = false;

		//Shortcuts.
		std::vector<GlobalShortcut> globalShortcutList;
		GlobalShortcut currentGlobalShortcut;
		bool globalShortcutMode = false;
		bool globalShortcutStart = false;
		bool globalShortcutRepeat = false;
		f32 globalShortcutTimer = 0.0f;

		//Tab groups.
		std::vector<TabGroup*> tabGroupList;
		bool tabForward = false;
		bool tabStart = false;
		bool tabRepeat = false;
		f32 tabTimer = 0.0f;

		//Grid.
		bool gridLayout = false;
		s32 gridColumnCount = 1;
		s32 gridRowCount = 1;

		//Widget.
		Widget* widgetSelected = nullptr;
		Widget* widgetFocused = nullptr;
		Widget* widgetMouseOver = nullptr;
		Widget* widgetMouseOverScroll = nullptr;
		Widget* widgetScroll = nullptr;
		Widget* widgetShortcut = nullptr;
		u8 widgetScrollDir = 0;
		s32 widgetScrollMousePos = 0;
		s32 widgetScrollMousePosPrev = 0;
		s32 widgetScrollBarPos = 0;

		std::vector<Widget*>widgetList;

		//Window.
		std::vector<Window*>allWindowsList;
		std::vector<Window*>openedWindowsList;
		std::vector<Window*>deletedWindowsList;
		Window* windowMouseOver = nullptr;
		bool windowMouseOverContainer = false;
		Window* windowSelected = nullptr;
		Window* windowScroll = nullptr;

		void setWindowScroll(Window* window);
		void updateWindowMouseOver();

		//Docking.
		bool dockSplitMouseOver = false;
		DockTab* dockTabDrag = nullptr;
		DockArea* dockTabDragArea = nullptr;
		s32 dockTabDragMouseXOffset = 0;
		s32 dockTabDragMouseYOffset = 0;
		s32 dockTabDragWidth = 0;
		UTF8String dockTabDragTitle = "";
		bool dockTabDragShowClose = false;
		s32 dockTabDragContainerWidth = 0;
		s32 dockTabDragContainerHeight = 0;

		//Menu bar.
		MenuBar* menuBarOpen = nullptr;

		//Drop down menu.
		DropDownMenu* dropDownMenuOpen = nullptr;
		std::vector<DropDownMenu*> dropDownMenuList;

		//Text edit.
		TextEdit* textEditSelected = nullptr;
		bool textEditDeselectedWithEnter = false;

		//Tooltips.
		Sprite* tooltipSprite = nullptr;
		Font* tooltipFont = nullptr;
		Widget* tooltipWidget = nullptr;
		Widget* tooltipWidgetPrev = nullptr;
		Window* tooltipWindow = nullptr;
		Window* tooltipWindowPrev = nullptr;
		UTF8String tooltipText;
		UTF8String tooltipTextPrev;
		f32 tooltipTimer = 0.0f;
		bool tooltipIsCursorSpriteCentered = false;
		bool tooltipUsesAbsolutePosition = false;
		s32 tooltipAbsX = 0;
		s32 tooltipAbsY = 0;
		s32 tooltipX = 0;
		s32 tooltipY = 0;
		s32 tooltipWidth = 0;
		s32 tooltipHeight = 0;

		//Cursor.
		u8 cursorType = 0;
		u8 cursorId = CURSOR_ARROW;
		Sprite* cursorSprite = nullptr;
		s32 cursorSub = 0;
		f32 cursorRot = 0;
		bool cursorCentered = false;

		//Theme.
		std::unordered_map<WidgetType, Theme*> defaultThemes;
		std::vector<Theme*> customThemeList;
		void createDefaultThemes();
		Theme* getDefaultTheme(WidgetType _type);

		PanelTheme* createPanelTheme();
		TreeViewTheme* createTreeViewTheme();

		~GUI();

		bool init(Renderer* _renderer);
		void update(f32 dt);
		void render();

		void onWindowResize(s32 _width, s32 _height, s32 _prevWidth, s32 _prevHeight);
		void onWindowFocus(bool _focused);
		void onKeyPress(u16 button);
		void onKeyRelease(u16 button);
		void onTextInput(u32 codepoint);
		void onMousePress(u8 button);
		void onMouseRelease(u8 button);
		void onMouseScroll(s8 dir);

		bool isKeyDown(u16 key);
		bool isKeyPressed(u16 key);
		bool isKeyReleased(u16 key);
		void clearKey(u16 _key);

		bool isMouseDown(u8 mouseButton);
		bool isMousePressed(u8 mouseButton);
		bool isMouseReleased(u8 mouseButton);
		bool isMouseScrollDown();
		bool isMouseScrollUp();
		void clearMouse(u8 mouseButton);
		void setMouseCursor(u8 _cursor);
		void setMouseCursor(Sprite* _cursorSprite, s32 _cursorSub = 0, bool centered = false);

		//Native File Dialog Extended.
		bool isFileDialogAvailable();
		std::string getOpenFileName(nfdfilteritem_t _filters[], nfdfiltersize_t _filterCount);
		std::wstring getOpenFileNameN(nfdnfilteritem_t _filters[], nfdfiltersize_t _filterCount);
		void getOpenFileNameMultiple(std::vector<std::string>& _pathList, nfdfilteritem_t _filters[], nfdfiltersize_t _filterCount);
		void getOpenFileNameMultipleN(std::vector<std::wstring>& _pathList, nfdnfilteritem_t _filters[], nfdfiltersize_t _filterCount);
		std::string getSaveFileName(nfdfilteritem_t _filters[], nfdfiltersize_t _filterCount, const std::string _defaultName = "");
		std::wstring getSaveFileNameN(nfdnfilteritem_t _filters[], nfdfiltersize_t _filterCount, const std::wstring _defaultName = L"");

		//Shortcuts.
		void addGlobalShortcut(u16 _keyCode, u8 _modifiers, bool _repeatable, std::function<bool()> _onCallback, std::function<bool()> _onStart = nullptr, std::function<void()> _onEnd = nullptr);
		bool isGlobalShortcutDown(GlobalShortcut& _shortcut);
		bool isGlobalShortcutPressed(GlobalShortcut& _shortcut);
		void stopGlobalShortcut();
		void stopLocalShortcut();

		//Tab groups.
		TabGroup* addTabGroup();
		bool doTabForward(Widget* _focusedWidget);
		bool doTabBackward(Widget* _focusedWidget);
		void stopTabbing();

		//Grid.
		void setGridColumnRowCount(s32 _numColumns = 1, s32 _numRows = 1);
		void setGridLayout(bool _enable);

		//Widget.
		void initWidget(Widget* _widget, Widget* _parent, Window* _window);

		template<typename T> T* createWidget(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		template<typename T> T* createWidget(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		/*DockArea* createDockArea(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		DockArea* createDockArea(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		MenuBar* createMenuBar(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		MenuBar* createMenuBar(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		ToolBar* createToolBar(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		ToolBar* createToolBar(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		Panel* createPanel(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		Panel* createPanel(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		Button* createButton(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		Button* createButton(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		MenuButton* createMenuButton(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		MenuButton* createMenuButton(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		ToggleButton* createToggleButton(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		ToggleButton* createToggleButton(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		SliderHor* createSliderHor(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		SliderHor* createSliderHor(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		ComboBox* createComboBox(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		ComboBox* createComboBox(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		Checkbox* createCheckbox(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		Checkbox* createCheckbox(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		RadioButton* createRadioButton(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		RadioButton* createRadioButton(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		Label* createLabel(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		Label* createLabel(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		TreeView* createTreeView(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		TreeView* createTreeView(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		TextEdit* createTextEdit(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		TextEdit* createTextEdit(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		LineEdit* createLineEdit(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		LineEdit* createLineEdit(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		NumberEdit* createNumberEdit(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		NumberEdit* createNumberEdit(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		Canvas* createCanvas(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		Canvas* createCanvas(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		CanvasAnimation* createCanvasAnimation(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		CanvasAnimation* createCanvasAnimation(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		CanvasLayers* createCanvasLayers(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		CanvasLayers* createCanvasLayers(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		CanvasTools* createCanvasTools(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		CanvasTools* createCanvasTools(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		ColorPicker* createColorPicker(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		ColorPicker* createColorPicker(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);

		ColorPalette* createColorPalette(Widget* _parent, std::string _parameters = "", Theme* _theme = nullptr);
		ColorPalette* createColorPalette(Window* _window, std::string _parameters = "", Theme* _theme = nullptr);*/

		void destroyWidget(Widget* widget);

		bool deselectWidget(Widget* widget);
		bool unfocusWidget(Widget* widget, bool _callOnUnfocused = true);

		void setWidgetSelected(Widget* widget, bool _callOnFocused = true);
		void setWidgetFocused(Widget* widget, bool _callOnFocused = true, bool _stopTabbing = true);

		Widget* getWidgetMouseOver();
		void setWidgetMouseOver(Widget* widget);

		void setWidgetScroll(Widget* widget);

		bool isMouseOverScrollbars(Window* _window, bool checkChildren = true);
		bool isMouseOverScrollbars(Widget* _widget, bool checkChildren = true);

		bool isMouseOverWidget(Window* _window, bool _isStart = true);
		bool isMouseOverWidget(Widget* _widget, bool _isStart = true);

		//Window.
		Window* createWindow();
		bool destroyWindow(Window* _window);
		bool hasPopupWindow();
		Window* getPopupWindow();

		//Drop down menu.
		DropDownMenu* createDropDownMenu();
		bool isMouseOverDropDownMenu();

		//Tooltip.
		void setTooltip(Widget* _widget, UTF8String _text, bool _isCursorSpriteCentered = false);
		void setTooltip(Window* _window, UTF8String _text, bool _isCursorSpriteCentered = false);
		void setTooltipAbsolute(Widget* _widget, UTF8String _text, s32 _x, s32 _y);

	};

	template<typename T> T* GUI::createWidget(Widget* _parent, std::string _parameters, Theme* _theme) {
		T* widget = new T(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;
	}

	template<typename T> T* GUI::createWidget(Window* _window, std::string _parameters, Theme* _theme) {
		T* widget = new T(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;
	}

}