#include "texture.h"
#include "fay/utility/image.h"

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

Texture::Texture(GLenum format, GLint filtering, GLint wrap) : texture_id{}
{
	glGenTextures(1, &texture_id);
	LOG_IF(WARNING, texture_id == 0u) << "Fail to create texture!";
	//glActiveTexture(GL_TEXTURE0 + tex_unit);
	glBindTexture(format, texture_id);	// ֮���κε�����ָ������ڵ�ǰ�󶨵�����

	glTexParameteri(format, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(format, GL_TEXTURE_MAG_FILTER, filtering);

	glTexParameteri(format, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(format, GL_TEXTURE_WRAP_T, wrap);
	if(format == GL_PROXY_TEXTURE_3D)
		glTexParameteri(format, GL_TEXTURE_WRAP_R, wrap);
	/*
	���ѡ���˱�Ե���ˣ�����Ҫָ����Ե��ɫ
	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	*/

	gl_check_errors();
}

// -----------------------------------------------------------------------------

Texture2D::Texture2D(const std::string& filepath, TexType textype, bool Mipmap) 
	: Texture(GL_TEXTURE_2D, GL_LINEAR, GL_REPEAT), texture_type{ textype }
{
	const Image img(filepath, Thirdparty::gl);
	w = img.width(), h = img.height();
	fmt = img.gl_format();

	create_texture2d(fmt, img.width(), img.height(), fmt, GL_UNSIGNED_BYTE, img.data(), Mipmap);
}

Texture2D::Texture2D(const Image& img, TexType textype, bool Mipmap)
	: Texture(GL_TEXTURE_2D, GL_LINEAR, GL_REPEAT), texture_type{ textype }, 
	w{ img.width() }, h{ img.height() }
{
	CHECK(img.third_party() == Thirdparty::gl) << "image thirdparty error";
	fmt = img.gl_format();

	create_texture2d(fmt, img.width(), img.height(), fmt, GL_UNSIGNED_BYTE, img.data(), Mipmap);
}

Texture2D::Texture2D(GLint internalFormat, GLsizei width, GLsizei height,
	GLenum format, GLenum type, unsigned char* pixels, bool Mipmap, TexType textype)
	: Texture(GL_TEXTURE_2D, GL_LINEAR, GL_REPEAT), texture_type{ textype },
	w{ width }, h{ height }, fmt{ format }	// TODO:internalFormat
{
	CHECK(pixels != nullptr) << "pixels in Texture2D is nullptr";
	create_texture2d(internalFormat, width, height, format, type, pixels, Mipmap);
}

void Texture2D::create_texture2d(GLint internalFormat, GLsizei width, GLsizei height,
	GLenum format, GLenum type, const unsigned char* data, bool Mipmap)
{
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
		format, type, data);

	if(Mipmap)
	{ 
		glGenerateMipmap(GL_TEXTURE_2D);	// Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// �༶����ֻ����������С�����
	}
}

// -----------------------------------------------------------------------------

Texture2DArray::Texture2DArray(std::vector<std::string> material_names, std::string path) :
	Texture(GL_TEXTURE_2D_ARRAY), material_nums{ material_names.size() }
{
	for(int i = 0; i < material_names.size(); ++i)
	{ 
		const Image img(path + material_names[i], Thirdparty::gl);	// ��ǰ·�� + �ļ���
		auto format = img.gl_format();

		// �״��ȴ���һ�� 2D ��������
		if (i == 0)
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, img.width(), img.height(), material_nums, 0, format, GL_UNSIGNED_BYTE, NULL);

		//modify the existing texture
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, img.width(), img.height(), 1, format, GL_UNSIGNED_BYTE, img.data());
	}
	gl_check_errors();
}


} // namespace fay