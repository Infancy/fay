#include "model.h"
#include "fay/resource/model.h"

namespace fay
{

model::model(const std::string& filepath, model_format model_format, bool gamma) : gamma_correction(gamma)
{
	assimp_model model(filepath, render_backend_type::opengl, model_format);
	for (auto& mesh : model.meshes)
	{
	#ifdef _DEBUG
		std::cout << '\n\n'
			<< mesh.vertices.size()
			<< ' ' << mesh.indices.size()
			<< ' ' << mesh.images.size();
			// << ' ' << mesh.images[0].first.file_path() << '\n';
		if (mesh.images.size() != 0)
			std::cout << ' ' << mesh.images[0].first.file_path();
	#endif // _DEBUG

		this->meshes.emplace_back(mesh.vertices, mesh.indices, mesh.images);
	}
}

/*
~modelMesh::modelMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}
*/

void model::draw(GLsizei sz)
{
	for (auto& mesh : meshes)
		mesh.draw(sz);
}

void model::draw(shader shader, GLsizei sz)
{
	for (auto& mesh : meshes)
		mesh.draw(shader, sz);
}

// -----------------------------------------------------------------------------

objmodel::objmodel(const std::string& filepath, bool gamma) : gamma_correction(gamma)
{
	obj_model model(filepath);
	for (auto& mesh : model.meshes)
	{ 
		auto& objmesh = mesh.first;
		auto& objmat = mesh.second;

	#ifdef _DEBUG
		std::cout
			<< objmesh.name
			<< ' ' << objmesh.mat_name
			<< ' ' << objmat.name
			<< ' ' << objmat.map_Kd << '\n';
	#endif // _DEBUG

		std::vector<std::pair<image_ptr, texture_format>> images;

		if (!objmat.map_Ka.empty()) images.emplace_back( 
			image_ptr(model.path + objmat.map_Ka, true), texture_format::ambient);

		if (!objmat.map_Kd.empty()) images.emplace_back(
			image_ptr(model.path + objmat.map_Kd, true), texture_format::diffuse);

		if (!objmat.map_Ks.empty()) images.emplace_back(
			image_ptr(model.path + objmat.map_Ks, true), texture_format::specular);

		if (!objmat.map_Ke.empty()) images.emplace_back(
			image_ptr(model.path + objmat.map_Ke, true), texture_format::emissive);

		if (!objmat.map_d.empty()) images.emplace_back(
			image_ptr(model.path + objmat.map_d, true), texture_format::alpha);

		if (!objmat.map_bump.empty()) images.emplace_back(
			image_ptr(model.path + objmat.map_bump, true), texture_format::displace);

		this->meshes.emplace_back(objmesh.vertices, objmesh.indices, images);
	}
}

void objmodel::draw(GLsizei sz)
{
	for (auto& mesh : meshes)
		mesh.draw(sz);
}

void objmodel::draw(shader shader, GLsizei sz)
{
	for (auto& mesh : meshes)
		mesh.draw(shader, sz);
}

}	//namespace fay
