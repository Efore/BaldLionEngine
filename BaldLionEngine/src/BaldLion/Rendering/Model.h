#pragma once
#include <vector>
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "Mesh.h"
#include "BaldLion/Animation/Joint.h"
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
			virtual void Draw() const;

			inline const DynamicArray<Mesh*>& GetSubMeshes() const { return m_subMeshes; }
			inline DynamicArray<Mesh*>& GetSubMeshes() { return m_subMeshes; }

		protected:

			virtual void ProcessNode(const aiNode *node, const aiScene *scene);
			static Mesh* ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene, const StringId modelFolderPath);

			static void FillVertexArrayData(const aiMesh *aimesh, DynamicArray<Vertex>& vertices, DynamicArray<ui32>& indices);

			static void FillTextureData(const aiMesh *aimesh,
				const aiScene *aiscene,
				const StringId modelFolderPath,
				aiColor3D& ambientColor,
				aiColor3D& diffuseColor,
				aiColor3D& specularColor,
				aiColor3D& emissiveColor,
				Texture*& ambientTex,
				Texture*& diffuseTex,
				Texture*& specularTex,
				Texture*& emissiveTex,
				Texture*& normalTex);

			static void GenerateJointMapping(const aiMesh *aimesh,
				HashTable<StringId, ui32>& jointMapping,
				HashTable<StringId, glm::mat4>& jointOffsetMapping);

			static void FillJointData(HashTable<StringId, ui32>& jointMapping,
				DynamicArray<Animation::Joint>& jointData,
				const HashTable<StringId, glm::mat4>& jointOffsetMapping,
				ui32& currentID,
				const int32_t parentID,
				const aiNode* node);

			static void FillVertexWeightData(const aiMesh* aimesh,
				const HashTable<StringId, ui32>& jointMapping,
				DynamicArray<VertexBone>& verticesBoneData);

		protected:
			StringId m_modelPath;
			StringId m_modelFolderPath;
			int m_importFlags;
			DynamicArray<Mesh*> m_subMeshes;			
		};
	}
}