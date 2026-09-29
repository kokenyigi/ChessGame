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

float FrameBuffer::GetDepthValueAt(int x, int y)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER,this->m_fboID);

    glReadBuffer(GL_NONE);
    float depthValue = 0.0f;
    glReadPixels(x,y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depthValue);

    glBindFramebuffer(GL_READ_FRAMEBUFFER,0);

    return depthValue;
}