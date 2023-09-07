#include "Engine/ZixelPCH.h"
#include "Engine/StringHelper.h"
#include "Engine/Math.h"

namespace Zixel {

	void StringHelper::split(std::vector<std::string>& list, std::string& str, char subStr, bool includeEmpties) {
		
		list.clear();

		std::string tok = "";

		for (char& cur : str) {

			if (cur != subStr) tok += cur;
			else if (tok != "" || includeEmpties) {
				list.push_back(tok);
				tok = "";
			} 

		}

		if (tok != "" || includeEmpties) list.push_back(tok);

	}

	void StringHelper::trim(std::string& str) {
		if (str.size() <= 0) return;

		while (str.front() == ' ') str.erase(0, 1);
		while (str.back() == ' ') str.erase(str.size() - 1, 1);
	}

	void StringHelper::clean(std::string& _str) {

		trim(_str);
		for (size_t i = 0; i < _str.length(); ++i) {

			if (_str[i] != ' ') continue;
			while (i + 1 < _str.length() && _str[i + 1] == ' ') _str.erase(i, 1);

		}

	}

	void StringHelper::crop(std::string& str, size_t pos, size_t len) {
		if (pos < 0 || pos >= str.size()) return;

		str.erase(pos + len, str.size() - (pos + len));
		str.erase(0, pos);
	}

	void StringHelper::replaceAll(std::string& str, char oldSub, char newSub) {
		for (s32 i = (s32)str.size() - 1; i >= 0; --i) {
			if (str[i] == oldSub) str[i] = newSub;
		}
	}

	void StringHelper::removeAll(std::string& str, char subStr) {

		for (s32 i = (s32)str.size() - 1; i >= 0; --i) {
			if (str[i] == subStr) str.erase(i, 1);
		}

	}

	bool StringHelper::contains(std::string& _str, char _char) {

		for (char curChar : _str) {
			
			if (curChar == _char) {
				return true;
			}

		}

		return false;

	}

	bool StringHelper::startsWith(const std::string& _str, const std::string& _subStr) {

		if (_str.length() < _subStr.length()) return false;

		for (size_t i = 0; i < _subStr.length(); ++i) {
			if (_str[i] != _subStr[i]) return false;
		}

		return true;

	}

	bool StringHelper::isDigit(char digit) {
		return (digit >= 48 && digit <= 57);
	}

	bool StringHelper::isLetter(char _char) {
		return isalpha(_char);
	}

	bool StringHelper::toInt(std::string& str, s32& value) {

		std::string copy = str;
		removeAll(copy, ' ');

		s32 len = (s32)copy.size();

		if (len <= 0) {

			value = 0;
			//ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toInt. String is empty.", copy);
			return true;

		}

		bool hasNum = false;

		for (s32 i = 0; i < len; ++i) {

			char& cur = copy[i];

			if (cur == '-') {

				if (i > 0) {

					value = 0;
					ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toInt. Invalid '-' found at position {}.", copy, i + 1);
					return false;

				}

			}
			else {

				if (!isDigit(cur)) {

					value = 0;
					ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toInt. Invalid character found at position {}.", copy, i + 1);
					return false;

				}

				hasNum = true;

			}

		}

		if (!hasNum) {

			value = 0;
			ZIXEL_WARN("Invalid input for StringHelper::toInt. No valid digits found.");
			return false;

		}

		value = std::stoi(copy);

		return true;

	}

	bool StringHelper::toFloat(std::string& str, f32& value) {

		std::string copy = str;
		removeAll(copy, ' ');

		s32 len = (s32)copy.size();

		if (len <= 0) {

			value = 0.0f;
			//ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toFloat. String is empty.", copy);
			return true;

		}

		bool point = false;
		bool hasNum = false;

		for(s32 i = 0; i < len; ++i){

			char& cur = copy[i];

			if (cur == '-') {

				if (i > 0) {

					value = 0.0f;
					ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toFloat. Invalid '-' found at position {}.", copy, i + 1);
					return false;

				}

			}
			else if (cur == '.') {

				if (point) {

					value = 0.0f;
					ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toFloat. Invalid '.' found at position {}.", copy, i + 1);
					return false;

				}

				point = true;

			}
			else {

				if (!isDigit(cur)) {

					value = 0.0f;
					ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toFloat. Invalid character found at position {}.", copy, i + 1);
					return false;

				}

				hasNum = true;
			}

		}

		if (!hasNum) {

			value = 0.0f;
			ZIXEL_WARN("Invalid input for StringHelper::toFloat. No valid digits found.");
			return false;

		}

		value = std::stof(copy);

		return true;
	}

	bool StringHelper::toDouble(std::string& str, f64& value) {

		std::string copy = str;
		removeAll(copy, ' ');

		s32 len = (s32)copy.size();

		if (len <= 0) {

			value = 0;
			//ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toDouble. String is empty.", copy);
			return true;

		}

		bool point = false;
		bool hasNum = false;

		for (s32 i = 0; i < len; ++i) {

			char& cur = copy[i];

			if (cur == '-') {

				if (i > 0) {

					value = 0;
					ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toDouble. Invalid '-' found at position {}.", copy, i + 1);
					return false;

				}

			}
			else if (cur == '.') {

				if (point) {

					value = 0;
					ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toDouble. Invalid '.' found at position {}.", copy, i + 1);
					return false;

				}

				point = true;

			}
			else {

				if (!isDigit(cur)) {

					value = 0;
					ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toDouble. Invalid character found at position {}.", copy, i + 1);
					return false;

				}

				hasNum = true;
			}

		}

		if (!hasNum) {

			value = 0;
			ZIXEL_WARN("Invalid input for StringHelper::toDouble. No valid digits found.");
			return false;

		}
		
		value = std::atof(copy.c_str());

		return true;
	}

	bool StringHelper::toBool(std::string& str, bool& value) {

		std::string copy = str;
		removeAll(copy, ' ');

		s32 len = (s32)copy.size();

		if (len <= 0) {

			value = false;
			//ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toBool. String is empty.", copy);
			return true;

		}
		
		if (copy == "false") {

			value = false;			
			return true;

		}

		if (copy == "true") {

			value = true;
			return true;

		}

		bool hasNum = false;

		for (s32 i = 0; i < len; ++i) {

			char& cur = copy[i];

			if (cur == '-') {

				if (i > 0) {

					value = 0;
					ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toBool. Invalid '-' found at position {}.", copy, i + 1);
					return false;

				}

			}
			else {

				if (!isDigit(cur)) {

					value = 0;
					ZIXEL_WARN("Invalid input \"{}\" for StringHelper::toBool. Invalid character found at position {}.", copy, i + 1);
					return false;

				}

				hasNum = true;
			}

		}

		if (!hasNum) {

			value = 0;
			ZIXEL_WARN("Invalid input for StringHelper::toBool. No valid digits found.");
			return false;

		}

		value = (bool)std::stoi(copy);

		return true;
	}

	void StringHelper::toString(std::string& _str, f32& _value, s32 _decimalPlaces) {
		
		std::ostringstream stream;
		stream.precision(_decimalPlaces);
		stream << std::fixed << _value;

		_str = stream.str();
		if (_decimalPlaces < 0) return;

		s32 pos = -1;
		s32 decimalsFound = 0;
		
		for (s32 i = 0; i < _str.length(); ++i) {

			if (pos < 0 && _str[i] == '.') {
				pos = i;
			}
			else if (pos >= 0) {
				++decimalsFound;
			}

		}

		if (pos < 0) {

			if (_decimalPlaces > 0) {

				_str += '.';

				for (s32 i = _decimalPlaces - 1; i >= 0; --i) {
					_str += '0';
				}

			}
			
		}
		else {
			
			if (decimalsFound > _decimalPlaces) {
				crop(_str, 0, _str.length() - ((size_t)decimalsFound - (size_t)_decimalPlaces));
			}
			else if (decimalsFound < _decimalPlaces) {

				for (s32 i = (_decimalPlaces - decimalsFound) - 1; i >= 0; --i) {
					_str += '0';
				}

			}

			if (_decimalPlaces == 0) {
				_str.erase(pos, 1);
			}

		}

	}

	void StringHelper::toString(std::string& _str, f64& _value, s32 _decimalPlaces) {

		std::ostringstream stream;
		stream.precision(_decimalPlaces);
		stream << std::fixed << _value;

		_str = stream.str();
		if (_decimalPlaces < 0) return;

		s32 pos = -1;
		s32 decimalsFound = 0;

		for (s32 i = 0; i < _str.length(); ++i) {

			if (pos < 0 && _str[i] == '.') {
				pos = i;
			}
			else if (pos >= 0) {
				++decimalsFound;
			}

		}

		if (pos < 0) {

			if (_decimalPlaces > 0) {

				_str += '.';

				for (s32 i = _decimalPlaces - 1; i >= 0; --i) {
					_str += '0';
				}

			}

		}
		else {

			if (decimalsFound > _decimalPlaces) {
				crop(_str, 0, _str.length() - ((size_t)decimalsFound - (size_t)_decimalPlaces));
			}
			else if (decimalsFound < _decimalPlaces) {

				for (s32 i = (_decimalPlaces - decimalsFound) - 1; i >= 0; --i) {
					_str += '0';
				}

			}

			if (_decimalPlaces == 0) {
				_str.erase(pos, 1);
			}

		}

	}

	void StringHelper::toString(std::string& _str, s32& _value) {

		_str = std::to_string(_value);

	}

	u32 StringHelper::readUTF8Char(std::string& _str, size_t _charPos) {

		if (_str.length() <= 0 || _charPos >= _str.length()) return 0;

		u32 endian = 255;
		bool isLittleEndian = ((endian & 0x000000FF)) == 255; //@TODO: Test this on other platforms.

		size_t index = 0;
		u8 charVal;
		u32 byteCount, combinedVal;
		bool error;

		for (size_t i = 0; i < _str.size(); ++i) {

			charVal = (u8)_str[i];

			if (charVal <= 127) {

				if (index == _charPos) return (u32)charVal;

				++index;
				continue;

			}

			byteCount = 0;
			error = false;

			for (u8 j = 0; j < 7; ++j) {

				if (((charVal & (0b10000000 >> j)) >> (7 - j)) == 1) {

					if (j == 6) {

						error = true;
						break; //Last bit can't be 1.

					}

					++byteCount;
					continue;

				}

				break;

			}

			if (byteCount <= 1 || error) continue;

			if (byteCount <= 4) { //32 bits is the max we'll handle for now.

				if (i + (size_t)byteCount - 1 >= _str.size()) return 0;

				if (index == _charPos) {

					combinedVal = 0;

					for (u32 j = 0; j < byteCount; ++j) {

						u8 c = (u8)_str[i + (size_t)j];
						u8 mask = (j == 0) ? 0xFF >> (byteCount + 1) : 0b00111111;
						
						if (isLittleEndian) combinedVal |= ((u32)(c & mask) << (((byteCount - 1) - j) * 6));
						else combinedVal |= ((u32)(c & mask) << (j * 6));

					}

					return combinedVal;

				}

			}

			i += ((size_t)byteCount - 1);
			++index;

		}

		return 0;

	}

	UTF8String::UTF8String() {}

	UTF8String::UTF8String(const char* _value) {
		append(_value);
	}

	UTF8String::UTF8String(std::string _value) : UTF8String(_value.c_str()) {}

	UTF8String::UTF8String(UTF8String& _value) {
		charList = _value.charList;
	}

	bool UTF8String::operator == (const UTF8String& _value) {
		return (charList == _value.charList);
	}

	bool UTF8String::operator == (const char* _value) {
		
		if (empty()) return (_value[0] == '\0');
		
		size_t i = 0;
		while (_value[i] != '\0') {

			if (i >= size()) return false;

			if ((u32)_value[i] != at(i)) return false;
			++i;

		}

		return (i == size());

	}

	bool UTF8String::operator != (const char* _value) {
		return !((*this) == _value);
	}

	bool UTF8String::operator != (const UTF8String& _value) {
		return (charList != _value.charList);
	}

	void UTF8String::operator += (const char* _value) {
		append(_value);
	}

	void UTF8String::operator += (char _value) {
		charList.push_back((u32)_value);
	}

	void UTF8String::operator += (const std::string& _value) {
		append(_value.c_str());
	}

	void UTF8String::operator += (const UTF8String& _value) {

		charList.reserve(charList.size() + _value.charList.size());

		for (const u32& val : _value.charList) {
			charList.push_back(val);
		}

	}

	u32 UTF8String::operator [] (size_t _charPos) const {
		return at(_charPos);
	}

	std::vector<u32>::const_iterator UTF8String::begin() const {
		return charList.begin();
	}
	
	std::vector<u32>::const_iterator UTF8String::end() const {
		return charList.end();
	}

	size_t UTF8String::size() const {
		return charList.size();
	}

	size_t UTF8String::length() const {
		return size();
	}

	u32 UTF8String::at(size_t _charPos) const {

		if (_charPos >= charList.size()) {

			throw std::out_of_range("_charPos out of range.");
			return 0;

		}

		return charList[_charPos];

	}

	bool UTF8String::empty() const {
		return charList.empty();
	}

	void UTF8String::reserve(size_t _size) {
		charList.reserve(_size);
	}

	void UTF8String::append(const char* _value) {

		u32 endian = 255;
		bool isLittleEndian = ((endian & 0x000000FF)) == 255; //@TODO: Test this on other platforms.

		size_t ind = 0;
		u8 val;

		while ((val = _value[ind]) != '\0') {

			if (val <= 127) {

				charList.push_back((u32)val);
				++ind;

				continue;

			}

			u32 byteCount = 0;
			bool error = false;

			for (u8 j = 0; j < 7; ++j) {

				if (((val & (0b10000000 >> j)) >> (7 - j)) == 1) {

					if (j == 6) {

						error = true;
						break; //Last bit can't be 1.

					}

					++byteCount;
					continue;

				}

				break;

			}

			if (byteCount <= 1 || error) {

				++ind;
				continue;

			}

			if (byteCount <= 4) { //32 bits is the max we'll handle for now.

				for (size_t j = ind + 1; j < ind + (size_t)byteCount; ++j) {
					if(_value[j] == '\0') return;
				}

				u32 combinedVal = 0;

				for (u32 j = 0; j < byteCount; ++j) {

					u8 c = (u8)_value[ind + (size_t)j];
					u8 mask = (j == 0) ? 0xFF >> (byteCount + 1) : 0b00111111;

					if (isLittleEndian) combinedVal |= ((u32)(c & mask) << (((byteCount - 1) - j) * 6));
					else combinedVal |= ((u32)(c & mask) << (j * 6));

				}

				charList.push_back(combinedVal);

			}

			ind += (size_t)byteCount;

		}

	}

	std::vector<u32>::iterator UTF8String::erase(std::vector<u32>::const_iterator _position) {
		return charList.erase(_position);
	}

}