/*
    Widget.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <functional>
#include "Engine/StringHelper.h"
#include "Engine/GUI/WidgetType.h"

#define GUI_WIDGET_FOCUS_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1)

namespace Zixel {

	enum class WidgetAlign : u8 {
		Left, Center, Right,
		Top, Middle, Bottom,
	};

	struct Renderer;
	struct Sprite;
	struct GUI;
	struct TabGroup;
	struct Window;

	struct LocalShortcut {

		u16 keyCode = 0;
		u8 modifiers = 0;

		bool repeatable = false;
		bool callOnEndImplicitly = true;

		std::function<bool()> onStart;
		std::function<bool()> onCallback;
		std::function<void()> onEnd;

	};

	struct Widget {

		Renderer* renderer;
		GUI* gui;

		Sprite* sprFocus;
		Sprite* sprScrollVer;
		Sprite* sprScrollHor;
		Sprite* sprScrollCorner;

		WidgetType type = WidgetType::Base;
		std::string id = "";

		Widget* parent = nullptr;
		Window* window = nullptr;
		TabGroup* tabGroup = nullptr;

		bool renderFocus = true;

		bool mouseOver = false;
		bool mouseOverScrollbars = false;

		s32 x = 0, y = 0;
		s32 xPrev = 0, yPrev = 0;
		s32 xNoScroll = 0, yNoScroll = 0;
		
		s32 minWidth = 0, minHeight = 0;
		s32 width = 64, height = 64;
		s32 widthDraw = width, heightDraw = height;
		s32 widthDrawPrev = width, heightDrawPrev = height;
		s32 viewportWidth = width, viewportHeight = height;

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

		bool canScrollVer = true;
		bool canScrollHor = true;

		bool canScrollWhileSelected = false;

		bool fillX = false;
		bool fillY = false;

		WidgetAlign hAlign = WidgetAlign::Left;
		WidgetAlign vAlign = WidgetAlign::Top;

		s32 marginTop = 0;
		s32 marginLeft = 0;
		s32 marginRight = 0;
		s32 marginBottom = 0;

		s32 paddingTop = 0;
		s32 paddingLeft = 0;
		s32 paddingRight = 0;
		s32 paddingBottom = 0;

		bool gridLayout = false;
		s32 gridColumnCount = 1;
		s32 gridRowCount = 1;
		s32 gridColumn = 0;
		s32 gridRow = 0;

		bool canResize = true;

		bool includeWhenCalculatingParentContentWidth = true;
		bool includeWhenCalculatingParentContentHeight = true;

		bool cutContent = false;
		s32 contentWidth = 0;
		s32 contentHeight = 0;

		s32 insideWidth = 0;
		s32 insideHeight = 0;

		bool enabled = true;
		bool visible = true;

		bool enabledGlobal = true;
		bool visibleGlobal = true;

		std::vector<Widget*> widgetList;
		std::vector<LocalShortcut> localShortcutList;

		bool overrideNonModifierShortcuts = false;
		bool overrideTabKey = false;

		bool shortcutMode = false;
		bool shortcutStart = false;
		bool shortcutRepeat = false;
		f32 shortcutTimer = 0.0f;
		LocalShortcut currentShortcut;

		UTF8String tooltip = "";

		std::function<void(Widget*)> onFocus;

		virtual void onFocused();
        virtual void onUnfocused();
		virtual void onResize(s32 prevWidth, s32 prevHeight);
		virtual void update(f32 dt);
		virtual void render();

		Widget(GUI* _gui, Widget* _parent, Window* _window);
		virtual ~Widget();

		void init(std::string& parameters);

		void destroyChild(Widget* child);

		Widget* getRootWidget(Widget* widget);

		bool checkEnabledGlobal();
		bool checkVisibleGlobal();
		void updateEnabledGlobal();
		void updateVisibleGlobal();
		void setEnabledGlobal(bool _enabledGlobal);
		void setVisibleGlobal(bool _visibleGlobal);
		void setEnabled(bool _enabled);
		void setVisible(bool _visible);
		
		void setGridColumn(s32 _column, bool _updateTransform = true);
		void setGridRow(s32 _row, bool _updateTransform = true);
		void setGridColumnCount(s32 _count);
		void setGridRowCount(s32 _count);

		void setPadding(s32 _paddingLeft, s32 _paddingTop, s32 _paddingRight, s32 _paddingBottom);

		void setWidth(s32 _width);
		void setHeight(s32 _height);

		f32 getMaxVerScrollOffset();
		f32 getMaxHorScrollOffset();

		void setInsideSize(s32 _width, s32 _height);
		void setScrollOffset(f32 _horScroll, f32 _verScroll);

		void setParameters(std::string& parameters);

		void addLocalShortcut(LocalShortcut& _shortcut);
		void stopShortcutMode();
		bool inShortcutMode();
		bool isShortcutDown(LocalShortcut& _shortcut);
		bool isShortcutPressed(LocalShortcut& _shortcut);

		void setOnFocus(std::function<void(Widget*)> _onFocus);

		bool canInteract();
		bool shouldUnfocus();

		void updatePosSize();
		void calculateContentSize(s32& maxX, s32& maxY);
		void calculateContentSize();
		void updateScrollbars();
		void updateTransform(bool updateChildren = false, bool checkResize = true);
		void updateRootTransform(bool updateChildren, bool checkResize);

		s32 getParentX();
		s32 getParentY();
		s32 getParentWidth();
		s32 getParentHeight();

		void updateMain(f32 dt);
		void renderMain();

	};

}