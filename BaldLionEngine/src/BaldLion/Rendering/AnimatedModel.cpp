#include "blpch.h"
#include "AnimatedModel.h"
#include "AnimatedMesh.h"

namespace BaldLion
{
	namespace Rendering
	{

		AnimatedModel::AnimatedModel(const std::string& filePath) : Model(filePath)
		{

		}

		AnimatedModel::~AnimatedModel()
		{

		}

		int AnimatedModel::GenerateJoints(std::vector<Animation::Joint>& joints, const int parentID, int& currentID, const aiNode* node)
		{
			int jointID = ++currentID;
			glm::mat4 transformMatrix = glm::mat4(
				node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
				node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
				node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
				node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4
			);

			std::vector<int> childrenIDs(node->mNumChildren);
			joints.emplace_back(Animation::Joint({ jointID, parentID, childrenIDs }));

			for (size_t i = 0; i < node->mNumChildren; ++i)
			{
				int childID = GenerateJoints(joints, jointID, currentID, node->mChildren[i]);
				childrenIDs[i] = childID;
			}

			joints[jointID].childrenJointIDs = std::vector<int>(childrenIDs);

			return jointID;
		}


		Mesh AnimatedModel::ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene)
		{
			std::vector<VertexBoneData> verticesBoneData;
			std::vector<Animation::Joint> joints;

			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			FillVertexArrayData(aimesh, vertices, indices);

			aiColor3D ambientColor;
			aiColor3D diffuseColor;
			aiColor3D specularColor;
			aiColor3D emissiveColor;

			std::string ambientTexPath = "";
			std::string diffuseTexPath = "";
			std::string specularTexPath = "";
			std::string emissiveTexPath = "";
			std::string normalTexPath = "";

			FillTextureData(aimesh, aiscene, ambientColor, diffuseColor, specularColor, emissiveColor, ambientTexPath, diffuseTexPath, specularTexPath, emissiveTexPath, normalTexPath);

			//Create vertex bones array
			for (size_t i = 0; i < aimesh->mNumBones; ++i)
			{
				VertexBoneData vertexBoneData;
				
				for (size_t j = 0; j < NUM_WEIGHTS_PER_VEREX; ++j)
				{
					vertexBoneData.vertexids[j] = aimesh->mBones[i]->mWeights[j].mVertexId;
					vertexBoneData.weights[j] = aimesh->mBones[i]->mWeights[j].mWeight;
				}

				verticesBoneData.push_back(vertexBoneData);
			}

			//Create Joints array
			int currentID = -1;
			GenerateJoints(joints, -1, currentID, aiscene->mRootNode);					

			return AnimatedMesh(vertices, verticesBoneData, joints, indices,
				Material::Create("assets/shaders/BaseLit.glsl",
					glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b),
					glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b),
					glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b),
					glm::vec3(specularColor.r, specularColor.g, specularColor.b),
					32.0f,
					ambientTexPath,
					diffuseTexPath,
					emissiveTexPath,
					specularTexPath,
					normalTexPath));
		}


	}
}