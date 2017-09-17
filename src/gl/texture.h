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
		glActiveTexture(GL_TEXTURE0 + tex_unit);	
		shader.set_texture_unit(sampler, tex_unit);			
		glBindTexture(GL_TEXTURE_2D, texture_id);}
	void disable() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);}
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

// Texture1D??
template<typename T>
class TextureData : public Texture
{
public:
	// ���뿼�� width �� format �Ĺ�ϵ���� data<float> �� GL_RGBA���� width = data.size() / 4
	TextureData(GLint internalFormat, GLsizei width, 
		GLenum format, GLenum type, std::vector<T>& data) :
		Texture(GL_TEXTURE_2D, GL_NEAREST, GL_REPEAT)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, 1, 0,
			format, type, data.data());
		gl_check_errors();
	}
};

class TextureArray : public Texture
{
public:
	// ���뱣֤ GL_UNSIGNED_BYTE �� type���ҳߴ���ͬ
	TextureArray(std::vector<std::string> material_names, std::string filepath = {});

	size_t size() const { return material_nums; }

private:
	size_t material_nums;
};

//glm::vec4��glm::uvec4��glm::ivec4
template<typename T>
class TextureDataArray : public Texture
{
public:
	TextureDataArray(GLint internalFormat, GLenum format, GLenum type, std::vector<std::vector<T>>& datas) :
		Texture(GL_TEXTURE_2D_ARRAY, GL_NEAREST, GL_REPEAT)
	{
		// �ȴ���һ�� 2D ��������
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat,
			datas[0].size(), 1, datas.size(),     0,
			format, type, NULL);

		for (auto& data : datas)
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 
				0, 0, i, 
				data.size(), 1, 1, format, type, data.data());

		gl_check_errors();
	}
};

}	//namespace fay

#endif //FAY_GL_TEXTURE_H
