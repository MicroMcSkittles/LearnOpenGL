#pragma once
#include "Core/Core.h"
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>

enum class DataTreeType {
	String,
	Int,
	Float,
	Array,
	Object,
	Boolean,
	Null
};

struct DataTreeValue {
	DataTreeType type = DataTreeType::Null;
	std::string value;

	bool IsNull() { return type == DataTreeType::Null; }
	operator int() {
		if (type != DataTreeType::Int) {
			WARNING("Data tree is not an int");
			return 0;
		}
		return std::stoi(value);
	}
	operator float() {
		if (type != DataTreeType::Float) {
			WARNING("Data tree is not a float");
			return 0.0f;
		}
		return std::stof(value);
	}
	operator bool() {
		if (type != DataTreeType::Boolean) {
			WARNING("Data tree is not a bool");
			return false;
		}
		return value == "true";
	}
	operator std::string() {
		if (type != DataTreeType::String) {
			WARNING("Data tree is not a string");
			return "";
		}
		return value;
	}
};
struct DataTree {
	
	DataTreeType type = DataTreeType::Null;
	std::string value;
	std::vector<DataTree> elements;
	std::map<std::string, DataTree> children;

	DataTree() { }

	DataTree(DataTreeType type)
		: type(type) { }
	DataTree(int i) {
		type = DataTreeType::Int;
		value = std::to_string(i);
	}
	DataTree(float f) {
		type = DataTreeType::Float;
		value = std::to_string(f);
	}
	DataTree(bool b) {
		type = DataTreeType::Boolean;
		value = (b) ? "true" : "false";
	}
	DataTree(const std::string& str) {
		type = DataTreeType::String;
		value = str;
	}
	
	DataTree& At(int id) { 
		if (id < 0 || id > elements.size()) {
			WARNING("Element ID out of bounds");
			return DataTree();
		}
		return elements[id]; 
	}

	DataTree& operator[] (const std::string& name) {
		if (!children.count(name)) {
			WARNING("Data tree does not contain child ( " + name + " )");
			return DataTree();
		}
		return children.at(name);
	}
	const DataTree& operator[] (const std::string& name) const {
		if (!children.count(name)) {
			WARNING("Data tree does not contain child ( " + name + " )");
			return DataTree();
		}
		return children.at(name);
	}

	DataTreeValue GetValue() {
		return { type, value };
	}
};

class json {
private:
	struct Consumer {
		std::string source;
		std::stringstream buffer;

		char eat() {
			char c;
			buffer >> c;
			return c;
		}
		char peek() {
			return buffer.peek();
		}
		void push(char c) {
			buffer.seekp(-1, std::ios::cur);
			buffer << c;
		}
	};

public:
	static void LoadFile(const std::string& filename, DataTree* tree);
	static void ParseSource(const std::string& source, DataTree* tree);

	static void Serialize(const std::string& filename, DataTree* tree);

	// add the ability to add/remove nodes
	static void ImGuiInputDataTree(DataTree* tree, const std::string& name = "", int depthID = 0);

private:
	static void SerializeObject(std::ofstream& file, DataTree* tree, const std::string& tab = "");
	static void SerializeArray(std::ofstream& file, DataTree* tree, const std::string& tab = "");

	static void RemoveWhiteSpace(Consumer& consumer);

	static bool IsObjectValid(Consumer& consumer, DataTree* tree);
	static bool IsArrayValid(Consumer& consumer, DataTree* tree);

	static bool IsStringValid(std::string& str, Consumer& consumer);
	static bool IsNumberValid(std::string& str, DataTreeType& type, Consumer& consumer);
	static bool IsBooleanValid(std::string& str, Consumer& consumer);
	static bool IsNullValid(std::string& str, Consumer& consumer);
};