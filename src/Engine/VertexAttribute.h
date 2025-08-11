#pragma once

#include <cstddef>


struct VertexAttribute
{
	VertexAttribute(unsigned int index, unsigned int countPerVertex, unsigned int dataType, bool normalised = 0, size_t stride = 0, size_t startPointer = 0)
		: Index(index), CountPerVertex(countPerVertex), DataType(dataType), Normalised(normalised), Stride(stride), StartPointer((void*)startPointer)
	{
	}
	VertexAttribute();

	unsigned int Index;
	unsigned int CountPerVertex;
	unsigned int DataType;
	bool Normalised;
	int Stride;
	void* StartPointer;
};