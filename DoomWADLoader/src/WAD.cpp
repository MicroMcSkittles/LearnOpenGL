#include "WAD.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include "JSON.h"

void read(uint8_t& value, uint8_t* data, uint32_t& offset) {
	value = data[offset];
	offset += sizeof(uint8_t);
}
void read(uint16_t& value, uint8_t* data, uint32_t& offset) {
	value = (data[offset + 1] << 8) + data[offset];
	offset += sizeof(uint16_t);
}
void read(uint32_t& value, uint8_t* data, uint32_t& offset) {
	value = (((uint32_t)data[offset + 3]) << 24) | (((uint32_t)data[offset + 2]) << 16) | (((uint32_t)data[offset + 1]) << 8) | (uint32_t)data[offset];
	offset += sizeof(uint32_t);
}
void read(std::string& value, uint32_t size, uint8_t* data, uint32_t& offset) {
	for (int i = 0; i < size; ++i) {
		value.push_back(data[offset + i]);
	}
	offset += sizeof(uint8_t) * value.size();
}

void LoadHeader(uint8_t* data, WAD* wad) {

	uint32_t offset = 0;

	read(wad->signature, 4, data, offset);
	read(wad->lumpCount, data, offset);
	read(wad->directoryPointer, data, offset);
}

void LoadLump(uint8_t* data, uint32_t& offset, WAD* wad) {

	Lump lump;

	read(lump.pointer, data, offset);
	read(lump.size, data, offset);

	uint8_t c = NULL;
	for (int i = 0; i < 8; ++i) {
		read(c, data, offset);
		if (c == '\0') break;
		lump.name.push_back(c);
		//if (lump.name == "LINEDEFS")
		//	break;
	}

	if (lump.size != 0) {
		lump.data = (uint8_t*)malloc(lump.size * sizeof(uint8_t));
		memcpy(lump.data, data + lump.pointer, lump.size * sizeof(uint8_t));
	}
	else {
		wad->markers.insert({ lump.name, wad->lumps.size() });
	}

	wad->lumps.push_back(lump);
}

void LoadWAD(const std::string& path, WAD* wad)
{
	std::ifstream file;
	file.open(path, std::ios::binary);
	if (!file.is_open()) { FATAL_ERROR("Failed to load WAD file ( " + path + " )") }

	file.seekg(0, std::ios::end);
	uint32_t size = file.tellg();
	file.seekg(0, std::ios::beg);

	uint8_t* data = (uint8_t*)malloc(size);
	if (!data) { FATAL_ERROR("Failed to allocate memory for WAD data") }
	file.read(reinterpret_cast<char*>(data), size);

	LoadHeader(data, wad);

	for (int i = 0; i < wad->lumpCount; ++i) {
		uint32_t offset = wad->directoryPointer + (i * LUMP_SIZE);
		LoadLump(data, offset, wad);
	}

	file.close();
}

void LoadMap(WAD* wad, Map* map)
{
	uint32_t mapMarker = wad->GetMarker(map->name);
	Lump& lineDefsLump = wad->GetLump("LINEDEFS", mapMarker);
	Lump& sideDefsLump = wad->GetLump("SIDEDEFS", mapMarker);
	Lump& vertexLump   = wad->GetLump("VERTEXES", mapMarker);
	Lump& segsLump     = wad->GetLump("SEGS",     mapMarker);
	Lump& sSectorsLump = wad->GetLump("SSECTORS", mapMarker);
	Lump& nodesLump    = wad->GetLump("NODES",    mapMarker);
	Lump& sectorsLump  = wad->GetLump("SECTORS",  mapMarker);

	// Line Defs
	{
		uint32_t count = lineDefsLump.size / LINEDEF_SIZE;

		for (int i = 0; i < count; ++i) {
			LineDef lineDef;

			uint32_t loc = lineDefsLump.pointer + (i * LINEDEF_SIZE);
			read(lineDef.startVertex, lineDefsLump.data, loc);
			read(lineDef.endVertex,   lineDefsLump.data, loc);
			read(lineDef.flags,       lineDefsLump.data, loc);
			read(lineDef.special,     lineDefsLump.data, loc);
			read(lineDef.tag,         lineDefsLump.data, loc);
			read(lineDef.frontSide,   lineDefsLump.data, loc);
			read(lineDef.backSide,    lineDefsLump.data, loc);

			map->lineDefs.push_back(lineDef);
		}
	}

	// Vertices
	{
		uint32_t count = vertexLump.size / VERTEX_SIZE;

		for (int i = 0; i < count; ++i) {
			Vertex vertex;

			uint32_t loc = lineDefsLump.pointer + (i * VERTEX_SIZE);
			read(vertex.x, lineDefsLump.data, loc);
			read(vertex.y, lineDefsLump.data, loc);

			map->vertices.push_back(vertex);
		}
	}
}

void LoadGameConfig(WAD* wad, GameConfig* config)
{
	if (!wad->lumpExists("GAMECONF")) return;
	Lump& gameConfigLump = wad->GetLump("GAMECONF");

	std::stringstream buffer;
	buffer << (char*)gameConfigLump.data;

	std::string JSONSource;

	int bracketCount = 1;
	char c;
	buffer >> std::noskipws;
	buffer >> c;
	while (c) {
		JSONSource.push_back(c);
		buffer >> c;
		if (c == '{') bracketCount += 1;
		if (c == '}') {
			bracketCount -= 1;
			if (bracketCount == 0)
				break;
		}
	}
	if (c != '}') { FATAL_ERROR("Expected } in JSON source") }
	JSONSource.push_back(c);

	JSON::Data configData;
	JSON::ParseJSONData(JSONSource, &configData);
	JSON::Data& data = configData["data"];

	config->title = data["title"].ReadString();
	config->author = data["author"].ReadString();
	config->description = data["description"].ReadString();
	config->version = data["version"].ReadString();
	config->iwad = data["iwad"].ReadString();
	config->executable = data["executable"].ReadString();
	config->mode = data["mode"].ReadString();
	config->options = data["options"].ReadString();
}

std::string GenerateConsoleText(WAD* wad, Map* map, GameConfig* config)
{
	std::stringstream desc;
	if (!config->title.empty())       desc << "Title: "       << config->title << std::endl;
	if (!config->author.empty())      desc << "Author: "      << config->author << std::endl;
	if (!config->description.empty()) desc << "Description: " << config->description << std::endl;
	if (!config->version.empty())     desc << "Version: "     << config->version << std::endl;
	if (!config->executable.empty())  desc << "Executable: "  << config->executable << std::endl;
	if (!config->mode.empty())        desc << "Mode: "        << config->mode << std::endl;
	if (!config->options.empty())     desc << "Options: "     << config->options << std::endl;
	desc << std::endl;

	desc << "WAD data =====================" << std::endl;
	desc << "Signature: " << wad->signature << std::endl;
	desc << "Lump Count: " << wad->lumpCount << std::endl;
	desc << "Directory Pointer: " << wad->directoryPointer << std::endl;
	desc << std::endl;
	for (int i = wad->GetMarker(map->name) + 1; i < wad->lumpCount; ++i) {
		if (wad->lumps[i].size == 0) break;
		desc << std::left << std::setw(30) << std::setfill('=') << (wad->lumps[i].name + " ") << std::endl;
		desc << "Size (In Bytes): " << wad->lumps[i].size << std::endl;
		desc << "Pointer: " << wad->lumps[i].pointer << std::endl;
		desc << std::endl;
	}
	desc << std::endl;

	return desc.str();
}

Lump& WAD::GetLump(const std::string& name, uint32_t startID)
{
	for (int i = startID; i < lumps.size(); ++i) {
		if (lumps[i].name == name) return lumps[i];
	}
	FATAL_ERROR("Lump doesn't exist ( " + name + " )")
}
bool WAD::lumpExists(const std::string& name, uint32_t startID)
{
	for (int i = startID; i < lumps.size(); ++i) {
		if (lumps[i].name == name) return true;
	}
	return false;
}
uint32_t WAD::GetMarker(const std::string& name)
{
	if (!markers.count(name)) {
		SOFT_ERROR("Marker doesn't exist ( " + name + " )");
		return 0;
	}
	return markers[name];
}
