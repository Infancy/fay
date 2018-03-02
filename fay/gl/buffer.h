#if defined(_MSC_VER)
#pragma once
#endif

#ifndef FAY_GL_BUFFER_H
#define FAY_GL_BUFFER_H

#include "fay/gl/resources.h"

namespace fay
{

// template<int VertexAttrib>
// template<bool onlyPosition> Mesh
class Buffer // : boost::noncopyable
{
public:
	// vec3 position, vec3 normal, vec2 texCoords, vec3 tangent, vec3 bitangent
	// vertexAttribPointer
	Buffer(std::vector<Vertex1>& vertices, std::vector<uint32_t>& indices);
	Buffer(std::vector<Vertex3>& vertices, std::vector<uint32_t>& indices);
	Buffer(std::vector<Vertex5>& vertices, std::vector<uint32_t>& indices);
	//Buffer(std::vector<Vertex5>& vertices, std::vector<uint32_t>& indices, std::function<void()> vertexattribpointer);

	void draw();

	~Buffer(){ /* glDeleteVertexArrays(1, &VAO); glDeleteBuffers(1, &VBO); glDeleteBuffers(1, &IBO); */ }

private:
	GLuint vao{}, vbo{}, ibo{};
	GLsizei isz{};	// indices_size
};

} // namespace fay

#endif // FAY_GL_BUFFER_H