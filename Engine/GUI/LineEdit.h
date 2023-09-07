#pragma once

#include "Engine/GUI/TextEdit.h"

namespace Zixel {

	struct LineEdit : public TextEdit {

		LineEdit(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);

	};

}