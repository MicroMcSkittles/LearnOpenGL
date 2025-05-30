#include "Renderer.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include "Core/Application.h"
#include "Core/System.h"

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

	// Load Post Proc Shader 
	{
		std::string VertexSource = LoadFromFile("assets/shaders/PostProc.vert");
		std::string FragmentSource = LoadFromFile("assets/shaders/PostProc.frag");

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
				ErrorEvent err("Failed to compile post process vertex shader\n" + std::string(infoLog));
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
				ErrorEvent err("Failed to compile post process fragment shader\n" + std::string(infoLog));
				s_Data.callback(err);
			}
		}

		s_Data.ViewPortShader = glCreateProgram();
		{
			glAttachShader(s_Data.ViewPortShader, VertexShader);
			glAttachShader(s_Data.ViewPortShader, FragmentShader);
			glLinkProgram(s_Data.ViewPortShader);

			int success;
			char infoLog[512];
			glGetProgramiv(s_Data.ViewPortShader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(s_Data.ViewPortShader, 512, NULL, infoLog);
				ErrorEvent err("Failed to link post process shader program\n" + std::string(infoLog));
				s_Data.callback(err);
			}
		}

		glDeleteShader(VertexShader);
		glDeleteShader(FragmentShader);
	}

	// Create View Port Frame Buffer
	{
		glGenFramebuffers(1, &s_Data.ViewPortFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, s_Data.ViewPortFrameBuffer);

		TextureConfig bufferConfig;
		bufferConfig.Data = (void*)NULL;
		bufferConfig.Width = window->GetWidth();
		bufferConfig.Height = window->GetHeight();

		s_Data.ViewPortColorBuffer = new Texture(bufferConfig);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_Data.ViewPortColorBuffer->GetHandle(), 0);

		unsigned int RBO;
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window->GetWidth(), window->GetHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);


		glViewport(0, 0, window->GetWidth(), window->GetHeight());

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			ErrorEvent err("Failed to create Main Frame Buffer");
			Application::Get()->ProcEvent(err);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Create Main Frame Buffer
	{
		glGenFramebuffers(1, &s_Data.MainFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, s_Data.MainFrameBuffer);

		TextureConfig bufferConfig;
		bufferConfig.Data = (void*)NULL;
		bufferConfig.Width = window->GetWidth();
		bufferConfig.Height = window->GetHeight();

		s_Data.MainColorBuffer = new Texture(bufferConfig);

		bufferConfig.InternalFormat = TexFormat::RGBA;
		bufferConfig.Format = TexFormat::RGBA;
		s_Data.TransparencyColorBuffer = new Texture(bufferConfig);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_Data.MainColorBuffer->GetHandle(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, s_Data.TransparencyColorBuffer->GetHandle(), 0);

		unsigned int RBO;
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window->GetWidth(), window->GetHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);


		glViewport(0, 0, window->GetWidth(), window->GetHeight());

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			ErrorEvent err("Failed to create Main Frame Buffer");
			Application::Get()->ProcEvent(err);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	s_Data.QuadVertices.push_back({ { 1.0f,  1.0f }, { 1.0f, 1.0f } });
	s_Data.QuadVertices.push_back({ { 1.0f, -1.0f }, { 1.0f, 0.0f } });
	s_Data.QuadVertices.push_back({ {-1.0f, -1.0f }, { 0.0f, 0.0f } });
	s_Data.QuadVertices.push_back({ {-1.0f,  1.0f }, { 0.0f, 1.0f } });

	s_Data.QuadIndices.push_back(0);
	s_Data.QuadIndices.push_back(1);
	s_Data.QuadIndices.push_back(3);

	s_Data.QuadIndices.push_back(1);
	s_Data.QuadIndices.push_back(2);
	s_Data.QuadIndices.push_back(3);

	glGenVertexArrays(1, &s_Data.Quad);
	glBindVertexArray(s_Data.Quad);
	
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, s_Data.QuadVertices.size() * sizeof(s_Data.QuadVertices[0]), &s_Data.QuadVertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, s_Data.QuadIndices.size() * sizeof(uint32_t), &s_Data.QuadIndices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	// Create blank texture
	TextureConfig config;
	config.Width = 1;
	config.Height = 1;
	config.Data = malloc(3);
	std::memset(config.Data, 255, 3);
	s_Data.BlankTexture = new Texture(config);

	s_Data.DiagnosticInfo.BatchMS = 0;
	s_Data.DiagnosticInfo.DrawMS = 0;
	s_Data.DiagnosticInfo.DrawCallCount = 0;
}

void Renderer::StartFrame(Camera* camera) {
	s_Data.CurrentCamera = camera;
	s_Data.DiagnosticInfo.DrawCallCount = 0;
	s_Data.DiagnosticInfo.RenderGroupCount = 0;
	s_Data.DiagnosticInfo.QuadCount = 0;

	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, s_Data.MainFrameBuffer);
	GLenum bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, bufs);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	s_Data.DiagnosticInfo.StartBatchMS = System::GetTime();
}
void Renderer::ChangeCamera(Camera* camera)
{
	s_Data.CurrentCamera = camera;
}
void Renderer::EndFrame() {
	std::vector<int> validRenderGroups;
	s_Data.DiagnosticInfo.RenderGroupCount = s_Data.RenderGroups.size();

	for (int i = 0; i < s_Data.RenderGroups.size(); ++i) {
		RenderGroup* rg = s_Data.RenderGroups[i];
		if (rg->Vertices.size() == 0) { 

			rg->Vertices.clear();
			rg->Vertices.resize(0);
			rg->Vertices = std::vector<BatchVertex>();

			rg->Indices.clear();
			rg->Indices.resize(0);
			rg->Indices = std::vector<uint32_t>();

			glDeleteBuffers(1, &rg->VBO);
			glDeleteBuffers(1, &rg->EBO);
			glDeleteVertexArrays(1, &rg->VAO);

			delete rg;
			s_Data.RenderGroups.erase(s_Data.RenderGroups.begin() + i);
			i -= 1;
			continue; 
		}
		
		if (rg->VAO == (unsigned int)(-1)) {
			glGenVertexArrays(1, &rg->VAO);
			glBindVertexArray(rg->VAO);

			glGenBuffers(1, &rg->VBO);
			glBindBuffer(GL_ARRAY_BUFFER, rg->VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rg->Vertices[0]) * rg->Vertices.size(), &rg->Vertices[0], GL_DYNAMIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(2 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(4 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(3);

			glGenBuffers(1, &rg->EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rg->EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * rg->Indices.size(), &rg->Indices[0], GL_DYNAMIC_DRAW);
		
			glBindVertexArray(0);
		}
		else if(rg->Vertices.size() == rg->LastVertexCount) {
			glBindVertexArray(rg->VAO);

			glBindBuffer(GL_ARRAY_BUFFER, rg->VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, rg->Vertices.size() * sizeof(BatchVertex), &rg->Vertices[0]);
			
			glBindVertexArray(0);
		}
		else {
			glBindVertexArray(rg->VAO);

			glBindBuffer(GL_ARRAY_BUFFER, rg->VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rg->Vertices[0]) * rg->Vertices.size(), &rg->Vertices[0], GL_DYNAMIC_DRAW);
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rg->EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * rg->Indices.size(), &rg->Indices[0], GL_DYNAMIC_DRAW);
		
			glBindVertexArray(0);
		}
		
		rg->LastVertexCount = rg->Vertices.size();
		validRenderGroups.push_back(i);
	}

	s_Data.DiagnosticInfo.BatchMS = (System::GetTime() - s_Data.DiagnosticInfo.StartBatchMS) * 1000.0f;

	float startMS = System::GetTime();

	glUseProgram(s_Data.MainShader);

	for (int& id : validRenderGroups) {
		RenderGroup* rg = s_Data.RenderGroups[id];
		
		unsigned int viewproj_loc = glGetUniformLocation(s_Data.MainShader, "ViewProjection");
		unsigned int AtlasSize_loc = glGetUniformLocation(s_Data.MainShader, "AtlasSize");
		unsigned int texture_loc = glGetUniformLocation(s_Data.MainShader, "Texture");
		glm::mat4 viewproj = rg->Camera->GetViewProjection();
		glUniformMatrix4fv(viewproj_loc, 1, GL_FALSE, glm::value_ptr(viewproj));
		glUniform2f(AtlasSize_loc, rg->AtlasSize.x, rg->AtlasSize.y);
		glUniform1i(texture_loc, 0);

		glBindVertexArray(rg->VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rg->Texture->GetHandle());
		glDrawElements(GL_TRIANGLES, rg->Indices.size(), GL_UNSIGNED_INT, 0);
		s_Data.DiagnosticInfo.DrawCallCount += 1;

		glBindVertexArray(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Render the frame buffer to the screen
	glUseProgram(s_Data.ViewPortShader);

	glBindVertexArray(s_Data.Quad);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, s_Data.MainColorBuffer->GetHandle());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, s_Data.TransparencyColorBuffer->GetHandle());

	unsigned int mainColorLoc = glGetUniformLocation(s_Data.ViewPortShader, "MainColorBuffer");
	unsigned int tranColorLoc = glGetUniformLocation(s_Data.ViewPortShader, "TranColorBuffer");

	glUniform1i(mainColorLoc, 0);
	glUniform1i(tranColorLoc, 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	s_Data.DiagnosticInfo.DrawCallCount += 1;
	glBindVertexArray(0);

	s_Data.DiagnosticInfo.DrawMS = (System::GetTime() - startMS) * 1000.0f;
	
	// Clear vertices and indices in all render groups to prevent memory leaks
	startMS = System::GetTime();
	for (RenderGroup* rg : s_Data.RenderGroups) {
		rg->Vertices.clear();
		rg->Vertices.resize(0);

		rg->Indices.clear();
		rg->Indices.resize(0);
	}
	s_Data.DiagnosticInfo.BatchMS += (System::GetTime() - startMS) * 1000.0f;
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	ViewFrustum frustum = ViewFrustum::CalculateScreenFrustum();
	s_Data.CurrentCamera->SetViewFrustum(frustum);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, s_Data.ViewPortFrameBuffer);
	glViewport(0, 0, width, height);

	s_Data.ViewPortColorBuffer->Resize(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, s_Data.MainFrameBuffer);
	glViewport(0, 0, width, height);

	s_Data.MainColorBuffer->Resize(width, height);
	s_Data.TransparencyColorBuffer->Resize(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::ClearColor(const glm::vec3& color)
{
	glClearColor(color.r, color.g, color.b, 1.0f);
}

void Renderer::DrawQuad(const glm::vec2& pos, bool transparent, const glm::vec4& tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	Submit(model, { 1,1 }, { 0,0 }, tint, s_Data.BlankTexture, transparent);
	s_Data.DiagnosticInfo.QuadCount += 1;
}
void Renderer::DrawQuad(const glm::vec2& pos, const glm::vec2& scale, bool transparent, const glm::vec4& tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	model = glm::scale(model, { scale, 1 });
	Submit(model, { 1,1 }, { 0,0 }, tint, s_Data.BlankTexture, transparent);
	s_Data.DiagnosticInfo.QuadCount += 1;
}
void Renderer::DrawQuad(const glm::vec2& pos, const glm::vec2& scale, float rot, bool transparent, const glm::vec4& tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	model = glm::rotate(model, rot, { 0, 0, 1 });
	model = glm::scale(model, { scale, 1 });
	Submit(model, { 1,1 }, { 0,0 }, tint, s_Data.BlankTexture, transparent);
	s_Data.DiagnosticInfo.QuadCount += 1;
}

void Renderer::DrawScreenSpaceQuad(const glm::vec2& pos, const glm::vec2& scale, float rot, bool transparent, const glm::vec4& tint)
{
	glm::vec2 wPos = s_Data.CurrentCamera->ScreenToWorld(pos);
	glm::vec2 wScale = s_Data.CurrentCamera->ScreenToWorld(scale);

	glm::vec2 position = {
		(wPos.x + wScale.x) / 2,
		(wPos.y + wScale.y) / 2
	};

	wScale = {
		abs(wScale.x - wPos.x),
		abs(wScale.y - wPos.y)
	};

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { position, -1.0f });
	model = glm::rotate(model, rot, { 0, 0, 1 });
	model = glm::scale(model, { wScale, 1 });
	Submit(model, { 1,1 }, { 0,0 }, tint, s_Data.BlankTexture, transparent);
	s_Data.DiagnosticInfo.QuadCount += 1;
}

void Renderer::DrawQuad(glm::vec2 pos, Texture* texture, bool transparent, glm::vec4 tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	Submit(model, { 1,1 }, { 0,0 }, tint, texture, transparent);
	s_Data.DiagnosticInfo.QuadCount += 1;
}
void Renderer::DrawQuad(const glm::vec2& pos, const glm::vec2& scale, Texture* texture, bool transparent, const glm::vec4& tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	model = glm::scale(model, { scale, 1 });
	Submit(model, { 1,1 }, { 0,0 }, tint, texture, transparent);
	s_Data.DiagnosticInfo.QuadCount += 1;
}
void Renderer::DrawQuad(const glm::vec2& pos, const glm::vec2& scale, float rot, Texture* texture, bool transparent, const glm::vec4& tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	model = glm::rotate(model, rot, { 0, 0, 1 });
	model = glm::scale(model, { scale, 1 });
	Submit(model, { 1,1 }, { 0,0 }, tint, texture, transparent);
	s_Data.DiagnosticInfo.QuadCount += 1;
}

void Renderer::DrawQuadAtlas(glm::vec2 pos, Texture* texture, const glm::vec2& size, const glm::vec2& texid, bool transparent, glm::vec4 tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	Submit(model, size, texid, tint, texture, transparent);
	s_Data.DiagnosticInfo.QuadCount += 1;
}
void Renderer::DrawQuadAtlas(const glm::vec2& pos, const glm::vec2& scale, Texture* texture, const glm::vec2& size, const glm::vec2& texid, bool transparent, const glm::vec4& tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	model = glm::scale(model, { scale, 1 });
	Submit(model, size, texid, tint, texture, transparent);
	s_Data.DiagnosticInfo.QuadCount += 1;
}
void Renderer::DrawQuadAtlas(const glm::vec2& pos, const glm::vec2& scale, float rot, Texture* texture, const glm::vec2& size, const glm::vec2& texid, bool transparent, const glm::vec4& tint)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { pos, -1.0f });
	model = glm::rotate(model, rot, { 0, 0, 1 });
	model = glm::scale(model, { scale, 1 });
	Submit(model, size, texid, tint, texture, transparent);
	s_Data.DiagnosticInfo.QuadCount += 1;
}

void Renderer::DrawScreenSpaceQuad(const glm::vec2& pos, const glm::vec2& scale, float rot, Texture* texture, bool transparent, const glm::vec4& tint)
{
	glm::vec2 wPos = s_Data.CurrentCamera->ScreenToWorld(pos);
	glm::vec2 wScale = s_Data.CurrentCamera->ScreenToWorld(scale);

	glm::vec2 position = {
		(wPos.x + wScale.x) / 2,
		(wPos.y + wScale.y) / 2
	};

	wScale = {
		abs(wScale.x - wPos.x),
		abs(wScale.y - wPos.y)
	};

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { position, -1.0f });
	model = glm::rotate(model, rot, { 0, 0, 1 });
	model = glm::scale(model, { wScale, 1 });
	Submit(model, { 1, 1 }, { 0, 0 }, tint, texture, transparent);
	s_Data.DiagnosticInfo.QuadCount += 1;
}
void Renderer::DrawScreenSpaceQuadAtlas(const glm::vec2& pos, const glm::vec2& scale, float rot, Texture* texture, const glm::vec2& size, const glm::vec2& texid, bool transparent, const glm::vec4& tint)
{
	glm::vec2 wPos = s_Data.CurrentCamera->ScreenToWorld(pos);
	glm::vec2 wScale = s_Data.CurrentCamera->ScreenToWorld(scale);

	glm::vec2 position = {
		(wPos.x + wScale.x) / 2,
		(wPos.y + wScale.y) / 2
	};

	wScale = {
		abs(wScale.x - wPos.x),
		abs(wScale.y - wPos.y)
	};

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, { position, -1.0f });
	model = glm::rotate(model, rot, { 0, 0, 1 });
	model = glm::scale(model, { wScale, 1 });

	Submit(model, size, texid, tint, texture, transparent);
	s_Data.DiagnosticInfo.QuadCount += 1;
}

int Renderer::FindGroup(Texture* texture, Camera* camera, bool transparent)
{
	for (int i = 0; i < s_Data.RenderGroups.size(); ++i) {
		auto& rg = s_Data.RenderGroups[i];
		if (rg->Camera == camera && rg->Texture == texture) return i;
	}
	return -1;
}

void Renderer::Submit(const glm::mat4& transform, const glm::vec2& size, const glm::vec2& texid, const glm::vec4& tint, Texture* texture, bool transparent)
{
	int groupID = FindGroup(texture, s_Data.CurrentCamera, transparent);
	
	if (groupID == -1) {
		groupID = s_Data.RenderGroups.size();
		RenderGroup* rg = new RenderGroup();
		rg->VAO = -1;
		rg->Texture = texture;
		rg->AtlasSize = size;
		rg->Transparent = transparent;
		rg->Camera = s_Data.CurrentCamera;
		rg->LastVertexCount = 0;
		s_Data.RenderGroups.push_back(rg);
	}

	RenderGroup* rg = s_Data.RenderGroups[groupID];
	int quadCount = rg->Vertices.size() / 4;
	for (int i = 0; i < s_Data.QuadVertices.size(); ++i) {
		BatchVertex vert = {
			transform * glm::vec4(s_Data.QuadVertices[i].position / 2.0f, 0, 1),
			s_Data.QuadVertices[i].texCoord,
			texid,
			tint
		};
		rg->Vertices.push_back(vert);
	}
	for (int i = 0; i < s_Data.QuadIndices.size(); ++i) {
		rg->Indices.push_back(s_Data.QuadIndices[i] + (quadCount * 4));
	}
}
