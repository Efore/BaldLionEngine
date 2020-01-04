#pragma once
#include <vector>
#include "Mesh.h"
#include <assimp/scene.h>

namespace BaldLion
{
	class Model {

	public:

		Model(const std::string& filePath);
		~Model();

		void SetUpModel(const std::string& filePath);
		void Draw() const;

		const std::vector<Mesh>& GetSubMeshes() const { return m_subMeshes; }

	private:
		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	private:
		std::vector<Mesh> m_subMeshes;
	};
}