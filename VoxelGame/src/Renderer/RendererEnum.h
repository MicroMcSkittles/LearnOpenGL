#pragma once
#include "Core/Core.h"

enum ClearMask {
	ColorBufferBit   = BIT(0),
	DepthBufferBit   = BIT(1),
	StencilBufferBit = BIT(2)
};
// Converts clear mask type to the OpenGL mask type
uint32_t GL_ClearMask(uint32_t mask);

enum class DrawMode {
	None = 0,
	Points,
	Patches,
	Lines,
	LineLoop,
	LineStrip,
	LinesAdjacency,
	LineStripAdjacency,
	Triangles,
	TriangleFan,
	TriangleStrip,
	TriangleStripAdjacency,
	TrianglesAdjacency
};
// Converts draw mode to the OpenGL draw mode
uint32_t GL_DrawMode(DrawMode mode);

enum class DataType {
	None = 0,
	UnsignedByte,
	UnsignedShort,
	UnsignedInt,
	UnsignedInt_24_8,
	Int,
	Int2,
	Int3,
	Int4,
	Float,
	Float2,
	Float3,
	Float4,
	Matrix2,
	Matrix3,
	Matrix4,
	Texture2D
};
// Converts data type to the OpenGL data type
uint32_t GL_DataType(DataType type);
// Converts OpenGL data type to data type
DataType DataType_GL(uint32_t type);
// Returns how many values are in a type
uint32_t DataTypeCount(DataType type);
// Returns the size of data type in bytes
uint32_t DataTypeSize(DataType type);

enum class Usage {
	None = 0,
	StaticDraw,
	StaticRead,
	StaticCopy,
	DynamicDraw,
	DynamicRead,
	DynamicCopy,
	StreamDraw,
	StreamRead,
	StreamCopy,
};
// Converts usage type to the OpenGL usage type
uint32_t GL_Usage(Usage usage);

enum class TexFormat
{
	None = 0,
	RED,
	RG,
	RGB,
	RGBA,
	DEPTH24_STENCIL8,
	DEPTH_STENCIL,
};
// Converts texture format to OpenGL texture format
uint32_t GL_TexFormat(TexFormat format);

enum class TexFilterMode
{
	None = 0,
	Nearest,
	Linear
};
// Converts texture filter mode to OpenGL texture filter mode
uint32_t GL_TexFilterMode(TexFilterMode mode);

enum class TexWrapMode
{
	None = 0,
	Repeat,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder
};
// Converts texture wrap mode to OpenGL texture wrap mode
uint32_t GL_TexWrapMode(TexWrapMode mode);