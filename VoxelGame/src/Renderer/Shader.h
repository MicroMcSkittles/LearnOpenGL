#pragma once
#include "Core/Core.h"
#include "Renderer/RendererEnum.h"
#include <stdint.h>
#include <map>

enum ShaderType {
	None     = 0,
	Vertex   = BIT(0),
	Pixel    = BIT(1),
	Geometry = BIT(2)
};

struct ShaderConfig
{
	uint32_t type = ShaderType::Vertex | ShaderType::Pixel;
	bool source = false; // treats the following strings as source code when true

	std::string vertexShader;
	std::string pixelShader;
	std::string geometryShader;
};

class Shader {
public:
	Shader(const ShaderConfig& config);
	~Shader();

	void Bind();
	void Unbind();

	void SetUniform(const std::string& name, void* data);

	DataType GetUniformType(const std::string& name);
	void* GetUniformData(const std::string& name);

private:
	void FindUniforms();

private:
	ShaderConfig m_Config;

	struct Uniform {
		std::string name;
		DataType type;
		uint32_t location;
		void* data;
	};
	std::map<std::string, Uniform> m_Uniforms;

	uint32_t m_VertexShader;
	uint32_t m_PixelShader;
	uint32_t m_GeometryShader;

	uint32_t m_Handle;
};