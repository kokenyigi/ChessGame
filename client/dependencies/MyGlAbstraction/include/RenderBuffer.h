#pragma once
#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

class RenderBuffer
{
private:
    unsigned int m_rboId = 0;
    int m_width;
    int m_height;
public:
    void Init(int width, int height);
    
    void Resize(int width, int height);

	void Bind() const;
	void Unbind() const;

	inline int GetWidth() const { return m_width; }
	inline int GetHeight() const { return m_height; }
	inline unsigned int GetId() const {return m_rboId;}

	void Delete();
private:

};

#endif

