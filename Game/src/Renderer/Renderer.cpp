#include "Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include "Core/Application.h"

namespace {
	std::string LoadFromFile(const std::string& filename) {
		std::ifstream file;
		file.open(filename);
		if (!file.is_open()) {
			ErrorEvent err("Failed to load file ( " + filename + " )");
			Application::Get()->ProcEvent(err);
			return "";
		}
		std::stringstream ss;
		ss << file.rdbuf();
		return ss.str();
	}
}

void Renderer::Init(std::function<void(Event&)> callback) {
	s_Data.callback = callback;

	// Initialize GLAD 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		ErrorEvent err("Failed to initialize GLAD");
		s_Data.callback(err);
		return;
	}

	auto window = Application::Get()->GetWindow();
	glViewport(0, 0, window->GetWidth(), window->GetHeight());

	// Load main shader
	{
		std::string VertexSource = LoadFromFile("assets/shaders/shader.vert");
		std::string FragmentSource = LoadFromFile("assets/shaders/shader.frag");
		std::string TextureAtlasSource = LoadFromFile("assets/shaders/texture_atlas.frag");

		unsigned int VertexShader = glCreateShader(GL_VERTEX_SHADER);
		{
			const char* cVertexSource = VertexSource.c_str();
			glShaderSource(VertexShader, 1, &cVertexSource, NULL);
			glCompileShader(VertexShader);

			int success;
			char infoLog[512];
			glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(VertexShader, 512, NULL, infoLog);
				ErrorEvent err("Failed to compile vertex shader\n" + std::string(infoLog));
				s_Data.callback(err);
			}
		}

		unsigned int FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		{
			const char* cFragmentSource = FragmentSource.c_str();
			glShaderSource(FragmentShader, 1, &cFragmentSource, NULL);
			glCompileShader(FragmentShader);

			int success;
			char infoLog[512];
			glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(FragmentShader, 512, NULL, infoLog);
				ErrorEvent err("Failed to compile fragment shader\n" + std::string(infoLog));
				s_Data.callback(err);
			}
		}

		unsigned int AtlasShader = glCreateShader(GL_FRAGMENT_SHADER);
		{
			const char* cAtlasSource = TextureAtlasSource.c_str();
			glShaderSource(AtlasShader, 1, &cAtlasSource, NULL);
			glCompileShader(AtlasShader);

			int success;
			char infoLog[512];
			glGetShaderiv(AtlasShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(AtlasShader, 512, NULL, infoLog);
				ErrorEvent err("Failed to compile texture atlas shader\n" + std::string(infoLog));
				s_Data.callback(err);
			}
		}

		s_Data.MainShader = glCreateProgram();
		{
			glAttachShader(s_Data.MainShader, VertexShader);
			glAttachShader(s_Data.MainShader, FragmentShader);
			glLinkProgram(s_Data.MainShader);

			int success;
			char infoLog[512];
			glGetProgramiv(s_Data.MainShader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(s_Data.MainShader, 512, NULL, infoLog);
				ErrorEvent err("Failed to link main shader program\n" + std::string(infoLog));
				s_Data.callback(err);
			}
		}
		s_Data.TextureAtlasShader = glCreateProgram();
		{
			glAttachShader(s_Data.TextureAtlasShader, VertexShader);
			glAttachShader(s_Data.TextureAtlasShader, AtlasShader);
			glLinkProgram(s_Data.TextureAtlasShader);

			int success;
			char infoLog[512];
			glGetProgramiv(s_Data.TextureAtlasShader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(s_Data.TextureAtlasShader, 512, NULL, infoLog);
				ErrorEvent err("Failed to link texture atlas shader program\n" + std::string(infoLog));
				s_Data.callback(err);
			}
		}

		glDeleteShader(VertexShader);
		glDeleteShader(FragmentShader);
		glDeleteShader(AtlasShader);
	}

	// Load quad
	{
		glGenVertexArrays(1, &s_Data.Quad);
		glBindVertexArray(s_Data.Quad);

		float vertices[] = {
			 0.5f,  0.5f,  1.0f, 1.0f, // top right
			 0.5f, -0.5f,  1.0f, 0.0f, // bottom right
			-0.5f, -0.5f,  0.0f, 0.0f, // bottom left
			-0.5f,  0.5f,  0.0f, 1.0f  // top left 
		};
		unsigned int indices[] = {
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}

	// Create blank texture
	TextureConfig config;
	config.Width = 1;
	config.Height = 1;
	config.Data = malloc(3);
	std::memset(config.Data, 255, 3);
	s_Data.BlankTexture = new Texture(config);
}

void Renderer::StartFrame(Camera* camera) {
	s_Data.CurrentCamera = camera;

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(s_Data.MainShader);

	unsigned int viewproj_loc = glGetUniformLocation(s_Data.MainShader, "ViewProjection");
	glUniformMatrix4fv(viewproj_loc, 1, GL_FALSE, glm::value_ptr(s_Data.CurrentCamera->GetViewProjection()));

	glBindVertexArray(s_Data.Quad);
}
void Renderer::EndFrame() {

}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	ViewFrustum frustum = ViewFrustum::CalculateScreenFrustum();
	s_Data.CurrentCamera->SetViewFrustum(frustum);
	glViewport(0, 0, width, height);
}

void Renderer::ClearColor(const glm::vec3& color)
{
	glClearColor(color.r, color.g, color.b, 1.0f);
}

void Renderer::DrawQuad(const glm::vec2& pos, const glm::vec3& tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });

	unsigned int model_loc = glGetUniformLocation(s_Data.MainShader, "Model");
	unsigned int tint_loc =  glGetUniformLocation(s_Data.MainShader, "Tint");

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(tint_loc, tint.r, tint.g, tint.b);

	glBindTexture(GL_TEXTURE_2D, s_Data.BlankTexture->GetHandle());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::DrawQuad(const glm::vec2& pos, const glm::vec2& scale, const glm::vec3& tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	model = glm::scale(model, { scale, 1 });

	unsigned int model_loc = glGetUniformLocation(s_Data.MainShader, "Model");
	unsigned int tint_loc = glGetUniformLocation(s_Data.MainShader, "Tint");

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(tint_loc, tint.r, tint.g, tint.b);

	glBindTexture(GL_TEXTURE_2D, s_Data.BlankTexture->GetHandle());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::DrawQuad(const glm::vec2& pos, const glm::vec2& scale, float rot, const glm::vec3& tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	model = glm::rotate(model, rot, { 0, 0, 1 });
	model = glm::scale(model, { scale, 1 });

	unsigned int model_loc = glGetUniformLocation(s_Data.MainShader, "Model");
	unsigned int tint_loc = glGetUniformLocation(s_Data.MainShader, "Tint");

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(tint_loc, tint.r, tint.g, tint.b);

	glBindTexture(GL_TEXTURE_2D, s_Data.BlankTexture->GetHandle());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::DrawQuad(glm::vec2 pos, Texture* texture, glm::vec3 tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });

	unsigned int model_loc = glGetUniformLocation(s_Data.MainShader, "Model");
	unsigned int tint_loc = glGetUniformLocation(s_Data.MainShader, "Tint");

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(tint_loc, tint.r, tint.g, tint.b);

	glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::DrawQuad(const glm::vec2& pos, const glm::vec2& scale, Texture* texture, const glm::vec3& tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	model = glm::scale(model, { scale, 1 });

	unsigned int model_loc = glGetUniformLocation(s_Data.MainShader, "Model");
	unsigned int tint_loc = glGetUniformLocation(s_Data.MainShader, "Tint");

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(tint_loc, tint.r, tint.g, tint.b);

	glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::DrawQuad(const glm::vec2& pos, const glm::vec2& scale, float rot, Texture* texture, const glm::vec3& tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	model = glm::rotate(model, rot, { 0, 0, 1 });
	model = glm::scale(model, { scale, 1 });

	unsigned int model_loc = glGetUniformLocation(s_Data.MainShader, "Model");
	unsigned int tint_loc = glGetUniformLocation(s_Data.MainShader, "Tint");

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(tint_loc, tint.r, tint.g, tint.b);

	glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::DrawQuadAtlas(glm::vec2 pos, Texture* texture, const glm::vec2& size, const glm::vec2& texid, glm::vec3 tint)
{
	glUseProgram(s_Data.TextureAtlasShader);

	unsigned int viewproj_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "ViewProjection");
	glUniformMatrix4fv(viewproj_loc, 1, GL_FALSE, glm::value_ptr(s_Data.CurrentCamera->GetViewProjection()));

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });

	unsigned int model_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "Model");
	unsigned int tint_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "Tint");
	unsigned int texid_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "TextureID");
	unsigned int size_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "AtlasSize");

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(tint_loc, tint.r, tint.g, tint.b);
	glUniform2f(texid_loc, texid.x, texid.y);
	glUniform2f(size_loc, size.x, size.y);

	glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(s_Data.MainShader);

	viewproj_loc = glGetUniformLocation(s_Data.MainShader, "ViewProjection");
	glUniformMatrix4fv(viewproj_loc, 1, GL_FALSE, glm::value_ptr(s_Data.CurrentCamera->GetViewProjection()));
}
void Renderer::DrawQuadAtlas(const glm::vec2& pos, const glm::vec2& scale, Texture* texture, const glm::vec2& size, const glm::vec2& texid, const glm::vec3& tint)
{
	glUseProgram(s_Data.TextureAtlasShader);

	unsigned int viewproj_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "ViewProjection");
	glUniformMatrix4fv(viewproj_loc, 1, GL_FALSE, glm::value_ptr(s_Data.CurrentCamera->GetViewProjection()));

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	model = glm::scale(model, { scale, 1 });

	unsigned int model_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "Model");
	unsigned int tint_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "Tint");
	unsigned int texid_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "TextureID");
	unsigned int size_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "AtlasSize");

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(tint_loc, tint.r, tint.g, tint.b);
	glUniform2f(texid_loc, texid.x, texid.y);
	glUniform2f(size_loc, size.x, size.y);

	glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(s_Data.MainShader);

	viewproj_loc = glGetUniformLocation(s_Data.MainShader, "ViewProjection");
	glUniformMatrix4fv(viewproj_loc, 1, GL_FALSE, glm::value_ptr(s_Data.CurrentCamera->GetViewProjection()));
}
void Renderer::DrawQuadAtlas(const glm::vec2& pos, const glm::vec2& scale, float rot, Texture* texture, const glm::vec2& size, const glm::vec2& texid, const glm::vec3& tint)
{
	glUseProgram(s_Data.TextureAtlasShader);

	unsigned int viewproj_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "ViewProjection");
	glUniformMatrix4fv(viewproj_loc, 1, GL_FALSE, glm::value_ptr(s_Data.CurrentCamera->GetViewProjection()));

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	model = glm::rotate(model, rot, { 0, 0, 1 });
	model = glm::scale(model, { scale, 1 });

	unsigned int model_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "Model");
	unsigned int tint_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "Tint");
	unsigned int texid_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "TextureID");
	unsigned int size_loc = glGetUniformLocation(s_Data.TextureAtlasShader, "AtlasSize");

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(tint_loc, tint.r, tint.g, tint.b);
	glUniform2f(texid_loc, texid.x, texid.y);
	glUniform2f(size_loc, size.x, size.y);

	glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(s_Data.MainShader);

	viewproj_loc = glGetUniformLocation(s_Data.MainShader, "ViewProjection");
	glUniformMatrix4fv(viewproj_loc, 1, GL_FALSE, glm::value_ptr(s_Data.CurrentCamera->GetViewProjection()));
}
