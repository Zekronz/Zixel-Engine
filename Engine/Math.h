#pragma once

#include <stdint.h>

namespace Zixel {

#define PI_F 3.14159265358979323846f
#define PI_D 3.14159265358979323846

	struct Vector2u16 {

		u16 x = 0, y = 0;
		bool operator == (Vector2u16 _v);
		bool operator != (Vector2u16 _v);

	};

	struct Vector2i {

		s32 x = 0, y = 0;
		bool operator == (Vector2i _v);
		bool operator != (Vector2i _v);

	};

	struct Vector3i {

		s32 x = 0, y = 0, z = 0;
		bool operator == (Vector3i _v);
		bool operator != (Vector3i _v);

	};

	struct Vector4i {

		s32 x = 0, y = 0, z = 0, w = 0;
		bool operator == (Vector4i _v);
		bool operator != (Vector4i _v);

	};

	struct Vector2f {
		f32 x = 0.0f, y = 0.0f;
	};

	struct Vector3f {
		f32 x = 0.0f, y = 0.0f, z = 0.0f;
	};

	struct Vector4f {
		f32 x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
	};

	struct Rect {

		s32 x = 0, y = 0, width = 0, height = 0;

		bool operator == (Rect _rect);
		bool contains(Rect _rect);

	};

	struct Math {

		static bool compare(f32 _val1, f32 _val2, f32 _epsilon = 0.0001f);
		static bool compare(Vector2f _v1, Vector2f _v2, f32 _epsilon = 0.0001f);

		static s32 maxInt(s32 a, s32 b);
		static s32 maxInt(s32 a, s32 b, s32 c);
		static s32 maxInt(s32 a, s32 b, s32 c, s32 d);

		static s32 minInt(s32 a, s32 b);
		static s32 minInt(s32 a, s32 b, s32 c);
		static s32 minInt(s32 a, s32 b, s32 c, s32 d);

		static f32 maxFloat(f32 a, f32 b);
		static f32 maxFloat(f32 a, f32 b, f32 c);
		static f32 maxFloat(f32 a, f32 b, f32 c, f32 d);

		static f32 minFloat(f32 a, f32 b);
		static f32 minFloat(f32 a, f32 b, f32 c);
		static f32 minFloat(f32 a, f32 b, f32 c, f32 d);

		static s32 clampInt(s32 value, s32 minVal, s32 maxVal);
		static f32 clampFloat(f32 value, f32 minVal, f32 maxVal);
		static f64 clampDouble(f64 value, f64 minVal, f64 maxVal);

		static s32 absInt(s32 value);
		static f32 absFloat(f32 value);

		static void swapInt(s32& _value1, s32& _value2);
		static void swapFloat(f32& _value1, f32& _value2);
		static void swapDouble(f64& _value1, f64& _value2);

		static f32 warpFloat(f32 _value, f32 _minValue, f32 _maxValue);
		static f64 warpDouble(f64 _value, f64 _minValue, f64 _maxValue);

		static bool pointInRect(s32 _pX, s32 _pY, s32 _rX, s32 _rY, s32 _rW, s32 _rH);
		static bool rectInRect(Rect _rect1, Rect _rect2);
		static bool subtractRect(Rect _rect1, Rect _rect2, std::vector<Rect>& _result);
		static bool subtractRects(std::vector<Rect>& _rects, std::vector<Rect>& _result);

		static bool pointInTriangle(s32 _pX, s32 _pY, s32 _x1, s32 _y1, s32 _x2, s32 _y2, s32 _x3, s32 _y3);

		static s32 distance2DInt(s32 x1, s32 y1, s32 x2, s32 y2);
		static f32 distance2DFloat(s32 x1, s32 y1, s32 x2, s32 y2);
		static f32 distance2DFloat(f32 x1, f32 y1, f32 x2, f32 y2);

		static f32 direction2DFloat(s32 x1, s32 y1, s32 x2, s32 y2);
		static f32 direction2DFloat(f32 x1, f32 y1, f32 x2, f32 y2);
		static f64 direction2DDouble(s32 x1, s32 y1, s32 x2, s32 y2);

		static Vector2f lineIntersectionPoint(f32 _x1, f32 _y1, f32 _angle1, f32 _x2, f32 _y2, f32 _angle2);

		static f32 lengthDirXFloat(f32 _length, f32 _angle);
		static f32 lengthDirYFloat(f32 _length, f32 _angle);

	};

}