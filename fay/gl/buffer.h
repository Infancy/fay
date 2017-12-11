#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef FAY_GL_BUFFER_H
#define FAY_GL_BUFFER_H

#include <glad/glad.h>
#include "fay/utility/fay.h"

namespace fay
{

class Buffer // : boost::noncopyable
{
public:
	// Ҫ�����λ��������ά��
	Buffer(std::vector<float>& positions, std::vector<uint32_t>& indices);
	~Buffer();

	void draw();

private:
	GLuint indices_size;
	GLuint VBO, EBO, VAO;
};

class VertexBuffer // : boost::noncopyable
{
public:
	// Ҫ����Ķ������ǰ����㡢������������
	VertexBuffer(std::vector<float>& vertices, std::vector<uint32_t>& indices);

	void draw();

private:
	GLuint indices_size;
	GLuint VBO, EBO, VAO;
};

} // namespace fay


#endif // FAY_GL_BUFFER_H