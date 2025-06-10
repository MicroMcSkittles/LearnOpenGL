#pragma once
#include "Renderer/VertexArray.h"
#include <stdint.h>
#include <glm/glm.hpp>
#include <map>

struct VoxelVertex {
	glm::vec3 position;
	glm::vec2 texCoord;
    glm::vec2 textureID;
    float faceID;

    VoxelVertex(glm::vec3 position = { 0,0,0 }, glm::vec2 texCoord = { 0,0 }, glm::vec2 textureID = { 0,0 }, float faceID = 0)
		:position(position), texCoord(texCoord), faceID(faceID), textureID(textureID) { }
};

enum VoxelFace {
    Front  = 0,
    Back   = 1,
    Left   = 2,
    Right  = 3,
    Top    = 4,
    Bottom = 5
};

struct Voxel {
	uint32_t ID;
    bool IsFoliage = false;

    glm::vec2 FrontTextureID = { 0,0 };
    glm::vec2 BackTextureID = { 0,0 };
    glm::vec2 LeftTextureID = { 0,0 };
    glm::vec2 RightTextureID = { 0,0 };
    glm::vec2 TopTextureID = { 0,0 };
    glm::vec2 BottomTextureID = { 0,0 };

    Voxel(uint32_t ID, bool IsFoliage, glm::vec2 Front, glm::vec2 Back, glm::vec2 Left, glm::vec2 Right, glm::vec2 Top, glm::vec2 Bottom)
        :ID(ID), IsFoliage(IsFoliage), 
        FrontTextureID(Front), BackTextureID(Back),
        LeftTextureID(Left), RightTextureID(Right),
        TopTextureID(Top), BottomTextureID(Bottom) { }
};

struct VoxelData {
    std::map<std::string, uint32_t> VoxelMap;
    std::vector<Voxel> VoxelInfo;
    std::vector<glm::vec2> VoxelPreview;
    std::vector<uint32_t> TransparentVoxels;

    // Index 0 of VoxelIndo is empty so BlockCount may be off
    uint32_t BlockCount;

    struct {
        uint32_t VerticesCount;
        VoxelVertex* VoxelVertices;

        uint32_t IndicesCount;
        uint32_t* VoxelIndices;
    } Front;

    struct {
        uint32_t VerticesCount;
        VoxelVertex* VoxelVertices;

        uint32_t IndicesCount;
        uint32_t* VoxelIndices;
    } Back;

    struct {
        uint32_t VerticesCount;
        VoxelVertex* VoxelVertices;

        uint32_t IndicesCount;
        uint32_t* VoxelIndices;
    } Left;

    struct {
        uint32_t VerticesCount;
        VoxelVertex* VoxelVertices;

        uint32_t IndicesCount;
        uint32_t* VoxelIndices;
    } Right;

    struct {
        uint32_t VerticesCount;
        VoxelVertex* VoxelVertices;

        uint32_t IndicesCount;
        uint32_t* VoxelIndices;
    } Top;

    struct {
        uint32_t VerticesCount;
        VoxelVertex* VoxelVertices;

        uint32_t IndicesCount;
        uint32_t* VoxelIndices;
    } Bottom;

    struct {
        uint32_t VerticesCount;
        VoxelVertex* VoxelVertices;

        uint32_t IndicesCount;
        uint32_t* VoxelIndices;
    } Foliage;

    VoxelData();
};
static VoxelData* s_VoxelData = new VoxelData();

VertexArray* GetVoxelMesh(uint32_t voxel);