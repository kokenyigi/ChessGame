#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture
{
private:

	unsigned int m_textureId;
	unsigned char* m_localTextureBuffer;
	int m_width;
	int m_height;
	int m_bitsPerPixel;

public:
	Texture();

	
	void Init(const std::string& textureFilePath);
	void Init(int width, int height);

	void Resize(int width, int height);

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_width; }
	inline int GetHeight() const { return m_height; }
	inline unsigned int GetId() const {return m_textureId;}

	void Delete();
};


#endif