#include "Game/VoxelRenderer.h"
#include "Game/Voxel.h"
#include "Core/Core.h"
#include "Core/Window/Window.h"
#include "Renderer/RenderAPI.h"
#include "Game.h"

#include <glm/gtc/matrix_transform.hpp>

void VoxelRenderer::Init()
{
	if (s_Data) {
		WARNING("Voxel renderer data instance already exists");
	}
	s_Data = new RendererData();

	DataTree& renderTree = Game::GetConfig()["Renderer"];
	s_Data->settings.renderDistance = renderTree["VoxelSettings"]["RenderDistance"].GetValue();
	s_Data->settings.chunkSize = renderTree["VoxelSettings"]["ChunkSize"].GetValue();
	s_Data->settings.chunkHeight = renderTree["VoxelSettings"]["ChunkHeight"].GetValue();
	s_Data->settings.chunkArea = s_Data->settings.chunkSize * s_Data->settings.chunkSize;
	s_Data->settings.chunkVolume = s_Data->settings.chunkArea * s_Data->settings.chunkHeight;

	RenderAPI::SetViewPortSize(800, 600);

	RenderAPI::SetClearMask(ClearMask::ColorBufferBit | ClearMask::DepthBufferBit);
	RenderAPI::ClearColor({ (102.0f / 255.0f), (178.0f / 255.0f), 1, 1 });

	RenderAPI::SetDrawMode(DrawMode::Triangles);

	// Load Shader
	{
		std::string vertex_extention = renderTree["Shaders"]["Extention"].At(0).GetValue();
		std::string pixel_extention = renderTree["Shaders"]["Extention"].At(1).GetValue();

		ShaderConfig shaderConfig;

		std::string path = renderTree["Shaders"]["Main"].At(0).GetValue();
		shaderConfig.vertexShader = path + vertex_extention;
		shaderConfig.pixelShader = path + pixel_extention;
		s_Data->mainShader = new Shader(shaderConfig);

		path = renderTree["Shaders"]["UI"].At(0).GetValue();
		shaderConfig.vertexShader = path + vertex_extention;
		shaderConfig.pixelShader = path + pixel_extention;
		s_Data->UIShader = new Shader(shaderConfig);

		path = renderTree["Shaders"]["Selector"].At(0).GetValue();
		shaderConfig.vertexShader = path + vertex_extention;
		shaderConfig.pixelShader = path + pixel_extention;
		s_Data->selectorShader = new Shader(shaderConfig);

		path = renderTree["Shaders"]["PostProc"].At(0).GetValue();
		shaderConfig.vertexShader = path + vertex_extention;
		shaderConfig.pixelShader = path + pixel_extention;
		s_Data->postProcShader = new Shader(shaderConfig);
	}

	// Load Voxel Texture
	{
		TextureConfig textureConfig;
		textureConfig.slot = 0;
		textureConfig.minFilter = TexFilterMode::Nearest;
		textureConfig.magFilter = TexFilterMode::Nearest;
		s_Data->voxelAtlas = new Texture("assets/tilemap.png", textureConfig);
		s_Data->otherAtlas = new Texture("assets/MiscAtlas.png", textureConfig);
	}

	// Load Quad ( used for ui )
	{
		struct vertex {
			glm::vec3 position;
			glm::vec2 texCoord;
		};

		vertex vertices[] = {
			{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } },
			{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
			{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
			{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } }
		};
		uint32_t indices[] = {
			0, 3, 1,
			1, 3, 2
		};

		s_Data->quad = new VertexArray();
		s_Data->quad->Bind();

		VertexBuffer* vbo = new VertexBuffer(VertexLayout({ DataType::Float3, DataType::Float2 }));
		vbo->SetData<vertex>(vertices, sizeof(vertices));
		vbo->Initialize();
		s_Data->quad->AttachBuffer(vbo);

		ElementBuffer* ebo = new ElementBuffer();
		ebo->SetData(indices, sizeof(indices));
		ebo->Initialize();
		s_Data->quad->AttachBuffer(ebo);

		s_Data->quad->Unbind();
	}

	// Load Selector 
	{
		struct vertex {
			glm::vec3 position;
		};

		// I used 0.501f to make the selector slightly bigger than the voxels
		vertex vertices[] = {
			{ {  0.501f,  0.501f, -0.501f } },
			{ {  0.501f, -0.501f, -0.501f } },
			{ { -0.501f, -0.501f, -0.501f } },
			{ { -0.501f,  0.501f, -0.501f } },
			{ {  0.501f,  0.501f,  0.501f } },
			{ {  0.501f, -0.501f,  0.501f } },
			{ { -0.501f, -0.501f,  0.501f } },
			{ { -0.501f,  0.501f,  0.501f } },
		};
		uint32_t indices[] = {
			0,1,
			1,2,
			2,3,
			3,0,

			4,5,
			5,6,
			6,7,
			7,4,

			0,4,
			1,5,
			2,6,
			3,7,

			1,3,
			4,6,

			0,5,
			2,7,

			0,7,
			1,6
		};

		s_Data->selector = new VertexArray();
		s_Data->selector->Bind();

		VertexBuffer* vbo = new VertexBuffer(VertexLayout({ DataType::Float3 }));
		vbo->SetData<vertex>(vertices, sizeof(vertices));
		vbo->Initialize();
		s_Data->selector->AttachBuffer(vbo);

		ElementBuffer* ebo = new ElementBuffer();
		ebo->SetData(indices, sizeof(indices));
		ebo->Initialize();
		s_Data->selector->AttachBuffer(ebo);

		s_Data->selector->Unbind();
	}

	GenerateVoxelPreview();

	FrameBuffer::Config frameBufferConfig;
	frameBufferConfig.width = 800;
	frameBufferConfig.height = 600;
	frameBufferConfig.attachments = { { FrameBuffer::ColorAttachment } };
	s_Data->postProcBuffer = new FrameBuffer(frameBufferConfig);
}

void VoxelRenderer::Resize(uint32_t width, uint32_t height)
{
	RenderAPI::SetViewPortSize(width, height);
	s_Data->currentCamera->Resize(width, height);
	s_Data->currentUICamera->Resize(width, height);

	s_Data->postProcBuffer->Resize(width, height);
}

void VoxelRenderer::StartFrame(Camera* camera)
{
	auto renderInfo = RenderAPI::GetDiagnostic();
	renderInfo->ClearCount = 0;
	renderInfo->DrawCallCount = 0;

	s_Data->currentCamera = camera;

	RenderAPI::ClearColor({ (102.0f / 255.0f), (178.0f / 255.0f), 1, 1 });
	RenderAPI::SetClearMask(ClearMask::ColorBufferBit | ClearMask::DepthBufferBit);
	RenderAPI::Clear();

	s_Data->postProcBuffer->Bind();

	RenderAPI::Clear();

	s_Data->mainShader->SetUniform("tex", (void*)s_Data->voxelAtlas);
	s_Data->mainShader->SetUniform("ViewProjection", (void*)&s_Data->currentCamera->GetViewProjection());
	s_Data->selectorShader->SetUniform("ViewProjection", (void*)&s_Data->currentCamera->GetViewProjection());

	s_Data->mainShader->Bind();
	s_Data->voxelAtlas->Bind();
}
void VoxelRenderer::StartUIFrame(Camera* camera)
{
	s_Data->currentUICamera = camera;

	//RenderAPI::SetClearMask(ClearMask::DepthBufferBit);
	//RenderAPI::Clear();

	s_Data->UIShader->SetUniform("ViewProjection", (void*)&s_Data->currentUICamera->GetViewProjection());
	s_Data->UIShader->Bind();
}
void VoxelRenderer::EndFrame()
{
	s_Data->postProcBuffer->Unbind();

	s_Data->postProcShader->Bind();

	Texture* frame = s_Data->postProcBuffer->GetColorAttachment(0);
	s_Data->postProcShader->SetUniform("frame", (void*)frame);

	frame->Bind();
	s_Data->quad->Bind();
	RenderAPI::DrawElements(6, DataType::UnsignedInt, NULL);
	s_Data->quad->Unbind();
	frame->Unbind();

	s_Data->postProcShader->Unbind();
}

void VoxelRenderer::DrawChunk(Chunk* chunk)
{
	auto vao = chunk->GetMesh();
	if (vao == nullptr) return;
	glm::vec3 position = {
		chunk->GetPosition().x * (float)s_Data->settings.chunkSize,
		0,
		chunk->GetPosition().y * (float)s_Data->settings.chunkSize
	};

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
	s_Data->mainShader->SetUniform("Model", (void*)&translation);

	vao->Bind();
	RenderAPI::DrawElements(vao->GetElementBuffer()->GetIndexCount(), DataType::UnsignedInt, NULL);
	vao->Unbind();
}

void VoxelRenderer::DrawSelector(const glm::vec3& position)
{
	s_Data->selectorShader->Bind();

	glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
	s_Data->selectorShader->SetUniform("Model", (void*)&model);

	RenderAPI::SetDrawMode(DrawMode::Lines);

	s_Data->selector->Bind();
	RenderAPI::DrawElements(s_Data->selector->GetElementBuffer()->GetIndexCount(), DataType::UnsignedInt, NULL);
	s_Data->selector->Unbind();

	RenderAPI::SetDrawMode(DrawMode::Triangles);

	s_Data->selectorShader->Unbind();
}
void VoxelRenderer::DrawSelector(const glm::vec3& position, const glm::vec3& size) {
	s_Data->selectorShader->Bind();

	glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), position), size);
	s_Data->selectorShader->SetUniform("Model", (void*)&model);

	RenderAPI::SetDrawMode(DrawMode::Lines);

	s_Data->selector->Bind();
	RenderAPI::DrawElements(s_Data->selector->GetElementBuffer()->GetIndexCount(), DataType::UnsignedInt, NULL);
	s_Data->selector->Unbind();

	RenderAPI::SetDrawMode(DrawMode::Triangles);

	s_Data->selectorShader->Unbind();
}

void VoxelRenderer::DrawUIVoxelPreview(const glm::vec2& offset, uint32_t alignment, const glm::vec2& scale, uint32_t voxel)
{
	DrawUIElement(offset, alignment, scale, { 10, 16 }, s_Data->voxelAtlas, s_VoxelData->VoxelPreview[voxel]);
	//DrawUIElement(offset, alignment, scale, { 1,1 }, s_Data->voxelPreviews[voxel - 1], 0);
}

void VoxelRenderer::DrawUIElement(const glm::vec2& offset, uint32_t alignment, const glm::vec2& scale, const glm::vec2& textureID)
{
	DrawUIElement(offset, alignment, scale, { 8.0f, 8.0f }, s_Data->otherAtlas, textureID);
}

void VoxelRenderer::DrawUIElement(const glm::vec2& offset, uint32_t alignment, const glm::vec2& scale, const glm::vec2& size, Texture* texture, const glm::vec2& textureID)
{
	glm::vec3 position = { 0,0,-1 };
	
	auto window = Window::Get();
	if (alignment & Alignment::AlignMiddle) {
		position = { 0,0,-1 };
	}
	else {
		if (alignment & Alignment::AlignLeft) {
			position.x = s_Data->currentUICamera->ScreenToWorld({ 0,0 }).x;
		}
		else if (alignment & Alignment::AlignRight) {
			position.x = s_Data->currentUICamera->ScreenToWorld({ (float)window->GetWidth(),0 }).x;
		}
		if (alignment & Alignment::AlignBottom) {
			position.y = s_Data->currentUICamera->ScreenToWorld({ 0,(float)window->GetHeight() }).y;
		}
		else if (alignment & Alignment::AlignTop) {
			position.y = s_Data->currentUICamera->ScreenToWorld({ 0,0 }).y;
		}
	}

	position += glm::vec3(offset, 0);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(scale, 1));

	s_Data->UIShader->SetUniform("Model",     (void*)&model);
	s_Data->UIShader->SetUniform("TextureID", (void*)&textureID);
	s_Data->UIShader->SetUniform("AtlasSize", (void*)&size);
	s_Data->UIShader->SetUniform("tex",		  (void*)&texture);

	texture->Bind();
	s_Data->quad->Bind();
	RenderAPI::DrawElements(6, DataType::UnsignedInt, NULL);
	s_Data->quad->Unbind();
	texture->Unbind();
}

void VoxelRenderer::GenerateVoxelPreview()
{
	//float iconWidth = 128;
	//float iconHeight = 128;
	//int blockCount = s_VoxelData->BlockCount - 1;

	//Frustum frustum;
	//frustum.aspectRatio = iconWidth / iconHeight;
	//frustum.near = 0.1f;
	//frustum.far = 1000.0f;
	//frustum.fov = glm::radians(45.0f);
	//Camera* viewCamera = new PerspectiveCamera(frustum, { 0,0,-3 }, {0,0,-1});


	//s_Data->mainShader->SetUniform("tex", (void*)s_Data->voxelAtlas);
	//s_Data->mainShader->SetUniform("ViewProjection", (void*)&viewCamera->GetViewProjection());

	//s_Data->mainShader->Bind();
	//s_Data->voxelAtlas->Bind();

	//for (int i = 0; i < blockCount; i++) {
	//	FrameBuffer::Config viewConfig;
	//	viewConfig.height = iconHeight;
	//	viewConfig.width = iconWidth;
	//	viewConfig.attachments = { { FrameBuffer::ColorAttachment } };
	//	FrameBuffer* viewBuffer = new FrameBuffer(viewConfig);

	//	viewBuffer->Bind();

	//	RenderAPI::ClearColor({ 1,0,1,1 });
	//	RenderAPI::SetClearMask(ClearMask::ColorBufferBit | ClearMask::DepthBufferBit);
	//	RenderAPI::Clear();

	//	glm::vec2 position = { 0,0 };

	//	glm::mat4 model;
	//	model = glm::mat4(1.0f);

	//	VertexArray* mesh = GetVoxelMesh(i + 1);

	//	s_Data->mainShader->SetUniform("Model", (void*)&model);
	//	
	//	mesh->Bind();
	//	RenderAPI::DrawElements(mesh->GetElementBuffer()->GetIndexCount(), DataType::UnsignedInt, NULL);
	//	mesh->Unbind();

	//	viewBuffer->Unbind();

	//	s_Data->voxelPreviews.push_back(viewBuffer->GetColorAttachment(0));

	//	delete mesh;
	//	delete viewBuffer;
	//}

	//s_Data->voxelAtlas->Unbind();
	//s_Data->mainShader->Unbind();
	//

	///*s_Data->previewAtlas = viewBuffer->GetColorAttachment(0);
	//s_Data->previewSize = { blockCount, 1 };*/

	//delete viewCamera;
}
