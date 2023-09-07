#pragma once

#include "Engine/GUI/GUIMacros.h"
#include "Engine/GUI/Panel.h"
#include "Engine/GUI/TreeView.h"

namespace Zixel {

	struct Theme {};

	struct PanelTheme : public Theme {

		const char* sprPanel = GUI_PANEL_SPR;

	};

	struct TreeViewTheme : public Theme {

		const char* sprTreeViewBackground = GUI_TREE_VIEW_SPR_BACKGROUND;
		const char* sprTreeViewArrow = GUI_TREE_VIEW_SPR_ARROW;
		const char* sprTreeViewDragDir = GUI_TREE_VIEW_SPR_DRAG_DIR;
		const char* font = GUI_TREE_VIEW_FONT;
		Color4f colText = GUI_TREE_VIEW_TEXT_COL;
		Color4f colLine = GUI_TREE_VIEW_LINE_COL;
		Color4f colItemSelected = GUI_TREE_VIEW_ITEM_SELECTED_COL;
		Color4f colItemDrag = GUI_TREE_VIEW_ITEM_DRAG_COL;
		s32 itemHeight = GUI_TREE_VIEW_ITEM_HEIGHT;
		s32 itemHorSpacing = GUI_TREE_VIEW_ITEM_HOR_SPACING;
		s32 itemVerSpacing = GUI_TREE_VIEW_ITEM_VER_SPACING;
		s32 arrowHorSpacing = GUI_TREE_VIEW_ARROW_HOR_SPACING;
		s32 iconHorSpacing = GUI_TREE_VIEW_ICON_HOR_SPACING;
		s32 iconVerOffset = GUI_TREE_VIEW_ICON_VER_OFFSET;
		s32 textHorSpacing = GUI_TREE_VIEW_TEXT_HOR_SPACING;
		s32 textVerSpacing = GUI_TREE_VIEW_TEXT_VER_SPACING;
		s32 lineSpacing = GUI_TREE_VIEW_LINE_SPACING;
		s32 tabWidth = GUI_TREE_VIEW_TAB_WIDTH;
		s32 cutoffTop = GUI_TREE_VIEW_CUTOFF_TOP;
		s32 cutoffLeft = GUI_TREE_VIEW_CUTOFF_LEFT;
		s32 cutoffBottom = GUI_TREE_VIEW_CUTOFF_BOTTOM;
		s32 cutoffRight = GUI_TREE_VIEW_CUTOFF_RIGHT;
		s32 dragStartDistance = GUI_TREE_VIEW_DRAG_START_DISTANCE;
		s32 dragScrollRange = GUI_TREE_VIEW_DRAG_SCROLL_RANGE;
		f32 dragScrollSpeed = GUI_TREE_VIEW_DRAG_SCROLL_SPEED;
		f64 doubleClickTime = GUI_TREE_VIEW_DOUBLE_CLICK_TIME;
		s32 editMinWidth = GUI_TREE_VIEW_EDIT_MIN_WIDTH;
		s32 editHeight = GUI_TREE_VIEW_EDIT_HEIGHT;

	};

}