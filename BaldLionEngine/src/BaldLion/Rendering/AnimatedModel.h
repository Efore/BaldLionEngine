#pragma once

#include "SkinnedMesh.h"
#include <vector>

namespace BaldLion
{
	namespace Rendering
	{
		class AnimatedModel{

		public:

			AnimatedModel(const std::string& filePath, const glm::mat4& initialWorldTransform);
			~AnimatedModel();

			void SetUpModel();
			void Draw() const;

			inline const DynamicArray<SkinnedMesh*>& GetSubMeshes() const { return m_subMeshes; }
			inline DynamicArray<SkinnedMesh*>& GetSubMeshes() { return m_subMeshes; }

		private:			

			void ProcessNode(const aiNode *node, const aiScene *scene);

			SkinnedMesh* ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene);

			void FillJointData(std::unordered_map<std::string, uint32_t>& jointMapping,
				DynamicArray<Animation::Joint>& jointData,
				const std::unordered_map<std::string, glm::mat4>& jointOffsetMapping,
				uint32_t& currentID, 
				const int32_t parentID, 
				const aiNode* node);

			void FillVertexArrayData(const aiMesh *aimesh, 
				DynamicArray<Vertex>& vertices,
				DynamicArray<uint32_t>& indices,
				std::unordered_map<std::string, uint32_t>& jointMap, 
				std::unordered_map<std::string, glm::mat4>& jointOffsetMapping);

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

			void FillVertexWeightData(const aiMesh* aimesh, 
				const std::unordered_map<std::string, uint32_t>& jointMapping,
				DynamicArray<VertexBoneData>& verticesBoneData);

		protected:
			std::string m_modelPath;
			std::string m_modelFolderPath;
			DynamicArray<SkinnedMesh*> m_subMeshes;
			glm::mat4 m_worldTransform;
		};
	}
}
