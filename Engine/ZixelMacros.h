#pragma once

#include <functional>

namespace Zixel {

	//@TODO: Separate Zixel and app stuff.
	
	#define ZIXEL_WINDOW_CLOSE_CALLBACK(funcPointer) std::bind(&funcPointer, this)
	#define ZIXEL_FILE_DROP_CALLBACK(funcPointer) std::bind(&funcPointer, this, std::placeholders::_1, std::placeholders::_2)

	#define ZIXEL_VERSION "0.0.1"
	#define ZIXEL_FILE_VERSION 0

	#define ZIXEL_DEFAULT_CANVAS_NAME "untitled"
	#define ZIXEL_FILE_MAGIC_NUMBER_B1 0xAD
	#define ZIXEL_FILE_MAGIC_NUMBER_B2 0xDE

	#define ZIXEL_MAX_UNDO_COUNT 500

	#define ZIXEL_DEFAULT_CANVAS_WIDTH 64
	#define ZIXEL_DEFAULT_CANVAS_HEIGHT 64
	#define ZIXEL_MAX_CANVAS_WIDTH 8192
	#define ZIXEL_MAX_CANVAS_HEIGHT 8192

	#define ZIXEL_CHUNK_SIZE 64

	#define ZIXEL_DEFAULT_SHOW_TILED_MODE_GRID true
	#define ZIXEL_DEFAULT_TILE_COUNT 3
	#define ZIXEL_MAX_TILE_COUNT 99

	#define ZIXEL_MAX_LAYER_COUNT 999

	#define ZIXEL_MAX_PALETTE_COUNT 99999

	#define ZIXEL_TRANSFORM_CHECKER_SIZE 4
	#define ZIXEL_TRANSFORM_CHECKER_SPEED 15.0f

	#define ZIXEL_TRANSFORM_ROTATION_SNAP 15.0f

	#define ZIXEL_TRANSFORM_SHORTCUT_MOVE_CTRL_AMOUNT 8

	#define ZIXEL_ANIM_MAX_FPS 999
	#define ZIXEL_ANIM_DEFAULT_FPS 10
	#define ZIXEL_ANIM_MAX_FRAME_COUNT 9999

	#define ZIXEL_MAX_BRUSH_SIZE 128
	
	#define ZIXEL_GUI_MIN_COLOR_WINDOW_WIDTH 169
	#define ZIXEL_GUI_MIN_LAYERS_WINDOW_WIDTH 111
	#define ZIXEL_GUI_MIN_ANIMATION_WINDOW_HEIGHT 64
	#define ZIXEL_GUI_MIN_CANVAS_WINDOW_SIZE 48

	enum class SaveState : u8 {

		Success,
		DialogCancelled,
		Error,

	};

	enum class MultiSaveBehaviour : u8 {

		ExitApp,
		CloseAll,

	};

	enum class ToolBarGroup : u8 {

		CommonBrush,
		TiledMode,
		
	};

	enum class ToolType : u8 {

		None,
		Pencil,
		Eraser,
		Picker,
		Fill,
		Select,
		SelectMove,
		SelectResize,
		SelectRotate,
		SelectRotationAnchor,

	};

	enum class LayerType : u8 {

		Group,
		Layer,
		__Count,

	};

	enum class PlaybackMode : u8 {

		Forward = 0,
		Backward,
		PingPong,
		__Count,

	};

}