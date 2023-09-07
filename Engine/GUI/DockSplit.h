#pragma once

namespace Zixel {

	enum class DockSplitSide : u8 {
		Left,
		Right,
		Up,
		Down,
		None,
	};

	struct DockArea;
	struct DockContainer;

	struct DockSplit {

		s32 dir = 0;

		f32 x = 0.0f;
		f32 y = 0.0f;
		s32 xRound = 0;
		s32 yRound = 0;

		f32 posStart = 0;

		f32 length = 0.0f;
		f32 lengthStart = 0.0f;
		s32 lengthRound = 0;

		std::vector<DockSplit*> splits1;
		std::vector<DockSplit*> splits2;

		std::vector<DockContainer*> containers1;
		std::vector<DockContainer*> containers2;

	};

}