#include "VertexArray.h"
#include <glad/glad.h>

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_Handel);
	m_VertexBuffer = nullptr;
	m_ElementBuffer = nullptr;
}
VertexArray::~VertexArray()
{
	delete m_VertexBuffer;
	m_VertexBuffer = nullptr;

	delete m_ElementBuffer;
	m_ElementBuffer = nullptr;

	glDeleteVertexArrays(1, &m_Handel);
}

void VertexArray::AttachBuffer(VertexBuffer* vertexBuffer)
{
	m_VertexBuffer = vertexBuffer;
}
void VertexArray::AttachBuffer(ElementBuffer* elementBuffer)
{
	m_ElementBuffer = elementBuffer;
}

void VertexArray::Bind()
{
	glBindVertexArray(m_Handel);
}
void VertexArray::Unbind()
{
	glBindVertexArray(0);
}
