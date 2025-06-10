#include "Shader.h"
#include "Core/System.h"
#include "Renderer/Texture.h"
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

Shader::Shader(const ShaderConfig& config)
{
	m_Config = config;

	m_VertexShader = 0;
	m_PixelShader = 0;
	m_GeometryShader = 0;

	if (m_Config.type & ShaderType::Vertex) {
		m_VertexShader = glCreateShader(GL_VERTEX_SHADER);

		std::string source;
		if (!m_Config.source) System::LoadStringFromFile(source, m_Config.vertexShader);
		else source = m_Config.vertexShader;
		const char* cSource = source.c_str();

		glShaderSource(m_VertexShader, 1, &cSource, NULL);
		glCompileShader(m_VertexShader);

		int success;
		char info[512];
		glGetShaderiv(m_VertexShader, GL_COMPILE_STATUS, &success);
#ifdef DEBUG
		if (!success) {
			glGetShaderInfoLog(m_VertexShader, 512, NULL, info);
			std::stringstream ss;
			ss << "Failed to compile vertex shader ( " << m_Config.vertexShader << " ):" << std::endl;
			ss << std::string(info);
			SOFT_ERROR(ss.str());
		}
#elif RELEASE
		if (!success) return;
#endif
	}
	if (m_Config.type & ShaderType::Pixel) {
		m_PixelShader = glCreateShader(GL_FRAGMENT_SHADER);

		std::string source;
		if (!m_Config.source) System::LoadStringFromFile(source, m_Config.pixelShader);
		else source = m_Config.pixelShader;
		const char* cSource = source.c_str();

		glShaderSource(m_PixelShader, 1, &cSource, NULL);
		glCompileShader(m_PixelShader);

		int success;
		char info[512];
		glGetShaderiv(m_PixelShader, GL_COMPILE_STATUS, &success);
#ifdef DEBUG
		if (!success) {
			glGetShaderInfoLog(m_PixelShader, 512, NULL, info);
			std::stringstream ss;
			ss << "Failed to compile pixel shader ( " << m_Config.pixelShader << " ):" << std::endl;
			ss << (char)9 << std::string(info);
			SOFT_ERROR(ss.str());
		}
#elif RELEASE
		if (!success) return;
#endif
	}
	if (m_Config.type & ShaderType::Geometry) {
		m_GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		
		std::string source;
		if (!m_Config.source) System::LoadStringFromFile(source, m_Config.geometryShader);
		else source = m_Config.geometryShader;
		const char* cSource = source.c_str();

		glShaderSource(m_GeometryShader, 1, &cSource, NULL);
		glCompileShader(m_GeometryShader);

		int success;
		char info[512];
		glGetShaderiv(m_GeometryShader, GL_COMPILE_STATUS, &success);
#ifdef DEBUG
		if (!success) {
			glGetShaderInfoLog(m_GeometryShader, 512, NULL, info);
			std::stringstream ss;
			ss << "Failed to compile pixel shader ( " << m_Config.geometryShader << " ):" << std::endl;
			ss << (char)9 << std::string(info);
			SOFT_ERROR(ss.str());
		}
#elif RELEASE
		if (!success) return;
#endif
	}

	m_Handle = glCreateProgram();
	if (m_Config.type & ShaderType::Vertex)   glAttachShader(m_Handle, m_VertexShader);
	if (m_Config.type & ShaderType::Pixel)    glAttachShader(m_Handle, m_PixelShader);
	if (m_Config.type & ShaderType::Geometry) glAttachShader(m_Handle, m_GeometryShader);
	glLinkProgram(m_Handle);
	
	// Error checking
	{
		int success;
		char info[512];
		glGetProgramiv(m_Handle, GL_LINK_STATUS, &success);
#ifdef DEBUG
		if (!success) {
			glGetProgramInfoLog(m_Handle, 512, NULL, info);
			std::stringstream ss;
			ss << "Failed to link shader program:" << std::endl;
			ss << (char)9 << std::string(info);
			SOFT_ERROR(ss.str());
		}
#elif RELEASE
		if (!success) return;
#endif
	}

	FindUniforms();
}
Shader::~Shader()
{
	if (m_Config.type & ShaderType::Vertex)   glDeleteShader(m_VertexShader);
	if (m_Config.type & ShaderType::Pixel)    glDeleteShader(m_PixelShader);
	if (m_Config.type & ShaderType::Geometry) glDeleteShader(m_GeometryShader);
	glDeleteProgram(m_Handle);
}

void Shader::FindUniforms()
{
	glUseProgram(m_Handle);

	int uniformCount = 0;
	glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORMS, &uniformCount);

	for (int i = 0; i < uniformCount; ++i) {
		int maxNameLength = 0;
		int nameLength = 0;
		int size = 0;
		uint32_t type = GL_NONE;
		glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

		std::vector<char> name(maxNameLength);
		glGetActiveUniform(m_Handle, i, maxNameLength, &nameLength, &size, &type, name.data());
		
		Uniform u;
		u.name = std::string(name.begin(), name.begin() + nameLength);
		u.type = DataType_GL(type);
		u.location = glGetUniformLocation(m_Handle, u.name.c_str());

		m_Uniforms.insert({ u.name, u });
	}

	glUseProgram(0);
}

void Shader::Bind()
{
	glUseProgram(m_Handle);
}
void Shader::Unbind()
{
	glUseProgram(0);
}

void Shader::SetUniform(const std::string& name, void* data)
{
	if (m_Uniforms.find(name) == m_Uniforms.end()) {
		WARNING("Uniform ( " + name + " ) doesn't exist / is inactive");
		return;
	}

	Uniform& uniform = m_Uniforms[name];
	uniform.data = data;

	glUseProgram(m_Handle);

	switch (uniform.type)
	{
	case DataType::Float: {
		float value = *(float*)data;
		glUniform1f(uniform.location, value);
		break;
	}
	case DataType::Float2: {
		glm::vec2 value = *(glm::vec2*)data;
		glUniform2f(uniform.location, value.x, value.y);
		break;
	}
	case DataType::Float3: {
		glm::vec3 value = *(glm::vec3*)data;
		glUniform3f(uniform.location, value.x, value.y, value.z);
		break;
	}
	case DataType::Float4: {
		glm::vec4 value = *(glm::vec4*)data;
		glUniform4f(uniform.location, value.x, value.y, value.z, value.w);
		break;
	}
	case DataType::Matrix2: {
		glm::mat2 value = *(glm::mat2*)data;
		glUniformMatrix2fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
		break;
	}
	case DataType::Matrix3: {
		glm::mat3 value = *(glm::mat3*)data;
		glUniformMatrix3fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
		break;
	}
	case DataType::Matrix4: {
		glm::mat4 value = *(glm::mat4*)data;
		glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
		break;
	}
	case DataType::Int: {
		int value = *(int*)data;
		glUniform1i(uniform.location, value);
		break;
	}
	case DataType::Texture2D: {
		Texture* texture = (Texture*)data;
		glUniform1i(uniform.location, texture->GetSlot());
		break;
	}

	default:
		WARNING("Uniform type isn't supported");
	}

	//glUseProgram(0);
}

DataType Shader::GetUniformType(const std::string& name)
{
	if (m_Uniforms.find(name) == m_Uniforms.end()) {
		WARNING("Uniform ( " + name + " ) doesn't exist / is inactive");
		return DataType::None;
	}
	return m_Uniforms[name].type;
}
void* Shader::GetUniformData(const std::string& name)
{
	if (m_Uniforms.find(name) == m_Uniforms.end()) {
		WARNING("Uniform ( " + name + " ) doesn't exist / is inactive");
		return (void*)0;
	}
	return m_Uniforms[name].data;
}
