#include "Engine/ZixelPCH.h"
#include "Engine/Math.h"

namespace Zixel {

	bool Vector2u16::operator == (Vector2u16 _v) {
		return (x == _v.x && y == _v.y);
	}

	bool Vector2u16::operator != (Vector2u16 _v) {
		return (x != _v.x || y != _v.y);
	}

	bool Vector2i::operator == (Vector2i _v) {
		return (x == _v.x && y == _v.y);
	}

	bool Vector2i::operator != (Vector2i _v) {
		return (x != _v.x || y != _v.y);
	}

	bool Vector3i::operator == (Vector3i _v) {
		return (x == _v.x && y == _v.y && z == _v.z);
	}

	bool Vector3i::operator != (Vector3i _v) {
		return (x != _v.x || y != _v.y || z != _v.z);
	}

	bool Vector4i::operator == (Vector4i _v) {
		return (x == _v.x && y == _v.y && z == _v.z && w == _v.w);
	}

	bool Vector4i::operator != (Vector4i _v) {
		return (x != _v.x || y != _v.y || z != _v.z || w != _v.w);
	}

	bool Rect::operator == (Rect _rect) {
		return (x == _rect.x && y == _rect.y && width == _rect.width &&  height == _rect.height);
	}

	bool Rect::contains(Rect _rect) {

		if (*this == _rect) return true;
		return (_rect.x >= x && _rect.y >= y && _rect.x + _rect.width - 1 <= x + width - 1 && _rect.y + _rect.height - 1 <= y + height - 1);

	}

	bool Math::compare(f32 _val1, f32 _val2, f32 _epsilon) {
		return (fabsf(_val1 - _val2) <= _epsilon);
	}

	bool Math::compare(Vector2f _v1, Vector2f _v2, f32 _epsilon) {
		return (fabsf(_v1.x - _v2.x) <= _epsilon && fabsf(_v1.y - _v2.y) <= _epsilon);
	}

	s32 Math::maxInt(s32 a, s32 b) {
		if (a > b) return a;
		return b;
	}

	s32 Math::maxInt(s32 a, s32 b, s32 c) {
		return maxInt(maxInt(a, b), c);
	}

	s32 Math::maxInt(s32 a, s32 b, s32 c, s32 d) {
		return maxInt(maxInt(a, b, c), d);
	}

	s32 Math::minInt(s32 a, s32 b) {
		if (a < b) return a;
		return b;
	}

	s32 Math::minInt(s32 a, s32 b, s32 c) {
		return minInt(minInt(a, b), c);
	}

	s32 Math::minInt(s32 a, s32 b, s32 c, s32 d) {
		return minInt(minInt(a, b, c), d);
	}

	f32 Math::maxFloat(f32 a, f32 b) {
		if (a > b) return a;
		return b;
	}

	f32 Math::maxFloat(f32 a, f32 b, f32 c) {
		return maxFloat(maxFloat(a, b), c);
	}

	f32 Math::maxFloat(f32 a, f32 b, f32 c, f32 d) {
		return maxFloat(maxFloat(a, b, c), d);
	}

	f32 Math::minFloat(f32 a, f32 b) {
		if (a < b) return a;
		return b;
	}

	f32 Math::minFloat(f32 a, f32 b, f32 c) {
		return minFloat(minFloat(a, b), c);
	}

	f32 Math::minFloat(f32 a, f32 b, f32 c, f32 d) {
		return minFloat(minFloat(a, b, c), d);
	}

	s32 Math::clampInt(s32 value, s32 minVal, s32 maxVal) {
		if (value < minVal) return minVal;
		if (value > maxVal) return maxVal;
		return value;
	}

	f32 Math::clampFloat(f32 value, f32 minVal, f32 maxVal) {
		if (value < minVal) return minVal;
		if (value > maxVal) return maxVal;
		return value;
	}

	f64 Math::clampDouble(f64 value, f64 minVal, f64 maxVal) {
		if (value < minVal) return minVal;
		if (value > maxVal) return maxVal;
		return value;
	}

	s32 Math::absInt(s32 value) {
		return abs(value);
	}

	f32 Math::absFloat(f32 value) {
		return abs(value);
	}

	void Math::swapInt(s32& _value1, s32& _value2) {

		s32 temp = _value1;
		_value1 = _value2;
		_value2 = temp;

	}

	void Math::swapFloat(f32& _value1, f32& _value2) {

		f32 temp = _value1;
		_value1 = _value2;
		_value2 = temp;

	}

	void Math::swapDouble(f64& _value1, f64& _value2) {

		f64 temp = _value1;
		_value1 = _value2;
		_value2 = temp;

	}

	f32 Math::warpFloat(f32 _value, f32 _minValue, f32 _maxValue) {
		return fmodf((fmodf((_value - _minValue), (_maxValue - _minValue)) + (_maxValue - _minValue)), (_maxValue - _minValue)) + _minValue;
	}

	f64 Math::warpDouble(f64 _value, f64 _minValue, f64 _maxValue) {
		return fmod((fmod((_value - _minValue), (_maxValue - _minValue)) + (_maxValue - _minValue)), (_maxValue - _minValue)) + _minValue;
	}

	bool Math::pointInRect(s32 _pX, s32 _pY, s32 _rX, s32 _rY, s32 _rW, s32 _rH) {
		return (_pX >= _rX && _pY >= _rY && _pX < _rX + _rW && _pY < _rY + _rH);
	}

	bool Math::rectInRect(Rect _rect1, Rect _rect2) {
		return (_rect1.x + _rect1.width - 1 >= _rect2.x && _rect1.y + _rect1.height - 1 >= _rect2.y) && (_rect1.x  <= _rect2.x + _rect2.width - 1 && _rect1.y <= _rect2.y + _rect2.height - 1);
	}

	bool Math::subtractRect(Rect _rect1, Rect _rect2, std::vector<Rect>& _result) {

		_result.clear();
		if (!rectInRect(_rect1, _rect2)) return false;

		if (_rect1 == _rect2) {

			_result.push_back(_rect1);
			return true;

		}

		Rect baseRect = _rect2.contains(_rect1) ? _rect2 : _rect1;
		Rect otherRect = (baseRect == _rect1) ? _rect2 : _rect1;

		_result.push_back(otherRect);

		s32 leftWidth = otherRect.x - baseRect.x;
		if (leftWidth > 0) _result.push_back({ baseRect.x, baseRect.y, leftWidth, baseRect.height });

		s32 rightWidth = (baseRect.x + baseRect.width) - (otherRect.x + otherRect.width);
		if (rightWidth > 0) _result.push_back({ otherRect.x + otherRect.width, baseRect.y, rightWidth, baseRect.height });

		s32 horX = baseRect.x + Math::maxInt(0, leftWidth);
		s32 horWidth = baseRect.width - Math::maxInt(0, leftWidth) - Math::maxInt(0, rightWidth);

		s32 topHeight = otherRect.y - baseRect.y;
		if (topHeight > 0) _result.push_back({ horX, baseRect.y, horWidth, topHeight });

		s32 bottomHeight = (baseRect.y + baseRect.height) - (otherRect.y + otherRect.height);
		if (bottomHeight > 0) _result.push_back({ horX, otherRect.y + otherRect.height, horWidth, bottomHeight });

		return true;

	}

	bool Math::subtractRects(std::vector<Rect>& _rects, std::vector<Rect>& _result) {

		if (_rects.size() <= 1) return false;

		_result = _rects;
		std::vector<Rect> tempResult;

		bool intersected = false;

		for (size_t i = 0; i < _result.size(); ++i) {

			for (size_t j = i + 1; j < _result.size(); ++j) {

				if (!subtractRect(_result[i], _result[j], tempResult)) continue;
				for (const Rect& r : tempResult) _result.push_back(r);

				intersected = true;

				_result.erase(_result.begin() + j);
				_result.erase(_result.begin() + i);

				i = -1;
				break;

			}

		}

		return intersected;

	}

	bool Math::pointInTriangle(s32 _pX, s32 _pY, s32 _x1, s32 _y1, s32 _x2, s32 _y2, s32 _x3, s32 _y3) {

		s32 d1, d2, d3;
		bool hasNeg, hasPos;

		d1 = (_pX - _x2) * (_y1 - _y2) - (_x1 - _x2) * (_pY - _y2);
		d2 = (_pX - _x3) * (_y2 - _y3) - (_x2 - _x3) * (_pY - _y3);
		d3 = (_pX - _x1) * (_y3 - _y1) - (_x3 - _x1) * (_pY - _y1);

		hasNeg = (d1 < 0 || d2 < 0 || d3 < 0);
		hasPos = (d1 > 0 || d2 > 0 || d3 > 0);

		return !(hasNeg && hasPos);

	}

	s32 Math::distance2DInt(s32 x1, s32 y1, s32 x2, s32 y2) {

		f32 dX = (f32)x1 - (f32)x2;
		f32 dY = (f32)y1 - (f32)y2;

		return (s32)roundf(sqrtf((dX * dX) + (dY * dY)));
	}

	f32 Math::distance2DFloat(s32 x1, s32 y1, s32 x2, s32 y2) {

		f32 dX = (f32)x1 - (f32)x2;
		f32 dY = (f32)y1 - (f32)y2;

		return sqrtf((dX * dX) + (dY * dY));
	}

	f32 Math::distance2DFloat(f32 x1, f32 y1, f32 x2, f32 y2) {

		f32 dX = x1 - x2;
		f32 dY = y1 - y2;

		return sqrtf((dX * dX) + (dY * dY));
	}

	f32 Math::direction2DFloat(s32 x1, s32 y1, s32 x2, s32 y2) {
		return warpFloat((atan2f((f32)y1 - (f32)y2, (f32)x1 - (f32)x2) * (180.0f / PI_F)) + 180.0f, 0.0f, 360.0f);
	}

	f32 Math::direction2DFloat(f32 x1, f32 y1, f32 x2, f32 y2) {
		return warpFloat((atan2f(y1 - y2, x1 - x2) * (180.0f / PI_F)) + 180.0f, 0.0f, 360.0f);
	}

	f64 Math::direction2DDouble(s32 x1, s32 y1, s32 x2, s32 y2) {
		return warpDouble((atan2((f64)y1 - (f64)y2, (f64)x1 - (f64)x2) * (180.0 / PI_D)) + 180.0, 0.0, 360.0);
	}

	Vector2f Math::lineIntersectionPoint(f32 _x1, f32 _y1, f32 _angle1, f32 _x2, f32 _y2, f32 _angle2) {

		Vector2f o1 = { _x1 + cosf(_angle1 * (PI_F / 180.0f)), _y1 + sinf(_angle1 * (PI_F / 180.0f)) };
		Vector2f o2 = { _x2 + cosf(_angle2 * (PI_F / 180.0f)), _y2 + sinf(_angle2 * (PI_F / 180.0f)) };

		Vector2f u = { o1.x - _x1, o1.y - _y1 };
		Vector2f v = { o2.x - _x2, o2.y - _y2 };
		Vector2f w = { _x1 - _x2, _y1 - _y2 };

		f32 uD = (v.y * u.x) - (v.x * u.y);
		f32 uA = (uD != 0.0f) ? ((v.x * w.y) - (v.y * w.x)) / uD : 0.0f;

		return { _x1 + (uA * (o1.x - _x1)), _y1 + (uA * (o1.y - _y1)) };

	}

	f32 Math::lengthDirXFloat(f32 _length, f32 _angle) {
		return cosf(_angle * (PI_F / 180.0f)) * _length;
	}

	f32 Math::lengthDirYFloat(f32 _length, f32 _angle) {
		return sinf(_angle * (PI_F / 180.0f)) * _length;
	}

}