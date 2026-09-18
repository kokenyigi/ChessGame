#include "FrameBuffer.h"

#include "Debug.h"

void FrameBuffer::Init()
{
    GLCall(glGenFramebuffers(1, &m_fboID));
}

void FrameBuffer::AttachTexture(const Texture &texture)
{
    Bind();

    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.GetId(), 0));

    Unbind();
}

void FrameBuffer::AttachRenderBuffer(const RenderBuffer &renderBuffer)
{
    Bind();

    GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.GetId()));

    Unbind();
}

void FrameBuffer::Bind()
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_fboID));
}

void FrameBuffer::Unbind()
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
