#include "texture.h"
#include "stb_image.h"

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

using namespace std;

static void load_image(string& fullname, GLubyte* &data, int& width, int& height, GLenum& format)
{
	int channels{};
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(fullname.c_str(), &width, &height, &channels, 0);
	if (data == nullptr)
	{
		LOG(ERROR) << "Cannot load image: " << fullname;
		exit(EXIT_FAILURE);
	}

	switch (channels)
	{
	case 1: format = GL_RED;	break;	// grey
	case 2:	format = GL_RG32UI; break;	// grey&alpha
	case 3: format = GL_RGB;	break;
	case 4: format = GL_RGBA;	break;
	default:
		LOG(ERROR) << "channels failed to choose"; break;
	}
}

static void free_image(GLubyte* data) { stbi_image_free(data); }

Texture::Texture(GLenum format, GLint filtering, GLint wrap) : texture_id{}
{
	glGenTextures(1, &texture_id);
	LOG_IF(WARNING, texture_id == 0u) << "Fail to create texture!";
	//glActiveTexture(GL_TEXTURE0 + tex_unit);
	glBindTexture(format, texture_id);

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

Texture2D::Texture2D(const char* filename, bool Mipmap) : Texture(GL_TEXTURE_2D, GL_LINEAR, GL_REPEAT)
{
	int width, height, nrComponents;
	// stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename, &width, &height, &nrComponents, 0);
	CHECK(data != nullptr) << "Texture failed to load at path: ";

	GLenum format{};
	switch (nrComponents)
	{
	case 1: format = GL_RED; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	default:
		LOG(ERROR) << "nrComponents failed to choose"; break;
	}

	create_texture2d(format, width, height, format, GL_UNSIGNED_BYTE, data, Mipmap);

	stbi_image_free(data);
}

Texture2D::Texture2D(GLint internalFormat, GLsizei width, GLsizei height,
	GLenum format, GLenum type, unsigned char* pixels, bool Mipmap) : Texture(GL_TEXTURE_2D, GL_LINEAR, GL_REPEAT)
{
	CHECK(pixels != nullptr) << "pixels in Texture2D is nullptr";
	create_texture2d(internalFormat, width, height, format, type, pixels, Mipmap);
}

void Texture2D::create_texture2d(GLint internalFormat, GLsizei width, GLsizei height,
	GLenum format, GLenum type, unsigned char* data, bool Mipmap)
{
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
		format, type, data);

	if(Mipmap)
	{ 
		glGenerateMipmap(GL_TEXTURE_2D);	// Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// �༶����ֻ����������С�����
	}
}

TextureArray::TextureArray(vector<string> material_names, string filepath) :
	Texture(GL_TEXTURE_2D_ARRAY), material_nums{ material_names.size() }
{
	GLubyte* data{};
	int width = 0, height = 0;
	GLenum format{};

	for(int i = 0; i < material_names.size(); ++i)
	{ 
		auto fullpath = filepath + material_names[i];	// ��ǰ·�� + �ļ���
		load_image(fullpath, data, width, height, format);
		CHECK(data != nullptr);

		// �״��ȴ���һ�� 2D ��������
		if (i == 0)
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height, material_nums, 0, format, GL_UNSIGNED_BYTE, NULL);

		//modify the existing texture
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, data);
		free_image(data);
	}
	gl_check_errors();
}


} //namespace fay