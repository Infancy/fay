#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef FAY_GL_TEXTURE_H
#define FAY_GL_TEXTURE_H

#include "gl_utility.h"
#include "shader.h"

namespace fay
{

class Texture
{
public: 
	// ֻ���𴴽������Ҳ�ָ������Ԫ�������Ľӿڿ��Է�ֹ������
	Texture(GLenum format = GL_TEXTURE_2D, GLint filtering = GL_LINEAR, GLint wrap = GL_REPEAT);

	GLuint id() const { return texture_id; }
	/*
	void enable(uint32_t tex_unit, Shader& shader, std::string sampler) {
		glActiveTexture(GL_TEXTURE0 + tex_unit);	    // �����i������Ԫ
		shader.set_texture_unit(sampler, tex_unit);		// ����i������Ԫ���ӵ���ɫ���е�sampler����	
		glBindTexture(GL_TEXTURE_2D, texture_id);
	} 	// ���������󶨵���ǰ���������Ԫ��
	void disable() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	*/
private:
	GLuint texture_id;
	//GLuint texture_unit;
};

// ����������������Ų�ͬ�ļ�����Դ��ʽ

class Texture2D
{
public:
	Texture2D(const char* filename);

	GLuint id() const { return texture_id ; }
	/*
	void enable(uint32_t tex_unit = 0)  {
		glActiveTexture(GL_TEXTURE0 + tex_unit);	         
		glBindTexture(GL_TEXTURE_2D, texture_id); }      
	void enable(Shader& shader, std::string& sampler, uint32_t tex_unit) {
		glActiveTexture(GL_TEXTURE0 + tex_unit);	    // �����i������Ԫ
		shader.set_texture_unit(sampler, tex_unit);		// ����i������Ԫ���ӵ���ɫ���е�sampler����	
		glBindTexture(GL_TEXTURE_2D, texture_id); } 	// ���������󶨵���ǰ���������Ԫ��
	void disable() { 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0); }
	*/
private:
	GLuint texture_id;
};

class TextureArray : public Texture
{
public:
	TextureArray(std::vector<std::string> material_names, std::string filepath = {});

	size_t size() const { return material_nums; }

private:
	size_t material_nums;
};

// Texture1D??
template<typename T>
class TextureData : public Texture
{
public:
	TextureData(GLint internalFormat, GLsizei width, 
		GLenum format, GLenum type, std::vector<T>& data) :
		Texture(GL_TEXTURE_2D, GL_NEAREST, GL_REPEAT)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, 1, 0,
			format, type, data.data());
		gl_check_errors();
	}
};

}	//namespace fay

#endif //FAY_GL_TEXTURE_H
