#include "pch.h"
#include "GLBuffers.h"

#include <glad/glad.h>

namespace ft {
	/// Vertex Buffer
	GLVertexBuffer::GLVertexBuffer(const float* data, uint32_t size) : m_Size(size)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	};

	GLVertexBuffer::GLVertexBuffer(uint32_t size) : m_Size(size)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);

		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	};
	
	void GLVertexBuffer::Bind() const 
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	};

	void GLVertexBuffer::Unbind() const 
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	};

	GLVertexBuffer::~GLVertexBuffer() 
	{
		glDeleteBuffers(1, &m_ID);
	};


	/// Index Buffer
	GLIndexBuffer::GLIndexBuffer(const uint32_t* data, uint32_t count) : m_Count(count)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
	};

	void GLIndexBuffer::Bind() const 
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	};

	void GLIndexBuffer::Unbind() const 
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	};

	GLIndexBuffer::~GLIndexBuffer() 
	{
		glDeleteBuffers(1, &m_ID);
	};
}