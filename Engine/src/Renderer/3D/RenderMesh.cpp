#include "pch.h"
#include "RenderMesh.h"

namespace ft {
	RenderMesh::RenderMesh(bool isStatic) : m_Static(isStatic)
	{
		if (isStatic)
			return; // Static meshes are owned by the Renderer

		auto layout = BufferLayout({
			{ LayoutElementType::Float3, "inPosition" },
			{ LayoutElementType::Float3, "inNormal" }
			});

		m_VertexArray = std::unique_ptr<VertexArray>(VertexArray::Create(layout));

		m_VertexBuffer = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(1024));
		m_IndexBuffer = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(1024));

		m_VertexArray->SetVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void RenderMesh::Upload()
	{
		if (m_Static)
			return;
		m_VertexBuffer->SetData(vertexByteOffset, GetVertexCount() * m_VertexArray->GetBufferLayout().GetStride(), vertices.data());
		m_IndexBuffer->SetData(indexOffset, GetIndexByteSize(), indices.data());
	}

	void RenderMesh::Bind() const
	{
		m_VertexArray->Bind();
		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind();
	}
}