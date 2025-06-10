#include "Buffer.h"
#include "Renderer/RenderAPI.h"
#include <glad/glad.h>

VertexLayout::VertexLayout(const std::vector<DataType>& layout)
{
	m_Layout = layout;
	m_Stride = 0;
	for (int i = 0; i < m_Layout.size(); ++i) {
		m_Stride += DataTypeSize(m_Layout[i]);
	}
}
VertexLayout::~VertexLayout()
{
	m_Layout.clear();
	m_Layout = std::vector<DataType>();
}
void VertexLayout::SetAttributes()
{
	uint32_t offset = 0;
	for (int i = 0; i < m_Layout.size(); ++i) {
		glVertexAttribPointer(i, DataTypeCount(m_Layout[i]), GL_DataType(m_Layout[i]), GL_FALSE, m_Stride, (void*)offset);
		glEnableVertexAttribArray(i);
		offset += DataTypeSize(m_Layout[i]);
	}
}

VertexBuffer::VertexBuffer(const VertexLayout& layout, Usage usage)
{
	m_Layout = layout;
	m_Vertices = (void*)NULL;
	m_VerticesSize = 0;
	m_Usage = GL_Usage(usage);
	glGenBuffers(1, &m_Handle);
}
VertexBuffer::~VertexBuffer()
{
	m_Vertices = nullptr;
	glDeleteBuffers(1, &m_Handle);
}
void VertexBuffer::Initialize()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
	glBufferData(GL_ARRAY_BUFFER, m_VerticesSize, m_Vertices, m_Usage);

	m_Layout.SetAttributes();
}
void VertexBuffer::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
}
void VertexBuffer::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ElementBuffer::ElementBuffer(Usage usage)
{
	m_Indices = (void*)0;
	m_IndicesSize = 0;
	m_Usage = GL_Usage(usage);
	glGenBuffers(1, &m_Handle);
}
ElementBuffer::~ElementBuffer()
{
	m_Indices = nullptr;
	glDeleteBuffers(1, &m_Handle);
}
void ElementBuffer::SetData(const std::vector<uint32_t>& indices)
{
	m_Indices = (void*)indices.data();
	m_IndicesSize = indices.size() * sizeof(uint32_t);
}
void ElementBuffer::SetData(uint32_t* indices, uint32_t size)
{
	m_Indices = (void*)indices;
	m_IndicesSize = size;
}
void ElementBuffer::Initialize()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndicesSize, m_Indices, m_Usage);
}
void ElementBuffer::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
}
void ElementBuffer::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

FrameBuffer::FrameBuffer(const Config& config)
{
	m_Width = config.width;
	m_Height = config.height;

	m_ColorAttachmentCount = 0;
	m_DepthAttachmentID = 0;

	// used to make see if we need to make a RBO
	bool depthFlag = false;
	
	glGenFramebuffers(1, &m_Handle);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);

	for (int i = 0; i < config.attachments.size(); ++i) {
		auto& attachment = config.attachments[i];

		TextureConfig textureConfig;
		textureConfig.data = (void*)NULL;
		textureConfig.width = m_Width;
		textureConfig.height = m_Height;
		textureConfig.format = attachment.format;
		textureConfig.internalFormat = attachment.internalFormat;
		textureConfig.dataType = attachment.dataType;

		if (attachment.type & AttachmentType::DepthAttachment) {
			depthFlag = true;

			textureConfig.dataType = DataType::UnsignedInt_24_8;
			Texture* depthTexture = new Texture(textureConfig);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture->GetHandle(), 0);
			m_DepthAttachmentID = i;
			m_Attachments.push_back(depthTexture);
			break;
		}

		if (attachment.type & AttachmentType::ColorAttachment) {
			Texture* colorTexture = new Texture(textureConfig);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_ColorAttachmentCount, GL_TEXTURE_2D, colorTexture->GetHandle(), 0);
			m_Attachments.push_back(colorTexture);
			m_ColorAttachmentCount += 1;
		}
	}
	m_Buffers = new uint32_t[m_ColorAttachmentCount];
	for (int i = 0; i < m_ColorAttachmentCount; ++i) {
		m_Buffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	if (!depthFlag) {
		glGenRenderbuffers(1, &m_RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		SOFT_ERROR("Failed to create Frame Buffer");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FrameBuffer::Resize(uint32_t width, uint32_t height)
{
	m_Width = width;
	m_Height = height;

	glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);

	RenderAPI::SetViewPortSize(width, height);

	for (auto t : m_Attachments) {
		t->Resize(width, height);
	}

	if (m_DepthAttachmentID == 0) {
		glDeleteRenderbuffers(1, &m_RBO);

		glGenRenderbuffers(1, &m_RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
	glDrawBuffers(m_Attachments.size(), m_Buffers);
}
void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLenum bufs[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, bufs);
}
Texture* FrameBuffer::GetColorAttachment(uint32_t id)
{
	if (id >= m_ColorAttachmentCount) {
		WARNING("Color attachment out of bounds");
		return nullptr;
	}
	return m_Attachments[id];
}
Texture* FrameBuffer::GetDepthAttachment()
{
	if (m_DepthAttachmentID == 0) {
		WARNING("Frame buffer doesn't have a depth buffer");
		return nullptr;
	}
	return m_Attachments[m_DepthAttachmentID];
}
