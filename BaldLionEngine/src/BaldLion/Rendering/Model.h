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

			void SetUpModel();
			void Draw() const;

			const std::vector<Mesh>& GetSubMeshes() const { return m_subMeshes; }

		private:

			void ProcessNode(const aiNode *node, const aiScene *scene);
			Mesh ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene);

			void FillVertexArrayData(const aiMesh *aimesh, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
			void FillTextureData(const aiMesh *aimesh,
				const aiScene *aiscene,
				aiColor3D& ambientColor,
				aiColor3D& diffuseColor,
				aiColor3D& specularColor,
				aiColor3D& emissiveColor,
				Ref<Texture>& ambientTex,
				Ref<Texture>& diffuseTex,
				Ref<Texture>& specularTex,
				Ref<Texture>& emissiveTex,
				Ref<Texture>& normalTex);
			
		protected:
			std::string m_modelPath;
			std::string m_modelFolderPath;
			std::vector<Mesh> m_subMeshes;
		};
	}
}