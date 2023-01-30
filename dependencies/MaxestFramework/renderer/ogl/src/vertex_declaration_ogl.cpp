#ifdef RENDERER_OGL

#include <renderer_ogl/vertex_declaration_ogl.hpp>



void CVertexDeclaration::create(const string& name)
{
	destroy();

	this->size = 0;
	this->hash = "";

	this->name = name;
}



void CVertexDeclaration::destroy()
{
	vertexElements.clear();
}



void CVertexDeclaration::addVertexElement(byte offset, VertexElementType::TYPE type, VertexElementSemantic::TYPE semantic, byte semanticIndex)
{
	VertexElement vertexElement;

	vertexElement.offset = offset;
	vertexElement.type = type;
	vertexElement.semantic = semantic;
	vertexElement.semanticIndex = semanticIndex;

	vertexElements.push_back(vertexElement);
}



void CVertexDeclaration::build()
{
	VertexElement& lastVertexElement = vertexElements[vertexElements.size() - 1];
	size = lastVertexElement.offset + VertexElementType::getSize(lastVertexElement.type);

	hash = "";
	for (uint i = 0; i < vertexElements.size(); i++)
	{
		hash +=
			toString(vertexElements[i].offset) + "|" +
			toString(vertexElements[i].type) + "|" +
			toString(vertexElements[i].semantic) + "|" +
			toString(vertexElements[i].semanticIndex);
	}
}



#endif
