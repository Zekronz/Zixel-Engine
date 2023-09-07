#pragma once

#include <functional>
#include "Engine/GUI/Widget.h"

#define GUI_COMBO_BOX_ITEM_CHANGE_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2)
#define GUI_COMBO_BOX_ITEM_SELECT_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

#define GUI_COMBO_BOX_SPR_BACKGROUND "comboBox"
#define GUI_COMBO_BOX_SPR_ARROW "comboBoxArrow"
#define GUI_COMBO_BOX_FONT "robotoRegular12"
#define GUI_COMBO_BOX_TEXT_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_COMBO_BOX_TEXT_COL_DISABLED { 0.353f, 0.353f, 0.439f, 1.0f }
#define GUI_COMBO_BOX_HEIGHT 17
#define GUI_COMBO_BOX_TEXT_SPACING 7
#define GUI_COMBO_BOX_TEXT_HOR_OFFSET 0
#define GUI_COMBO_BOX_TEXT_VER_OFFSET 0
#define GUI_COMBO_BOX_ARROW_SPACING 4
#define GUI_COMBO_BOX_ICON_HOR_SPACING 2
#define GUI_COMBO_BOX_ICON_DISABLED_ALPHA 0.29f

namespace Zixel {

	struct Font;
	struct GUI;
	struct Theme;
	struct DropDownMenu;
	struct DropDownMenuItem;

	enum class TextAlign : u8;

	struct ComboBoxItem {

		std::string name = "";
		s32 index = -1;

		s32 getIndex();

	};

	struct ComboBox : public Widget {

		Sprite* sprComboBox;
		Sprite* sprComboBoxArrow;

		Font* fntText;

		DropDownMenu* menu;
		bool menuOpened = false;
		bool menuJustClosed = false;

		u8 ind = 0;

		std::vector<ComboBoxItem*> itemList;
		ComboBoxItem* itemSelected = nullptr;

		TextAlign arrowAlignment;

		Sprite* sprIcon = nullptr;
		u8 iconSub = 0;

		bool renderText = true;
		std::string textDisplay = "";
		s32 textDisplayWidth = 0;

		bool showArrow = true;

		bool showTooltip = true;

		bool renderIdleBackground = true;
		bool renderDisabledBackground = true;

		std::function<void(ComboBox*, ComboBoxItem*)> onItemChange;
		std::function<void(ComboBox*, ComboBoxItem*, ComboBoxItem*)> onItemSelect;

		bool onShortcutInteract();

		void updateTextDisplay();

		void menuItemCallback(DropDownMenu* _menu, DropDownMenuItem* _item);
		void menuCloseCallback(DropDownMenu* _menu);

		void onResize(s32 prevWidth, s32 prevHeight) override;
		void update(f32 dt) override;
		void render() override;

		ComboBox(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);
		~ComboBox();

		ComboBoxItem* addItem(std::string _name, bool _callOnItemChange = false);

		ComboBoxItem* getItemFromIndex(s32 _index);
		void selectItemFromIndex(s32 _index, bool _callOnItemChange = true);
		void selectItem(ComboBoxItem* _item, bool _callOnItemChange = true);

		void setArrowAlignment(TextAlign _alignment);
		void setRenderText(bool _renderText);
		void setIcon(Sprite* _sprIcon, u8 _iconSub);

		void setOnItemChange(std::function<void(ComboBox*, ComboBoxItem*)> _callback);
		void setOnItemSelect(std::function<void(ComboBox*, ComboBoxItem*, ComboBoxItem*)> _callback);
	};

}