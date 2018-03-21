#if defined(_MSC_VER)
#pragma once
#endif

#ifndef FAY_GL_TEXTURE_H
#define FAY_GL_TEXTURE_H

#include "gl.h"
#include "fay/utility/image.h"

namespace fay
{

// #if OPENGL_VERSION < ...

class BaseTexture
{
public: 
	// ֻ���𴴽������Ҳ�ָ������Ԫ�����������Ľӿ����ⱻ����
	BaseTexture(GLenum target = GL_TEXTURE_2D, GLint filtering = GL_LINEAR, GLint wrap = GL_REPEAT);

	void set_format(GLenum format) { format_ = format; }

	GLuint id() const { return id_; }
	GLenum target() const { return target_; }
	GLenum format() const { return format_; }

protected:
	GLuint id_;
	GLenum target_;
	GLenum format_;	// TODO: internalFormat??
};

// ����������������Ų�ͬ�ļ�����Դ��ʽ

// -----------------------------------------------------------------------------

class Texture2D : public BaseTexture
{
public:
	Texture2D(const std::string& filepath, TexType textype = TexType::diffuse, bool Mipmap = true);
	
	Texture2D(const ImagePtr& img, TexType textype = TexType::diffuse, bool Mipmap = true);
	
	Texture2D(GLint internalFormat, GLsizei width, GLsizei height,
		GLenum format, GLenum type, unsigned char* pixels, bool Mipmap = true, TexType textype = TexType::diffuse);

	int width()  const { return w; }
	int height() const { return h; }

	TexType type() const { return texture_type; }

private:
	void create_texture2d(GLint internalFormat, GLsizei width, GLsizei height,
		GLenum format, GLenum type, const unsigned char* pixels, bool Mipmap);

private:
	int w, h;
	TexType texture_type;
};

// -----------------------------------------------------------------------------

class TextureCube : public BaseTexture
{
public:
	// filepath + 6 * filename( right, left, top, bottom, back, front )
	TextureCube(const std::string files[7], TexType textype = TexType::cubemap)
		: BaseTexture(GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_CLAMP_TO_EDGE), texture_type{ textype }
	{
		int width, height, format;
		for (int i = 0; i < 6; ++i)
		{
			ImagePtr img(files[0] + files[i + 1]/*, Thirdparty::gl*/);

			if (i == 0)
				format_ = img.gl_format();

			CHECK(format_ == img.gl_format()) << "TextureCube: channels error!";
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format_,
				img.width(), img.height(), 0, format_, GL_UNSIGNED_BYTE, img.data());
		}
		gl_check_errors();
	}

	TexType type() const { return texture_type; }

private:
	TexType texture_type;
};

// -----------------------------------------------------------------------------

class Texture2DArray : public BaseTexture
{
public:
	// ���뱣֤ GL_UNSIGNED_BYTE �� type����ÿ������ĳߴ���ͬ
	// TODO�����������ĳ��������� glTexImage3D
	Texture2DArray(std::vector<std::string> material_names, std::string path = {});

	size_t size() const { return material_nums; }

private:
	size_t material_nums;
};

// ͨ���������������� ------------------------------------------------------------

// Texture1D??
template<typename T>
class TextureData : public BaseTexture
{
public:
	// ���뿼�� width �� format �Ĺ�ϵ���� data<float> �� GL_RGBA���� width = data.size() / 4
	TextureData(GLint internalFormat, GLsizei width, 
		GLenum format, GLenum type, std::vector<T>& data) :
		BaseTexture(GL_TEXTURE_2D, GL_NEAREST, GL_REPEAT)
	{
		format_ = internalFormat;
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, 1, 0,
			format, type, data.data());
		gl_check_errors();
	}
};

//glm::vec4��glm::uvec4��glm::ivec4
template<typename T>
class TextureDataArray : public BaseTexture
{
public:
	TextureDataArray(GLint internalFormat, GLenum format, GLenum type, std::vector<std::vector<T>>& datas) :
		BaseTexture(GL_TEXTURE_2D_ARRAY, GL_NEAREST, GL_REPEAT)
	{
		format_ = internalFormat;
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

} // namespace fay

#endif // FAY_GL_TEXTURE_H
