#pragma once

#include <functional>
#include "Engine/Color.h"
#include "Engine/Math.h"
#include "Engine/GUI/Widget.h"

#define GUI_TEXT_EDIT_TEXT_CHANGED_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1)
#define GUI_TEXT_EDIT_FOCUS_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1)
#define GUI_TEXT_EDIT_CONFIRM_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1)
#define GUI_TEXT_EDIT_RETURN_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1)

#define GUI_TEXT_EDIT_SPR "textEdit"
#define GUI_TEXT_EDIT_FONT "robotoRegular12"
#define GUI_TEXT_EDIT_LINE_NUMBER_FONT "robotoRegular11"
#define GUI_TEXT_EDIT_CHAR_POSITION_FONT "robotoRegular12"
#define GUI_TEXT_EDIT_BACKGROUND_COL { 0.125f, 0.129f, 0.157f }
#define GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL { 0.0f, 0.0f, 0.0f, 0.0f }
#define GUI_TEXT_EDIT_CHAR_FOREGROUND_DISABLED_COL { 0.306f, 0.306f, 0.357f, 1.0f }
#define GUI_TEXT_EDIT_PLACEHOLDER_COL { 0.306f, 0.306f, 0.357f, 1.0f }
#define GUI_TEXT_EDIT_LINE_NUMBER_COL { 0.443f, 0.447f, 0.510f, 1.0f }
#define GUI_TEXT_EDIT_CHAR_POSITION_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_TEXT_EDIT_LINE_SELECTED_COL { 0.169f, 0.173f, 0.220f, 1.0f }
#define GUI_TEXT_EDIT_TEXT_SELECTED_COL { 0.227f, 0.235f, 0.298f, 1.0f }
#define GUI_TEXT_EDIT_TEXT_VER_OFFSET 0
#define GUI_TEXT_EDIT_CURSOR_COL { 1.0f, 1.0f, 1.0f, 1.0f }
#define GUI_TEXT_EDIT_LINE_NUMBERS_WIDTH 41
#define GUI_TEXT_EDIT_LINE_NUMBERS_SPACING 7
#define GUI_TEXT_EDIT_LINE_NUMBERS_VER_OFFSET 0
#define GUI_TEXT_EDIT_CHAR_POSITION_HEIGHT 21
#define GUI_TEXT_EDIT_CHAR_POSITION_SPACING 6
#define GUI_TEXT_EDIT_CHAR_POSITION_VER_OFFSET -1
#define GUI_TEXT_EDIT_LINE_HEIGHT 18
#define GUI_TEXT_EDIT_HOR_SPACING 3
#define GUI_TEXT_EDIT_VER_SPACING 5
#define GUI_TEXT_EDIT_CHAR_SPACING 0
#define GUI_TEXT_EDIT_SPRITE_SPACING 3
#define GUI_TEXT_EDIT_PLACEHOLDER_SPACING 2
#define GUI_TEXT_EDIT_TAB_SIZE 4
#define GUI_TEXT_EDIT_SELECT_NEW_LINE_SIZE 10
#define GUI_TEXT_EDIT_CUTOFF_SIZE 1
#define GUI_TEXT_EDIT_CURSOR_HEIGHT 18
#define GUI_TEXT_EDIT_CURSOR_WIDTH 1
#define GUI_TEXT_EDIT_CURSOR_BLINK_SPEED 0.53f
#define GUI_TEXT_EDIT_KEY_PRESS_START_TIME 0.5f
#define GUI_TEXT_EDIT_KEY_PRESS_TIME 0.03f
#define GUI_TEXT_EDIT_DOUBLE_CLICK_TIME 300.0f
#define GUI_TEXT_EDIT_DOUBLE_CLICK_MOVE_RANGE 5

namespace Zixel {

	struct Font;
	struct Surface;
	struct GUI;
	struct Theme;
	struct DropDownMenu;

	enum class TextAlign : u8;

	enum class TextEditSelectSide : u8 {

		Left,
		Right,

	};

	enum class TextEditCharType : u8 {

		Text,
		Sprite,
		Tab,

	};

	enum class TextEditWordType : u8 {

		Letter,
		SpaceAndTab,
		Sprite,
		Other,

	};

	enum class TextEditPlaceholderStyle : u8 {

		Empty,
		BehindText,

	};

	struct TextEditChar {

		TextEditCharType type = TextEditCharType::Text;
		Color4 colBackground;
		Color4 colForeground;
		char c = 0;
		Sprite* spr = nullptr;
		s32 sub = 0;

	};

	struct TextEdit : public Widget {

		Sprite* sprTextEdit;

		Font* fntText;
		Font* fntLineNumber;
		Font* fntCharPosition;

		bool prevEnabled = true; //For updating surface when enabled state changes.

		std::vector<std::vector<TextEditChar>> lineList;
		std::vector<s32> lineWidthList;

		std::string placeholderText = "";
		TextEditPlaceholderStyle placeholderStyle = TextEditPlaceholderStyle::Empty;

		bool deselectOnEnter = false;

		bool showLineNumbers = false;
		bool showCharPosition = false;

		bool highlightCurLine = true;

		char textMask = 0;
		std::string textFilter = "";
		bool allowSprites = false;
		bool allowPastingSprites = true;
		bool readOnly = false;

		bool numberOnly = false;
		bool numberRound = false;
		bool numberHasMinValue = false;
		bool numberHasMaxValue = false;
		f64 numberMin = 0;
		f64 numberMax = 0;
		u8 numberDecimalPlaces = 2;

		bool deselectTextOnDeselected = false;
		bool selectTextOnFocused = true;

		s32 maxLineCharCount = -1;
		s32 maxLineCount = -1;
		bool centerIfSingleLine = false;

		s32 curLine = 0;
		s32 curChar = 0;
		s32 curLinePrev = 0;
		s32 curCharPrev = 0;

		s32 cursorX = 0;
		s32 cursorXMoveTo = 0;
		s32 cursorY = 0;
		bool cursorShow = false;
		f32 cursorTimer = 0.0f;

		TextEditSelectSide selectSide = TextEditSelectSide::Left;
		s32 selectStartX = -1;
		s32 selectStartY = -1;
		s32 selectEndX = -1;
		s32 selectEndY = -1;
		Vector2i selectMousePos;
		u8 selectMouse = 0;

		u16 key = 0;
		char keyChar = 0;
		bool keyStart = false;
		bool keyPress = false;
		f32 keyTimerStart = 0.0f;
		f32 keyTimer = 0.0f;
		bool keyCtrl = false;
		bool keyShift = false;
		bool keyAlt = false;
		bool keyRAlt = false;

		s32 scrollToLine = -1;
		TextAlign scrollToLineAlign;

		s32 scrollToChar = -1;
		TextAlign scrollToCharAlign;

		s32 maxLineWidth = 0;

		bool textChanged = false;
		bool updateInsideSize = true;

		Surface* surf = nullptr;
		bool surfUpdate = false;
		s32 surfWidth = 0;
		s32 surfHeight = 0;
		s32 surfScrollX = 0;
		s32 surfScrollY = 0;

		f64 doubleClickTime = -1;
		u8 doubleClickCount = 0;
		s32 doubleClickLine = -1;
		s32 doubleClickChar = -1;
		s32 doubleClickMouseX = 0;
		s32 doubleClickMouseY = 0;
		bool doubleClickMove = false;

		DropDownMenu* menu = nullptr;

		std::function<void(TextEdit*)> onTextChanged;
		std::function<void(TextEdit*)> onTextEditFocus;
		std::function<void(TextEdit*)> onConfirm;
		std::function<void(TextEdit*)> onReturn;

		void getTextAreaSize(s32& _width, s32& _height);

		void getCharXY(s32& _x, s32& _y, s32 _linePos, s32 _charPos, bool _middle = true, bool _local = false, bool _end = false);
		void getClosestChar(s32& _linePos, s32& _charPos, s32 _x, s32 _y, bool _middle = true, bool _includeScroll = true);

		void getCursorXY(s32& _x, s32& _y, bool _local = false);
		void setCursorPos(s32 _linePos, s32 _charPos, bool _scrollToCursor = true);
		void updateCursorPos();

		s32 getLineY(s32 _linePos);
		void updateLineWidth(s32 _linePos);
		void updateMaxLineWidth();

		void scrollLineTop(s32 _linePos);
		void scrollLineBottom(s32 _linePos);
		void scrollCharLeft(s32 _charX);
		void scrollCharRight(s32 _charX);

		void checkScrollHor();
		void checkScrollVer();

		void calculateInsideSize();

		bool isLetter(char _char);
		TextEditWordType charToWordType(TextEditChar& _char);

		bool canDisplayPlaceholder();

		void updateOverrideTabKey();

		bool checkConfirm(bool _callOnConfirm = true);

		void doKeyPress(u16 _key, char _char, bool _start);

		void doSelectLeft();
		void doSelectRight();
		void doSelectUp();
		void doSelectDown();
		void doSelectPageUp();
		void doSelectPageDown();
		void doSelectHome();
		void doSelectEnd();

		void onFocused() override;
		void onResize(s32 prevWidth, s32 prevHeight) override;
		void update(f32 dt) override;
		void render() override;

		TextEdit(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);
		~TextEdit();

		void insertText(s32 _linePos, s32 _charPos, std::string& _text, Color4f _foregroundCol = GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, Color4f _backgroundCol = GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, bool _callOnTextChanged = true);
		void insertText(s32& _resultLine, s32& _resultChar, bool& _resultChanged, s32 _linePos, s32 _charPos, std::string& _text, Color4f _foregroundCol = GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, Color4f _backgroundCol = GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, bool _callOnTextChanged = true);
		void insertSprite(s32 _linePos, s32 _charPos, Sprite* _sprite, s32 _sub = 0, Color4f _backgroundCol = GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, bool _callOnTextChanged = true);
		void insertSprite(s32& _resultLine, s32& _resultChar, bool& _resultChanged, s32 _linePos, s32 _charPos, Sprite* _sprite, s32 _sub = 0, Color4f _backgroundCol = GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, bool _callOnTextChanged = true);
		bool deleteText(s32 _lineStart, s32 _charStart, s32 _lineEnd, s32 _charEnd, bool _callOnTextChanged = true);
		bool deleteLine(s32 _linePos, bool _callOnTextChanged = true);
		bool clearText(bool _callOnTextChanged = true);
		void setText(std::string& _text, Color4f _foregroundCol = GUI_TEXT_EDIT_DEFAULT_CHAR_FOREGROUND_COL, Color4f _backgroundCol = GUI_TEXT_EDIT_DEFAULT_CHAR_BACKGROUND_COL, bool _callOnTextChanged = true);
		std::string getText(s32 _lineStart, s32 _charStart, s32 _lineEnd, s32 _charEnd);
		bool copyText(s32 _lineStart, s32 _charStart, s32 _lineEnd, s32 _charEnd);
		bool copySelection();

		bool isEmpty();

		void selectAll();

		s32 getLineCount();
		s32 getLineCharCount(s32 _linePos);
		void getLineText(std::string& _text, s32 _linePos);

		void setTextMask(char _textMask);
		void setTextFilter(std::string _filter);

		void setPlaceholderText(const std::string _placeholderText);
		void setPlaceholderStyle(const TextEditPlaceholderStyle _style);

		void setNumberOnly(bool _numberOnly);
		f64 getNumber(s32 _linePos = 0, f64 _defaultValue = 0, bool _clampValueToRange = true);
		void setMinNumber(f64 _minNumber);
		void setMaxNumber(f64 _maxNumber);
		void setNumberRange(f64 _minNumber, f64 _maxNumber);

		void setReadOnly(bool _readOnly);

		bool setMaxLineCharCount(s32 _maxLineCharCount, bool _callOnTextChanged = true);
		bool setMaxLineCount(s32 _maxLineCount, bool _callOnTextChanged = true);
		void setCenterCursorIfSingleLine(bool _centerIfSingleLine);

		void setShowLineNumbers(bool _showLineNumbers);
		void setShowCharPosition(bool _showCharPosition);

		void setFont(std::string _font);

		void deselectText();
		void deselect(bool _checkConfirm = true, bool _keepFocus = false);

		void setOnTextChanged(std::function<void(TextEdit*)> _callback);
		void setOnTextEditFocus(std::function<void(TextEdit*)> _callback);
		void setOnConfirm(std::function<void(TextEdit*)> _callback);
		void setOnReturn(std::function<void(TextEdit*)> _callback);
		
	};

}