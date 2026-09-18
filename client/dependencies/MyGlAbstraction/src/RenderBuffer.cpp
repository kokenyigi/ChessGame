#include "RenderBuffer.h"

#include "Debug.h"

void RenderBuffer::Init(int width, int height)
{
	GLCall(glGenRenderbuffers(1, &m_rboId));

	m_width = width;
	m_height = height;

	Bind();

	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,width,height));

	Unbind();
}

void RenderBuffer::Resize(int width, int height)
{
    m_width = width;
	m_height = height;

	Bind();

	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,width,height));

	Unbind();
}

void RenderBuffer::Bind() const
{
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_rboId));
}

void RenderBuffer::Unbind() const
{
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

void RenderBuffer::Delete()
{
	GLCall(glDeleteRenderbuffers(1, &m_rboId));
}

