#pragma once
#include "Core/Core.h"
#include "Renderer/RendererEnum.h"
#include "Renderer/Texture.h"
#include <stdint.h>
#include <vector>

class VertexLayout {
public:
	VertexLayout(const std::vector<DataType>& layout = { });
	~VertexLayout();

	void SetAttributes();

private:
	std::vector<DataType> m_Layout;
	uint32_t m_Stride;
};

class VertexBuffer {
public:
	VertexBuffer(const VertexLayout& layout, Usage usage = Usage::StaticDraw);
	~VertexBuffer();

	template <typename T>
	void SetData(const std::vector<T>& vertices) { 
		if (vertices.size() == 0) WARNING("Vertices must have vertex data");
		m_Vertices = (void*)vertices.data(); 
		m_VerticesSize = vertices.size() * sizeof(float);
	}

	template <typename T>
	void SetData(T* vertices, uint32_t size) {
		if (size == 0 || !vertices) WARNING("Vertices must have vertex data");
		m_Vertices = vertices;
		m_VerticesSize = size;
	}

	void Initialize();

	void Bind();
	void Unbind();

private:
	void* m_Vertices;
	uint32_t m_VerticesSize;
	uint32_t m_Usage;
	VertexLayout m_Layout;

	uint32_t m_Handle;
};

class ElementBuffer {
public:
	ElementBuffer(Usage usage = Usage::StaticDraw);
	~ElementBuffer();

	uint32_t GetIndexCount() { return m_IndicesSize / sizeof(uint32_t); }

	void SetData(const std::vector<uint32_t>& indices);
	void SetData(uint32_t* indices, uint32_t size);

	void Initialize();

	void Bind();
	void Unbind();

private:
	void* m_Indices;
	uint32_t m_IndicesSize;
	uint32_t m_Usage;

	uint32_t m_Handle;
};

class FrameBuffer {
public:
	enum AttachmentType {
		ColorAttachment = BIT(0),
		DepthAttachment = BIT(1)
	};
	struct Attachment {
		uint32_t type;
		// used when creating texture
		TexFormat format = TexFormat::RGB;
		// used when creating texture
		TexFormat internalFormat = TexFormat::RGB;
		// used when creating color texture
		DataType dataType = DataType::UnsignedByte;
	};

	struct Config {
		uint32_t width;
		uint32_t height;

		// color textures will be attached in the order given here
		// depth attachment must be at the end of the list if it exists
		std::vector<Attachment> attachments;
	};

public:
	FrameBuffer(const Config& config);

	void Resize(uint32_t width, uint32_t height);

	void Bind();
	void Unbind();

	Texture* GetColorAttachment(uint32_t id);
	Texture* GetDepthAttachment();

private:
	uint32_t m_Handle;
	uint32_t m_RBO;

	uint32_t m_Width;
	uint32_t m_Height;

	uint32_t m_ColorAttachmentCount;
	uint32_t m_DepthAttachmentID;
	uint32_t* m_Buffers;
	std::vector<Texture*> m_Attachments;
};