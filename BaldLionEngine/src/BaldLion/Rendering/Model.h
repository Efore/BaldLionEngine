#pragma once
#include <vector>
#include "Mesh.h"
#include <assimp/scene.h>

namespace BaldLion
{
	namespace Rendering
	{
		class Model {

		public:

			Model(const std::string& filePath);
			~Model();

			void SetUpModel(const std::string& filePath);
			void Draw() const;

			const std::vector<Mesh>& GetSubMeshes() const { return m_subMeshes; }

		protected:
			void processNode(const aiNode *node, const aiScene *scene);
			virtual Mesh processMesh(const aiMesh *aimesh, const aiScene *aiscene);

		protected:
			std::string m_modelPath;
			std::vector<Mesh> m_subMeshes;
		};
	}
}