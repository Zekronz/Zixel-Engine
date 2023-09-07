#pragma once

#include <string>
#include <shlwapi.h>
#include <wincodec.h>

#include "Engine/Types.h"

namespace Zixel {

	template<class T>
	struct ComPtr {

		ComPtr() : ptr(nullptr) {}
		ComPtr(T* _ptr) : ptr(_ptr) {}

		~ComPtr() {

			if (ptr != nullptr) {

				ptr->Release();
				ptr = nullptr;

			}

		}

		T* ptr = nullptr;

	};

	struct PixelBuffer;

	class Clipboard {

	public:
		static bool setText(std::string& _text);
		static bool getText(std::string& _text);

		static bool setPixelBuffer(PixelBuffer* _buffer);
		static PixelBuffer* getPixelBuffer();
		static bool getPixelBufferSize(s32& _width, s32& _height);
		static bool pixelBufferAvailable();

	private:
		static bool writePixelBufferToPNGStream(PixelBuffer* _buffer, IStream* _stream);
		static PixelBuffer* readPNGData(HGLOBAL _hMem, u8* _data, size_t _dataSize);
		static bool readPNGSize(HGLOBAL _hMem, u8* _data, size_t _dataSize, s32& _width, s32& _height);
		static u8 getShiftFromMask(u32 _mask);

	};

}