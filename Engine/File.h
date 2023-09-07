#pragma once

#include <string>
#include <fstream>
#include <filesystem>

namespace Zixel {

	enum class FileMode : u8 {

		BinaryRead,
		BinaryWrite,

	};

	enum class FileStatus : u8 {

		Idle,
		Error,
		Opened,

	};

	struct FileHandle {

		std::ifstream input;
		std::ofstream output;

		FileMode mode = FileMode::BinaryRead;
		FileStatus status = FileStatus::Idle;

		bool isLittleEndian = false; //If the current platform uses little endianness or not.

		uintmax_t fileSize = 0;
		uintmax_t seekPos = 0;

		bool close();
		bool isOpened();
		bool endOfFile();

		uintmax_t getFileSize();

		void seekAbsolute(uintmax_t _seekPos);
		void seekRelative(uintmax_t _seekPos);

		bool writeU8(u8 _val);
		bool writeU16(u16 _val);
		bool writeS16(s16 _val);
		bool writeU32(u32 _val);
		bool writeS32(s32 _val);

		u8 readU8();
		u16 readU16();
		s16 readS16();
		u32 readU32();
		s32 readS32();

	};

	class File {
	public:
		static std::string getNameFromPath(const std::string& _filePath, bool _includeExtension = false);
		static std::wstring getNameFromPath(const std::wstring& _filePath, bool _includeExtension = false);

		static bool fileExists(const std::string& _filePath);

		static FileHandle open(const std::string& _filePath, FileMode _mode);

	};

}
