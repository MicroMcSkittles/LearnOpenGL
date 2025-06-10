#include "JSON.h"
#include <iostream>
#include <sstream>


namespace JSON {
	// Removes all white space from source and populations parseBuffer
	void CleanBuffer(const std::string& source, std::stringstream& parseBuffer) {
		bool quoteFlag = false;
		char lastChar = NULL;
		std::string spaceCount;
		for (char c : source) {
			if (c == '\"' && lastChar != '\\') quoteFlag = !quoteFlag;
			lastChar = c;
			if (isspace(c) && quoteFlag) {
				spaceCount.push_back(c);
				continue;
			}
			else if (isspace(c)) continue;
			parseBuffer << spaceCount << c;
			spaceCount.clear();
		}
		if(quoteFlag) { FATAL_ERROR("Expected \" in JSON source") }
	}

	// Parse buffer must be at the char after the first " mark, ends at the last " mark
	void ParseString(std::string& str, std::stringstream& parseBuffer) {
		char c;
		parseBuffer >> std::noskipws;
		while (parseBuffer >> c) {
			if (c == '\"') break;
			str.push_back(c);
		}
		if (c != '\"') { FATAL_ERROR("Expected \" in JSON source") }
	}
	// Parse buffer must be at the first digit
	void ParseNumber(std::string& number, std::stringstream& parseBuffer) {
		char c;
		while (parseBuffer >> c) {
			if (!isdigit(c)) break;
			number.push_back(c);
		}
		parseBuffer.seekg(-1, std::ios::cur);
	}
	// Parse buffer must be at the first letter
	void ParseBoolean(std::string& boolean, std::stringstream& parseBuffer) {
		int count = 0;
		char c;
		parseBuffer >> c;
		if (c == 't') count = 3;
		else if (c == 'f') count = 4;

		boolean.push_back(c);
		for (int i = 0; i < count; i++) {
			parseBuffer >> c;
			boolean.push_back(c);
		}
	}
	// Parse buffer must be at the first letter
	void ParseNull(std::stringstream& parseBuffer) {
		char c;
		for (int i = 0; i < 4; i++) {
			parseBuffer >> c;
		}
	}

	// Parse buffer must be at the first " mark
	bool IsStringValid(std::string& name, std::stringstream& parseBuffer) {
		char c;
		parseBuffer >> c;
		if (c != '\"') {
			parseBuffer.seekg(-1, std::ios::cur);
			return false; 
		}

		ParseString(name, parseBuffer);
		return true;
	}
	// Parse buffer must be at the first digit
	bool IsNumberValid(std::string& number, std::stringstream& parseBuffer) {
		char c = parseBuffer.peek();
		if (!isdigit(c)) return false;

		ParseNumber(number, parseBuffer);
		return true;
	}
	// Parse buffer must be at the first letter
	bool IsBooleanValid(std::string& boolean, std::stringstream& parseBuffer) {
		char c = parseBuffer.peek();
		if (c != 't' && c != 'f') return false;

		ParseBoolean(boolean, parseBuffer);
		return true;
	}
	// Parse buffer must be at the first letter
	bool IsNullValid(std::stringstream& parseBuffer) {
		if (parseBuffer.peek() != 'n') return false;

		ParseNull(parseBuffer);
		return true;
	}

	bool IsArrayValid(Data* data, std::stringstream& parseBuffer);

	void ParseArray(Data* data, std::stringstream& parseBuffer) {
		char c;
		parseBuffer.seekg(-1, std::ios::cur);
		while (parseBuffer >> c) {
			if (c == ']') break;
			if (c == ',') parseBuffer >> c;
			else { FATAL_ERROR("Expected ] in JSON source") }

			Data element;

			if (IsStringValid(element.value, parseBuffer)) element.type = DataType::String;
			else if (IsNumberValid(element.value, parseBuffer)) element.type = DataType::Number;
			else if (IsBooleanValid(element.value, parseBuffer)) element.type = DataType::Boolean;
			else if (IsNullValid(parseBuffer)) element.type = DataType::Null;
			else if (IsArrayValid(&element, parseBuffer)) element.type = DataType::Array;
			else if (c == '{') {
				std::string source = "{";
				while (parseBuffer >> c) {
					if (c == '}') break;
					source.push_back(c);
				}
				if (c != '}') { FATAL_ERROR("Expected } in JSON source") }
				source.push_back(c);
				ParseJSONData(source, &element);
				element.type = DataType::Object;
			}
			else {
				FATAL_ERROR("Invalid value for array element in JSON source")
			}
			
			data->elements.push_back(element);
		}
		if (c != ']') { FATAL_ERROR("Expected ] in JSON source") }
	}
	// Parse buffer must be at the [
	bool IsArrayValid(Data* data, std::stringstream& parseBuffer) {
		char c;
		parseBuffer >> c;
		parseBuffer << c;
		if (c != '[') return false;

		ParseArray(data, parseBuffer);
	}

	// Return true if entry exists
	void ParseEntry(Data* data, std::stringstream& parseBuffer) {
		std::string name;
		char c;
		if (!IsStringValid(name, parseBuffer)) {
			FATAL_ERROR("Invalid name for entry") 
		}

		parseBuffer >> c;
		if(c != ':') { FATAL_ERROR("Expected : in JSON source") }

		parseBuffer >> c;
		parseBuffer.seekg(-1, std::ios::cur);

		Data child;

		if (IsStringValid(child.value, parseBuffer)) child.type = DataType::String;
		else if (IsNumberValid(child.value, parseBuffer)) child.type = DataType::Number;
		else if (IsBooleanValid(child.value, parseBuffer)) child.type = DataType::Boolean;
		else if (IsNullValid(parseBuffer)) child.type = DataType::Null;
		else if (IsArrayValid(&child, parseBuffer)) child.type = DataType::Array;
		else if (c == '{') {
			std::string source = "{";
			while (parseBuffer >> c) {
				if (c == '}') break;
				source.push_back(c);
			}
			if (c != '}') { FATAL_ERROR("Expected } in JSON source") }
			source.push_back(c);
			ParseJSONData(source, &child);
		}
		else {
			FATAL_ERROR("Invalid value for entry ( " + name + " ) in JSON source")
		}

		data->children.insert({ name, child });
	}

	void ParseJSONData(const std::string& source, Data* data) {
		if (source.empty()) { FATAL_ERROR("JSON source can't empty") }

		std::stringstream parseBuffer;
		CleanBuffer(source, parseBuffer);

		data->type = DataType::Object;

		char c;
		parseBuffer >> c;
		if (c != '{') { FATAL_ERROR("Expected { at start of JSON source") }

		if (parseBuffer.peek() == '}') return;

		while (true) {
			ParseEntry(data, parseBuffer);
			parseBuffer >> c;
			if (c == ',') continue;
			else if (c == '}') break;
			else { 
				FATAL_ERROR("Expected } in JSON source") 
			}
		}

		if(c != '}') { FATAL_ERROR("Expected } at end of JSON source") }
	}
}