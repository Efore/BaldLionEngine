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

		BaldLion::Rendering::Mesh AnimatedModel::processMesh(const aiMesh *aimesh, const aiScene *aiscene)
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
			std::vector<Texture> textures;
			std::vector<VertexBoneData> verticesBoneData;
			std::vector<Animation::Joint> joints;

			//Create vertices array
			for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
			{
				// process vertex positions, normals and texture coordinates
				glm::vec3 position = glm::vec3(0.0f);
				if (aimesh->HasPositions())
				{
					position.x = aimesh->mVertices[i].x;
					position.y = aimesh->mVertices[i].y;
					position.z = aimesh->mVertices[i].z;
				}

				glm::vec3 normal = glm::vec3(0.0f);
				if (aimesh->HasNormals())
				{
					normal.x = aimesh->mNormals[i].x;
					normal.y = aimesh->mNormals[i].y;
					normal.z = aimesh->mNormals[i].z;
				}

				glm::vec3 color = glm::vec3(0.0f);
				if (aimesh->HasVertexColors(i))
				{
					color.x = aimesh->mColors[i]->r;
					color.y = aimesh->mColors[i]->g;
					color.z = aimesh->mColors[i]->b;
				}

				glm::vec2 texCoord = glm::vec2(0.0f);
				if (aimesh->HasTextureCoords(0)) // does the mesh contain texture coordinates?
				{
					texCoord.x = aimesh->mTextureCoords[0][i].x;
					texCoord.y = aimesh->mTextureCoords[0][i].y;
				}

				vertices.emplace_back(Vertex({ position, color, normal, texCoord }));
			}

			//Create vertex bones array
			for (unsigned int i = 0; i < aimesh->mNumBones; ++i)
			{
				VertexBoneData vertexBoneData;
				
				for (size_t j = 0; j < NUM_BONES_PER_VEREX; ++j)
				{
					vertexBoneData.vertexids[j] = aimesh->mBones[i]->mWeights[j].mVertexId;
					vertexBoneData.weights[j] = aimesh->mBones[i]->mWeights[j].mWeight;
				}

				verticesBoneData.push_back(vertexBoneData);
			}

			//Create Joints array
			int currentID = -1;
			GenerateJoints(joints, -1, currentID, aiscene->mRootNode);

			//Create indices array
			for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
			{
				aiFace face = aimesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			return AnimatedMesh(vertices, verticesBoneData, joints, indices,
				Material::Create("assets/shaders/BaseLit.glsl", 
					glm::vec3(1.0f),
					glm::vec3(1.0f), 
					glm::vec3(1.0f), 
					glm::vec3(1.0f), 
					32.0f,
					"assets/textures/Glock_17/albedo.png",
					"assets/textures/Glock_17/albedo.png",
					"assets/textures/Glock_17/albedo.png",
					"assets/textures/Glock_17/metallic.png",
					"assets/textures/Glock_17/normal.png"));
		}

		int AnimatedModel::GenerateJoints(std::vector<Animation::Joint>& joints, const int parentID, int& currentID, const aiNode* node)
		{
			std::vector<int> childrenIDs(node->mNumChildren);
			int jointID = ++currentID;
			glm::mat4 transformMatrix = glm::mat4(
				node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
				node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
				node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
				node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4
			);

			for (size_t i = 0; i < node->mNumChildren; ++i)
			{
				int childID = GenerateJoints(joints, jointID, currentID, node->mChildren[i]);
				childrenIDs.push_back(childID);
			}

			joints.emplace_back(Animation::Joint({ jointID, parentID, childrenIDs }));

			return jointID;
		}

	}
}