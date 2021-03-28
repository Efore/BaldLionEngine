#pragma once

#include "SkinnedMesh.h"
#include "Model.h"

namespace BaldLion
{
	namespace Rendering
	{
		class AnimatedModel : public Model {

		public:

			AnimatedModel(const std::string& filePath, const glm::mat4& initialWorldTransform);
			~AnimatedModel();

			virtual void Draw() const override;		

		private:			

			using Model::ProcessMesh;
			using Model::FillVertexArrayData;

			virtual void ProcessNode(const aiNode *node, const aiScene *scene) override;
			SkinnedMesh* ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene);

			void FillJointData(HashTable<StringId, ui32>& jointMapping,
				DynamicArray<Animation::Joint>& jointData,
				const HashTable<StringId, glm::mat4>& jointOffsetMapping,
				ui32& currentID, 
				const int32_t parentID, 
				const aiNode* node);

			void FillVertexArrayData(const aiMesh *aimesh, 
				DynamicArray<Vertex>& vertices,
				DynamicArray<ui32>& indices,
				HashTable<StringId, ui32>& jointMap,
				HashTable<StringId, glm::mat4>& jointOffsetMapping);

			void FillVertexWeightData(const aiMesh* aimesh, 
				const HashTable<StringId, ui32>& jointMapping,
				DynamicArray<VertexBoneData>& verticesBoneData);			
		};
	}
}
