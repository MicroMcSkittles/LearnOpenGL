#pragma once
#include <string>
#include <vector>
#include <map>
#include "Core.h"

namespace JSON {
	enum class DataType {
		None = 0,
		String,
		Number,
		Object,
		Array,
		Boolean,
		Null
	};

	// If type is an object then value is empty and children has a value
	// If type is null then there's no value
	struct Data {
		DataType type;
		std::string value;
		// Only poplulated if type is an array
		std::vector<Data> elements;
		std::map<std::string, Data> children;

		const std::string& ReadString() { 
			if (type == DataType::Null) return "";
			if (type != DataType::String) {
				SOFT_ERROR("JSON node is not a string")
				return "";
			}
			return value; 
		}
		int ReadNumber() { 
			if (type == DataType::Null) return NULL;
			if (type != DataType::Number) {
				SOFT_ERROR("JSON node is not a string")
				return 0;
			}
			return stoi(value); 
		}
		bool ReadBool() { 
			if (type == DataType::Null) return false;
			if (type != DataType::Boolean) {
				SOFT_ERROR("JSON node is not a string")
				return false;
			}
			return (value == "true"); 
		}

		Data ReadArray(uint32_t id) {
			if (id >= elements.size() || id < 0) { FATAL_ERROR("Invalid array id for JSON node") }
			return elements[id];
		}

		Data operator [](const std::string& id) {
			if (!children.count(id)) {
				SOFT_ERROR("JSON node has no child with id ( " + id + " )")
			}
			return children[id];
		}
	};

	void ParseJSONData(const std::string& source, Data* data);
}