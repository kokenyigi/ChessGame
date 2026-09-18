#include "Debug.h"
#include "VertexArray.h"

VertexArray::VertexArray()
{
	m_vaoID = 0;
}

void VertexArray::Init()
{
	GLCall(glGenVertexArrays(1, &m_vaoID));
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_vaoID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}

void VertexArray::Delete()
{
	GLCall(glDeleteVertexArrays(1, &m_vaoID));
}