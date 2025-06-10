#include "Renderer/RenderAPI.h"
#include <string>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef DEBUG

std::string ClearMaskToString(uint32_t mask) {
	std::stringstream ss;
	ss << ((mask & ClearMask::ColorBufferBit) ? "( Color Buffer Bit ) " : " ");
	ss << ((mask & ClearMask::DepthBufferBit) ? "( Depth Buffer Bit ) " : " ");
	ss << ((mask & ClearMask::StencilBufferBit) ? "( Stencil Buffer Bit )" : " ");
	return ss.str();
}

std::string DrawModeToString(DrawMode mode) {
	switch (mode)
	{
	case DrawMode::Triangles: return "Triangles";
	}
	return "";
}

#endif

uint32_t GL_ClearMask(uint32_t mask) {
	uint32_t gl_mask = 0;
	gl_mask |= (mask & ClearMask::ColorBufferBit)   ? GL_COLOR_BUFFER_BIT   : 0;
	gl_mask |= (mask & ClearMask::DepthBufferBit)   ? GL_DEPTH_BUFFER_BIT   : 0;
	gl_mask |= (mask & ClearMask::StencilBufferBit) ? GL_STENCIL_BUFFER_BIT : 0;
	return gl_mask;
}

uint32_t GL_DrawMode(DrawMode mode) {
	switch (mode)
	{
	case DrawMode::Points: return GL_POINTS;
	case DrawMode::Patches: return GL_PATCHES;
	case DrawMode::Lines: return GL_LINES;
	case DrawMode::LineLoop: return GL_LINE_LOOP;
	case DrawMode::LineStrip: return GL_LINE_STRIP;
	case DrawMode::LinesAdjacency: return GL_LINES_ADJACENCY;
	case DrawMode::LineStripAdjacency: return GL_LINE_STRIP_ADJACENCY;
	case DrawMode::Triangles: return GL_TRIANGLES;
	case DrawMode::TriangleFan: return GL_TRIANGLE_FAN;
	case DrawMode::TriangleStrip: return GL_TRIANGLE_STRIP;
	case DrawMode::TrianglesAdjacency: return GL_TRIANGLES_ADJACENCY;
	case DrawMode::TriangleStripAdjacency: return GL_TRIANGLE_STRIP_ADJACENCY;
	default:
		WARNING("Draw mode not supported");
		return 0;
	}
}

uint32_t GL_DataType(DataType type) {
	switch (type)
	{
	case DataType::UnsignedByte:  return GL_UNSIGNED_BYTE;
	case DataType::UnsignedShort: return GL_UNSIGNED_SHORT;
	case DataType::UnsignedInt:   return GL_UNSIGNED_INT;
	case DataType::Int:			  return GL_INT;
	case DataType::Int2:		  return GL_INT_VEC2;
	case DataType::Int3:		  return GL_INT_VEC3;
	case DataType::Int4:		  return GL_INT_VEC4;
	case DataType::Float:         return GL_FLOAT;
	case DataType::Float2:        return GL_FLOAT;
	case DataType::Float3:        return GL_FLOAT;
	case DataType::Float4:        return GL_FLOAT;
	case DataType::Matrix2:       return GL_FLOAT_MAT2;
	case DataType::Matrix3:       return GL_FLOAT_MAT3;
	case DataType::Matrix4:       return GL_FLOAT_MAT4;
	case DataType::Texture2D:     return GL_SAMPLER_2D;
	default:
		WARNING("Unsupported data type");
		return 0;
	}
}
DataType DataType_GL(uint32_t type) {
	switch (type)
	{
	case GL_UNSIGNED_BYTE:  return DataType::UnsignedByte;
	case GL_UNSIGNED_SHORT: return DataType::UnsignedShort;
	case GL_UNSIGNED_INT:   return DataType::UnsignedInt;
	case GL_INT:			return DataType::Int;
	case GL_INT_VEC2:		return DataType::Int2;
	case GL_INT_VEC3:		return DataType::Int3;
	case GL_INT_VEC4:		return DataType::Int4;
	case GL_FLOAT:          return DataType::Float;
	case GL_FLOAT_VEC2:     return DataType::Float2;
	case GL_FLOAT_VEC3:     return DataType::Float3;
	case GL_FLOAT_VEC4:     return DataType::Float4;
	case GL_FLOAT_MAT2:     return DataType::Matrix2;
	case GL_FLOAT_MAT3:     return DataType::Matrix3;
	case GL_FLOAT_MAT4:     return DataType::Matrix4;
	case GL_SAMPLER_2D:     return DataType::Texture2D;
	default:
		WARNING("OpenGL data type not supported ( " + std::to_string(type) + " )");
		return DataType::None;
	}
}
uint32_t DataTypeCount(DataType type) {
	switch (type)
	{
	case DataType::UnsignedByte:  return 1;
	case DataType::UnsignedShort: return 1;
	case DataType::UnsignedInt:   return 1;
	case DataType::Float:         return 1;
	case DataType::Float2:        return 2;
	case DataType::Float3:        return 3;
	case DataType::Float4:        return 4;
	default:
		WARNING("Unsupported data type");
		return 0;
	}
}
uint32_t DataTypeSize(DataType type) {
	switch (type)
	{
	case DataType::UnsignedByte:  return sizeof(unsigned char)  * 1;
	case DataType::UnsignedShort: return sizeof(unsigned short) * 1;
	case DataType::UnsignedInt:   return sizeof(unsigned int)   * 1;
	case DataType::Float:         return sizeof(float)          * 1;
	case DataType::Float2:        return sizeof(float)          * 2;
	case DataType::Float3:        return sizeof(float)          * 3;
	case DataType::Float4:        return sizeof(float)          * 4;
	default:
		WARNING("Unsupported data type");
		return 0;
	}
}

uint32_t GL_Usage(Usage usage) {
	switch (usage)
	{
	case Usage::StaticDraw: return GL_STATIC_DRAW;
	case Usage::StaticRead: return GL_STATIC_READ;
	case Usage::StaticCopy: return GL_STATIC_COPY;
	case Usage::DynamicDraw: return GL_DYNAMIC_DRAW;
	case Usage::DynamicRead: return GL_DYNAMIC_READ;
	case Usage::DynamicCopy: return GL_DYNAMIC_COPY;
	case Usage::StreamDraw: return GL_STREAM_DRAW;
	case Usage::StreamRead: return GL_STREAM_READ;
	case Usage::StreamCopy: return GL_STREAM_COPY;
	default:
		WARNING("Usage not supported");
		return 0;
	}
}

uint32_t GL_TexFormat(TexFormat format) {
	switch (format)
	{
	case TexFormat::RED:              return GL_RED;
	case TexFormat::RG:               return GL_RG;
	case TexFormat::RGB:              return GL_RGB;
	case TexFormat::RGBA:             return GL_RGBA;
	case TexFormat::DEPTH24_STENCIL8: return GL_DEPTH24_STENCIL8;
	case TexFormat::DEPTH_STENCIL:    return GL_DEPTH_STENCIL;
	default:
		WARNING("Format not supported");
		return 0;
	}
}

uint32_t GL_TexFilterMode(TexFilterMode mode) {
	switch (mode)
	{
	case TexFilterMode::Nearest: return GL_NEAREST;
	case TexFilterMode::Linear:  return GL_LINEAR;
	default:
		WARNING("Filter mode not supported");
		return 0;
	}
}

uint32_t GL_TexWrapMode(TexWrapMode mode) {
	switch (mode)
	{
	case TexWrapMode::Repeat: return GL_REPEAT;
	case TexWrapMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
	case TexWrapMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
	case TexWrapMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
	default:
		WARNING("Wrap mode not supported");
		return 0;
	}
}

void RenderAPI::Init()
{
#ifdef DEBUG
	if (s_DiagnosticInstance) WARNING("Renderer API diagnostic instance already exists");
	s_DiagnosticInstance = new RenderAPIDiagnostic();
#endif
	if (s_Data) WARNING("Renderer API data already exists");
	s_Data = new RenderAPIData();

	// Initalize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
		FATAL_ERROR("Failed to Initialize GLAD");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glLineWidth(5);

	NORM_MESSAGE("Renderer API Initialized successfully");
}

RenderAPIDiagnostic* RenderAPI::GetDiagnostic()
{
#ifdef DEBUG
	return s_DiagnosticInstance;
#elif RELEASE
	return new RenderAPIDiagnostic();
#endif
}

void RenderAPI::SetViewPortSize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
	LOW_MESSAGE("View port resized to ( " + std::to_string(width) + ", " + std::to_string(height) + " )");
}

void RenderAPI::ClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	LOW_MESSAGE("Clear color set to ( " + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + " )");
}
void RenderAPI::ClearColor(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
	LOW_MESSAGE("Clear color set to ( " + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + ", " + std::to_string(color.a) + " )");
}

void RenderAPI::SetClearMask(uint32_t mask)
{
	s_Data->clearMask = GL_ClearMask(mask);
#ifdef DEBUG
	LOW_MESSAGE("Clear mask set to flags " + ClearMaskToString(mask));
#endif
}
void RenderAPI::Clear()
{
	glClear(s_Data->clearMask);
#ifdef DEBUG
	s_DiagnosticInstance->ClearCount += 1;
#endif
}

void RenderAPI::SetDrawMode(DrawMode mode)
{
	s_Data->drawMode = GL_DrawMode(mode);
#ifdef DEBUG
	LOW_MESSAGE("Draw mode set to ( " + DrawModeToString(mode) + " )");
#endif
}

void RenderAPI::DrawElements(uint32_t count, DataType type, const void* indices)
{
	glDrawElements(s_Data->drawMode, count, GL_DataType(type), indices);
#ifdef DEBUG
	s_DiagnosticInstance->DrawCallCount += 1;
#endif
}
