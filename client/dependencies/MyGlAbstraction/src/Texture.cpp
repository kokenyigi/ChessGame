#include "Texture.h"

#include "Debug.h"

#include "stb_image.h"

Texture::Texture() :
	m_localTextureBuffer(nullptr),
	m_width(0), m_height(0), m_bitsPerPixel(0),m_textureId(0)
{
	
}

void Texture::Init(const std::string& textureFilePath)
{
	
	stbi_set_flip_vertically_on_load(1);
	m_localTextureBuffer = stbi_load(textureFilePath.c_str(), &m_width, &m_height, &m_bitsPerPixel, 4);

	GLCall(glGenTextures(1, &m_textureId));
	
	Bind(0);

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localTextureBuffer));

	Unbind();

	if (m_localTextureBuffer)
	{
		stbi_image_free(m_localTextureBuffer);
	}
}

void Texture::Init(int width, int height)
{
	GLCall(glGenTextures(1, &m_textureId));

	m_width = width;
	m_height = height;

	Bind(0);

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

	Unbind();
}

void Texture::Resize(int width, int height)
{
	m_width = width;
	m_height = height;

	Bind(0);

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

	Unbind();
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_textureId));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::Delete()
{
	GLCall(glDeleteTextures(0, &m_textureId));
}
