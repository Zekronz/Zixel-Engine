#include "Engine/ZixelPCH.h"
#include "Engine/File.h"
#include "Engine/Math.h"

namespace Zixel {

	bool FileHandle::close() {

		if (status != FileStatus::Opened) return false;

		if (mode == FileMode::BinaryWrite) {
			output.close();
		}
		else if (mode == FileMode::BinaryRead) {
			input.close();
		}

		status = FileStatus::Idle;

		return true;

	}

	bool FileHandle::isOpened() {
		return (status == FileStatus::Opened);
	}

	bool FileHandle::endOfFile() {
		return (seekPos >= fileSize);
	}

	uintmax_t FileHandle::getFileSize() {
		return fileSize;
	}

	void FileHandle::seekAbsolute(uintmax_t _seekPos) {

		if (status != FileStatus::Opened || mode != FileMode::BinaryRead) return;

		seekPos = (_seekPos <= fileSize) ? _seekPos : fileSize;
		input.seekg(seekPos, input.beg);

	}

	void FileHandle::seekRelative(uintmax_t _seekPos) {

		if (status != FileStatus::Opened || mode != FileMode::BinaryRead) return;

		seekPos += _seekPos;
		if (seekPos > fileSize) seekPos = fileSize;

		input.seekg(seekPos, input.beg);

	}

	bool FileHandle::writeU8(u8 _val) {

		if (status != FileStatus::Opened || mode != FileMode::BinaryWrite) return false;

		output.put(_val);

		return true;

	}

	bool FileHandle::writeU16(u16 _val) {

		if (status != FileStatus::Opened || mode != FileMode::BinaryWrite) return false;

		if (isLittleEndian) {

			output.put(_val & 0x00FF);
			output.put((_val & 0xFF00) >> 8);

		}
		else {

			output.put((_val & 0xFF00) >> 8);
			output.put(_val & 0x00FF);

		}

		return true;

	}

	bool FileHandle::writeS16(s16 _val) {

		if (status != FileStatus::Opened || mode != FileMode::BinaryWrite) return false;

		if (isLittleEndian) {

			output.put(_val & 0x00FF);
			output.put((_val & 0xFF00) >> 8);

		}
		else {

			output.put((_val & 0xFF00) >> 8);
			output.put(_val & 0x00FF);

		}

		return true;

	}

	bool FileHandle::writeU32(u32 _val) {

		if (status != FileStatus::Opened || mode != FileMode::BinaryWrite) return false;

		if (isLittleEndian) {

			output.put(_val & 0x000000FF);
			output.put((_val & 0x0000FF00) >> 8);
			output.put((_val & 0x00FF0000) >> 16);
			output.put((_val & 0xFF000000) >> 24);

		}
		else {

			output.put((_val & 0xFF000000) >> 24);
			output.put((_val & 0x00FF0000) >> 16);
			output.put((_val & 0x0000FF00) >> 8);
			output.put(_val & 0x000000FF);

		}

		return true;

	}

	bool FileHandle::writeS32(s32 _val) {

		if (status != FileStatus::Opened || mode != FileMode::BinaryWrite) return false;

		if (isLittleEndian) {

			output.put(_val & 0x000000FF);
			output.put((_val & 0x0000FF00) >> 8);
			output.put((_val & 0x00FF0000) >> 16);
			output.put((_val & 0xFF000000) >> 24);

		}
		else {

			output.put((_val & 0xFF000000) >> 24);
			output.put((_val & 0x00FF0000) >> 16);
			output.put((_val & 0x0000FF00) >> 8);
			output.put(_val & 0x000000FF);

		}

		return true;

	}

	u8 FileHandle::readU8() {

		if (status != FileStatus::Opened || mode != FileMode::BinaryRead) return 0;
		if (endOfFile()) return 0;

		++seekPos;

		return (u8)input.get();

	}

	u16 FileHandle::readU16() {

		if (status != FileStatus::Opened || mode != FileMode::BinaryRead) return 0;
		if (endOfFile()) return 0;

		u8 bytes[2] = { 0, 0 };
		for (u8 i = 0; i < 2; ++i) {

			bytes[i] = (u8)input.get();
			++seekPos;

			if (endOfFile()) break;

		}

		if (isLittleEndian) {
			return (u16)(bytes[0] | (bytes[1] << 8));
		}

		return (u16)(bytes[1] | (bytes[0] << 8));

	}

	s16 FileHandle::readS16() {
		return (s32)readU16();
	}

	u32 FileHandle::readU32() {

		if (status != FileStatus::Opened || mode != FileMode::BinaryRead) return 0;
		if (endOfFile()) return 0;

		u8 bytes[4] = { 0, 0 };
		for (u8 i = 0; i < 4; ++i) {

			bytes[i] = (u8)input.get();
			++seekPos;

			if (endOfFile()) break;

		}

		if (isLittleEndian) {
			return (u32)(bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24));
		}

		return (u32)(bytes[3] | (bytes[2] << 8) | (bytes[1] << 16) | (bytes[0] << 24));

	}

	s32 FileHandle::readS32() {
		return (s32)readU32();
	}

	std::string File::getNameFromPath(const std::string& _filePath, bool _includeExtension) {

		if (_filePath.length() <= 0) return "";

		s32 start = -1, end = -1;
		if (_includeExtension) start = (s32)_filePath.length() - 1;

		for (s32 i = (s32)_filePath.length() - 1; i >= 0; --i) {

			char cur = _filePath[i];

			if (cur == '.' && start == -1) start = i - 1;
			if ((cur == '\\' || cur == '/') && end == -1) end = i + 1;

		}
		
		if (start == -1) start = (s32)_filePath.length() - 1;
		if (end == -1) end = 0;

		return _filePath.substr(end, (size_t)start - (size_t)end + 1);

	}

	std::wstring File::getNameFromPath(const std::wstring& _filePath, bool _includeExtension) {

		if (_filePath.length() <= 0) return L"";

		s32 start = -1, end = -1;
		if (_includeExtension) start = (s32)_filePath.length() - 1;

		for (s32 i = (s32)_filePath.length() - 1; i >= 0; --i) {

			wchar_t cur = _filePath[i];

			if (cur == '.' && start == -1) start = i - 1;
			if ((cur == '\\' || cur == '/') && end == -1) end = i + 1;

		}

		if (start == -1) start = (s32)_filePath.length() - 1;
		if (end == -1) end = 0;

		return _filePath.substr(end, (size_t)start - (size_t)end + 1);

	}

	bool File::fileExists(const std::string& _filePath) {

		if (_filePath.empty()) return false;

		std::filesystem::path path = std::filesystem::u8path(_filePath);
		return (std::filesystem::exists(path) && std::filesystem::is_regular_file(path));

	}

	FileHandle File::open(const std::string& _filePath, FileMode _mode) {

		FileHandle handle;
		std::filesystem::path path = std::filesystem::u8path(_filePath);

		u32 endian = 255;
		handle.isLittleEndian = ((endian & 0x000000FF)) == 255; //@TODO: Test this on other platforms.

		handle.mode = _mode;

		if (_mode == FileMode::BinaryWrite) {

			handle.output.open(path, std::ios::out | std::ios::binary | std::ios::trunc);

			if (!handle.output.good()) {
				
				handle.status = FileStatus::Error;
				handle.output.close(); //Probably not necessary.

			}
			else {
				
				handle.output.imbue(std::locale::classic());
				handle.status = FileStatus::Opened;

			}

		}
		else if (_mode == FileMode::BinaryRead) {
			
			std::error_code errorCode;
			handle.fileSize = std::filesystem::file_size(path, errorCode);

			if (errorCode) {

				handle.status = FileStatus::Error;
				handle.fileSize = 0;

				return handle;

			}

			handle.input.open(path, std::ios::in | std::ios::binary);

			if (!handle.input.good()) {

				handle.status = FileStatus::Error;
				handle.input.close(); //Probably not necessary.

				handle.seekPos = handle.fileSize; //This just makes sure that endOfFile() returns true so that we don't get stuck in an infinite loop while trying to read the entire file.

			}
			else {

				handle.input.imbue(std::locale::classic());
				handle.status = FileStatus::Opened;
				handle.input.seekg(0, handle.input.beg);

			}

		}

		return handle;

	}

}