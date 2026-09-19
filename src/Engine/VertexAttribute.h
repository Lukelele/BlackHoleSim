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

	unsigned int Index;
	unsigned int CountPerVertex;
	unsigned int DataType;
	bool Normalised;
	int Stride;
	void* StartPointer;
};