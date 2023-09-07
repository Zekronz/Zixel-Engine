/*
    Clipboard.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/ZixelPCH.h"
#include "Engine/Clipboard.h"
#include "Engine/Math.h"
#include "Engine/PixelBuffer.h"
#include "Engine/Color.h"

namespace Zixel {

	//@TODO: Platform specific.

	bool Clipboard::setText(std::string& _text) {

		if (_text.length() <= 0) {
			
			if (!OpenClipboard(NULL)) {
				return false;
			}

			if (!EmptyClipboard()) {

				CloseClipboard();
				return false;

			}

			CloseClipboard();

			return true;

		}

		bool allocatedText = false;
		bool allocatedUnicode = false;

		//Allocate CF_TEXT.
		const size_t lengthText = _text.size() + 1;

		HGLOBAL memText = GlobalAlloc(GHND, lengthText);

		if (memText != NULL) {
			
			LPVOID memLockText = GlobalLock(memText);

			if (memLockText != NULL) {
				
				memcpy(memLockText, _text.c_str(), lengthText);

				if (!(!GlobalUnlock(memText) && GetLastError() != NO_ERROR)) {
					allocatedText = true;
				}

			}

		}

		//Allocate CF_UNICODETEXT.
		const size_t lengthUnicode = (_text.size() + 1) * 2;

		HGLOBAL memUnicode = GlobalAlloc(GHND, lengthUnicode);

		if (memUnicode != NULL) {

			LPVOID memLockUnicode = GlobalLock(memUnicode);

			if (memLockUnicode != NULL) {

				std::wstring unicodeStr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(_text);
				
				memcpy(memLockUnicode, unicodeStr.c_str(), lengthUnicode);

				if (!(!GlobalUnlock(memUnicode) && GetLastError() != NO_ERROR)) {
					allocatedUnicode = true;
				}

			}

		}
		
		if (!allocatedText && !allocatedUnicode) {

			if (memText != NULL) GlobalFree(memText);
			if (memUnicode != NULL) GlobalFree(memUnicode);

			return false;
		}

		if (!OpenClipboard(NULL)) {

			if (memText != NULL) GlobalFree(memText);
			if (memUnicode != NULL) GlobalFree(memUnicode);

			return false;

		}

		if (!EmptyClipboard()) {

			if (memText != NULL) GlobalFree(memText);
			if (memUnicode != NULL) GlobalFree(memUnicode);

			CloseClipboard();

			return false;

		}

		bool setText = false;
		bool setUnicode = false;

		if (allocatedText) {

			if (SetClipboardData(CF_TEXT, memText) != NULL) {
				setText = true;
			}

		}
		
		if ((!allocatedText || !setText) && memText != NULL) {
			GlobalFree(memText);
		}

		if (allocatedUnicode) {

			if (SetClipboardData(CF_UNICODETEXT, memUnicode) != NULL) {
				setUnicode = true;
			}

		}

		if ((!allocatedUnicode || !setUnicode) && memUnicode != NULL) {
			GlobalFree(memUnicode);
		}

		CloseClipboard();

		return (setText || setUnicode);

	}

	bool Clipboard::getText(std::string& _text) {

		_text.clear();

		if (!OpenClipboard(NULL)) {
			return false;
		}

		if (IsClipboardFormatAvailable(CF_TEXT)) {

			HANDLE data = GetClipboardData(CF_TEXT);

			if (data == NULL) {

				CloseClipboard();
				return false;

			}

			LPVOID dataLock = GlobalLock(data);
			if (dataLock == NULL) {

				CloseClipboard();
				return false;

			}

			char* cData = (char*)dataLock;

			if (cData == NULL || cData[0] == 0) {

				CloseClipboard();
				return false;

			}

			_text += cData;
			_text.erase(std::remove(_text.begin(), _text.end(), '\r'), _text.end());
			
			GlobalUnlock(data);

		}
		else if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
			
			HANDLE data = GetClipboardData(CF_UNICODETEXT);

			if (data == NULL) {

				CloseClipboard();
				return false;

			}

			LPVOID dataLock = GlobalLock(data);
			if (dataLock == NULL) {

				CloseClipboard();
				return false;

			}

			WCHAR* cData = (WCHAR*)dataLock;

			if (cData == NULL || cData[0] == 0) {

				CloseClipboard();
				return false;

			}

			size_t count = 0;
			while (cData[count] != 0) {

				if (cData[count] < 128) {
					
					if (cData[count] != '\r') {
						_text += (char)cData[count];
					}

				}
				else {
					_text += '?';
				}

				++count;

			}

			GlobalUnlock(data);

		}
		
		CloseClipboard();

		return true;

	}

	bool Clipboard::setPixelBuffer(PixelBuffer* _buffer) {

		if (!OpenClipboard(NULL)) {
			return false;
		}

		if (!EmptyClipboard()) {
			
			CloseClipboard();
			return false;

		}

		bool allocatedPNG = false;
		bool allocatedDIBV5 = false;

		//PNG.
		UINT pngFormat = RegisterClipboardFormatA("PNG");
		if (pngFormat != 0) {

			HRESULT result;

			result = CoInitialize(NULL);

			if (result == S_OK || result == S_FALSE) {

				ComPtr<IStream> stream;
				result = CreateStreamOnHGlobal(NULL, false, &stream.ptr);

				if (!FAILED(result)) {

					HGLOBAL handle;
					result = GetHGlobalFromStream(stream.ptr, &handle);

					if (!FAILED(result)) {

						if (writePixelBufferToPNGStream(_buffer, stream.ptr)) {

							if (SetClipboardData(pngFormat, handle) != NULL) {
								allocatedPNG = true;
							}
							else {
								GlobalFree(handle);
							}

						}
						else {
							GlobalFree(handle);
						}

					}

				}

				CoUninitialize();

			}

		}

		//DIBV5.
		s32 bytesPerRow = _buffer->width * 4;
		HGLOBAL memDIBV5 = GlobalAlloc(GHND, sizeof(BITMAPV5HEADER) + ((SIZE_T)bytesPerRow * (SIZE_T)_buffer->height));

		if (memDIBV5 != NULL) {
			
			BITMAPV5HEADER* bitmapHeader = (BITMAPV5HEADER*)GlobalLock(memDIBV5);

			if (bitmapHeader != NULL) {

				bitmapHeader->bV5Size = sizeof(BITMAPV5HEADER);
				bitmapHeader->bV5Width = _buffer->width;
				bitmapHeader->bV5Height = _buffer->height;
				bitmapHeader->bV5Planes = 1;
				bitmapHeader->bV5BitCount = 32;
				bitmapHeader->bV5Compression = BI_RGB;
				bitmapHeader->bV5SizeImage = bytesPerRow * _buffer->height;
				bitmapHeader->bV5RedMask = 0x00FF0000;
				bitmapHeader->bV5GreenMask = 0x0000FF00;
				bitmapHeader->bV5BlueMask = 0x000000FF;
				bitmapHeader->bV5AlphaMask = 0xFF000000;
				bitmapHeader->bV5CSType = LCS_WINDOWS_COLOR_SPACE;
				bitmapHeader->bV5Intent = LCS_GM_GRAPHICS;
				bitmapHeader->bV5ClrUsed = 0;

				u8* dst = (((u8*)bitmapHeader) + bitmapHeader->bV5Size) + (((SIZE_T)_buffer->height - 1) * (SIZE_T)bytesPerRow);

				for (s32 pixelY = 0; pixelY < _buffer->height; ++pixelY) {

					u32* dstX = (u32*)dst;

					for (s32 pixelX = 0; pixelX < _buffer->width; ++pixelX, ++dstX) {

						Color4 src = _buffer->readPixel(pixelX, pixelY);

						if (src.a <= 0) {

							*dstX = (255 << 16) | (255 << 8) | (255 << 0) | (255 << 24);
							continue;

						}

						if (src.a < 255) {
							src = Color::blendColor(src, { 255, 255, 255, 255 }, BlendMode::Normal);
						}

						*dstX = (src.r << 16) | (src.g << 8) | (src.b << 0) | (255 << 24);

					}

					dst -= bytesPerRow;
				}

				if (!(!GlobalUnlock(memDIBV5) && GetLastError() != NO_ERROR)) {
					allocatedDIBV5 = true;
				}

			}

		}

		if (allocatedDIBV5) {
			SetClipboardData(CF_DIBV5, memDIBV5);
		}
		else if (memDIBV5 != NULL) {
			GlobalFree(memDIBV5);
		}

		CloseClipboard();

		return (allocatedPNG || allocatedDIBV5);

	}

	PixelBuffer* Clipboard::getPixelBuffer() {

		if (!OpenClipboard(NULL)) {
			return nullptr;
		}

		UINT pngFormat = RegisterClipboardFormatA("PNG");
		if (pngFormat != 0 && IsClipboardFormatAvailable(pngFormat)) {
			
			HANDLE pngHandle = GetClipboardData(pngFormat);
			if (pngHandle != NULL) {
				
				size_t dataSize = GlobalSize(pngHandle);
				u8* data = (u8*)GlobalLock(pngHandle);

				if (data != NULL && dataSize > 0) {
					
					HRESULT result = CoInitialize(nullptr);

					if (result == S_OK || result == S_FALSE) {
						
						PixelBuffer* buffer = nullptr;
						HGLOBAL hMem = GlobalAlloc(GHND, dataSize);
						
						if (hMem != NULL) {

							buffer = readPNGData(hMem, data, dataSize);
							GlobalFree(hMem);

						}

						CoUninitialize();
						GlobalUnlock(pngHandle);

						if (buffer != nullptr) {

							CloseClipboard();
							return buffer;

						}

					}

				}
				else if (data != NULL) {
					GlobalUnlock(pngHandle);
				}

			}

		}

		bool hasDIBV5 = IsClipboardFormatAvailable(CF_DIBV5);
		bool hasDIB = IsClipboardFormatAvailable(CF_DIB);

		if (hasDIBV5 || hasDIB) {
			
			BITMAPV5HEADER* bitmap5 = nullptr;
			BITMAPINFO* bitmap = nullptr;
				
			if (hasDIBV5) bitmap5 = (BITMAPV5HEADER*)GetClipboardData(CF_DIBV5);
			if (hasDIB) bitmap = (BITMAPINFO*)GetClipboardData(CF_DIB);

			if (bitmap5 != nullptr || bitmap != nullptr) {

				s32 width = 0;
				s32 height = 0;

				u32 redMask = 0x00FF0000;
				u32 greenMask = 0x0000FF00;
				u32 blueMask = 0x000000FF;
				u32 alphaMask = 0xFF000000;

				u8 redShift = 16;
				u8 greenShift = 8;
				u8 blueShift = 0;
				u8 alphaShift = 24;

				u16 bitCount = 0;
				u32 compression = 0;

				bool setData = false;

				if (bitmap5 != nullptr) {
					
					if (bitmap5->bV5BitCount == 32 && (bitmap5->bV5Compression == BI_RGB || (bitmap5->bV5Compression == BI_BITFIELDS && bitmap5->bV5RedMask && bitmap5->bV5GreenMask && bitmap5->bV5BlueMask && bitmap5->bV5AlphaMask))) {
						
						width = bitmap5->bV5Width;
						height = bitmap5->bV5Height;

						bitCount = bitmap5->bV5BitCount;
						compression = bitmap5->bV5Compression;
						
						if (compression == BI_BITFIELDS) {

							redMask = bitmap5->bV5RedMask;
							greenMask = bitmap5->bV5GreenMask;
							blueMask = bitmap5->bV5BlueMask;
							alphaMask = bitmap5->bV5AlphaMask;

							redShift = getShiftFromMask(redMask);
							greenShift = getShiftFromMask(greenMask);
							blueShift = getShiftFromMask(blueMask);
							alphaShift = getShiftFromMask(alphaMask);

						}

						setData = true;

					}

				}
				
				if(!setData && bitmap != nullptr){
					
					width = bitmap->bmiHeader.biWidth;
					height = bitmap->bmiHeader.biHeight;

					bitCount = bitmap->bmiHeader.biBitCount;
					compression = bitmap->bmiHeader.biCompression;

					if (compression == BI_BITFIELDS) {

						//Don't really understand this tbh.
						redMask = *((u32*)&bitmap->bmiColors[0]);
						greenMask = *((u32*)&bitmap->bmiColors[1]);
						blueMask = *((u32*)&bitmap->bmiColors[2]);

						redShift = getShiftFromMask(redMask);
						greenShift = getShiftFromMask(greenMask);
						blueShift = getShiftFromMask(blueMask);
						alphaShift = getShiftFromMask(alphaMask);

					}
					else if (compression == BI_RGB) {

						if (bitCount == 16) {

							redMask = 0x7C00;
							greenMask = 0x03E0;
							blueMask = 0x001F;

						}

					}

					setData = true;

				}
				
				if (setData) {

					PixelBuffer* buffer = new PixelBuffer(width, Math::absInt(height), { 0, 0, 0, 255 }, false);

					//@TODO: 8/16 bit?
					if (bitCount == 32 || bitCount == 24) {

						const u8* src = nullptr;

						if (compression == BI_BITFIELDS || compression == BI_RGB) {

							if (bitmap5 != nullptr) {
								src = ((u8*)bitmap5) + bitmap5->bV5Size;
							}
							else {
								src = ((u8*)bitmap) + bitmap->bmiHeader.biSize;
							}

							if (compression == BI_BITFIELDS) {
								src += sizeof(RGBQUAD) * 3;
							}

						}

						if (src != nullptr) {
							
							const size_t srcBytesPerRow = (size_t)width * (((size_t)bitCount + 7) / 8);
							const size_t padding = (4 - (srcBytesPerRow & 3)) & 3;

							const size_t byteCount = srcBytesPerRow / (size_t)width;

							bool alphaOverZero = false;
							bool hasPremultipliedAlpha = true;

							for (s32 pixelY = buffer->height - 1; pixelY >= 0; --pixelY) {

								for (s32 pixelX = 0; pixelX < width; ++pixelX) {

									size_t ind = (size_t)pixelX * byteCount;

									if (bitCount == 32) {

										u32 color = ((u32)src[ind + 3] << 24) | ((u32)src[ind + 2] << 16) | ((u32)src[ind + 1] << 8) | (u32)src[ind];

										u8 red = ((color & redMask) >> redShift);
										u8 green = ((color & greenMask) >> greenShift);
										u8 blue = ((color & blueMask) >> blueShift);
										u8 alpha = ((color & alphaMask) >> alphaShift);
										
										if (alpha > 0) alphaOverZero = true;
										if (red > alpha || green > alpha || blue > alpha) hasPremultipliedAlpha = false;

										buffer->writePixel(pixelX, (height < 0) ? buffer->height - 1 - pixelY : pixelY, { red, green, blue, alpha });

									}
									else if (bitCount == 24) {

										u32 color = ((u32)src[ind + 2] << 16) | ((u32)src[ind + 1] << 8) | (u32)src[ind];

										u8 red = ((color & redMask) >> redShift);
										u8 green = ((color & greenMask) >> greenShift);
										u8 blue = ((color & blueMask) >> blueShift);

										buffer->writePixel(pixelX, (height < 0) ? buffer->height - 1 - pixelY : pixelY, { red, green, blue, 255 });

									}
								}

								src += (size_t)srcBytesPerRow + (size_t)padding;

							}

							if (bitCount == 32 && hasPremultipliedAlpha) {
								
								for (s32 pixelX = 0; pixelX < buffer->width; ++pixelX) {

									for (s32 pixelY = 0; pixelY < buffer->height; ++pixelY) {

										Color4 color = buffer->readPixel(pixelX, pixelY);

										if (!alphaOverZero) {
											color.a = 255;
										}
										else if(color.a > 0) {

											color.r = (u8)roundf(color.r * (255.0f / color.a));
											color.g = (u8)roundf(color.g * (255.0f / color.a));
											color.b = (u8)roundf(color.b * (255.0f / color.a));

										}

										buffer->writePixel(pixelX, pixelY, color);

									} 

								}

							}

						}

					}
					else if (bitCount == 8) {
						//Too lazy to implement.
					}

					CloseClipboard();

					return buffer;

				}

			}

		}

		CloseClipboard();

		return nullptr;

	}

	bool Clipboard::getPixelBufferSize(s32& _width, s32& _height) {

		if (!OpenClipboard(NULL)) return false;
		
		//Read PNG.
		UINT pngFormat = RegisterClipboardFormatA("PNG");
		if (pngFormat != 0 && IsClipboardFormatAvailable(pngFormat)) {

			HANDLE pngHandle = GetClipboardData(pngFormat);
			if (pngHandle != NULL) {
				
				size_t dataSize = GlobalSize(pngHandle);
				u8* data = (u8*)GlobalLock(pngHandle);

				if (data != NULL && dataSize > 0) {
					
					HRESULT result = CoInitialize(nullptr);

					if (result == S_OK || result == S_FALSE) {
						
						HGLOBAL hMem = GlobalAlloc(GHND, dataSize);

						s32 width, height;
						bool success = false;

						if (hMem != NULL) {
							
							success = readPNGSize(hMem, data, dataSize, width, height);
							GlobalFree(hMem);

						}

						CoUninitialize();
						GlobalUnlock(pngHandle);
						
						if (success) {

							_width = width;
							_height = height;
							
							CloseClipboard();

							return true;

						}

					}

				}
				else if (data != NULL) {
					GlobalUnlock(pngHandle);
				}

			}

		}

		//Read bitmap.
		bool hasDIBV5 = IsClipboardFormatAvailable(CF_DIBV5);
		bool hasDIB = IsClipboardFormatAvailable(CF_DIB);

		if (!hasDIBV5 && !hasDIB) {

			CloseClipboard();
			return false;

		}

		BITMAPV5HEADER* bitmap5 = nullptr;
		BITMAPINFO* bitmap = nullptr;

		if (hasDIBV5) bitmap5 = (BITMAPV5HEADER*)GetClipboardData(CF_DIBV5);
		if (hasDIB) bitmap = (BITMAPINFO*)GetClipboardData(CF_DIB);

		if (bitmap5 != nullptr) {

			if (bitmap5->bV5BitCount == 32 && (bitmap5->bV5Compression == BI_RGB || (bitmap5->bV5Compression == BI_BITFIELDS && bitmap5->bV5RedMask && bitmap5->bV5GreenMask && bitmap5->bV5BlueMask && bitmap5->bV5AlphaMask))) {

				_width = bitmap5->bV5Width;
				_height = bitmap5->bV5Height;

				CloseClipboard();

				return true;

			}

		}

		if (bitmap != nullptr) {

			_width = bitmap->bmiHeader.biWidth;
			_height = bitmap->bmiHeader.biHeight;

			CloseClipboard();

			return true;

		}

		CloseClipboard();

		return false;

	}

	bool Clipboard::pixelBufferAvailable() {

		if (!OpenClipboard(NULL)) {
			return false;
		}

		bool available = (IsClipboardFormatAvailable(CF_DIBV5) || IsClipboardFormatAvailable(CF_DIB));

		if (!available) {

			UINT pngFormat = RegisterClipboardFormatA("PNG");
			available = IsClipboardFormatAvailable(pngFormat);

		}

		CloseClipboard();

		return available;

	}

	bool Clipboard::writePixelBufferToPNGStream(PixelBuffer* _buffer, IStream* _stream) {

		HRESULT result;

		ComPtr<IWICBitmapEncoder> encoder;
		result = CoCreateInstance(CLSID_WICPngEncoder, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&encoder.ptr));
		if (FAILED(result)) return false;
		
		result = encoder.ptr->Initialize(_stream, WICBitmapEncoderNoCache);
		if (FAILED(result)) return false;

		ComPtr<IWICBitmapFrameEncode> frame;
		ComPtr<IPropertyBag2> options;

		result = encoder.ptr->CreateNewFrame(&frame.ptr, &options.ptr);
		if (FAILED(result)) return false;

		result = frame.ptr->Initialize(options.ptr);
		if (FAILED(result)) return false;

		WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
		result = frame.ptr->SetPixelFormat(&format);
		if (FAILED(result)) return false;

		result = frame.ptr->SetSize(_buffer->width, _buffer->height);
		if (FAILED(result)) return false;

		s32 bytesPerRow = _buffer->width * 4;

		std::vector<u32>data((size_t)_buffer->width * (size_t)_buffer->height);
		u32* dst = (u32*)&data[0];

		for (s32 pixelY = 0; pixelY < _buffer->height; ++pixelY) {

			for (s32 pixelX = 0; pixelX < _buffer->width; ++pixelX) {

				Color4 src = _buffer->readPixel(pixelX, pixelY);

				(*dst) = (src.a << 24) | (src.r << 16) | (src.g << 8) | src.b;

				++dst;

			}

		}

		result = frame.ptr->WritePixels(_buffer->height, bytesPerRow, _buffer->height * bytesPerRow, (u8*)&data[0]);
		if (FAILED(result)) return false;

		result = frame.ptr->Commit();
		if (FAILED(result)) return false;

		result = encoder.ptr->Commit();
		if (FAILED(result)) return false;

		return true;

	}

	PixelBuffer* Clipboard::readPNGData(HGLOBAL _hMem, u8* _data, size_t _dataSize) {

		HRESULT result;

		ComPtr<IStream> stream;
		result = CreateStreamOnHGlobal(_hMem, false, &stream.ptr);
		if (FAILED(result)) return nullptr;

		result = stream.ptr->Write(_data, (ULONG)_dataSize, NULL);
		if (FAILED(result)) return nullptr;

		ComPtr<IWICBitmapDecoder> decoder;
		result = CoCreateInstance(CLSID_WICPngDecoder2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&decoder.ptr));
		if (FAILED(result)) {

			result = CoCreateInstance(CLSID_WICPngDecoder1, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&decoder.ptr));

			if (FAILED(result)) return nullptr;

		}

		result = decoder.ptr->Initialize(stream.ptr, WICDecodeMetadataCacheOnDemand);
		if (FAILED(result)) return nullptr;

		ComPtr<IWICBitmapFrameDecode> frame;
		result = decoder.ptr->GetFrame(0, &frame.ptr);
		if (FAILED(result)) return nullptr;

		WICPixelFormatGUID format;
		result = frame.ptr->GetPixelFormat(&format);
		if (FAILED(result)) return nullptr;

		//@TODO: Add suport for more pixel formats.
		if (format != GUID_WICPixelFormat32bppBGRA) return nullptr;

		UINT width = 0, height = 0;
		result = frame.ptr->GetSize(&width, &height);
		if (FAILED(result)) return nullptr;

		PixelBuffer* buffer = new PixelBuffer(width, height, { 0, 0, 0, 0 }, false);

		UINT bytesPerRow = (4 * width);
		result = frame.ptr->CopyPixels(nullptr, bytesPerRow, bytesPerRow * height, (BYTE*)buffer->buffer);
		if (FAILED(result)) {

			delete buffer;
			return nullptr;

		}

		//Fix channel order.
		for (s32 pixelX = (s32)width - 1; pixelX >= 0; --pixelX) {

			for (s32 pixelY = (s32)height - 1; pixelY >= 0; --pixelY) {

				Color4 color = buffer->readPixel(pixelX, pixelY);
				buffer->writePixel(pixelX, pixelY, { color.b, color.g, color.r, color.a });

			}

		}
		
		return buffer;

	}

	bool Clipboard::readPNGSize(HGLOBAL _hMem, u8* _data, size_t _dataSize, s32& _width, s32& _height) {

		HRESULT result;

		ComPtr<IStream> stream;
		result = CreateStreamOnHGlobal(_hMem, false, &stream.ptr);
		if (FAILED(result)) return false;

		result = stream.ptr->Write(_data, (ULONG)_dataSize, NULL);
		if (FAILED(result)) return false;

		ComPtr<IWICBitmapDecoder> decoder;
		result = CoCreateInstance(CLSID_WICPngDecoder2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&decoder.ptr));
		if (FAILED(result)) {

			result = CoCreateInstance(CLSID_WICPngDecoder1, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&decoder.ptr));
			if (FAILED(result)) return false;

		}

		result = decoder.ptr->Initialize(stream.ptr, WICDecodeMetadataCacheOnDemand);
		if (FAILED(result)) return false;

		ComPtr<IWICBitmapFrameDecode> frame;
		result = decoder.ptr->GetFrame(0, &frame.ptr);
		if (FAILED(result)) return false;

		WICPixelFormatGUID format;
		result = frame.ptr->GetPixelFormat(&format);
		if (FAILED(result)) return false;

		//@TODO: Add suport for more pixel formats.
		if (format != GUID_WICPixelFormat32bppBGRA) return false;

		UINT width = 0, height = 0;
		result = frame.ptr->GetSize(&width, &height);
		if (FAILED(result)) return false;

		_width = (s32)width;
		_height = (s32)height;

		return true;

	}

	u8 Clipboard::getShiftFromMask(u32 _mask) {

		if (_mask == 0xFF000000) return 24;
		if (_mask == 0x00FF0000) return 16;
		if (_mask == 0x0000FF00) return 8;

		return 0;

	}

}