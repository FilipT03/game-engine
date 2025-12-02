#include "pch.h"
#include "GLVertexArray.h"
#include <glad/glad.h>

namespace ft {
	static GLenum GLTypeFromLayoutElementType(LayoutElementType type)
	{
		switch (type)
		{
		case LayoutElementType::Float:   return GL_FLOAT;
		case LayoutElementType::Float2:  return GL_FLOAT;
		case LayoutElementType::Float3:  return GL_FLOAT;
		case LayoutElementType::Float4:  return GL_FLOAT;
		case LayoutElementType::Int:     return GL_INT;
		case LayoutElementType::Int2:    return GL_INT;
		case LayoutElementType::Int3:    return GL_INT;
		case LayoutElementType::Int4:    return GL_INT;
		case LayoutElementType::Mat3:    return GL_FLOAT;
		case LayoutElementType::Mat4:    return GL_FLOAT;
		case LayoutElementType::Bool:    return GL_BOOL;
		default:                         return 0;
		}
	}

	GLVertexArray::GLVertexArray(BufferLayout layout) : m_Layout(layout)
	{
		glGenVertexArrays(1, &m_ID);
		glBindVertexArray(m_ID);
	}

	GLVertexArray::~GLVertexArray() 
	{
		glDeleteVertexArrays(1, &m_ID);
	}
	
	void GLVertexArray::Bind() const
	{
		glBindVertexArray(m_ID);
	}
	
	void GLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}
	
	void GLVertexArray::SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		m_VertexBuffer = vertexBuffer;

		glBindVertexArray(m_ID);
		vertexBuffer->Bind();

		int index = 0;
		for (auto& element : m_Layout.GetElements())
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, element.GetComponentCount(), GLTypeFromLayoutElementType(element.Type), 
				element.Normalized ? GL_TRUE : GL_FALSE, m_Layout.GetStride(), (const void*)element.Offset);
			index++;
		}
	}
	
	void GLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		m_IndexBuffer = indexBuffer;

		glBindVertexArray(m_ID);
		indexBuffer->Bind();
	}
}