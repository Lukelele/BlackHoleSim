#pragma once

struct VertexAttribute
{
	VertexAttribute(unsigned int attributeIndex, unsigned int attributeCountsPerVertex, unsigned int attributeDataVariableType, bool attributeNormalised = 0, int attributeStride = 0, void* attributeStartPointer = 0)
		:index(attributeIndex), countsPerVertex(attributeCountsPerVertex), dataVariableType(attributeDataVariableType), normalised(attributeNormalised), stride(attributeStride), startPointer(attributeStartPointer)
	{
	}
	VertexAttribute();

	unsigned int index;
	unsigned int countsPerVertex;
	unsigned int dataVariableType;
	bool normalised;
	int stride;
	void* startPointer;
};