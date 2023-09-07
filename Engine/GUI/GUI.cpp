#include "Engine/ZixelPCH.h"
#include "Engine/Renderer.h"
#include "Engine/GUI/GUIMacros.h"
#include "Engine/GUI/WidgetIncludes.h"
#include "Engine/GUI/Window.h"
#include "Engine/GUI/Widget.h"
#include "Engine/GUI/GUI.h"
#include "Engine/KeyCodes.h"
#include "Engine/TextureAtlas.h"

namespace Zixel {

	GUI::~GUI() {

		for (TabGroup* tabGroup : tabGroupList) {
			delete tabGroup;
		}

		for (s32 i = (s32)widgetList.size() - 1; i >= 0; --i) {
			destroyWidget(widgetList[i]);
		}

		for (Window* window : allWindowsList) {
			delete window;
		}

		for (Window* window : deletedWindowsList) {
			delete window;
		}
		
		if (dockTabDrag != nullptr) {

			delete dockTabDrag;

			dockTabDrag = nullptr;
			dockTabDragArea = nullptr;

		}

		for (DropDownMenu* menu : dropDownMenuList) {
			delete menu;
		}

		for (const auto& it : defaultThemes) {
			delete it.second;
		}
		
		for (Theme* theme : customThemeList) {
			delete theme;
		}

		if (fileDialogAvailable) NFD_Quit();

		ZIXEL_INFO("Destroyed GUI.");

	}

	bool GUI::init(Renderer* _renderer) {

		if (initialized) {

			ZIXEL_WARN("GUI already initialized.");
			return true;

		}

		renderer = _renderer;

		fileDialogAvailable = (NFD_Init() == NFD_OKAY);
		if (!fileDialogAvailable) ZIXEL_WARN("Error initializing Native File Dialog Extended.");

		createDefaultThemes();

		tooltipSprite = renderer->getTextureAtlasSprite(GUI_TOOLTIP_SPR);
		tooltipFont = renderer->getTextureAtlasFont(GUI_TOOLTIP_FONT);
		tooltipText.reserve(200);
		tooltipTextPrev.reserve(200);

		ZIXEL_INFO("Initialized GUI.");

		initialized = true;

		return true;

	}

	//Theme.
	void GUI::createDefaultThemes() {

		defaultThemes[WidgetType::Panel] = new PanelTheme();
		defaultThemes[WidgetType::TreeView] = new TreeViewTheme();

	}

	Theme* GUI::getDefaultTheme(WidgetType _type) {

		const auto& it = defaultThemes.find(_type);

		if (it == defaultThemes.end()) {
			return nullptr;
		}

		return it->second;

	}

	PanelTheme* GUI::createPanelTheme() {

		PanelTheme* theme = new PanelTheme();
		customThemeList.push_back((Theme*)theme);

		return theme;

	}

	TreeViewTheme* GUI::createTreeViewTheme() {
		
		TreeViewTheme* theme = new TreeViewTheme();
		customThemeList.push_back((Theme*)theme);

		return theme;

	}

	void TabGroup::addWidget(Widget* _widget) {

		if (_widget->tabGroup != nullptr) {
			return;
		}

		if (widgetList.size() <= 0) {
			window = _widget->window;
		}
		else if (_widget->window != window) {
			return;
		}

		_widget->tabGroup = this;

		widgetList.push_back(_widget);

	}

	//Widget creation.
	void GUI::initWidget(Widget* _widget, Widget* _parent, Window* _window) {

		_widget->gui = this;
		_widget->renderer = renderer;

		_widget->parent = _parent;

		if (_parent != nullptr) {
			_widget->window = _parent->window;
		}
		else {
			_widget->window = _window;
		}

		if (_parent == nullptr) {

			if (_window != nullptr) {
				_window->widgetList.push_back(_widget);
			}
			else {
				widgetList.push_back(_widget);
			}

		}
		else {
			_parent->widgetList.push_back(_widget);
		}

	}

	//Dock Area.
	/*DockArea* GUI::createDockArea(Widget* _parent, std::string _parameters, Theme* _theme) {

		DockArea* widget = new DockArea(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	DockArea* GUI::createDockArea(Window* _window, std::string _parameters, Theme* _theme) {

		DockArea* widget = new DockArea(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Menu Bar.
	MenuBar* GUI::createMenuBar(Widget* _parent, std::string _parameters, Theme* _theme) {

		MenuBar* widget = new MenuBar(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	MenuBar* GUI::createMenuBar(Window* _window, std::string _parameters, Theme* _theme) {

		MenuBar* widget = new MenuBar(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Tool Bar.
	ToolBar* GUI::createToolBar(Widget* _parent, std::string _parameters, Theme* _theme) {

		ToolBar* widget = new ToolBar(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	ToolBar* GUI::createToolBar(Window* _window, std::string _parameters, Theme* _theme) {

		ToolBar* widget = new ToolBar(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Panel Edit.
	Panel* GUI::createPanel(Widget* _parent, std::string _parameters, Theme* _theme) {

		Panel* widget = new Panel(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	Panel* GUI::createPanel(Window* _window, std::string _parameters, Theme* _theme) {

		Panel* widget = new Panel(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Button.
	Button* GUI::createButton(Widget* _parent, std::string _parameters, Theme* _theme) {

		Button* widget = new Button(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	Button* GUI::createButton(Window* _window, std::string _parameters, Theme* _theme) {

		Button* widget = new Button(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Menu Button.
	MenuButton* GUI::createMenuButton(Widget* _parent, std::string _parameters, Theme* _theme) {

		MenuButton* widget = new MenuButton(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	MenuButton* GUI::createMenuButton(Window* _window, std::string _parameters, Theme* _theme) {

		MenuButton* widget = new MenuButton(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Toggle Button.
	ToggleButton* GUI::createToggleButton(Widget* _parent, std::string _parameters, Theme* _theme) {

		ToggleButton* widget = new ToggleButton(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	ToggleButton* GUI::createToggleButton(Window* _window, std::string _parameters, Theme* _theme) {

		ToggleButton* widget = new ToggleButton(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Slider Hor.
	SliderHor* GUI::createSliderHor(Widget* _parent, std::string _parameters, Theme* _theme) {

		SliderHor* widget = new SliderHor(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	SliderHor* GUI::createSliderHor(Window* _window, std::string _parameters, Theme* _theme) {

		SliderHor* widget = new SliderHor(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Combo Box.
	ComboBox* GUI::createComboBox(Widget* _parent, std::string _parameters, Theme* _theme) {

		ComboBox* widget = new ComboBox(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	ComboBox* GUI::createComboBox(Window* _window, std::string _parameters, Theme* _theme) {

		ComboBox* widget = new ComboBox(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Checkbox.
	Checkbox* GUI::createCheckbox(Widget* _parent, std::string _parameters, Theme* _theme) {

		Checkbox* widget = new Checkbox(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	Checkbox* GUI::createCheckbox(Window* _window, std::string _parameters, Theme* _theme) {

		Checkbox* widget = new Checkbox(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Radio Button.
	RadioButton* GUI::createRadioButton(Widget* _parent, std::string _parameters, Theme* _theme) {

		RadioButton* widget = new RadioButton(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	RadioButton* GUI::createRadioButton(Window* _window, std::string _parameters, Theme* _theme) {

		RadioButton* widget = new RadioButton(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Label.
	Label* GUI::createLabel(Widget* _parent, std::string _parameters, Theme* _theme) {

		Label* widget = new Label(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	Label* GUI::createLabel(Window* _window, std::string _parameters, Theme* _theme) {

		Label* widget = new Label(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Tree View.
	TreeView* GUI::createTreeView(Widget* _parent, std::string _parameters, Theme* _theme) {

		TreeView* widget = new TreeView(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	TreeView* GUI::createTreeView(Window* _window, std::string _parameters, Theme* _theme) {

		TreeView* widget = new TreeView(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Text Edit.
	TextEdit* GUI::createTextEdit(Widget* _parent, std::string _parameters, Theme* _theme) {

		TextEdit* widget = new TextEdit(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	TextEdit* GUI::createTextEdit(Window* _window, std::string _parameters, Theme* _theme) {

		TextEdit* widget = new TextEdit(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Line Edit.
	LineEdit* GUI::createLineEdit(Widget* _parent, std::string _parameters, Theme* _theme) {

		LineEdit* widget = new LineEdit(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	LineEdit* GUI::createLineEdit(Window* _window, std::string _parameters, Theme* _theme) {

		LineEdit* widget = new LineEdit(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Number Edit.
	NumberEdit* GUI::createNumberEdit(Widget* _parent, std::string _parameters, Theme* _theme) {

		NumberEdit* widget = new NumberEdit(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	NumberEdit* GUI::createNumberEdit(Window* _window, std::string _parameters, Theme* _theme) {

		NumberEdit* widget = new NumberEdit(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Canvas.
	Canvas* GUI::createCanvas(Widget* _parent, std::string _parameters, Theme* _theme) {

		Canvas* widget = new Canvas(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	Canvas* GUI::createCanvas(Window* _window, std::string _parameters, Theme* _theme) {

		Canvas* widget = new Canvas(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Canvas Animation.
	CanvasAnimation* GUI::createCanvasAnimation(Widget* _parent, std::string _parameters, Theme* _theme) {

		CanvasAnimation* widget = new CanvasAnimation(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	CanvasAnimation* GUI::createCanvasAnimation(Window* _window, std::string _parameters, Theme* _theme) {

		CanvasAnimation* widget = new CanvasAnimation(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Canvas Layers.
	CanvasLayers* GUI::createCanvasLayers(Widget* _parent, std::string _parameters, Theme* _theme) {

		CanvasLayers* widget = new CanvasLayers(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	CanvasLayers* GUI::createCanvasLayers(Window* _window, std::string _parameters, Theme* _theme) {

		CanvasLayers* widget = new CanvasLayers(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Canvas Tools.
	CanvasTools* GUI::createCanvasTools(Widget* _parent, std::string _parameters, Theme* _theme) {

		CanvasTools* widget = new CanvasTools(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	CanvasTools* GUI::createCanvasTools(Window* _window, std::string _parameters, Theme* _theme) {

		CanvasTools* widget = new CanvasTools(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Color Picker.
	ColorPicker* GUI::createColorPicker(Widget* _parent, std::string _parameters, Theme* _theme) {

		ColorPicker* widget = new ColorPicker(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	ColorPicker* GUI::createColorPicker(Window* _window, std::string _parameters, Theme* _theme) {

		ColorPicker* widget = new ColorPicker(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}

	//Color Palette.
	ColorPalette* GUI::createColorPalette(Widget* _parent, std::string _parameters, Theme* _theme) {

		ColorPalette* widget = new ColorPalette(this, _parent, nullptr, _theme);
		widget->init(_parameters);

		return widget;

	}

	ColorPalette* GUI::createColorPalette(Window* _window, std::string _parameters, Theme* _theme) {

		ColorPalette* widget = new ColorPalette(this, nullptr, _window, _theme);
		widget->init(_parameters);

		return widget;

	}*/

	void GUI::destroyWidget(Widget* widget) {

		if (widgetSelected == widget) {
			setWidgetSelected(nullptr); //This also stops and shortcuts and sets the focused widget to nullptr.
		}
		
		if (widgetMouseOver == widget) {
			setWidgetMouseOver(nullptr);
		}

		if (widgetMouseOverScroll == widget) {
			widgetMouseOverScroll = nullptr;
		}

		if (widgetScroll == widget) {
			setWidgetScroll(nullptr);
		}

		Widget* parent = widget->parent;

		//No parent widget.
		if (parent == nullptr) {

			Window* window = widget->window;

			//No window parent.
			if (window == nullptr) {

				for (s32 i = (s32)widgetList.size() - 1; i >= 0; --i) {
					if (widgetList[i] == widget) {
						widgetList.erase(widgetList.begin() + i);
						break;
					}
				}

			}

			//Has parent window.
			else {

				std::vector<Widget*>& children = window->getChildren();
				for (s32 i = (s32)children.size() - 1; i >= 0; --i) {
					if (children[i] == widget) {
						children.erase(children.begin() + i);
						break;
					}
				}

			}
			
		}

		//Has parent widget.
		else {

			for (s32 i = (s32)parent->widgetList.size() - 1; i >= 0; --i) {
				if (parent->widgetList[i] == widget) {
					parent->widgetList.erase(parent->widgetList.begin() + i);
					break;
				}
			}

		}

		delete widget;

	}

	bool GUI::deselectWidget(Widget* widget) {

		if (widgetSelected == widget) {

			widgetSelected = nullptr;
			return true;

		}

		return false;

	}

	bool GUI::unfocusWidget(Widget* widget, bool _callOnUnfocused) {

		if (widgetFocused == widget) {

			if (_callOnUnfocused) widget->onUnfocused();

			widgetFocused = nullptr;
			return true;

		}

		return false;

	}

	bool GUI::isKeyDown(u16 key) {

		if (key == KEY_ANY) return (keyDownCounter > 0);
		if (key == KEY_CONTROL) return (isKeyDown(KEY_LEFT_CONTROL) || isKeyDown(KEY_RIGHT_CONTROL));
		if (key == KEY_SHIFT) return (isKeyDown(KEY_LEFT_SHIFT) || isKeyDown(KEY_RIGHT_SHIFT));

		s32 ind = (key / 8);
		u8 bit = (u8)(key - (ind * 8));

		return ((keyDown[ind] >> bit) & 0x01) == 1 && ((keyClearState[ind] >> bit) & 0x01) == 0;
	}

	bool GUI::isKeyPressed(u16 key) {

		if (key == KEY_ANY) return (keyPressCounter > 0);
		if (key == KEY_CONTROL) return (isKeyPressed(KEY_LEFT_CONTROL) || isKeyPressed(KEY_RIGHT_CONTROL));
		if (key == KEY_SHIFT) return (isKeyPressed(KEY_LEFT_SHIFT) || isKeyPressed(KEY_RIGHT_SHIFT));

		s32 ind = (key / 8);
		u8 bit = (u8)(key - (ind * 8));

		return ((keyPress[ind] >> bit) & 0x01) == 1 && ((keyClearState[ind] >> bit) & 0x01) == 0;
	}

	bool GUI::isKeyReleased(u16 key) {

		if (key == KEY_ANY) return (keyReleaseCounter > 0);
		if (key == KEY_CONTROL) return (isKeyReleased(KEY_LEFT_CONTROL) || isKeyReleased(KEY_RIGHT_CONTROL));
		if (key == KEY_SHIFT) return (isKeyReleased(KEY_LEFT_SHIFT) || isKeyReleased(KEY_RIGHT_SHIFT));

		s32 ind = (key / 8);
		u8 bit = (u8)(key - (ind * 8));

		return ((keyRelease[ind] >> bit) & 0x01) == 1 && ((keyClearState[ind] >> bit) & 0x01) == 0;

	}

	void GUI::clearKey(u16 _key) {

		s32 ind = (_key / 8);
		u8 bit = (u8)(_key - (ind * 8));

		if (((keyDown[ind] >> bit) & 0x01) == 1) --keyDownCounter;
		if (((keyPress[ind] >> bit) & 0x01) == 1) --keyPressCounter;
		if (((keyRelease[ind] >> bit) & 0x01) == 1) --keyReleaseCounter;

		keyDown[ind] &= ((0x01 << bit) ^ 0xFF);
		keyPress[ind] &= ((0x01 << bit) ^ 0xFF);
		keyRelease[ind] &= ((0x01 << bit) ^ 0xFF);

		keyClearState[ind] |= (0x01 << bit);

	}

	bool GUI::isMouseDown(u8 mouseButton) {

		if (mouseButton == MOUSE_LEFT_RIGHT_MIDDLE) return (isMouseDown(MOUSE_LEFT) || isMouseDown(MOUSE_RIGHT) || isMouseDown(MOUSE_MIDDLE));
		return (((mouseDown >> mouseButton) & 0x01) == 1 && ((mouseClearState >> mouseButton) & 0x01) == 0);

	}

	bool GUI::isMousePressed(u8 mouseButton) {

		if (mouseButton == MOUSE_LEFT_RIGHT_MIDDLE) return (isMousePressed(MOUSE_LEFT) || isMousePressed(MOUSE_RIGHT) || isMousePressed(MOUSE_MIDDLE));
		return (((mousePress >> mouseButton) & 0x01) == 1 && ((mouseClearState >> mouseButton) & 0x01) == 0);

	}

	bool GUI::isMouseReleased(u8 mouseButton) {

		if (mouseButton == MOUSE_LEFT_RIGHT_MIDDLE) return (isMouseReleased(MOUSE_LEFT) || isMouseReleased(MOUSE_RIGHT) || isMouseReleased(MOUSE_MIDDLE));
		return (((mouseRelease >> mouseButton) & 0x01) == 1 && ((mouseClearState >> mouseButton) & 0x01) == 0);

	}

	bool GUI::isMouseScrollDown() {
		return (mouseScrollDir == 1);
	}

	bool GUI::isMouseScrollUp() {
		return (mouseScrollDir == -1);
	}

	void GUI::clearMouse(u8 mouseButton) {

		mousePress &= ((0x01 << mouseButton) ^ 0xFF);
		mouseDown &= ((0x01 << mouseButton) ^ 0xFF);
		mouseRelease &= ((0x01 << mouseButton) ^ 0xFF);

		mouseClearState |= (0x01 << mouseButton);

	}

	void GUI::setMouseCursor(u8 _cursor) {

		cursorType = 0;
		cursorId = _cursor;
		cursorSprite = nullptr;
		cursorSub = 0;
		cursorCentered = false;

	}

	void GUI::setMouseCursor(Sprite* _cursorSprite, s32 _cursorSub, bool centered) {

		cursorType = 1;
		cursorId = CURSOR_ARROW;
		cursorSprite = _cursorSprite;
		cursorSub = _cursorSub;
		cursorCentered = centered;

	}

	//Native File Dialog Extended.
	bool GUI::isFileDialogAvailable() {
		return fileDialogAvailable;
	}

	std::string GUI::getOpenFileName(nfdfilteritem_t _filters[], nfdfiltersize_t _filterCount) {

		if (!fileDialogAvailable) return "";

		nfdu8char_t* out;
		
		#pragma warning(suppress : 26812)
		nfdresult_t result = NFD_OpenDialogU8(&out, _filters, _filterCount, nullptr);

		if (result != NFD_OKAY) return "";

		std::string outStr = out;

		NFD_FreePath(out);

		return outStr;

	}

	std::wstring GUI::getOpenFileNameN(nfdnfilteritem_t _filters[], nfdfiltersize_t _filterCount) {

		if (!fileDialogAvailable) return L"";

		nfdnchar_t* out;

		#pragma warning(suppress : 26812)
		nfdresult_t result = NFD_OpenDialogN(&out, _filters, _filterCount, nullptr);

		if (result != NFD_OKAY) return L"";

		std::wstring outStr = out;

		NFD_FreePathN(out);

		return outStr;

	}

	void GUI::getOpenFileNameMultiple(std::vector<std::string>& _pathList, nfdfilteritem_t _filters[], nfdfiltersize_t _filterCount) {

		if (!fileDialogAvailable) return;

		const nfdpathset_t* pathSet;

		#pragma warning(suppress : 26812)
		nfdresult_t result = NFD_OpenDialogMultipleU8(&pathSet, _filters, _filterCount, nullptr);
		
		if (result != NFD_OKAY) return;
		
		nfdpathsetsize_t count = 0;
		result = NFD_PathSet_GetCount(pathSet, &count);
		
		if (result != NFD_OKAY || count <= 0) {

			NFD_PathSet_Free(pathSet);
			return;

		}

		for (nfdpathsetsize_t i = 0; i < count; ++i) {

			nfdu8char_t* out;
			result = NFD_PathSet_GetPathU8(pathSet, i, &out);
			if (result != NFD_OKAY) continue;
			
			std::string path = out;
			if (path == "") continue;

			_pathList.push_back(path);
			
			NFD_FreePathU8(out);

		}

		NFD_PathSet_Free(pathSet);

	}

	void GUI::getOpenFileNameMultipleN(std::vector<std::wstring>& _pathList, nfdnfilteritem_t _filters[], nfdfiltersize_t _filterCount) {

		if (!fileDialogAvailable) return;

		const nfdpathset_t* pathSet;

		#pragma warning(suppress : 26812)
		nfdresult_t result = NFD_OpenDialogMultipleN(&pathSet, _filters, _filterCount, nullptr);

		if (result != NFD_OKAY) return;

		nfdpathsetsize_t count = 0;
		result = NFD_PathSet_GetCount(pathSet, &count);

		if (result != NFD_OKAY || count <= 0) {

			NFD_PathSet_Free(pathSet);
			return;

		}

		for (nfdpathsetsize_t i = 0; i < count; ++i) {

			nfdnchar_t* out;
			result = NFD_PathSet_GetPathN(pathSet, i, &out);
			if (result != NFD_OKAY) continue;

			std::wstring path = out;
			if (path.empty()) continue;

			_pathList.push_back(path);

			NFD_FreePathN(out);

		}

		NFD_PathSet_Free(pathSet);

	}

	std::string GUI::getSaveFileName(nfdfilteritem_t _filters[], nfdfiltersize_t _filterCount, const std::string _defaultName) {

		if (!fileDialogAvailable) return "";

		nfdu8char_t* out;

		#pragma warning(suppress : 26812)
		nfdresult_t result = NFD_SaveDialogU8(&out, _filters, _filterCount, nullptr, _defaultName.c_str());

		if (result != NFD_OKAY) return "";

		std::string outStr = out;

		NFD_FreePath(out);

		return outStr;

	}

	std::wstring GUI::getSaveFileNameN(nfdnfilteritem_t _filters[], nfdfiltersize_t _filterCount, const std::wstring _defaultName) {

		if (!fileDialogAvailable) return L"";

		nfdnchar_t* out;

		#pragma warning(suppress : 26812)
		nfdresult_t result = NFD_SaveDialogN(&out, _filters, _filterCount, nullptr, _defaultName.c_str());

		if (result != NFD_OKAY) return L"";

		std::wstring outStr = out;

		NFD_FreePathN(out);

		return outStr;

	}

	//Shortcuts.
	void GUI::addGlobalShortcut(u16 _keyCode, u8 _modifiers, bool _repeatable, std::function<bool()> _onCallback, std::function<bool()> _onStart, std::function<void()> _onEnd) {

		if (_keyCode == KEY_TAB) {
			return;
		}

		for (GlobalShortcut& shortcut : globalShortcutList) {

			if (shortcut.keyCode == _keyCode && shortcut.modifiers == _modifiers) {
				return;
			}

		}

		GlobalShortcut shortcut;
		shortcut.keyCode = _keyCode;
		shortcut.modifiers = _modifiers;
		shortcut.repeatable = _repeatable;

		shortcut.onCallback = _onCallback;
		if (_onStart) shortcut.onStart = _onStart;
		if (_onEnd) shortcut.onEnd = _onEnd;

		globalShortcutList.push_back(shortcut);

	}

	bool GUI::isGlobalShortcutDown(GlobalShortcut& _shortcut) {

		bool modCtrl = isKeyDown(KEY_CONTROL);
		bool modShift = isKeyDown(KEY_SHIFT);
		bool modAlt = isKeyDown(KEY_LEFT_ALT);

		u16 keyCode = _shortcut.keyCode;
		u8 modifiers = _shortcut.modifiers;

		if ((modifiers & 0x01) == 1 && !modCtrl) return false;
		if (((modifiers >> 1) & 0x01) == 1 && !modShift) return false;
		if (((modifiers >> 2) & 0x01) == 1 && !modAlt) return false;

		return isKeyDown(keyCode);

	}

	bool GUI::isGlobalShortcutPressed(GlobalShortcut& _shortcut) {

		u8 modifiers = _shortcut.modifiers;

		bool modCtrl = isKeyDown(KEY_CONTROL);
		bool modShift = isKeyDown(KEY_SHIFT);
		bool modAlt = isKeyDown(KEY_LEFT_ALT);
		
		return (isKeyPressed(_shortcut.keyCode) && (modifiers & 0x01) == modCtrl && ((modifiers >> 1) & 0x01) == modShift && ((modifiers >> 2) & 0x01) == modAlt);

	}

	void GUI::stopGlobalShortcut() {

		if ((globalShortcutStart || globalShortcutRepeat) && currentGlobalShortcut.onEnd) {
			currentGlobalShortcut.onEnd();
		}

		globalShortcutMode = false;
		globalShortcutStart = false;
		globalShortcutRepeat = false;

	}

	void GUI::stopLocalShortcut() {
		if (widgetShortcut != nullptr) widgetShortcut->stopShortcutMode();
	}

	//Tab groups.
	TabGroup* GUI::addTabGroup() {

		TabGroup* tabGroup = new TabGroup();
		tabGroupList.push_back(tabGroup);

		return tabGroup;

	}

	bool GUI::doTabForward(Widget* _focusedWidget) {

		TabGroup* tabGroup = _focusedWidget->tabGroup;

		if (tabGroup == nullptr) return false;

		const auto& it = std::find(tabGroup->widgetList.begin(), tabGroup->widgetList.end(), _focusedWidget);

		s32 ind = (s32)(it - tabGroup->widgetList.begin());
		s32 indNew = ind;

		for (s32 i = ind + 1; i < tabGroup->widgetList.size(); ++i) {

			Widget* temp = tabGroup->widgetList[i];
			if (temp->visibleGlobal && temp->enabledGlobal) {

				indNew = i;
				break;

			}

		}

		if (ind == indNew) {

			for (s32 i = 0; i < ind; ++i) {

				Widget* temp = tabGroup->widgetList[i];
				if (temp->visibleGlobal && temp->enabledGlobal) {

					indNew = i;
					break;

				}

			}

		}

		if (ind != indNew) {

			setWidgetFocused(tabGroup->widgetList[indNew], true, false);
			return true;

		}

		return false;

	}

	bool GUI::doTabBackward(Widget* _focusedWidget) {

		TabGroup* tabGroup = _focusedWidget->tabGroup;

		const auto& it = std::find(tabGroup->widgetList.begin(), tabGroup->widgetList.end(), _focusedWidget);

		s32 ind = (s32)(it - tabGroup->widgetList.begin());
		s32 indNew = ind;

		for (s32 i = ind - 1; i >= 0; --i) {

			Widget* temp = tabGroup->widgetList[i];
			if (temp->visibleGlobal && temp->enabledGlobal) {

				indNew = i;
				break;

			}

		}

		if (ind == indNew) {

			for (s32 i = (s32)tabGroup->widgetList.size() - 1; i > ind; --i) {

				Widget* temp = tabGroup->widgetList[i];
				if (temp->visibleGlobal && temp->enabledGlobal) {

					indNew = i;
					break;

				}

			}

		}

		if (ind != indNew) {

			setWidgetFocused(tabGroup->widgetList[indNew], true, false);
			return true;

		}

		return false;
		
	}

	void GUI::stopTabbing() {

		tabStart = false;
		tabRepeat = false;

	}

	//Grid.
	void GUI::setGridColumnRowCount(s32 _numColumns, s32 _numRows) {

		if (_numColumns <= 0) {
			_numColumns = 1;
			ZIXEL_WARN("Error in GUI::setGridColumnRowCount. Column count can't be less than 1.");
		}

		if (_numRows <= 0) {
			_numRows = 1;
			ZIXEL_WARN("Error in GUI::setGridColumnRowCount. Row count can't be less than 1.");
		}

		gridColumnCount = _numColumns;
		gridRowCount = _numRows;

		if (gridLayout) {

			for (Widget* child : widgetList) {

				if (child->gridColumn >= gridColumnCount) child->gridColumn = 0;
				if (child->gridRow >= gridRowCount) child->gridRow = 0;

				child->updateTransform(true);

			}

		}

	}

	void GUI::setGridLayout(bool _enable) {

		if (gridLayout != _enable) {

			gridLayout = _enable;

			for (Widget* child : widgetList) {
				child->updateTransform(true);
			}

		}

	}

	//Widget.
	void GUI::setWidgetSelected(Widget* widget, bool _callOnFocused) {

		stopTabbing();
		stopLocalShortcut();
		stopGlobalShortcut();

		if (textEditSelected != nullptr) {

			if ((TextEdit*)widget != textEditSelected) {
				textEditSelected->deselect();
			}

		}

		widgetSelected = widget;

		if (widgetFocused != widget) { //I changed this and it might break something?

            if(widgetFocused != nullptr) widgetFocused->onUnfocused();
			widgetFocused = widget;

			if (_callOnFocused && widget != nullptr) {

				widget->onFocused();

				if (widget->onFocus) {
					widget->onFocus(widget);
				}

			}

		}

	}

	void GUI::setWidgetFocused(Widget* widget, bool _callOnFocused, bool _stopTabbing) {

		if (_stopTabbing) stopTabbing();
		if (widgetShortcut != nullptr && widgetShortcut != widget) stopLocalShortcut();

		if ((widgetSelected == nullptr || widgetSelected == widget) && widgetFocused != widget) {

			if (textEditSelected != nullptr) {

				if ((TextEdit*)widget != textEditSelected) {
					textEditSelected->deselect();
				}

			}

            if(widgetFocused != nullptr) widgetFocused->onUnfocused();
			widgetFocused = widget;

			if (_callOnFocused) {

				widget->onFocused();

				if (widget->onFocus) {
					widget->onFocus(widget);
				}

			}

		}

	}

	Widget* GUI::getWidgetMouseOver() {
		return widgetMouseOver;
	}

	void GUI::setWidgetMouseOver(Widget* widget) {
		widgetMouseOver = widget;
	}

	void GUI::setWidgetScroll(Widget* widget) {

		stopTabbing();
		stopLocalShortcut();
		stopGlobalShortcut();

		widgetScroll = widget;
		windowScroll = nullptr;
		
		if (widget != nullptr) setWidgetFocused(widget, widget->type != WidgetType::TextEdit); //@Consider: Properly check the behaviour of TextEdit widgets.

	}

	bool GUI::isMouseOverScrollbars(Window* _window, bool checkChildren) {

		if (!_window->opened) return false;

		s32 containerX = _window->getContainerX();
		s32 containerY = _window->getContainerY();
		s32 containerWidth = _window->getContainerWidth();
		s32 containerHeight = _window->getContainerHeight();

		if (_window->scrollVerShow && Math::pointInRect(mouseX, mouseY, containerX + containerWidth - _window->sprScrollVer->sizeX, containerY + _window->scrollVerBarPos, _window->sprScrollVer->sizeX, _window->scrollVerBarLen)) return true;
		if (_window->scrollHorShow && Math::pointInRect(mouseX, mouseY, containerX + _window->scrollHorBarPos, containerY + containerHeight - _window->sprScrollHor->sizeY, _window->scrollHorBarLen, _window->sprScrollHor->sizeY)) return true;

		if (checkChildren) {

			for (s32 i = (s32)_window->widgetList.size() - 1; i >= 0; --i) {

				Widget* child = _window->widgetList[i];
				if (isMouseOverScrollbars(child, true)) return true;

			}

		}

		return false;
	}

	bool GUI::isMouseOverScrollbars(Widget* _widget, bool checkChildren) {

		if (!_widget->visibleGlobal) return false;

		if (_widget->scrollVerShow && Math::pointInRect(mouseX, mouseY, _widget->x + _widget->widthDraw - _widget->sprScrollVer->sizeX, _widget->y + _widget->scrollVerBarPos, _widget->sprScrollVer->sizeX, _widget->scrollVerBarLen)) return true;
		if (_widget->scrollHorShow && Math::pointInRect(mouseX, mouseY, _widget->x + _widget->scrollHorBarPos, _widget->y + _widget->heightDraw - _widget->sprScrollHor->sizeY, _widget->scrollHorBarLen, _widget->sprScrollHor->sizeY)) return true;

		if (checkChildren) {

			for (s32 i = (s32)_widget->widgetList.size() - 1; i >= 0; --i) {

				Widget* child = _widget->widgetList[i];
				if (isMouseOverScrollbars(child, true)) return true;

			}

			if (_widget->type == WidgetType::DockArea) {

				DockArea* dockArea = (DockArea*)_widget;

				for (DockContainer* container : dockArea->containerList) {

					if (container->tabList.size() > 0 && container->tabSelected != nullptr) {
						if (isMouseOverScrollbars(container->tabSelected->window, true)) return true;
					}

				}

			}

		}

		return false;
	}

	bool GUI::isMouseOverWidget(Window* _window, bool _isStart) {

		if (!Math::pointInRect(mouseX, mouseY, _window->x, _window->y, _window->width - (_window->sprScrollVer->sizeX * _window->scrollVerShow), _window->height - (_window->sprScrollHor->sizeY * _window->scrollHorShow))) return false;

		if (_window->docked) {

			return isMouseOverWidget(_window->dockTab->dockArea, false);

		}
		else {

			if (!_window->opened) return false;
			return true;

		}

		return false;

	}

	bool GUI::isMouseOverWidget(Widget* _widget, bool _isStart) {

		if (!_widget->visibleGlobal) return false;

		bool mouseOver = Math::pointInRect(mouseX, mouseY, _widget->x, _widget->y, _widget->viewportWidth, _widget->viewportHeight);
		if ((_isStart && !mouseOver) || (!_isStart && _widget->cutContent && !mouseOver)) return false;

		if (_widget->parent != nullptr) {
			return isMouseOverWidget(_widget->parent, false);
		}
		else if (_widget->window != nullptr) {
			return isMouseOverWidget(_widget->window, false);
		}

		return true;

	}

	//Window.
	Window* GUI::createWindow() {
		return new Window(this);
	}

	bool GUI::destroyWindow(Window* _window) {

		if (_window == nullptr) return false;
		if (_window->deleted) return false;

		if(_window->opened || _window->docked) _window->close();
		_window->deleted = true;

		const auto& it = std::find(allWindowsList.begin(), allWindowsList.end(), _window);
		if (it != allWindowsList.end()) {
			allWindowsList.erase(it);
		}

		if (std::find(deletedWindowsList.begin(), deletedWindowsList.end(), _window) == deletedWindowsList.end()) {
			deletedWindowsList.push_back(_window);
		}

		return true;

	}

	bool GUI::hasPopupWindow() {

		if (openedWindowsList.size() <= 0) return false;
		return (openedWindowsList[0]->popup);

	}

	Window* GUI::getPopupWindow() {

		if (openedWindowsList.size() <= 0) return nullptr;

		if (openedWindowsList[0]->popup) {
			return openedWindowsList[0];
		}

		return nullptr;

	}

	void GUI::setWindowScroll(Window* window) {
		
		stopTabbing();
		stopLocalShortcut();
		stopGlobalShortcut();

		windowScroll = window;
		widgetScroll = nullptr;

	}

	void GUI::updateWindowMouseOver() {

		windowMouseOver = nullptr;
		windowMouseOverContainer = false;

		if (isMouseOverDropDownMenu()) return;

		for (s32 i = 0; i < openedWindowsList.size(); ++i) { //No, we can't store the size in a variable.

			Window* window = openedWindowsList[i];

			if ((getPopupWindow() == nullptr || getPopupWindow() == window) && Math::pointInRect(mouseX, mouseY, window->getX() - GUI_WINDOW_RESIZE_GRAB_RANGE, window->getY(), window->getWidth() + (GUI_WINDOW_RESIZE_GRAB_RANGE * 2), window->getHeight() + GUI_WINDOW_RESIZE_GRAB_RANGE)) {

				windowMouseOver = window;
				windowMouseOverContainer = Math::pointInRect(mouseX, mouseY, window->getContainerX(), window->getContainerY(),
					window->getContainerWidth() - (window->sprScrollVer->sizeX * window->scrollVerShow),
					window->getContainerHeight() - (window->sprScrollHor->sizeY * window->scrollHorShow)
				);

				break;

			}

		}

	}

	//Drop down menu.
	DropDownMenu* GUI::createDropDownMenu() {
		return new DropDownMenu(this);
	}

	bool GUI::isMouseOverDropDownMenu() {
		return (dropDownMenuOpen != nullptr && dropDownMenuOpen->mouseOverPage);
	}

	//Tooltip.
	void GUI::setTooltip(Widget* _widget, UTF8String _text, bool _isCursorSpriteCentered) {

		//@TODO: Assign shortcuts to tooltips.

		if (_widget == nullptr) {

			tooltipWidget = nullptr;
			tooltipWindow = nullptr;

			return;

		}

		if (_text.empty()) {

			if(!tooltipText.empty()) tooltipText = "";
			return;

		}

		tooltipWidget = _widget;
		tooltipWindow = nullptr;

		tooltipText = _text;

		if ((tooltipWidgetPrev != _widget || tooltipWindowPrev != nullptr) || tooltipTextPrev != _text) {

			tooltipIsCursorSpriteCentered = _isCursorSpriteCentered;
			tooltipUsesAbsolutePosition = false;
			tooltipWidth = renderer->getStringWidth(tooltipFont, _text) + (GUI_TOOLTIP_HOR_SPACING * 2);
			tooltipHeight = renderer->getStringHeight(tooltipFont, _text) + (GUI_TOOLTIP_VER_SPACING * 2);

		}

	}

	void GUI::setTooltip(Window* _window, UTF8String _text, bool _isCursorSpriteCentered) {

		if (_window == nullptr) {

			tooltipWidget = nullptr;
			tooltipWindow = nullptr;

			return;

		}

		if (_text.empty()) {

			if (!tooltipText.empty()) tooltipText = "";
			return;

		}

		tooltipWidget = nullptr;
		tooltipWindow = _window;

		tooltipText = _text;

		if ((tooltipWindowPrev != _window || tooltipWidgetPrev != nullptr) || tooltipTextPrev != _text) {

			tooltipIsCursorSpriteCentered = _isCursorSpriteCentered;
			tooltipUsesAbsolutePosition = false;
			tooltipWidth = renderer->getStringWidth(tooltipFont, _text) + (GUI_TOOLTIP_HOR_SPACING * 2);
			tooltipHeight = renderer->getStringHeight(tooltipFont, _text) + (GUI_TOOLTIP_VER_SPACING * 2);

		}

	}

	void GUI::setTooltipAbsolute(Widget* _widget, UTF8String _text, s32 _x, s32 _y) {

		if (_widget == nullptr) {

			tooltipWidget = nullptr;
			tooltipWindow = nullptr;

			return;

		}

		if (_text.empty()) {

			if (!tooltipText.empty()) tooltipText = "";
			return;

		}

		tooltipWidget = _widget;
		tooltipWindow = nullptr;

		tooltipText = _text;

		if ((tooltipWidgetPrev != _widget || tooltipWindowPrev != nullptr) || tooltipTextPrev != _text) {

			tooltipUsesAbsolutePosition = true;
			tooltipAbsX = _x;
			tooltipAbsY = _y;
			tooltipWidth = renderer->getStringWidth(tooltipFont, _text) + (GUI_TOOLTIP_HOR_SPACING * 2);
			tooltipHeight = renderer->getStringHeight(tooltipFont, _text) + (GUI_TOOLTIP_VER_SPACING * 2);

		}

	}

	void GUI::onKeyPress(u16 button) {

		s32 ind = (button / 8);
		u8 bit = (u8)(button - (ind * 8));

		keyDown[ind] |= (0x01 << bit);
		keyPress[ind] |= (0x01 << bit);
		keyClearState[ind] &= ((0x01 << bit) ^ 0xFF);

		++keyDownCounter;
		++keyPressCounter;

		lastKey = button;

	}

	void GUI::onKeyRelease(u16 button) {

		s32 ind = (button / 8);
		u8 bit = (u8)(button - (ind * 8));

		keyDown[ind] &= ((0x01 << bit) ^ 0xFF);
		keyRelease[ind] |= (0x01 << bit);

		--keyDownCounter;
		++keyReleaseCounter;

	}

	void GUI::onTextInput(u32 codepoint) {

		//@TODO: Add 32 bit unicode support.

		lastChar = (codepoint < 128) ? codepoint : 0;

	}

	void GUI::onMousePress(u8 button) {

		mouseDown |= (0x01 << button);
		mousePress |= (0x01 << button);

		mouseClearState &= ((0x01 << button) ^ 0xFF);

	}

	void GUI::onMouseRelease(u8 button) {

		mouseDown &= ((0x01 << button) ^ 0xFF);
		mouseRelease |= (0x01 << button);

	}

	void GUI::onMouseScroll(s8 dir) {
		mouseScrollDir = dir;
	}

	void GUI::onWindowResize(s32 _width, s32 _height, s32 _prevWidth, s32 _prevHeight) {

		/*if (dropDownMenuOpen != nullptr && dropDownMenuOpen->page != nullptr) {
			dropDownMenuOpen->updatePagePos(dropDownMenuOpen->page);
		}*/

		if (dropDownMenuOpen != nullptr) dropDownMenuOpen->close();

		for (Widget* child : widgetList) {
			child->updateTransform(true);
		}

		for (Window* window : openedWindowsList) {

			if (window->isMaximized()) window->setRect(0, 0, _width, _height);
			else {

				if (_prevWidth > 0) {

					f32 centerX = window->xFloat + (window->width * 0.5f);
					f32 xScale = (f32)centerX / (f32)_prevWidth;

					if (centerX >= (f32)_prevWidth) {
					
						f32 d = (centerX - (f32)_prevWidth);
						centerX = (f32)_width + d;

					}
					else if (centerX < 0) {
						//Don't move it.
					}
					else {
						centerX = _width * xScale;
					}

					window->xFloat = Math::clampFloat(centerX - (window->width * 0.5f), (f32)(-window->width) + GUI_WINDOW_CLAMP_BORDER, (f32)_width - GUI_WINDOW_CLAMP_BORDER);

				}

				if (_prevHeight > 0 && !(window->y == 0 && _height <= window->height)) {

					f32 centerY = window->yFloat + (window->height * 0.5f);
					f32 yScale = (f32)centerY / (f32)_prevHeight;

					if (window->y == 0 && _prevHeight < window->height && _height > window->height) {
						centerY = _height * 0.5f;
					}
					else if (centerY >= (f32)_prevHeight) {

						f32 d = (centerY - (f32)_prevHeight);
						centerY = (f32)_height + d;


					}
					else {
						centerY = _height * yScale;
					}


					window->yFloat = Math::clampFloat(centerY - (window->height * 0.5f), 0.0f, Math::maxFloat(0.0f, (f32)_height - GUI_WINDOW_CLAMP_BORDER));

				}

				if (_prevWidth > 0 || _prevHeight > 0) window->setRect((s32)roundf(window->xFloat), (s32)roundf(window->yFloat), window->getWidth(), window->getHeight(), false);

			}

		}

	}

	void GUI::onWindowFocus(bool _focused) {
		if (dropDownMenuOpen != nullptr) dropDownMenuOpen->close();
	}

	void GUI::update(f32 dt) {

		renderer->getMousePos(mouseX, mouseY);

		u8 prevCursorType = cursorType;
		u8 prevCursorId = cursorId;

		cursorType = 0;
		cursorId = CURSOR_ARROW;
		cursorSprite = nullptr;
		cursorSub = 0;
		cursorRot = 0;
		cursorCentered = false;

		tooltipWidgetPrev = tooltipWidget;
		tooltipWindowPrev = tooltipWindow;
		if (tooltipText != tooltipTextPrev) tooltipTextPrev = tooltipText;

		if (tooltipWidget != nullptr || tooltipWindow != nullptr) {

			tooltipWidget = nullptr;
			tooltipWindow = nullptr;
			tooltipText = "";

		}

		if (dropDownMenuOpen != nullptr) {
			dropDownMenuOpen->update(dt);
		}

		widgetMouseOver = nullptr;
		widgetMouseOverScroll = nullptr;

		windowMouseOver = nullptr;
		windowMouseOverContainer = false;

		if (dropDownMenuOpen == nullptr || !isMouseOverDropDownMenu()) {

			bool moveToFront = false;
			for (s32 i = 0; i < openedWindowsList.size(); ++i) { //No, we can't store the size in a variable.

				Window* window = openedWindowsList[i];

				if ((getPopupWindow() == nullptr || getPopupWindow() == window) && Math::pointInRect(mouseX, mouseY, window->getX() - GUI_WINDOW_RESIZE_GRAB_RANGE, window->getY(), window->getWidth() + (GUI_WINDOW_RESIZE_GRAB_RANGE * 2), window->getHeight() + GUI_WINDOW_RESIZE_GRAB_RANGE)) {

					if (windowMouseOver == nullptr) {

						windowMouseOver = window;
						windowMouseOverContainer = Math::pointInRect(mouseX, mouseY, window->getContainerX(), window->getContainerY(),
							window->getContainerWidth() - (window->sprScrollVer->sizeX * window->scrollVerShow),
							window->getContainerHeight() - (window->sprScrollHor->sizeY * window->scrollHorShow)
						);

						if (isMousePressed(MOUSE_LEFT_RIGHT_MIDDLE)) moveToFront = true;

					}

				}

				window->update(dt);

				if (!window->isOpened()) i--;
				else {

					std::vector<Widget*>& children = window->getChildren();
					for (s32 j = (s32)children.size() - 1; j >= 0; --j) {

						if (children[j]->visibleGlobal) {
							children[j]->updateMain(dt);
						}

					}

				}

			}

			if (moveToFront) {

				stopTabbing();
				stopLocalShortcut();
				stopGlobalShortcut();

				windowMouseOver->moveToFront();

				if (windowMouseOver->onFocus) {
					windowMouseOver->onFocus(windowMouseOver);
				}

			}

		}
		
		for (s32 i = (s32)widgetList.size() - 1; i >= 0; --i) {

			Widget* child = widgetList[i];

			if (child->visibleGlobal) {
				child->updateMain(dt);
			}

		}
		
		if (windowScroll != nullptr) {
			windowScroll->moveScrollbars();
		}
		else {

			for (Window* window : openedWindowsList) {
				window->checkScrollbars();
			}

		}
		
		if (deletedWindowsList.size() > 0) {

			for (Window* window : deletedWindowsList) {
				delete window;
			}

			deletedWindowsList.clear();

		}
		
		//@TODO: Add global shortcut support for windows.
		//@Consider: Global shortcut support for docked windows. (We need to keep track of what DockArea is focused and what DockTab is active.)
		bool canUseShortcuts = (widgetSelected == nullptr && widgetScroll == nullptr && windowSelected == nullptr && windowScroll == nullptr && !textEditDeselectedWithEnter);
		
		if (canUseShortcuts) {

			bool modCtrl = isKeyDown(KEY_CONTROL);
			bool modShift = isKeyDown(KEY_SHIFT);
			bool modAlt = isKeyDown(KEY_LEFT_ALT);

			//Tabbing between widgets.
			if (dropDownMenuOpen == nullptr && widgetFocused != nullptr && widgetFocused->tabGroup != nullptr && !widgetFocused->overrideTabKey) {

				bool tabShift = (!modCtrl && modShift && !modAlt);

				if (isKeyPressed(KEY_TAB)) {
					
					stopLocalShortcut();
					stopGlobalShortcut();

					if (tabShift) {

						if (doTabBackward(widgetFocused)) {

							tabForward = false;
							tabStart = true;
							tabRepeat = false;
							tabTimer = 0.0f;

						}

					}
					else {

						if (doTabForward(widgetFocused)) {

							tabForward = true;
							tabStart = true;
							tabRepeat = false;
							tabTimer = 0.0f;

						}

					}

				}

				if (tabStart) {

					if (!isKeyDown(KEY_TAB) || (!tabForward && !tabShift)) {
						tabStart = false;
					}
					else {

						tabTimer += dt;
						if (tabTimer >= GUI_TAB_START_TIME) {

							tabStart = false;
							tabRepeat = true;
							tabTimer = GUI_TAB_REPEAT_TIME;

						}

					}

				}
				
				if (tabRepeat) {

					if (!isKeyDown(KEY_TAB) || (!tabForward && !tabShift)) {
						tabRepeat = false;
					}
					else {

						tabTimer += dt;
						if (tabTimer >= GUI_TAB_REPEAT_TIME) {

							tabTimer = 0.0f;

							if (tabForward) {

								if (!doTabForward(widgetFocused)) {
									tabRepeat = false;
								}

							}
							else {

								if (!doTabBackward(widgetFocused)) {
									tabRepeat = false;
								}

							}

						}

					}

				}

			}
			else {

				tabStart = false;
				tabRepeat = false;

			}

			if (!hasPopupWindow()) {

				//Global shortcuts.
				for (GlobalShortcut& shortcut : globalShortcutList) {

					if (!shortcut.onCallback) continue;

					u16 key = shortcut.keyCode;
					u8 modifiers = shortcut.modifiers;

					//Make sure that global shortcuts don't override any DropDownMenu shortcuts if one is open.
					if (dropDownMenuOpen != nullptr && (key == KEY_ESCAPE || key == KEY_ENTER || key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN || key == KEY_TAB)) {
						continue;
					}

					//Make sure that global shortcuts don't override any local shortcuts of a focused widget.
					if (widgetFocused != nullptr) {

						if (widgetFocused->overrideNonModifierShortcuts && !modCtrl && !modShift && !modAlt) {
							continue;
						}

						bool foundShortcut = false;

						for (LocalShortcut& local : widgetFocused->localShortcutList) {

							if (local.keyCode == key && local.modifiers == modifiers) {

								foundShortcut = true;
								break;

							}

						}

						if (foundShortcut) {
							continue;
						}

					}

					if (isGlobalShortcutPressed(shortcut)) {

						stopTabbing();
						stopLocalShortcut();
						stopGlobalShortcut();

						if (dropDownMenuOpen != nullptr) {
							dropDownMenuOpen->close();
						}

						menuBarOpen = nullptr;

						globalShortcutMode = true;
						currentGlobalShortcut = shortcut;

						if (shortcut.onStart) {

							if (shortcut.onStart()) {

								if (shortcut.onCallback()) {

									if (shortcut.repeatable) {

										globalShortcutStart = true;
										globalShortcutRepeat = false;
										globalShortcutTimer = 0.0f;

									}
									else if (!shortcut.onEnd) {
										globalShortcutMode = false;
									}

								}
								else {

									if (shortcut.onEnd) shortcut.onEnd();
									globalShortcutMode = false;

								}

							}
							else {
								globalShortcutMode = false;
							}

						}
						else {

							if (shortcut.onCallback()) {

								if (shortcut.repeatable) {

									globalShortcutStart = true;
									globalShortcutRepeat = false;
									globalShortcutTimer = 0.0f;

								}
								else if (!shortcut.onEnd) {
									globalShortcutMode = false;
								}

							}
							else {

								if (shortcut.onEnd) shortcut.onEnd();
								globalShortcutMode = false;

							}

						}

						break;

					}

				}

				if (globalShortcutMode) {

					if (!currentGlobalShortcut.repeatable) {

						if (!isGlobalShortcutDown(currentGlobalShortcut)) {

							globalShortcutMode = false;
							currentGlobalShortcut.onEnd();

						}

					}
					else {

						if (globalShortcutStart) {

							if (!isGlobalShortcutDown(currentGlobalShortcut)) {

								globalShortcutMode = false;
								globalShortcutStart = false;

								if (currentGlobalShortcut.onEnd) {
									currentGlobalShortcut.onEnd();
								}

							}
							else {

								globalShortcutTimer += dt;
								if (globalShortcutTimer >= GUI_GLOBAL_SHORTCUT_START_TIME) {

									globalShortcutStart = false;
									globalShortcutRepeat = true;
									globalShortcutTimer = GUI_GLOBAL_SHORTCUT_REPEAT_TIME;

								}

							}

						}

						if (globalShortcutRepeat) {

							if (!isGlobalShortcutDown(currentGlobalShortcut)) {

								globalShortcutMode = false;
								globalShortcutRepeat = false;

								if (currentGlobalShortcut.onEnd) {
									currentGlobalShortcut.onEnd();
								}

							}
							else {

								globalShortcutTimer += dt;
								if (globalShortcutTimer >= GUI_GLOBAL_SHORTCUT_REPEAT_TIME) {

									globalShortcutTimer = 0.0f;

									if (!currentGlobalShortcut.onCallback()) {

										globalShortcutMode = false;
										globalShortcutRepeat = false;

										if (currentGlobalShortcut.onEnd) {
											currentGlobalShortcut.onEnd();
										}

									}

								}

							}

						}

					}

				}

			}
			else {
				stopGlobalShortcut();
			}

		}
		else {

			stopTabbing();
			stopGlobalShortcut();

		}

		//Closing window using escape key.
		//I think we only want this for popup windows.
		//@Reminder: If we add shortcuts for popup windows, we need to check that the escape key doesn't override any of those shortcuts.
		if (hasPopupWindow() && isKeyPressed(KEY_ESCAPE) && canUseShortcuts) {

			bool canClose = true;

			if (dropDownMenuOpen != nullptr) canClose = false;

			//Make sure that the escape key doesn't override any local shortcuts of a focused widget.
			if (widgetFocused != nullptr) {

				if (widgetFocused->overrideNonModifierShortcuts) {
					canClose = false;
				}
				else {

					for (LocalShortcut& local : widgetFocused->localShortcutList) {

						if (local.keyCode == KEY_ESCAPE && local.modifiers == 0) {

							canClose = false;
							break;

						}

					}

				}

			}

			if (canClose) {

				stopTabbing();
				stopLocalShortcut();
				stopGlobalShortcut();

				openedWindowsList[0]->close();

			}

		}

		if (tooltipWidgetPrev != tooltipWidget || tooltipWindowPrev != tooltipWindow || tooltipTextPrev != tooltipText) {
			tooltipTimer = 0.0f;
		}
		else if ((tooltipWidgetPrev != nullptr || tooltipWindowPrev != nullptr) && !tooltipTextPrev.empty()) {

			if (tooltipTimer < GUI_TOOLTIP_TIMER) {

				tooltipTimer += dt;
				if (tooltipTimer >= GUI_TOOLTIP_TIMER) {

					tooltipX = tooltipUsesAbsolutePosition ? tooltipAbsX : mouseX + GUI_TOOLTIP_X_OFFSET;
					tooltipY = tooltipUsesAbsolutePosition ? tooltipAbsY : mouseY + (tooltipIsCursorSpriteCentered ? GUI_TOOLTIP_Y_OFFSET / 2 : GUI_TOOLTIP_Y_OFFSET);

					if (tooltipX + tooltipWidth - 1 >= renderer->windowWidth) {
						tooltipX = mouseX - tooltipWidth;
					}

					if (tooltipY + tooltipHeight - 1 >= renderer->windowHeight) {
						tooltipY = mouseY - tooltipHeight;
					}

					tooltipX = Math::clampInt(tooltipX, 0, renderer->windowWidth - tooltipWidth);
					tooltipY = Math::clampInt(tooltipY, 0, renderer->windowHeight - tooltipHeight);

				}

			}

		}

		if (prevCursorType != cursorType) {

			if (prevCursorType == 1) {

				renderer->showMouseCursor();
				renderer->setMouseCursor(cursorId);

			}
			else if (prevCursorType == 0) {
				renderer->hideMouseCursor();
			}

		}
		else {

			if (prevCursorId != cursorId) {
				renderer->setMouseCursor(cursorId);
			}

		}

		for (s32 i = 0; i < 48; ++i) {

			keyPress[i] = 0;
			keyRelease[i] = 0;

		}

		keyPressCounter = 0;
		keyReleaseCounter = 0;

		lastKey = 0;
		lastChar = 0;

		mousePress = 0x00;
		mouseRelease = 0x00;
		mouseScrollDir = 0;

		textEditDeselectedWithEnter = false;

	}

	void GUI::render() {

		for (Widget* child : widgetList) {

			if (child->visibleGlobal) {
				child->renderMain();
			}

		}

		for (s32 i = (s32)openedWindowsList.size() - 1; i >= 0; --i) {

			if (hasPopupWindow() && i == 0) {
				renderer->renderRect(0, 0, renderer->windowWidth, renderer->windowHeight, { 0.0f, 0.0f, 0.0f, GUI_WINDOW_POPUP_BACKGROUND_ALPHA });
			}

			openedWindowsList[i]->render();

		}

		if (dockTabDragArea != nullptr && dockTabDrag != nullptr) {
			
			s32 tabX = mouseX - dockTabDragMouseXOffset;
			s32 tabY = mouseY - dockTabDragMouseYOffset;
			s32 tabHeight = dockTabDragArea->sprDockTab->sizeY;

			renderer->render9P(dockTabDragArea->sprDockContainer, 1, tabX, tabY + tabHeight, dockTabDragContainerWidth, dockTabDragContainerHeight - tabHeight);

			renderer->render3PHor(dockTabDragArea->sprDockTab, 2, tabX, tabY, dockTabDragWidth);

			renderer->cutStart(tabX, tabY, dockTabDragWidth, tabHeight);

			renderer->renderText(dockTabDragArea->fntDockTab, dockTabDragTitle, tabX + GUI_DOCK_TAB_TEXT_HOR_SPACING, tabY + (tabHeight / 2) + GUI_DOCK_TAB_TEXT_VER_SPACING, TextAlign::Left, TextAlign::Middle, GUI_DOCK_TAB_TITLE_COL_FOCUSED);

			if (dockTabDragShowClose) renderer->renderSprite(dockTabDragArea->sprDockTabClose, 1, tabX + dockTabDragWidth - dockTabDragArea->sprDockTabClose->sizeX - GUI_DOCK_TAB_CLOSE_HOR_SPACING, tabY + (tabHeight / 2) - (dockTabDragArea->sprDockTabClose->sizeY / 2));

			renderer->cutEnd();

		}

		if (dropDownMenuOpen != nullptr) {
			dropDownMenuOpen->render();
		}

		//renderer->renderRect(0, 0, 512, 512, { 0.2f, 0.2f, 0.4f, 1.0f });
		//renderer->renderTexture(renderer->getTextureAtlas()->getTexture(), 0, 0, 512, 512); //@TODO: Atlas generation kinda scuffed.

		//Font rendering test.
		//Font* font = renderer->getTextureAtlasFont("robotoRegular13");

		//std::string str = "100";

		//s32 w = renderer->getStringWidth(font, str);
		//s32 h = renderer->getStringHeight(font, str);

		//renderer->renderRect(128, 128, w, h, { 0.2f, 0.2f, 0.2f, 1.0f });

		//renderer->renderText(font, str, 128, 128, TextAlign::Left, TextAlign::Top);

		if ((tooltipWidget != nullptr || tooltipWindow != nullptr) && !tooltipText.empty() && tooltipTimer >= GUI_TOOLTIP_TIMER) {
			
			renderer->render9P(tooltipSprite, 0, tooltipX, tooltipY, tooltipWidth, tooltipHeight);
			renderer->renderText(tooltipFont, tooltipText, tooltipX + GUI_TOOLTIP_HOR_SPACING + GUI_TOOLTIP_TEXT_HOR_OFFSET, tooltipY + (tooltipHeight / 2) + GUI_TOOLTIP_TEXT_VER_OFFSET, TextAlign::Left, TextAlign::Middle);

		}

		if (cursorSprite != nullptr) {

			s32 cursorX = mouseX, cursorY = mouseY;
			if (cursorCentered) {
				cursorX -= (cursorSprite->sizeX / 2);
				cursorY -= (cursorSprite->sizeY / 2);
			}

			//@TODO: This doesn't render outside the window.
			renderer->renderSprite(cursorSprite, cursorSub, cursorX, cursorY); //@TODO: Add sprite rotation.
		}

	}

}