#pragma once
#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include "Core.h"

#define HEADER_SIZE  (sizeof(char) * 12)
#define LUMP_SIZE    (sizeof(char) * 16)

#define LINEDEF_SIZE (sizeof(char) * 14)
#define SIDEDEF_SIZE (sizeof(char) * 30)
#define VERTEX_SIZE  (sizeof(char) * 4)
#define SEG_SIZE     (sizeof(char) * 12)
#define SSECTOR_SIZE (sizeof(char) * 4)
#define NODE_SIZE    (sizeof(char) * 28)
#define SECTOR_SIZE  (sizeof(char) * 26)

struct Lump {
	uint32_t pointer;
	uint32_t size;
	std::string name;
	
	uint8_t* data;
};
struct WAD {
	// Header Data
	std::string signature;
	uint32_t lumpCount;
	uint32_t directoryPointer;

	// Everything else
	std::vector<Lump> lumps;
	std::map<std::string, uint32_t> markers;

	// Functions
	Lump& GetLump(const std::string& name, uint32_t startID = 0);
	bool lumpExists(const std::string& name, uint32_t startID = 0);
	uint32_t GetMarker(const std::string& name);
};

struct GameConfig {
	std::string title;
	std::string author;
	std::string description;
	std::string version;
	std::string iwad;
	std::vector<std::string> pwads;
	std::vector<std::string> playerTranslations;
	std::string executable;
	std::string mode;
	std::string options;
};

struct Vertex {
	uint16_t x;
	uint16_t y;
};
struct LineDef {
	uint16_t startVertex;
	uint16_t endVertex;
	uint16_t flags;
	uint16_t special;
	uint16_t tag;
	uint16_t frontSide;
	uint16_t backSide;
};
struct SideDef {
	uint16_t xOffset;
	uint16_t yOffset;
	std::string upperTexture;
	std::string middleTexture;
	std::string lowerTexture;
	uint16_t faceSector;
};
struct Segment {
	int16_t startVertex;
	int16_t endVertex;
	int16_t angle;
	int16_t lineDef;
	int16_t direction;
	int16_t offset;
};
struct SubSector {
	uint16_t segmentCount;
	uint16_t firstSegment;
};
struct Sector {
	uint16_t floorHeight;
	uint16_t ceilingHeight;
	std::string floorTexture;
	std::string ceilingTexture;
	uint16_t lightLevel;
	uint16_t specialType;
	uint16_t tagNumber;
};

struct BoundingBox {
	uint16_t top;
	uint16_t bottom;
	uint16_t left;
	uint16_t right;
};
struct Node {
	uint16_t x;
	uint16_t y;
	uint16_t xDelta;
	uint16_t yDelta;
	BoundingBox rightBox;
	BoundingBox leftBox;
	uint16_t rightChild;
	uint16_t leftChild;
};

struct Map {
	std::string name;

	std::vector<Vertex> vertices;
	std::vector<LineDef> lineDefs;
	std::vector<SideDef> sideDefs;
	std::vector<Segment> segments;
	std::vector<SubSector> subSectors;
	std::vector<SubSector> sectors;

	std::vector<Node> nodes;
};

void LoadWAD(const std::string& path, WAD* wad);
void LoadMap(WAD* wad, Map* map);
void LoadGameConfig(WAD* wad, GameConfig* config);

std::string GenerateConsoleText(WAD* wad, Map* map, GameConfig* config);