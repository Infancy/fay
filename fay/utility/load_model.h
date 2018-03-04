#if defined(_MSC_VER)
#pragma once
#endif

#ifndef FAY_UTILITY_LOAD_MODEL_H
#define FAY_UTILITY_LOAD_MODEL_H

#include "fay/gl/resources.h"
#include "fay/utility/image.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace fay
{

// template<tynename Vertex>
template<typename Vertex>
struct BaseMesh
{
	std::vector<Vertex>   vertices;
	std::vector<uint32_t> indices;
	std::vector<std::pair<Image, TexType>> images;

	BaseMesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, 
		std::vector<std::pair<Image, TexType>>& images) :
		vertices{ vertices }, indices{ indices }, images{ images }
	{}
};

using AssimpMesh = BaseMesh<Vertex5>;

// model -----------------------------------------------------------------------

struct BaseModel
{
	const std::string path;	// resources_directory
	Thirdparty api;

	// glm::vec3 min{}, max{};

	BaseModel(const std::string& filepath, Thirdparty api);
};

// load model by fay -----------------------------------------------------------

enum class ObjKeyword
{
	comment, // '#'
	v, vn, vt, 
	o, g, s, f,
	mtllib, usemtl, newmtl,
	Ns, Ni, d, Tr, Tf, illum,
	Ka, Kd, Ks, Ke,
	map_Ka, map_Kd, map_Ks, map_Ke, map_d, map_bump
};

struct ObjMesh
{
	std::string name{};
	std::string mat_name{};
	int smoothing_group{};

	std::vector<Vertex3>  vertices{};
	std::vector<uint32_t> indices{};
};

struct ObjMaterial
{
	std::string name{};

	// glm::vec3 ambient{};
	// glm::vec3 diffuse{};
	// glm::vec3 specular{};

	float Ns, Ni, d, Tr, Tf;
	int illum;
	glm::vec3 Ka{};
	glm::vec3 Kd{};
	glm::vec3 Ks{};
	glm::vec3 Ke{};

	// static constexpr int nMap = 6;
	std::string map_Ka, map_Kd, map_Ks, map_Ke, map_d, map_bump;
	// std::vector<uint32_t> sub_indices;
	// int offset;
	// int count;

	// ObjMaterial() { memset(this, 0, sizeof(ObjMaterial)); }
};

class ObjModel : public BaseModel
{
public:
	ObjModel(const std::string& filepath, Thirdparty api = Thirdparty::gl);

	std::pair<glm::vec3, glm::vec3> bbox();	// ����� bbox �ԣ���Ϊ����Ҫ�Ķ����������� & ���⸴�ӵļ��ش���

private:
	std::vector<ObjMesh> load_meshs(const std::string& firstline, std::ifstream& file);
	// Ҳ������ load_materials() ��ֱ�ӹ��� materials��������д�����
	std::unordered_map<std::string, ObjMaterial> 
		load_materials(const std::string& filepath);

public:
	std::vector<std::pair<ObjMesh, ObjMaterial>> meshes;
};

// std::vector<Mesh> create_meshes(const ObjModel& obj);

// load model by assimp --------------------------------------------------------

class AssimpModel : public BaseModel
{
public:
	AssimpModel(const std::string& filepath, Thirdparty api = Thirdparty::gl);

private:
	void process_node(aiNode* node, const aiScene* scene);
	AssimpMesh process_mesh(aiMesh* mesh, const aiScene* scene);

	std::vector<std::pair<Image, TexType>> 
	load_images(aiMaterial* mat, aiTextureType type, TexType textype);

public:
	std::vector<AssimpMesh> meshes;

private:
	std::unordered_map<std::string, Image> images_cache;	// �����Ѽ��ص�ͼ�񣬱����ظ�����
};

} // namespace fay

#endif // FAY_UTILITY_LOAD_MODEL_H