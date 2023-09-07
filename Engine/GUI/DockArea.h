/*
    DockArea.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include "Engine/GUI/Widget.h"
#include "Engine/GUI/DockSplit.h"

#define GUI_DOCK_GRAB_RANGE 16
#define GUI_DOCK_SIZE_DIVIDER 4
#define GUI_DOCK_TOOLTIP_CLOSE "Close"

namespace Zixel {

	struct GUI;
	struct Theme;
	struct Window;
	struct DockContainer;
	struct DockTab;

	struct DockArea : public Widget {
		
		Sprite* sprDockArea;
		Sprite* sprDockSplitVer;
		Sprite* sprDockSplitHor;
		Sprite* sprDockContainer;
		Sprite* sprDockTab;
		Sprite* sprDockTabClose;
		Sprite* sprDockTabLine;
		Sprite* sprDockPreview;

		Font* fntDockTab;

		std::vector<DockContainer*> containerList;

		std::vector<DockSplit*> splitList;
		DockSplit* splitHovered = nullptr;
		f32 splitPosStart = 0;
		s32 splitMouseStart = 0;

		DockTab* tabHovered = nullptr;
		DockTab* tabMouseOver = nullptr;
		DockTab* tabClose = nullptr;
		DockTab* tabMove = nullptr;
		s32 tabMoveX = 0;
		s32 tabMoveXStart = 0;
		s32 tabMoveMouseXStart = 0;
		s32 tabMoveMouseYStart = 0;

		bool isDocking = false;
		DockContainer* dockToContainer = nullptr;
		bool dockToContainerEmpty = false;
		DockSplitSide dockToSide = DockSplitSide::None;

		bool destroyEmptyContainerWhenUndocking = false;

		bool updateResizeSize = true;
		s32 resizeWidth;
		s32 resizeHeight;

		bool showTooltip = true;
		DockTab* tooltipTab = nullptr;

		std::string tooltipClose = GUI_DOCK_TOOLTIP_CLOSE;

		void onResize(s32 prevWidth, s32 prevHeight) override;
		void update(f32 dt) override;
		void render() override;

		DockArea(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);
		~DockArea();

		DockContainer* addContainer(DockContainer* _parent = nullptr, DockSplitSide _splitSide = DockSplitSide::Left, f32 _width = -1.0f, f32 _height = -1.0f, bool _sizeIsPercentage = true);
		void destroyContainer(DockContainer* _container, bool _redock = true);

		void updateAllContainerSizes();
		void realignSplits();
		void fixHorizontal(bool _realignAndUpdateContainerSizes);
		void fixVertical(bool _realignAndUpdateContainerSizes);

		void getLeftMostContainers(std::vector<DockContainer*>& _containers);
		void getUpMostContainers(std::vector<DockContainer*>& _containers);
		void getRightMostContainers(std::vector<DockContainer*>& _containers);
		void getDownMostContainers(std::vector<DockContainer*>& _containers);

		void pushSplitLeft(DockSplit* _split);
		void pushSplitRight(DockSplit* _split);
		void pushSplitUp(DockSplit* _split);
		void pushSplitDown(DockSplit* _split);

		void clampSplitFromLeft(DockSplit* _split);
		void clampSplitFromRight(DockSplit* _split);
		void clampSplitFromUp(DockSplit* _split);
		void clampSplitFromDown(DockSplit* _split);

		s32 getMinAreaWidthContainer(DockContainer* _container, s32 _width);
		s32 getMinAreaHeightContainer(DockContainer* _container, s32 _height);
		s32 getMinAreaWidth();
		s32 getMinAreaHeight();
		s32 getMinAreaWidthFromLeftToRight(DockSplit* _split, s32 _width);
		s32 getMinAreaHeightFromUpToDown(DockSplit* _split, s32 _height);
		s32 getMinAreaWidthFromRightToLeft(DockSplit* _split, s32 _width);
		s32 getMinAreaHeightFromDownToUp(DockSplit* _split, s32 _height);

		void handleLeftFixedSize(DockContainer* _container);
		void handleUpFixedSize(DockContainer* _container);
		void handleRightFixedSize(DockContainer* _container);
		void handleDownFixedSize(DockContainer* _container);

		bool shouldHandleLeftFixedSize(DockContainer* _container);
		bool shouldHandleUpFixedSize(DockContainer* _container);
		bool shouldHandleRightFixedSize(DockContainer* _container);
		bool shouldHandleDownFixedSize(DockContainer* _container);

		void checkDockTo();
		void checkTabOrder();

	};

}