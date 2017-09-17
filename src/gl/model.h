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

struct modelVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;

	glm::vec3 tangent;
	glm::vec3 bitangent;
};

// һ�� modelTexture ���Ա����е� mesh ���ã�����ʹ�ø� texture �� mesh ��¼���� OpenGL �е� id ����
struct modelTexture
{
	modelTexture(GLuint id, std::string type, aiString path, std::string _filepath) : 
		id(id), type(type), filepath(path), _filepath(_filepath) {}

	GLuint	    id;
	std::string type;	// enum class Type { diffuse, specular, normal, height } type;
	aiString    filepath;
	std::string _filepath;	// ��������Ĳ��ִ��޸�
};

class modelMesh // : public boost::noncopyable
{
public:
	modelMesh(std::vector<modelVertex>& vertices, std::vector<uint32_t>& indices, std::vector<modelTexture>& textures);
	
	void draw(Shader shader);

public:
	std::vector<modelVertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<modelTexture> textures;

	GLuint VAO, VBO, EBO;
	GLuint indices_size, textures_size;
};

class Model
{
public:
	Model(const std::string& path, bool gamma = false);
	~Model() {}

	void draw(Shader shader);

	void transform_to_TextureDataArray(
		std::vector<glm::vec4>& positions,
		std::vector<glm::uvec4>& indices,
		std::vector<std::string>& texpaths)
	{
		/* ���ǵ��ϲ����ظ���������� texpaths ���ܸ�Сһ��
		positions[meshPosits1,  meshPosits2,  ...... meshPositsN ]
		indices  [meshinds1,    meshinds2,    ...... meshindsN   ]
		texpahts [meshtexpath1, meshtexpath2, ...... meshtexpathN] 
		*/
		//positions.reserve(meshes.size()); indices.reserve(meshes.size());
		for (auto& mesh : meshes)
		{
			for (auto& mver : mesh.vertices)
				positions.emplace_back(glm::vec4{ mver.position, 0.f });
			
			uint32_t tex_index{};
			if (!mesh.textures.empty())
			{
				// ֻʹ�õ�һ������
				std::string tex_filepath = { mesh.textures[0]._filepath };

				if (std::find(texpaths.begin(), texpaths.end(), tex_filepath) == texpaths.end())
					texpaths.push_back(tex_filepath);

				for (; tex_index < texpaths.size(); ++tex_index)
					if (texpaths[tex_index] == tex_filepath)
						break;
			}
			else
				tex_index = 255;

			// ����ɫ����ͨ�� tex_index����������±꣬�ҵ���Ӧ������
			auto& minds = mesh.indices;
			for (size_t j{}; j < minds.size(); j += 3)
				indices.emplace_back(glm::vec4{ minds[j], minds[j + 1], minds[j + 2], tex_index });
		}
		LOG(INFO) << " transform_to_TextureDataArray: "
			<< " positions: " << positions.size()
			<< " indices: "   << indices.size()
			<< " texpaths: "  << texpaths.size();
	}

private:
	void process_node(aiNode *node, const aiScene *scene);

	modelMesh process_mesh(aiMesh *mesh, const aiScene *scene);

	std::vector<modelTexture> load_textures(aiMaterial* mat, aiTextureType type, std::string typeName);

	GLuint create_texture(const char* filename, bool gamma = false);

private:
	std::vector<modelMesh> meshes;

	//map<path, bool>
	std::vector<modelTexture> textures_loaded;	// �����Ѽ��ص����������ظ�����
	std::string resources_directory;
	bool gamma_correction;
};

}	//namespace fay

#endif //FAY_OPENGL_MODEL_H
