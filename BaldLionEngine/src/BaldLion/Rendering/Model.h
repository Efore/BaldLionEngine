#pragma once
#include <vector>
#include "BaldLion/Core/Containers/DynamicArray.h"
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

			const DynamicArray<Mesh*>& GetSubMeshes() const { return m_subMeshes; }

		private:

			void ProcessNode(const aiNode *node, const aiScene *scene);
			Mesh* ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene);

			void FillVertexArrayData(const aiMesh *aimesh, DynamicArray<Vertex>& vertices, DynamicArray<uint32_t>& indices);
			void FillTextureData(const aiMesh *aimesh,
				const aiScene *aiscene,
				aiColor3D& ambientColor,
				aiColor3D& diffuseColor,
				aiColor3D& specularColor,
				aiColor3D& emissiveColor,
				Texture*& ambientTex,
				Texture*& diffuseTex,
				Texture*& specularTex,
				Texture*& emissiveTex,
				Texture*& normalTex);
			
		protected:
			std::string m_modelPath;
			std::string m_modelFolderPath;
			DynamicArray<Mesh*> m_subMeshes;
		};
	}
}