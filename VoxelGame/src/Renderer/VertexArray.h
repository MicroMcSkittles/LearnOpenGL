#pragma once
#include <stdint.h>
#include "Renderer/Buffer.h"

class VertexArray {
public:
	VertexArray();
	~VertexArray();

	VertexBuffer* GetVertexBuffer() { return m_VertexBuffer; }
	ElementBuffer* GetElementBuffer() { return m_ElementBuffer; }

	void AttachBuffer(VertexBuffer* vertexBuffer);
	void AttachBuffer(ElementBuffer* elementBuffer);

	void Bind();
	void Unbind();

private:
	VertexBuffer* m_VertexBuffer;
	ElementBuffer* m_ElementBuffer;
	uint32_t m_Handel;
};