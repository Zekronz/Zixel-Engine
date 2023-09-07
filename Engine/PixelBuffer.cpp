#include "Engine/ZixelPCH.h"
#include "Engine/PixelBuffer.h"
#include "Engine/Math.h"
#include "Engine/MaskBuffer.h"

namespace Zixel {

	PixelBuffer::PixelBuffer(s32 _width, s32 _height, Color4 _fillColor, bool _useBBox, bool _makeEmptyPixelsBlack) {

		if (_width < 1 || _height < 1) {

			ZIXEL_WARN("Error in PixelBuffer::PixelBuffer. Size cannot be less than 1: {}x{}", _width, _height);
			return;

		}

		if (_width > 8192) {
			ZIXEL_WARN("Warning in PixelBuffer::PixelBuffer. Width exceeds 8192 pixels: {}", _width);
		}

		if (_height > 8192) {
			ZIXEL_WARN("Warning in PixelBuffer::PixelBuffer. Height exceeds 8192 pixels: {}", _height);
		}

		width = _width;
		height = _height;

		useBBox = _useBBox;
		makeEmptyPixelsBlack = _makeEmptyPixelsBlack;

		s32 size = (_width * _height * 4); //Red, green, blue, alpha.
		buffer = new u8[size]();

		if (buffer == nullptr) {
			ZIXEL_CRITICAL("Unable to allocate memory for PixelBuffer.");
			return;
		}

		if (_fillColor.r != 0 || _fillColor.g != 0 || _fillColor.b != 0 || _fillColor.a != 0) {

			if (!makeEmptyPixelsBlack || _fillColor.a > 0) {

				for (s32 i = 0; i < size; i += 4) {

					buffer[i] = _fillColor.r;
					buffer[i + 1] = _fillColor.g;
					buffer[i + 2] = _fillColor.b;
					buffer[i + 3] = _fillColor.a;

				}

			}

			if (_fillColor.a != 0) {

				pixelCount = (width * height);
				if (useBBox) __setBBox(0, 0, width - 1, height - 1);

			}

		}

	}

	PixelBuffer::~PixelBuffer() {

		if (buffer != nullptr) {
			delete[] buffer;
		}

	}

	void PixelBuffer::__setBBox(s32 _left, s32 _top, s32 _right, s32 _bottom) {

		bBoxLeft = _left;
		bBoxTop = _top;
		bBoxRight = _right;
		bBoxBottom = _bottom;

	}

	void PixelBuffer::checkBBoxIncrease(s32 _x, s32 _y) {

		if (bBoxLeft == -1) {

			bBoxLeft = _x;
			bBoxRight = _x;
			bBoxTop = _y;
			bBoxBottom = _y;

			return;

		}

		if (_x < bBoxLeft) bBoxLeft = _x; else if (_x > bBoxRight) bBoxRight = _x;
		if (_y < bBoxTop) bBoxTop = _y; else if (_y > bBoxBottom) bBoxBottom = _y;

	}

	void PixelBuffer::calculateBBox(bool _startFromCurrentBBox) {

		if (isEmpty()) {
			__setBBox(-1, -1, -1, -1);
		}

		bool startFromBBox = (_startFromCurrentBBox && bBoxLeft != -1);

		s32 startX = startFromBBox ? bBoxLeft : 0;
		s32 startY = startFromBBox ? bBoxTop : 0;
		s32 endX = startFromBBox ? bBoxRight : width - 1;
		s32 endY = startFromBBox ? bBoxBottom : height - 1;

		s32 potentialTop = -1, potentialBottom = -1;
		__setBBox(-1, -1, -1, -1);

		//Find bbox left and potential bbox top.
		bool shouldBreak = false;
		for (s32 x = startX; x <= endX; ++x) {

			for (s32 y = startY; y <= endY; ++y) {

				if (readAlpha(x, y) == 0) continue;

				bBoxLeft = x;
				potentialTop = y;

				shouldBreak = true;
				break;

			}

			if (shouldBreak) break;

		}

		if (bBoxLeft == -1) return;
		else if (pixelCount == 1) {

			bBoxRight = bBoxLeft;
			bBoxTop = potentialTop;
			bBoxBottom = potentialTop;
			
			return;

		}

		//Find bbox right and potential bbox bottom.
		shouldBreak = false;
		for (s32 x = endX; x >= startX; --x) {

			for (s32 y = endY; y >= startY; --y) {

				if (readAlpha(x, y) == 0) continue;

				bBoxRight = x;
				potentialBottom = y;

				shouldBreak = true;
				break;

			}

			if (shouldBreak) break;

		}

		for (s32 x = bBoxLeft; x <= bBoxRight; ++x) {

			//Find actual bbox top.
			for (s32 y = startY; y < potentialTop; ++y) {

				if (readAlpha(x, y) > 0 && (bBoxTop == -1 || y < bBoxTop)) {
					bBoxTop = y;
				}

			}

			if (bBoxTop == -1) bBoxTop = potentialTop;

			//Find actual bbox bottom.
			for (s32 y = endY; y > potentialBottom; --y) {

				if (readAlpha(x, y) > 0 && (bBoxBottom == -1 || y > bBoxBottom)) {
					bBoxBottom = y;
				}

			}

			if (bBoxBottom == -1) bBoxBottom = potentialBottom;

		}

	}

	bool PixelBuffer::isEmpty() {
		return (pixelCount == 0);
	}

	void PixelBuffer::fill(Color4 _color) {

		if (makeEmptyPixelsBlack && _color.a == 0) _color = { 0, 0, 0, 0 };

		s32 size = (width * height * 4);
		for (s32 i = 0; i < size; i += 4) {

			buffer[i] = _color.r;
			buffer[i + 1] = _color.g;
			buffer[i + 2] = _color.b;
			buffer[i + 3] = _color.a;

		}

		if (_color.a != 0) {

			pixelCount = (width * height);
			if (useBBox) __setBBox(0, 0, width - 1, height - 1);

		}
		else {

			pixelCount = 0;
			if (useBBox) __setBBox(-1, -1, -1, -1);

		}

	}

	bool PixelBuffer::fillCheckModified(Color4 _color) {

		bool modified = false;

		if (makeEmptyPixelsBlack && _color.a == 0) _color = { 0, 0, 0, 0 };

		s32 size = (width * height * 4);
		for (s32 i = 0; i < size; i += 4) {

			if (!Color::match(_color, { buffer[i], buffer[i + 1], buffer[i + 2], buffer[i + 3] })) {

				buffer[i] = _color.r;
				buffer[i + 1] = _color.g;
				buffer[i + 2] = _color.b;
				buffer[i + 3] = _color.a;

				modified = true;

			}

		}

		if (_color.a != 0) {

			pixelCount = (width * height);
			if (useBBox) __setBBox(0, 0, width - 1, height - 1);

		}
		else {

			pixelCount = 0;
			if (useBBox) __setBBox(-1, -1, -1, -1);

		}

		return modified;

	}

	void PixelBuffer::writePixel(s32 _x, s32 _y, Color4 _color, BlendMode _blendMode, bool _calculateBBox) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in PixelBuffer::writePixel. Write position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return;

		}

		if (_blendMode != BlendMode::Overwrite) _color = Color::blendColor(_color, readPixel(_x, _y), _blendMode);
		if (makeEmptyPixelsBlack && _color.a == 0) _color = { 0, 0, 0, 0 };

		s32 ind = ((_y * width) + _x) * 4;
		buffer[ind] = _color.r;
		buffer[ind + 1] = _color.g;
		buffer[ind + 2] = _color.b;

		u8 prevAlpha = buffer[ind + 3];
		buffer[ind + 3] = _color.a;

		if (_color.a == 0 && prevAlpha != 0) {

			--pixelCount;
			if (useBBox && _calculateBBox) calculateBBox(true);

		}
		else if (_color.a != 0 && prevAlpha == 0) {

			++pixelCount;
			if (useBBox && _calculateBBox) checkBBoxIncrease(_x, _y);

		}

	}

	bool PixelBuffer::writePixelCheckModified(s32 _x, s32 _y, Color4 _color, BlendMode _blendMode, bool _calculateBBox) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in PixelBuffer::writePixelCheckModified. Write position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return false;

		}

		if (_blendMode != BlendMode::Overwrite) _color = Color::blendColor(_color, readPixel(_x, _y), _blendMode);
		if (makeEmptyPixelsBlack && _color.a == 0) _color = { 0, 0, 0, 0 };

		s32 ind = ((_y * width) + _x) * 4;
		u8 prevAlpha = buffer[ind + 3];

		if (!Color::match(_color, { buffer[ind], buffer[ind + 1], buffer[ind + 2], prevAlpha })) {

			buffer[ind] = _color.r;
			buffer[ind + 1] = _color.g;
			buffer[ind + 2] = _color.b;
			buffer[ind + 3] = _color.a;

			if (_color.a == 0 && prevAlpha != 0) {

				--pixelCount;
				if (useBBox && _calculateBBox) calculateBBox(true);

			}
			else if (_color.a != 0 && prevAlpha == 0) {

				++pixelCount;
				if (useBBox && _calculateBBox) checkBBoxIncrease(_x, _y);

			}

			return true;

		}

		return false;

	}

	void PixelBuffer::writeLine(s32 _x1, s32 _y1, s32 _x2, s32 _y2, Color4 _color, BlendMode _blendMode, bool _writeFirstPixel, bool _calculateBBox) {

		s32 deltaX = Math::absInt(_x2 - _x1);
		s32 deltaY = Math::absInt(_y2 - _y1);

		s32 signedX = (_x1 < _x2) ? 1 : -1;
		s32 signedY = (_y1 < _y2) ? 1 : -1;

		s32 err = deltaX - deltaY;

		bool hasAlpha = (_color.a > 0);

		if (_writeFirstPixel) writePixel(_x1, _y1, _color, _blendMode, hasAlpha && _calculateBBox); //This could break if we add some sort of blend mode that somehow changes the alpha.

		while (_x1 != _x2 || _y1 != _y2) {

			s32 err2 = err << 1;

			if (err2 > -deltaY) {

				err -= deltaY;
				_x1 += signedX;

			}

			if (err2 < deltaX) {

				err += deltaX;
				_y1 += signedY;

			}

			if (_x1 < 0 || _y1 < 0 || _x1 >= width || _y1 >= height) continue;

			writePixel(_x1, _y1, _color, _blendMode, hasAlpha && _calculateBBox);

		}

		if (useBBox && _calculateBBox && !hasAlpha) {
			calculateBBox(true);
		}

	}

	bool PixelBuffer::writeLineCheckModified(s32 _x1, s32 _y1, s32 _x2, s32 _y2, Color4 _color, BlendMode _blendMode, bool _writeFirstPixel, bool _calculateBBox) {

		s32 deltaX = Math::absInt(_x2 - _x1);
		s32 deltaY = Math::absInt(_y2 - _y1);

		s32 signedX = (_x1 < _x2) ? 1 : -1;
		s32 signedY = (_y1 < _y2) ? 1 : -1;

		s32 err = deltaX - deltaY;

		bool hasAlpha = (_color.a > 0);

		bool modified = _writeFirstPixel ? writePixelCheckModified(_x1, _y1, _color, _blendMode, hasAlpha && _calculateBBox) : false; //This could break if we add some sort of blend mode that somehow changes the alpha.

		while (_x1 != _x2 || _y1 != _y2) {

			s32 err2 = err << 1;

			if (err2 > -deltaY) {

				err -= deltaY;
				_x1 += signedX;

			}

			if (err2 < deltaX) {

				err += deltaX;
				_y1 += signedY;

			}

			if (_x1 < 0 || _y1 < 0 || _x1 >= width || _y1 >= height) continue;

			if (writePixelCheckModified(_x1, _y1, _color, _blendMode, hasAlpha && _calculateBBox)) {
				modified = true;
			}

		}

		if (modified) {

			if (useBBox && _calculateBBox && !hasAlpha) {
				calculateBBox(true);
			}

		}

		return modified;

	}

	void PixelBuffer::writeRect(s32 _x, s32 _y, s32 _width, s32 _height, Color4 _color, BlendMode _blendMode, bool _calculateBBox) {

		if (_x >= width || _y >= height || _x + _width - 1 < 0 || _y + _height - 1 < 0) {
			return;
		}

		bool calcBBox = false;

		for (s32 pixelX = Math::maxInt(_x, 0); pixelX < Math::minInt(_x + _width, width); ++pixelX) {
			for (s32 pixelY = Math::maxInt(_y, 0); pixelY < Math::minInt(_y + _height, height); ++pixelY) {

				Color4 dest = readPixel(pixelX, pixelY);
				Color4 blended = Color::blendColor(_color, dest, _blendMode);

				writePixel(pixelX, pixelY, blended, BlendMode::Overwrite, false);

				if (blended.a == 0 && dest.a > 0) calcBBox = true;
				else if (blended.a > 0 && dest.a == 0 && _calculateBBox) checkBBoxIncrease(pixelX, pixelY);

			}
		}

		if (calcBBox && _calculateBBox) calculateBBox(true);

	}

	void PixelBuffer::writeRed(s32 _x, s32 _y, u8 _red) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in PixelBuffer::writeRed. Write position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return;

		}

		s32 ind = ((_y * width) + _x) * 4;

		if (makeEmptyPixelsBlack && buffer[ind + 3] == 0) _red = 0;
		buffer[ind] = _red;

	}

	void PixelBuffer::writeGreen(s32 _x, s32 _y, u8 _green) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in PixelBuffer::writeGreen. Write position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return;

		}

		s32 ind = ((_y * width) + _x) * 4;

		if (makeEmptyPixelsBlack && buffer[ind + 3] == 0) _green = 0;
		buffer[ind + 1] = _green;

	}

	void PixelBuffer::writeBlue(s32 _x, s32 _y, u8 _blue) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in PixelBuffer::writeBlue. Write position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return;

		}

		s32 ind = ((_y * width) + _x) * 4;

		if (makeEmptyPixelsBlack && buffer[ind + 3] == 0) _blue = 0;
		buffer[ind + 2] = _blue;

	}

	void PixelBuffer::writeAlpha(s32 _x, s32 _y, u8 _alpha, bool _calculateBBox) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in PixelBuffer::writeAlpha. Write position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return;

		}

		if (makeEmptyPixelsBlack && _alpha == 0) {

			writePixel(_x, _y, { 0, 0, 0, 0 }, BlendMode::Overwrite, _calculateBBox);
			return;

		}

		s32 ind = ((_y * width) + _x) * 4;

		u8 prevAlpha = buffer[ind + 3];
		buffer[ind + 3] = _alpha;

		if (_alpha == 0 && prevAlpha != 0) {

			--pixelCount;
			if (useBBox && _calculateBBox) calculateBBox(true);

		}
		else if (_alpha != 0 && prevAlpha == 0) {

			++pixelCount;
			if (useBBox && _calculateBBox) checkBBoxIncrease(_x, _y);

		}

	}

	bool PixelBuffer::writeAlphaCheckModified(s32 _x, s32 _y, u8 _alpha, bool _calculateBBox) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in PixelBuffer::writeAlphaCheckModified. Write position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return false;

		}

		s32 ind = ((_y * width) + _x) * 4;
		u8 prevAlpha = buffer[ind + 3];

		if (prevAlpha == _alpha) return false;

		if (makeEmptyPixelsBlack && _alpha == 0) {

			writePixelCheckModified(_x, _y, { 0, 0, 0, 0 }, BlendMode::Overwrite, _calculateBBox);
			return true;

		}

		buffer[ind + 3] = _alpha;

		if (_alpha == 0 && prevAlpha != 0) {

			--pixelCount;
			if (useBBox && _calculateBBox) calculateBBox(true);

		}
		else if (_alpha != 0 && prevAlpha == 0) {

			++pixelCount;
			if (useBBox && _calculateBBox) checkBBoxIncrease(_x, _y);

		}

		return true;

	}

	Color4 PixelBuffer::readPixel(s32 _x, s32 _y) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in PixelBuffer::readPixel. Read position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return { 0, 0, 0, 0 };

		}

		s32 ind = ((_y * width) + _x) * 4;

		return { buffer[ind], buffer[ind + 1], buffer[ind + 2], buffer[ind + 3] };

	}

	u8 PixelBuffer::readRed(s32 _x, s32 _y) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in PixelBuffer::readRed. Read position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return 0;

		}

		s32 ind = ((_y * width) + _x) * 4;

		return buffer[ind];

	}

	u8 PixelBuffer::readGreen(s32 _x, s32 _y) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in PixelBuffer::readGreen. Read position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return 0;

		}

		s32 ind = ((_y * width) + _x) * 4;

		return buffer[ind + 1];

	}

	u8 PixelBuffer::readBlue(s32 _x, s32 _y) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in PixelBuffer::readBlue. Read position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return 0;

		}

		s32 ind = ((_y * width) + _x) * 4;

		return buffer[ind + 2];

	}

	u8 PixelBuffer::readAlpha(s32 _x, s32 _y) {

		if (_x < 0 || _y < 0 || _x >= width || _y >= height) {

			ZIXEL_WARN("Error in PixelBuffer::readAlpha. Read position ({}, {}) out of range. Valid range: (0-{}, 0-{})", _x, _y, width - 1, height - 1);
			return 0;

		}

		s32 ind = ((_y * width) + _x) * 4;

		return buffer[ind + 3];

	}

	void PixelBuffer::copy(PixelBuffer* _sourceBuffer) {

		if (width != _sourceBuffer->width || height != _sourceBuffer->height) {

			ZIXEL_WARN("Error in PixelBuffer::copy. Source buffer size ({}, {}) and destination buffer size ({}, {}) do not match.", _sourceBuffer->width, _sourceBuffer->height, width, height);
			return;

		}

		memcpy_s(buffer, ((size_t)width * (size_t)height * 4), _sourceBuffer->buffer, ((size_t)_sourceBuffer->width * (size_t)_sourceBuffer->height * 4));
		
		pixelCount = _sourceBuffer->pixelCount;

		//This just copies the bbox directly from the source buffer.
		//If the source buffer's bbox hasn't been calculated properly, the destination buffer's bbox will be incorrect too.
		if (useBBox) __setBBox(_sourceBuffer->bBoxLeft, _sourceBuffer->bBoxTop, _sourceBuffer->bBoxRight, _sourceBuffer->bBoxBottom);

	}

	bool PixelBuffer::merge(PixelBuffer* _sourceBuffer, BlendMode _blendMode, f32 _sourceOpacity) {

		if (width != _sourceBuffer->width || height != _sourceBuffer->height) {

			ZIXEL_WARN("Error in PixelBuffer::merge. Source buffer size ({}, {}) and destination buffer size ({}, {}) do not match.", _sourceBuffer->width, _sourceBuffer->height, width, height);
			return false;

		}

		bool modified = false;
		bool calcBBox = false;

		for (s32 x = 0; x < width; ++x) {

			for (s32 y = 0; y < height; ++y) {

				Color4 source = _sourceBuffer->readPixel(x, y);
				source.a = (u8)roundf(source.a * _sourceOpacity);

				Color4 dest = readPixel(x, y);
				
				Color4 blended = Color::blendColor(source, dest, _blendMode);
				if (!Color::match(dest, blended)) {

					writePixel(x, y, blended, BlendMode::Overwrite, false);
					modified = true;

				}

				if (useBBox) {

					if (dest.a == 0 && blended.a > 0) checkBBoxIncrease(x, y);
					else if (dest.a > 0 && blended.a == 0) calcBBox = true;

				}

			}

		}

		if (calcBBox) calculateBBox(true);
		
		return modified;

	}

	bool PixelBuffer::merge(PixelBuffer* _sourceBuffer, s32 _destX, s32 _destY, BlendMode _blendMode, f32 _sourceOpacity, MaskBuffer* _maskBuffer) {

		if (_destX >= width || _destY >= height || _destX + _sourceBuffer->width - 1 < 0 || _destY + _sourceBuffer->height - 1 < 0) return false;

		bool modified = false;
		bool calcBBox = false;

		s32 endX = Math::minInt(_destX + _sourceBuffer->width - 1, width - 1);
		s32 endY = Math::minInt(_destY + _sourceBuffer->height - 1, height - 1);

		for (s32 x = Math::maxInt(_destX, 0); x <= endX; ++x) {

			for (s32 y = Math::maxInt(_destY, 0); y <= endY; ++y) {

				if (_maskBuffer != nullptr && !_maskBuffer->read(x - _destX, y - _destY)) continue;

				Color4 source = _sourceBuffer->readPixel(x - _destX, y - _destY);
				source.a = (u8)roundf(source.a * _sourceOpacity);

				Color4 dest = readPixel(x, y);

				Color4 blended = Color::blendColor(source, dest, _blendMode);
				if (!Color::match(dest, blended)) {

					writePixel(x, y, blended, BlendMode::Overwrite, false);
					modified = true;

				}

				if (useBBox) {

					if (dest.a == 0 && blended.a > 0) checkBBoxIncrease(x, y);
					else if (dest.a > 0 && blended.a == 0) calcBBox = true;

				}

			}

		}

		if (calcBBox) calculateBBox(true);

		return modified;

	}

	bool PixelBuffer::compare(PixelBuffer* _buffer) {

		if (_buffer == nullptr) {
			return false;
		}

		if (width != _buffer->width || height != _buffer->height) {
			return false;
		}

		return (memcmp(buffer, _buffer->buffer, ((size_t)width * (size_t)height) * 4) == 0);

	}

	PixelBuffer* PixelBuffer::clone() {

		PixelBuffer* cloned = new PixelBuffer(width, height);

		cloned->pixelCount = pixelCount;
		if (useBBox) cloned->__setBBox(bBoxLeft, bBoxTop, bBoxRight, bBoxBottom);

		cloned->useBBox = useBBox;
		cloned->makeEmptyPixelsBlack = makeEmptyPixelsBlack;

		s32 size = (width * height * 4);

		for (s32 i = 0; i < size; ++i) {
			cloned->buffer[i] = buffer[i];
		}

		return cloned;

	}

}