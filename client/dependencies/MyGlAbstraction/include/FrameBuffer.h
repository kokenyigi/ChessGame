#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Texture.h"
#include "RenderBuffer.h"


class FrameBuffer
{
private:
    unsigned int m_fboID = 0;
public:
    FrameBuffer(){}
    ~FrameBuffer(){}

    void Init();

    void AttachTexture(const Texture& texture);
    void AttachRenderBuffer(const RenderBuffer& renderBuffer);

    void Bind();
    void Unbind();

    void Delete();
private:
};

#endif