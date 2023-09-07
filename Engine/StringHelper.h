#pragma once

#include <string>
#include <vector>

namespace Zixel {

	class StringHelper {
	public:

		static void split(std::vector<std::string>& list, std::string& str, char subStr, bool includeEmpties = false);
		static void trim(std::string& str);
		static void clean(std::string& _str);
		static void crop(std::string& str, size_t pos, size_t len);
		static void replaceAll(std::string& str, char oldSub, char newSub);
		static void removeAll(std::string& str, char subStr);
		static bool contains(std::string& _str, char _char);
		static bool startsWith(const std::string& _str, const std::string& _subStr);
		static bool isDigit(char digit);
		static bool isLetter(char _char);
		static bool toInt(std::string& str, s32& value);
		static bool toFloat(std::string& str, f32& value);
		static bool toDouble(std::string& str, f64& value);
		static bool toBool(std::string& str, bool& value);
		static void toString(std::string& _str, s32& _value);
		static void toString(std::string& _str, f32& _value, s32 _decimalPlaces = -1);
		static void toString(std::string& _str, f64& _value, s32 _decimalPlaces = -1);
		static u32 readUTF8Char(std::string& _str, size_t _charPos);
		
	};

	class UTF8String {
	private:
		std::vector<u32> charList;

	public:
		UTF8String();
		UTF8String(const char* _value);
		UTF8String(std::string _value); //@TODO: String reference?
		UTF8String(UTF8String& _value);

		bool operator == (const UTF8String& _value);
		bool operator == (const char* _value);
		bool operator != (const UTF8String& _value);
		bool operator != (const char* _value);
		void operator += (const char* _value);
		void operator += (char _value);
		void operator += (const std::string& _value);
		void operator += (const UTF8String& _value);

		u32 operator [] (size_t _charPos) const;

		std::vector<u32>::const_iterator begin() const;
		std::vector<u32>::const_iterator end() const;

		size_t size() const;
		size_t length() const;
		
		u32 at(size_t _charPos) const;

		bool empty() const;

		void reserve(size_t _size);
		void append(const char* _value);
		std::vector<u32>::iterator erase(std::vector<u32>::const_iterator _position);

	};

}