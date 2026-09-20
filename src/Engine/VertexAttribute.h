#pragma once

#include <cstddef>
#include <glad/glad.h>


struct VertexAttribute
{
	VertexAttribute(unsigned int index, unsigned int countPerVertex, unsigned int dataType = GL_FLOAT, bool normalised = 0, size_t stride = 0, size_t startPointer = 0)
		: Index(index), CountPerVertex(countPerVertex), DataType(dataType), Normalised(normalised), Stride(stride), StartPointer((void*)startPointer)
	{
	}
	VertexAttribute() = default;

	unsigned int Index = 0;
	unsigned int CountPerVertex = 0;
	unsigned int DataType = GL_FLOAT;
	bool Normalised = false;
	int Stride = 0;
	void* StartPointer = nullptr;
};