#pragma once

#include "AnimatedMesh.h"
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

			inline const std::vector<Ref<AnimatedMesh>>& GetSubMeshes() const { return m_subMeshes; }
			inline std::vector<Ref<AnimatedMesh>>& GetSubMeshes() { return m_subMeshes; }

		private:			

			void ProcessNode(const aiNode *node, const aiScene *scene);

			Ref<AnimatedMesh> ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene);

			void FillJointData(std::unordered_map<std::string, uint32_t>& jointMapping,
				std::vector<Animation::Joint>& jointData, 
				const std::unordered_map<std::string, glm::mat4>& jointOffsetMapping,
				uint32_t& currentID, 
				const int32_t parentID, 
				const aiNode* node);

			void FillVertexArrayData(const aiMesh *aimesh, 
				std::vector<Vertex>& vertices, 
				std::vector<uint32_t>& indices,  
				std::unordered_map<std::string, uint32_t>& jointMap, 
				std::unordered_map<std::string, glm::mat4>& jointOffsetMapping);

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

			void FillVertexWeightData(const aiMesh* aimesh, 
				const std::unordered_map<std::string, uint32_t>& jointMapping,
				std::vector<VertexBoneData>& verticesBoneData);

		protected:
			std::string m_modelPath;
			std::string m_modelFolderPath;
			std::vector<Ref<AnimatedMesh>> m_subMeshes;
			glm::mat4 m_worldTransform;
		};
	}
}
