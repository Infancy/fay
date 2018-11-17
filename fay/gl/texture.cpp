#include "texture.h"

namespace fay
{

/*

������ˣ�
GL_NEAREST  Nearest Neighbor Filtering���ڽ����ˣ�OpenGLĬ�ϵ�������˷�ʽ��ѡ�����ĵ���ӽ�����������Ǹ�����
GL_LINEAR   (Bi)linear Filtering�����Թ��ˣ������������긽�����������أ������һ����ֵ�����Ƴ���Щ��������֮�����ɫ��

���Ʒ�ʽ��
GL_REPEAT			�������Ĭ����Ϊ���ظ�����ͼ��
GL_MIRRORED_REPEAT	��GL_REPEATһ������ÿ���ظ�ͼƬ�Ǿ�����õġ�
GL_CLAMP_TO_EDGE	��������ᱻԼ����0��1֮�䣬�����Ĳ��ֻ��ظ���������ı�Ե������һ�ֱ�Ե�������Ч����
GL_CLAMP_TO_BORDER	����������Ϊ�û�ָ���ı�Ե��ɫ��

*/

base_texture::base_texture(GLenum target, GLint filtering, GLint wrap) : 
	id_{}, target_{ target }
{
	glGenTextures(1, &id_);
	LOG_IF(WARNING, id_ == 0u) << "Fail to create texture!";
	//glActiveTexture(GL_TEXTURE0 + tex_unit);
	glBindTexture(target_, id_);	// ֮���κε�����ָ������ڵ�ǰ�󶨵�����

	if (target_ != GL_TEXTURE_2D_MULTISAMPLE)
	{
		glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, filtering);

		glTexParameteri(target_, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(target_, GL_TEXTURE_WRAP_T, wrap);
	}

	if (target_ == GL_PROXY_TEXTURE_3D ||
		target_ == GL_TEXTURE_CUBE_MAP)
		glTexParameteri(target_, GL_TEXTURE_WRAP_R, wrap);
	
	// ���ѡ���˱�Ե���ˣ�����Ҫָ����Ե��ɫ

	gl_check_errors();
}

void base_texture::set_border_color(std::array<GLfloat, 4> borderColor)
{
	glTexParameterfv(target_, GL_TEXTURE_BORDER_COLOR, borderColor.data());
}

// -----------------------------------------------------------------------------

texture2d::texture2d(const std::string& filepath, texture_format textype, bool Mipmap) 
	: base_texture(GL_TEXTURE_2D, GL_LINEAR, GL_REPEAT), textype_{ textype }
{
	const image_ptr img(filepath, true);

	create(img.gl_format(), img.width(), img.height(), img.gl_format(), GL_UNSIGNED_BYTE, img.data(), Mipmap);
}

texture2d::texture2d(const image_ptr& img, texture_format textype, bool Mipmap)
	: base_texture(GL_TEXTURE_2D, GL_LINEAR, GL_REPEAT), textype_{ textype }
{
	CHECK(img.is_flip_vertical() == true) << "image thirdparty error";
	// create(format_, img.width(), img.height(), format_, GL_UNSIGNED_BYTE, img.data(), Mipmap);
	create(img.gl_format(), img.width(), img.height(), img.gl_format(), GL_UNSIGNED_BYTE, img.data(), Mipmap);
}

texture2d::texture2d(GLint filtering, GLint wrap, texture_format textype)
	// : base_texture(GL_TEXTURE_2D, GL_LINEAR, GL_REPEAT), texture_format{ textype }
	: base_texture(GL_TEXTURE_2D, filtering, wrap), textype_{ textype }
{
}

void texture2d::create(GLint internalFormat, GLsizei width, GLsizei height,
	GLenum format, GLenum type, const uint8_t* data, bool Mipmap)
{
	// https://stackoverflow.com/questions/34497195/difference-between-format-and-internalformat
	
	w = width; h = height; format_ = internalFormat;

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);

	if(Mipmap)
	{ 
		glGenerateMipmap(GL_TEXTURE_2D);	// Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// �༶����ֻ����������С�����
	}
	gl_check_errors();
}

// -----------------------------------------------------------------------------

texture2d_array::texture2d_array(std::vector<std::string> material_names, std::string path) :
	base_texture(GL_TEXTURE_2D_ARRAY), material_nums{ material_names.size() }
{
	for(int i = 0; i < material_names.size(); ++i)
	{ 
		const image_ptr img(path + material_names[i], true);	// ��ǰ·�� + �ļ���
		auto format = img.gl_format();

		// �״��ȴ���һ�� 2D ��������
		if (i == 0)
		{
			format_ = format;
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, img.width(), img.height(), material_nums, 0, format, GL_UNSIGNED_BYTE, NULL);
		}
		CHECK(format_ == format) << "texture2d_array: different format!";
		//modify the existing texture
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, img.width(), img.height(), 1, format, GL_UNSIGNED_BYTE, img.data());
	}
	gl_check_errors();
}

} // namespace fay