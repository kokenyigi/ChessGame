#pragma once
#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

class VertexArray
{
private:
	unsigned int m_vaoID;

public:
	VertexArray();

	void Init();

	void Bind() const;
	void Unbind() const;

	void Delete();
};


#endif
