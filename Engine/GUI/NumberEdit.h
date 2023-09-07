#pragma once

#include "Engine/GUI/TextEdit.h"

namespace Zixel {

	struct NumberEdit : public TextEdit {

		NumberEdit(GUI* _gui, Widget* _parent, Window* _window, Theme* _theme = nullptr);

	};

}