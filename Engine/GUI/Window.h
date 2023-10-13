/*
    Window.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <functional>
#include "Engine/GUI/Widget.h"

#define GUI_WINDOW_FOCUS_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1)
#define GUI_WINDOW_PRE_CLOSE_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1)
#define GUI_WINDOW_CLOSE_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)

#define GUI_WINDOW_SPR_TITLE_BAR "windowTitleBar"
#define GUI_WINDOW_SPR_CONTAINER "windowContainer"
#define GUI_WINDOW_SPR_CLOSE "windowClose"
#define GUI_WINDOW_SPR_MAXIMIZE "windowMaximize"
#define GUI_WINDOW_FONT "robotoRegular12"
#define GUI_WINDOW_CLAMP_BORDER 30
#define GUI_WINDOW_RESIZE_GRAB_RANGE 7
#define GUI_WINDOW_MAXIMIZE_DRAG_RANGE 5
#define GUI_WINDOW_TITLE_HOR_SPACING 10
#define GUI_WINDOW_TITLE_VER_SPACING 0
#define GUI_WINDOW_TITLE_COL_FOCUSED { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_WINDOW_TITLE_COL_UNFOCUSED { 0.584f, 0.596f, 0.678f, 1.0f }
#define GUI_WINDOW_BUTTON_HOR_PADDING 8
#define GUI_WINDOW_BUTTON_SPACING 3
#define GUI_WINDOW_DOUBLE_CLICK_TIME 300
#define GUI_WINDOW_BORDER_TOP 0
#define GUI_WINDOW_BORDER_LEFT 2
#define GUI_WINDOW_BORDER_BOTTOM 2
#define GUI_WINDOW_BORDER_RIGHT 2
#define GUI_WINDOW_POPUP_BACKGROUND_ALPHA 0.3f

namespace Zixel {

	enum class WindowMoveDir : u8 {

		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight,
		Top,
		Left,
		Bottom,
		Right,
		Move,
		Close,
		Maximize,

	};

	struct Font;
	struct DockArea;
	struct DockContainer;
	struct DockTab;

	struct Window {

		Renderer* renderer;
		GUI* gui;

		Sprite* sprScrollVer;
		Sprite* sprScrollHor;
		Sprite* sprScrollCorner;
		Sprite* sprTitleBar;
		Sprite* sprContainer;
		Sprite* sprClose;
		Sprite* sprMaximize;

		Font* fntTitle;

		s32 x = 0;
		s32 y = 0;
		s32 xStart = 0;
		s32 yStart = 0;
		s32 xPrevState = 0;
		s32 yPrevState = 0;
		f32 xFloat = 0.0f; //Just used when the main window gets resized.
		f32 yFloat = 0.0f;

		s32 width = 360;
		s32 height = 300;
		s32 widthStart = 0;
		s32 heightStart = 0;
		s32 widthPrevState = 0;
		s32 heightPrevState = 0;

		bool hasDefaultSize = false;
		s32 defaultWidth = 0;
		s32 defaultHeight = 0;

		s32 mouseXStart = 0;
		s32 mouseYStart = 0;

		WindowMoveDir moveDir = WindowMoveDir::TopLeft;

		//Grid.
		bool gridLayout = false;
		s32 gridColumnCount = 1;
		s32 gridRowCount = 1;

		bool docked = false;
		DockArea* dockArea = nullptr;
		DockTab* dockTab = nullptr;
		bool dockFixedWidth = false;
		bool dockFixedHeight = false;

		UTF8String title;
		UTF8String titleDraw;
		UTF8String tooltip;

		bool opened = false;
		bool popup = false;
		bool maximized = false;
		bool deleted = false;

		bool canResize = true;
		bool canMove = true;

		bool showButtonClose = true;
		bool showButtonMaximize = true;

		bool mouseOverButtonClose = false;
		bool mouseOverButtonMaximize = false;

		bool saveState = false; //Used to display an "*" in window title.

		f64 doubleClickTime = -1;

		s32 actualMinWidth = 0;
		s32 actualMinHeight = 0;
		s32 minWidth = 0;
		s32 minHeight = 0;

		//Used for checking if dock containers should use the window min size or not.
		bool hasSetMinWidth = false;
		bool hasSetMinHeight = false;

		s32 contentWidth = 0;
		s32 contentHeight = 0;

		bool scrollVerVisible = true;
		bool scrollVerShow = false;
		s32 scrollVerLen = 0;
		s32 scrollVerBarLen = 0;
		s32 scrollVerBarMaxPos = 0;
		s32 scrollVerBarPos = 0;
		f32 scrollVerOffset = 0.0f;
		f32 scrollVerOffsetPrev = 0.0f;
		u8 scrollVerInd = 0;

		bool scrollHorVisible = true;
		bool scrollHorShow = false;
		s32 scrollHorLen = 0;
		s32 scrollHorBarLen = 0;
		s32 scrollHorBarMaxPos = 0;
		s32 scrollHorBarPos = 0;
		f32 scrollHorOffset = 0.0f;
		f32 scrollHorOffsetPrev = 0.0f;
		u8 scrollHorInd = 0;

		bool showTooltip = true;

		std::vector<Widget*> widgetList;

		std::function<void(Window*)> onFocus;
		std::function<bool(Window*)> onPreClose;
		std::function<void(Window*, bool, DockContainer*, bool&)> onClose;

		void calculateContentSize(s32& maxX, s32& maxY);
		void calculateContentSize();

		void updateScrollbars();

		bool canInteract();

		s32 getAvailableTitleSpace();
		void updateTitleDraw();

		//Public.
		Window(GUI* _gui);
		~Window();

		void checkScrollbars();
		void moveScrollbars();

		void updateTransform(bool updateChildren = false, bool checkResize = true);

		Window* getRootWindow();

		void moveToFront();
		bool isInFront(bool _includePopupWindows = true);

		void open(s32 _x, s32 _y, s32 _width = -1, s32 _height = -1, bool _popup = false);
		void open();
		void openPopup();
		void close(bool _callOnClose = true, bool _callOnPreClose = true);
		void __close(bool _destroyTabReference = true, bool _forceSetDestroyDockContainerIfZeroTabs = false, bool _destroyDockContainerIfZeroTabs = true, bool _callOnClose = true, bool _callOnPreClose = true);
		void maximize();
		void center();
		DockTab* dockTo(DockContainer* _container);

		bool isOpened();
		void setOpened(bool _opened);
		bool isMaximized();
		void setMaximized(bool _maximized);
		bool isFocused();

		bool isDocked();
		void setDocked(bool _docked);
		void setDockArea(DockArea* _area);
		void setDockTab(DockTab* _tab);
		void setDockFixedSize(bool _fixedWidth, bool _fixedHeight);

		void setTitle(UTF8String _title);
		UTF8String getTitle();

		bool isMouseOverTitle();

		void setTooltip(UTF8String _tooltip);

		void setSaveState(bool _enable);

		void setMinimumSize(s32 _width = -1, s32 _height = -1);

		bool setRect(s32 _x, s32 _y, s32 _width = -1, s32 _height = -1, bool _setFloatPos = true);
		bool setSize(s32 _width, s32 _height);

		void setDefaultSize(s32 _width, s32 _height);
		void resetToDefaultSize();
		s32 getDefaultWidth();
		s32 getDefaultHeight();

		s32 getX();
		s32 getY();
		s32 getWidth();
		s32 getHeight();

		s32 getContainerX();
		s32 getContainerY();
		s32 getContainerWidth();
		s32 getContainerHeight();

		//Grid.
		void setGridColumnRowCount(s32 _numColumns = 1, s32 _numRows = 1);
		void setGridLayout(bool _enable);

		std::vector<Widget*>& getChildren();

		void setOnFocus(std::function<void(Window*)> _onFocus);
		void setOnPreClose(std::function<bool(Window*)> _onPreClose);
		void setOnClose(std::function<void(Window*, bool, DockContainer*, bool&)> _onClose);

		void update(f32 dt);
		void render();

	};

}