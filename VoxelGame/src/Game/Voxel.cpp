#include "Game/Voxel.h"
#include "Core/JSON.h"
#include <algorithm>

VoxelData::VoxelData()
{
    // Block data
    {
        auto index = [](const glm::vec2& pos) { return (uint32_t)(pos.x + 10 * pos.y); };

        VoxelInfo.push_back({ 0, false,{0,0},{0,0},{0,0},{0,0},{0,0},{0,0} });
        VoxelPreview.push_back({ 9,15 });

        DataTree data;
        json::LoadFile("userdata/config.json", &data);
        DataTree& BlockData = data["Game"]["BlockSettings"];
        
        for (auto& block : BlockData.elements) {
            int count = block.elements.size();
            if (count == 5) {
                std::string name = block.At(0).GetValue();
                glm::vec2 previewTexture = {
                    (int)block.At(1).At(0).GetValue(),
                    (int)block.At(1).At(1).GetValue()
                };
                glm::vec2 sideTexture = {
                    (int)block.At(2).At(0).GetValue(),
                    (int)block.At(2).At(1).GetValue()
                };
                bool isFoliage = block.At(3).GetValue();
                bool isTransparent = block.At(4).GetValue();
                if (isFoliage) TransparentVoxels.push_back((uint32_t)VoxelInfo.size());
                if (isTransparent) TransparentVoxels.push_back((uint32_t)VoxelInfo.size());
                VoxelMap.insert({ name, (uint32_t)VoxelInfo.size() });
                VoxelInfo.push_back({ (uint32_t)VoxelInfo.size(), isFoliage, sideTexture,sideTexture,sideTexture,sideTexture,sideTexture,sideTexture });
                VoxelPreview.push_back(previewTexture);
            }
            else if (count == 7) {
                std::string name = block.At(0).GetValue();
                glm::vec2 previewTexture = {
                    (int)block.At(1).At(0).GetValue(),
                    (int)block.At(1).At(1).GetValue()
                };
                glm::vec2 sideTexture = {
                    (int)block.At(2).At(0).GetValue(),
                    (int)block.At(2).At(1).GetValue()
                };
                glm::vec2 topTexture = {
                    (int)block.At(3).At(0).GetValue(),
                    (int)block.At(3).At(1).GetValue()
                };
                glm::vec2 bottomTexture = {
                    (int)block.At(4).At(0).GetValue(),
                    (int)block.At(4).At(1).GetValue()
                };
                bool isFoliage = block.At(5).GetValue();
                bool isTransparent = block.At(6).GetValue();
                if (isFoliage) TransparentVoxels.push_back((uint32_t)VoxelInfo.size());
                if (isTransparent) TransparentVoxels.push_back((uint32_t)VoxelInfo.size());
                VoxelMap.insert({ name, (uint32_t)VoxelInfo.size() });
                VoxelInfo.push_back({ (uint32_t)VoxelInfo.size(), isFoliage, sideTexture,sideTexture,sideTexture,sideTexture,topTexture,bottomTexture });
                VoxelPreview.push_back(previewTexture);
            }

        }

        BlockCount = VoxelInfo.size();
    }

    // Front Face
    {
        VoxelVertex Vertices[] = {
            { { -0.5f, -0.5f,  0.5f },  { 0.0f, 0.0f }, { 0,0 }, VoxelFace::Front },
            { {  0.5f, -0.5f,  0.5f },  { 1.0f, 0.0f }, { 0,0 }, VoxelFace::Front },
            { {  0.5f,  0.5f,  0.5f },  { 1.0f, 1.0f }, { 0,0 }, VoxelFace::Front },
            { { -0.5f,  0.5f,  0.5f },  { 0.0f, 1.0f }, { 0,0 }, VoxelFace::Front },
        };
        uint32_t Indices[] = {
            0, 1, 2,
            2, 3 ,0,
        };
        
        Front.VerticesCount = sizeof(Vertices) / sizeof(VoxelVertex);
        Front.IndicesCount  = sizeof(Indices)  / sizeof(uint32_t);
        
        Front.VoxelVertices = new VoxelVertex[Front.VerticesCount];
        Front.VoxelIndices =  (uint32_t*)malloc(sizeof(Indices));
        
        memcpy(Front.VoxelVertices, Vertices, sizeof(Vertices));
        std::copy(std::begin(Indices), std::end(Indices), Front.VoxelIndices);
    }

    // Back Face
    {
        VoxelVertex Vertices[] = {
            { { -0.5f, -0.5f, -0.5f },  { 1.0f, 0.0f }, { 0,0 }, VoxelFace::Back },
            { {  0.5f, -0.5f, -0.5f },  { 0.0f, 0.0f }, { 0,0 }, VoxelFace::Back },
            { {  0.5f,  0.5f, -0.5f },  { 0.0f, 1.0f }, { 0,0 }, VoxelFace::Back },
            { { -0.5f,  0.5f, -0.5f },  { 1.0f, 1.0f }, { 0,0 }, VoxelFace::Back },
        };
        uint32_t Indices[] = {
            0, 3, 2,
            2, 1, 0,
        };

        Back.VerticesCount = sizeof(Vertices) / sizeof(VoxelVertex);
        Back.IndicesCount = sizeof(Indices) / sizeof(uint32_t);

        Back.VoxelVertices = new VoxelVertex[Back.VerticesCount];
        Back.VoxelIndices = (uint32_t*)malloc(sizeof(Indices));

        memcpy(Back.VoxelVertices, Vertices, sizeof(Vertices));
        std::copy(std::begin(Indices), std::end(Indices), Back.VoxelIndices);
    }

    // Left Face
    {
        VoxelVertex Vertices[] = {
            { { -0.5f,  0.5f, -0.5f },  { 0.0f, 1.0f }, { 0,0 }, VoxelFace::Left },
            { { -0.5f, -0.5f, -0.5f },  { 0.0f, 0.0f }, { 0,0 }, VoxelFace::Left },
            { { -0.5f, -0.5f,  0.5f },  { 1.0f, 0.0f }, { 0,0 }, VoxelFace::Left },
            { { -0.5f,  0.5f,  0.5f },  { 1.0f, 1.0f }, { 0,0 }, VoxelFace::Left },
        };
        uint32_t Indices[] = {
            3, 0, 1,
            1, 2, 3,
        };

        Left.VerticesCount = sizeof(Vertices) / sizeof(VoxelVertex);
        Left.IndicesCount = sizeof(Indices) / sizeof(uint32_t);

        Left.VoxelVertices = new VoxelVertex[Left.VerticesCount];
        Left.VoxelIndices = (uint32_t*)malloc(sizeof(Indices));

        memcpy(Left.VoxelVertices, Vertices, sizeof(Vertices));
        std::copy(std::begin(Indices), std::end(Indices), Left.VoxelIndices);
    }

    // Right Face
    {
        VoxelVertex Vertices[] = {
            { {  0.5f, -0.5f, -0.5f },  { 1.0f, 0.0f }, { 0,0 }, VoxelFace::Right },
            { {  0.5f,  0.5f, -0.5f },  { 1.0f, 1.0f }, { 0,0 }, VoxelFace::Right },
            { {  0.5f,  0.5f,  0.5f },  { 0.0f, 1.0f }, { 0,0 }, VoxelFace::Right },
            { {  0.5f, -0.5f,  0.5f },  { 0.0f, 0.0f }, { 0,0 }, VoxelFace::Right },
        };
        uint32_t Indices[] = {
            0, 1, 2,
            2, 3, 0,
        };

        Right.VerticesCount = sizeof(Vertices) / sizeof(VoxelVertex);
        Right.IndicesCount = sizeof(Indices) / sizeof(uint32_t);

        Right.VoxelVertices = new VoxelVertex[Right.VerticesCount];
        Right.VoxelIndices = (uint32_t*)malloc(sizeof(Indices));

        memcpy(Right.VoxelVertices, Vertices, sizeof(Vertices));
        std::copy(std::begin(Indices), std::end(Indices), Right.VoxelIndices);
    }

    // Top Face
    {
        VoxelVertex Vertices[] = {
            { {  0.5f,  0.5f, -0.5f },  { 0.0f, 0.0f }, { 0,0 }, VoxelFace::Top },
            { { -0.5f,  0.5f, -0.5f },  { 1.0f, 0.0f }, { 0,0 }, VoxelFace::Top },
            { { -0.5f,  0.5f,  0.5f },  { 1.0f, 1.0f }, { 0,0 }, VoxelFace::Top },
            { {  0.5f,  0.5f,  0.5f },  { 0.0f, 1.0f }, { 0,0 }, VoxelFace::Top },
        };
        uint32_t Indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        Top.VerticesCount = sizeof(Vertices) / sizeof(VoxelVertex);
        Top.IndicesCount = sizeof(Indices) / sizeof(uint32_t);

        Top.VoxelVertices = new VoxelVertex[Top.VerticesCount];
        Top.VoxelIndices = (uint32_t*)malloc(sizeof(Indices));

        memcpy(Top.VoxelVertices, Vertices, sizeof(Vertices));
        std::copy(std::begin(Indices), std::end(Indices), Top.VoxelIndices);
    }

    // Bottom Face
    {
        VoxelVertex Vertices[] = {
            { { -0.5f, -0.5f, -0.5f },  { 0.0f, 0.0f }, { 0,0 }, VoxelFace::Bottom },
            { {  0.5f, -0.5f, -0.5f },  { 1.0f, 0.0f }, { 0,0 }, VoxelFace::Bottom },
            { {  0.5f, -0.5f,  0.5f },  { 1.0f, 1.0f }, { 0,0 }, VoxelFace::Bottom },
            { { -0.5f, -0.5f,  0.5f },  { 0.0f, 1.0f }, { 0,0 }, VoxelFace::Bottom },
        };
        uint32_t Indices[] = {
            0, 1, 2,
            2, 3, 0,
        };

        Bottom.VerticesCount = sizeof(Vertices) / sizeof(VoxelVertex);
        Bottom.IndicesCount = sizeof(Indices) / sizeof(uint32_t);

        Bottom.VoxelVertices = new VoxelVertex[Bottom.VerticesCount];
        Bottom.VoxelIndices = (uint32_t*)malloc(sizeof(Indices));

        memcpy(Bottom.VoxelVertices, Vertices, sizeof(Vertices));
        std::copy(std::begin(Indices), std::end(Indices), Bottom.VoxelIndices);
    }

    // Foliage
    {
        VoxelVertex Vertices[] = {
            { { -0.5f, -0.5f,  -0.5f },  { 0.0f, 0.0f }, { 0,0 }, VoxelFace::Front },
            { {  0.5f, -0.5f,   0.5f },  { 1.0f, 0.0f }, { 0,0 }, VoxelFace::Front },
            { {  0.5f,  0.5f,   0.5f },  { 1.0f, 1.0f }, { 0,0 }, VoxelFace::Front },
            { { -0.5f,  0.5f,  -0.5f },  { 0.0f, 1.0f }, { 0,0 }, VoxelFace::Front },

            { { -0.5f, -0.5f,   0.5f },  { 0.0f, 0.0f }, { 0,0 }, VoxelFace::Front },
            { {  0.5f, -0.5f,  -0.5f },  { 1.0f, 0.0f }, { 0,0 }, VoxelFace::Front },
            { {  0.5f,  0.5f,  -0.5f },  { 1.0f, 1.0f }, { 0,0 }, VoxelFace::Front },
            { { -0.5f,  0.5f,   0.5f },  { 0.0f, 1.0f }, { 0,0 }, VoxelFace::Front },
        };
        uint32_t Indices[] = {
            0, 1, 2,
            2, 3, 0,

            2, 1, 0,
            0, 3, 2,

            4, 5, 6,
            6, 7, 4,

            6, 5, 4,
            4, 7, 6,
        };

        Foliage.VerticesCount = sizeof(Vertices) / sizeof(VoxelVertex);
        Foliage.IndicesCount = sizeof(Indices) / sizeof(uint32_t);

        Foliage.VoxelVertices = new VoxelVertex[Foliage.VerticesCount];
        Foliage.VoxelIndices = (uint32_t*)malloc(sizeof(Indices));

        memcpy(Foliage.VoxelVertices, Vertices, sizeof(Vertices));
        std::copy(std::begin(Indices), std::end(Indices), Foliage.VoxelIndices);
    }
}

VertexArray* GetVoxelMesh(uint32_t voxel)
{
    //Voxel& v = s_VoxelData->VoxelInfo[voxel];

    //std::vector<VoxelVertex> vertexData;
    //std::vector<uint32_t> indexData;
    //uint32_t index = 0;

    //// Front Face 
    //for (int i = 0; i < s_VoxelData->Front.VerticesCount; ++i) {
    //    VoxelVertex vertex = s_VoxelData->Front.VoxelVertices[i];
    //    vertex.textureID = (float)v.FrontTextureID;
    //    vertexData.push_back(vertex);
    //}
    //for (int i = 0; i < s_VoxelData->Front.IndicesCount; ++i) {
    //    indexData.push_back(s_VoxelData->Front.VoxelIndices[i] + index);
    //}
    //index += s_VoxelData->Front.VerticesCount;
    //

    //// Back Face 
    //
    //for (int i = 0; i < s_VoxelData->Back.VerticesCount; ++i) {
    //    VoxelVertex vertex = s_VoxelData->Back.VoxelVertices[i];
    //    vertex.textureID = (float)v.BackTextureID;
    //    vertexData.push_back(vertex);
    //}
    //for (int i = 0; i < s_VoxelData->Back.IndicesCount; ++i) {
    //    indexData.push_back(s_VoxelData->Back.VoxelIndices[i] + index);
    //}
    //index += s_VoxelData->Back.VerticesCount;
    //

    //// Left Face 
    //
    //for (int i = 0; i < s_VoxelData->Left.VerticesCount; ++i) {
    //    VoxelVertex vertex = s_VoxelData->Left.VoxelVertices[i];
    //    vertex.textureID = (float)v.LeftTextureID;
    //    vertexData.push_back(vertex);
    //}
    //for (int i = 0; i < s_VoxelData->Left.IndicesCount; ++i) {
    //    indexData.push_back(s_VoxelData->Left.VoxelIndices[i] + index);
    //}
    //index += s_VoxelData->Left.VerticesCount;
    //

    //// Right Face 
    //
    //for (int i = 0; i < s_VoxelData->Right.VerticesCount; ++i) {
    //    VoxelVertex vertex = s_VoxelData->Right.VoxelVertices[i];
    //    vertex.textureID = (float)v.RightTextureID;
    //    vertexData.push_back(vertex);
    //}
    //for (int i = 0; i < s_VoxelData->Right.IndicesCount; ++i) {
    //    indexData.push_back(s_VoxelData->Right.VoxelIndices[i] + index);
    //}
    //index += s_VoxelData->Right.VerticesCount;
    //

    //// Top Face 
    //
    //for (int i = 0; i < s_VoxelData->Top.VerticesCount; ++i) {
    //    VoxelVertex vertex = s_VoxelData->Top.VoxelVertices[i];
    //    vertex.textureID = (float)v.TopTextureID;
    //    vertexData.push_back(vertex);
    //}
    //for (int i = 0; i < s_VoxelData->Top.IndicesCount; ++i) {
    //    indexData.push_back(s_VoxelData->Top.VoxelIndices[i] + index);
    //}
    //index += s_VoxelData->Top.VerticesCount;
    //

    //// Bottom Face 
    //for (int i = 0; i < s_VoxelData->Bottom.VerticesCount; ++i) {
    //    VoxelVertex vertex = s_VoxelData->Bottom.VoxelVertices[i];
    //    vertex.textureID = (float)v.BottomTextureID;
    //    vertexData.push_back(vertex);
    //}
    //for (int i = 0; i < s_VoxelData->Bottom.IndicesCount; ++i) {
    //    indexData.push_back(s_VoxelData->Bottom.VoxelIndices[i] + index);
    //}
    //index += s_VoxelData->Bottom.VerticesCount;

    //VertexArray* mesh = new VertexArray();
    //mesh->Bind();

    //VertexBuffer* vbo = new VertexBuffer(VertexLayout({ DataType::Float3, DataType::Float2, DataType::Float, DataType::Float }));
    //vbo->SetData<VoxelVertex>(&vertexData[0], vertexData.size() * sizeof(VoxelVertex));
    //vbo->Initialize();
    //mesh->AttachBuffer(vbo);

    //ElementBuffer* ebo = new ElementBuffer();
    //ebo->SetData(&indexData[0], indexData.size() * sizeof(uint32_t));
    //ebo->Initialize();
    //mesh->AttachBuffer(ebo);

    //mesh->Unbind();

    //return mesh;
    return nullptr;
}
