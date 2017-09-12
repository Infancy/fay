#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef FAY_OPENGL_MODEL_H
#define FAY_OPENGL_MODEL_H

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "fay.h"
#include "shader.h"

namespace fay
{

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;

	glm::vec3 tangent;
	glm::vec3 bitangent;
};

// һ�� Texture ���Ա����е� mesh ���ã�����ʹ�ø� texture �� mesh ��¼���� OpenGL �е� id ����
struct Texture
{
	Texture(GLuint id, std::string type, aiString path) : id(id), type(type), filepath(path) {}

	GLuint	    id;
	std::string type;	// enum class Type { diffuse, specular, normal, height } type;
	aiString    filepath;
};

using namespace std;


class Mesh // : public boost::noncopyable
{
public:
	Mesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures);
	
	void draw(Shader shader);

private:
	vector<Texture> textures;

	GLuint VAO, VBO, EBO;
	GLuint indices_size, textures_size;
};


class Model
{
public:
	Model(const std::string& path, bool gamma = false);
	~Model() {}

	void draw(Shader shader);

private:
	void process_node(aiNode *node, const aiScene *scene);

	Mesh process_mesh(aiMesh *mesh, const aiScene *scene);

	std::vector<Texture> load_textures(aiMaterial* mat, aiTextureType type, std::string typeName);

	GLuint create_texture(const char* filename, bool gamma = false);

private:
	std::vector<Mesh> meshes;

	//map<path, bool>
	std::vector<Texture> textures_loaded;	// �����Ѽ��ص����������ظ�����
	std::string resources_directory;
	bool gamma_correction;
};

}	//namespace fay

#endif //FAY_OPENGL_MODEL_H
