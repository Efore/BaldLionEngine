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

			Model(const std::string& filePath, const glm::mat4& initialWorldTransform);
			~Model();

			void SetUpModel();
			virtual void Draw() const;

			inline const DynamicArray<Mesh*>& GetSubMeshes() const { return m_subMeshes; }
			inline DynamicArray<Mesh*>& GetSubMeshes() { return m_subMeshes; }
			inline const glm::mat4& GetWorldTransform() const { return m_worldTransform; }

		protected:

			virtual void ProcessNode(const aiNode *node, const aiScene *scene);
			Mesh* ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene);

			void FillVertexArrayData(const aiMesh *aimesh, DynamicArray<Vertex>& vertices, DynamicArray<ui32>& indices);
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
			StringId m_modelPath;
			StringId m_modelFolderPath;
			glm::mat4 m_worldTransform;
			int m_importFlags;
			DynamicArray<Mesh*> m_subMeshes;
		};
	}
}