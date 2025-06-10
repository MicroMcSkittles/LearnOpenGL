#include "JSON.h"
#include "Core/System.h"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

void json::RemoveWhiteSpace(Consumer& consumer) {
	bool quoteFlag = false; // don't remove spaces inside quotes
	char lastChar = ' ';
	for (auto& c : consumer.source) {
		if (c == '\"' && lastChar != '\\') quoteFlag = !quoteFlag;
		if (isspace(c) && !quoteFlag) continue;
		consumer.buffer << c;
		lastChar = c;
	}
	if (quoteFlag) {
		FATAL_ERROR("Expected \"");
	}
}

bool json::IsObjectValid(Consumer& consumer, DataTree* tree)
{
	if (consumer.peek() != '{') {
		return false;
	}
	consumer.eat();

	tree->type = DataTreeType::Object;

	std::string name;
	while (IsStringValid(name, consumer)) {
		if (consumer.peek() != ':') {
			FATAL_ERROR("Expected : after ( " + name + " )");
		}
		consumer.eat();

		DataTree child;
		if (IsStringValid(child.value, consumer)) child.type = DataTreeType::String;
		else if (IsNumberValid(child.value, child.type, consumer));
		else if (IsBooleanValid(child.value, consumer)) child.type = DataTreeType::Boolean;
		else if (IsNullValid(child.value, consumer)) child.type = DataTreeType::Null;
		else if (IsObjectValid(consumer, &child));
		else if (IsArrayValid(consumer, &child));

		tree->children.insert({ name, child });
		name = "";
		if (consumer.peek() == ',') {
			consumer.eat();
			continue;
		}
		if (consumer.peek() == '}') break;
		break;
	}
	if (consumer.peek() != '}') {
		FATAL_ERROR("Expected }");
	}
	consumer.eat();
	return true;
}

bool json::IsArrayValid(Consumer& consumer, DataTree* tree)
{
	if (consumer.peek() != '[') {
		return false;
	}
	consumer.eat();

	tree->type = DataTreeType::Array;

	while (true) {

		DataTree child;
		if (IsStringValid(child.value, consumer)) child.type = DataTreeType::String;
		else if (IsNumberValid(child.value, child.type, consumer));
		else if (IsBooleanValid(child.value, consumer)) child.type = DataTreeType::Boolean;
		else if (IsNullValid(child.value, consumer)) child.type = DataTreeType::Null;
		else if (IsObjectValid(consumer, &child));
		else if (IsArrayValid(consumer, &child));

		tree->elements.push_back(child );
		if (consumer.peek() == ',') {
			consumer.eat();
			continue;
		}
		if (consumer.peek() == ']') break;
		break;
	}
	if (consumer.eat() != ']') {
		FATAL_ERROR("Expected ]");
	}
	return true;
}

bool json::IsStringValid(std::string& str, Consumer& consumer)
{
	if(consumer.peek() != '\"') return false;
	consumer.eat();

	char c;
	while (c = consumer.eat()) {
		if (c == '\"') break;
		str.push_back(c);
	}
	if (c != '\"') {
		FATAL_ERROR("Expected \"");
	}
	return true;
}
bool json::IsNumberValid(std::string& str, DataTreeType& type, Consumer& consumer)
{
	if(!isdigit(consumer.peek()) && consumer.peek() != '-') return false;
	type = DataTreeType::Int;
	char c;
	while (c = consumer.peek()) {
		if (isdigit(c) || c == '.' || c == '-') {
			str.push_back(c);
			if(c == '.')
				type = DataTreeType::Float;
			consumer.eat();
			continue;
		}
		break;
	}
	return true;
}
bool json::IsBooleanValid(std::string& str, Consumer& consumer)
{
	if(consumer.peek() != 't' && consumer.peek() != 'f') return false;
	str.push_back(consumer.eat());
	if (str[0] == 't') {
		std::string test = "rue";
		for (int i = 0; i < 3; ++i) {
			if (consumer.peek() != test[i]) {
				FATAL_ERROR("Did you mean true?");
			}
			str.push_back(consumer.eat());
		}
	}
	else {
		std::string test = "alse";
		for (int i = 0; i < 4; ++i) {
			if (consumer.peek() != test[i]) {
				FATAL_ERROR("Did you mean false?");
			}
			str.push_back(consumer.eat());
		}
	}
	return true;
}
bool json::IsNullValid(std::string& str, Consumer& consumer)
{
	if (consumer.peek() != 'n') return false;
	str.push_back(consumer.eat());
	std::string test = "ull";
	for (int i = 0; i < 3; ++i) {
		if (consumer.peek() != test[i]) {
			FATAL_ERROR("Did you mean null?");
		}
		str.push_back(consumer.eat());
	}

	return true;
}

void json::ParseSource(const std::string& source, DataTree* tree) {
	Consumer consumer;
	consumer.source = source;
	RemoveWhiteSpace(consumer);
	consumer.buffer >> std::noskipws;

	if (!IsObjectValid(consumer, tree)) {
		FATAL_ERROR("Invalid object");
	}
}
void json::LoadFile(const std::string& filename, DataTree* tree) {
	std::string source;
	System::LoadStringFromFile(source, filename);
	ParseSource(source, tree);
}

void json::Serialize(const std::string& filename, DataTree* tree) {
	std::ofstream file;
	file.open(filename);
	if (!file.is_open()) {
		WARNING("Failed to open file ( " + filename + " )");
		return;
	}

	if (tree->type != DataTreeType::Object) {
		WARNING("Failed to save to file ( " + filename + " ) Invalid data tree");
		return;
	}

	std::string tab;
	tab.push_back((char)9);
	SerializeObject(file, tree, tab);

	file.close();
}
void json::SerializeObject(std::ofstream& file, DataTree* tree, const std::string& tab)
{
	file << "{\n";
	for (auto& [name, child] : tree->children) {
		std::string nameHeader = "\"" + name + "\": ";
		file << tab << nameHeader;
		if (child.type == DataTreeType::String) {
			file << "\"" << child.value << "\"";
		}
		else if (child.type == DataTreeType::Array) {
			SerializeArray(file, &child, tab + (char)9);
		}
		else if (child.type == DataTreeType::Object) {
			SerializeObject(file, &child, tab + (char)9);
		}
		else {
			file << child.value;
		}
		if (name != std::prev(tree->children.end())->first) {
			file << "," << std::endl;
		}
	}
	file << std::endl << std::string(tab.begin(), tab.end() - 1) << "}";
}
void json::SerializeArray(std::ofstream& file, DataTree* tree, const std::string& tab)
{
	file << "[" << std::endl;
	for (int i = 0; i < tree->elements.size(); ++i) {
		auto& child = tree->elements[i];
		file << tab;
		if (child.type == DataTreeType::String) {
			file << "\"" << child.value << "\"";
		}
		else if (child.type == DataTreeType::Array) {
			SerializeArray(file, &child, tab + (char)9);
		}
		else if (child.type == DataTreeType::Object) {
			SerializeObject(file, &child, tab + (char)9);
		}
		else {
			file << child.value;
		}
		if (i != tree->elements.size() - 1) {
			file << "," << std::endl;
		}
	}
	file << std::endl << std::string(tab.begin(), tab.end() - 1) << "]";
}

void json::ImGuiInputDataTree(DataTree* tree, const std::string& name, int depthID)
{
	if (tree->type == DataTreeType::Object) {
		if (ImGui::TreeNode((name + std::to_string(depthID)).c_str(), name.c_str())) {
			int id = 0;
			for (auto& [name, child] : tree->children) {
				id += 1;
				ImGuiInputDataTree(&child, name, depthID + 1);
			}
			ImGui::TreePop();
		}
		return;
	}

	if (tree->type == DataTreeType::Array) {
		if (ImGui::TreeNode((name + std::to_string(depthID)).c_str(), name.c_str())) {
			int id = 0;
			for (auto& element : tree->elements) {
				id += 1;
				ImGuiInputDataTree(&element, std::to_string(id), depthID + 1);
			}
			ImGui::TreePop();
		}
		return;
	}

	if (tree->type == DataTreeType::String) {
		ImGui::InputText(name.c_str(), &tree->value);
		return;
	}
	if (tree->type == DataTreeType::Int) {
		int i = std::stoi(tree->value);
		ImGui::InputInt(name.c_str(), &i);
		if (std::stoi(tree->value) != i) tree->value = std::to_string(i);
		return;
	}
	if (tree->type == DataTreeType::Float) {
		float i = std::stof(tree->value);
		ImGui::InputFloat(name.c_str(), &i);
		if (std::stof(tree->value) != i) tree->value = std::to_string(i);
		return;
	}
	if (tree->type == DataTreeType::Boolean) {
		bool i = tree->value == "true";
		ImGui::Checkbox(name.c_str(), &i);
		if ((tree->value == "true") != i) tree->value = (i) ? "true" : "false";
		return;
	}
	if (tree->type == DataTreeType::Null) {
		ImGui::Text("s% Null", name.c_str());
		return;
	}
}


