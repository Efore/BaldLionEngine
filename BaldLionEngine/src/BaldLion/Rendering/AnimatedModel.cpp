#include "blpch.h"
#include "AnimatedModel.h"
#include "Renderer.h"
#include "BaldLion/Animation/AnimationManager.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

using namespace BaldLion::Animation;

namespace BaldLion
{
	namespace Rendering
	{

		AnimatedModel::AnimatedModel(const std::string& filePath, const glm::mat4& initialWorldTransform) : 
			Model(filePath, initialWorldTransform)
		{		
			m_importFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights;
			m_subMeshes = DynamicArray<SkinnedMesh*>(AllocationType::FreeList_Renderer, 1);
		}

		AnimatedModel::~AnimatedModel()
		{
			for (ui32 i = 0; i < m_subMeshes.Size(); ++i)
			{
				MemoryManager::DeleteNoDestructor(m_subMeshes[i]);
			}
			m_subMeshes.Clear();
		}

		void AnimatedModel::Draw() const
		{
			BL_PROFILE_FUNCTION();
			for (ui32 i = 0; i < m_subMeshes.Size(); ++i)
			{
				m_subMeshes[i]->Draw(m_worldTransform);
			}
		}
		
		void AnimatedModel::FillVertexArrayData(const aiMesh *aimesh, 
			DynamicArray<Vertex>& vertices,
			DynamicArray<ui32>& indices,
			HashTable<StringId, ui32>& jointMapping,
			HashTable<StringId, glm::mat4>& jointOffsetMapping)
		{
			for (ui32 i = 0; i < aimesh->mNumVertices; i++)
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
				if (aimesh->HasVertexColors((ui32)i))
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

				glm::vec3 tangent = glm::vec3(0.0f);
				glm::vec3 bitangent = glm::vec3(0.0f);
				if (aimesh->HasTangentsAndBitangents())
				{
					tangent.x = aimesh->mTangents[i].x;
					tangent.y = aimesh->mTangents[i].y;

					bitangent.x = aimesh->mBitangents[i].x;
					bitangent.y = aimesh->mBitangents[i].y;
				}

				vertices.PushBack(Vertex({ position, color, normal, texCoord, tangent, bitangent}));
			}
			
			for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
			{
				aiFace face = aimesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.PushBack(face.mIndices[j]);
			}	

			for (ui32 i = 0; i < aimesh->mNumBones; ++i)
			{				
				jointMapping.Emplace(STRING_TO_ID(aimesh->mBones[i]->mName.data), std::move(i));
				jointOffsetMapping.Emplace(STRING_TO_ID(aimesh->mBones[i]->mName.data), SkinnedMesh::AiMat4ToGlmMat4(aimesh->mBones[i]->mOffsetMatrix));
			}
		}	

		void AnimatedModel::FillJointData(HashTable<StringId, ui32>& jointMapping,
			DynamicArray<Animation::Joint>& jointsData,
			const HashTable<StringId, glm::mat4>& jointOffsetMapping,
			ui32& currentID,
			const int32_t parentID,
			const aiNode* node)
		{
			const StringId jointName = STRING_TO_ID(node->mName.data);

			if (jointMapping.Contains(jointName))
			{
				jointMapping.Get(jointName) = currentID;
				jointsData[currentID].jointID = currentID;
				jointsData[currentID].parentID = parentID;
				jointsData[currentID].jointBindTransform = jointOffsetMapping.Get(jointName);
				jointsData[currentID].jointAnimationTransform = jointsData[currentID].jointModelSpaceTransform = glm::mat4(1.0f);

				++currentID;
			}
						
			for (ui32 i = 0; i < node->mNumChildren; ++i)
			{
				FillJointData(jointMapping, jointsData, jointOffsetMapping, currentID, jointMapping.Contains(jointName) ? jointMapping.Get(jointName) : parentID, node->mChildren[i]);
			}			
		}

		void AnimatedModel::FillVertexWeightData(const aiMesh* aimesh,
			const HashTable<StringId, ui32>& jointMapping,
			DynamicArray<VertexBoneData>& vertices)
		{
			ui32* jointsAssigned = new ui32[aimesh->mNumVertices]{ 0 };

			//Fill jointIDs and weights
			for (ui32 i = 0; i < aimesh->mNumBones; ++i)
			{
				for (ui32 j = 0; j < aimesh->mBones[i]->mNumWeights; ++j)
				{
					ui32 vertexID = aimesh->mBones[i]->mWeights[j].mVertexId;

					const StringId jointName = STRING_TO_ID(aimesh->mBones[i]->mName.data);

					switch (jointsAssigned[vertexID])
					{
					case 0:
						vertices[vertexID].jointIDs.x = jointMapping.Get(jointName);
						vertices[vertexID].weights.x = aimesh->mBones[i]->mWeights[j].mWeight;
						break;
					case 1:
						vertices[vertexID].jointIDs.y = jointMapping.Get(jointName);
						vertices[vertexID].weights.y = aimesh->mBones[i]->mWeights[j].mWeight;
						break;
					case 2:
						vertices[vertexID].jointIDs.z = jointMapping.Get(jointName);
						vertices[vertexID].weights.z = aimesh->mBones[i]->mWeights[j].mWeight;
						break;
					default:
						break;
					}

					jointsAssigned[vertexID]++;
				}		

			}

			delete jointsAssigned;
		}		

		void AnimatedModel::ProcessNode(const aiNode *node, const aiScene *scene)
		{
			// process all the node's meshes (if any)
			for (ui32 i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
				m_subMeshes.PushBack(ProcessMesh(mesh, scene));
			}
			// then do the same for each of its children
			for (ui32 i = 0; i < node->mNumChildren; i++)
			{
				ProcessNode(node->mChildren[i], scene);
			}
		}

		SkinnedMesh* AnimatedModel::ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene)
		{		
			DynamicArray<Vertex> vertices(AllocationType::Linear_Frame, aimesh->mNumVertices);
			DynamicArray<VertexBoneData> verticesBoneData(AllocationType::Linear_Frame, aimesh->mNumVertices);
			DynamicArray<ui32> indices (AllocationType::Linear_Frame, aimesh->mNumVertices * 3);
			DynamicArray<Animation::Joint> jointsData(AllocationType::FreeList_Renderer, aimesh->mNumBones);

			HashTable<StringId, ui32> jointMapping (AllocationType::Linear_Frame, aimesh->mNumBones * 2);
			HashTable<StringId, glm::mat4> jointOffsetMapping (AllocationType::Linear_Frame, aimesh->mNumBones * 2);

			verticesBoneData.Fill();
			jointsData.Fill();

			aiColor3D ambientColor;
			aiColor3D diffuseColor;
			aiColor3D specularColor;
			aiColor3D emissiveColor;

			Texture* ambientTex = nullptr;
			Texture* diffuseTex = nullptr;
			Texture* specularTex = nullptr;
			Texture* emissiveTex = nullptr;
			Texture* normalTex = nullptr;

			FillVertexArrayData(aimesh, vertices, indices, jointMapping, jointOffsetMapping);

			FillTextureData(aimesh, aiscene, ambientColor, diffuseColor, specularColor, emissiveColor, ambientTex, diffuseTex, specularTex, emissiveTex, normalTex);			

			ui32 firstID = 0;
			FillJointData(jointMapping, jointsData, jointOffsetMapping, firstID, -1, aiscene->mRootNode);

			FillVertexWeightData(aimesh, jointMapping, verticesBoneData);

			Material* meshMaterial = Material::Create(
				glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b),
				glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b),
				glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b),
				glm::vec3(specularColor.r, specularColor.g, specularColor.b),
				32.0f,
				ambientTex,
				diffuseTex,
				specularTex,
				emissiveTex,
				normalTex);

			meshMaterial->AssignShader("assets/shaders/monster.glsl");

			SkinnedMesh* animatedMesh = MemoryManager::New<SkinnedMesh>(STRING_TO_ID("Skinned Mesh"), AllocationType::FreeList_Renderer, jointsData, meshMaterial);

			animatedMesh->SetUpMesh(vertices, verticesBoneData, indices);

			AnimationManager::GenerateAnimator(aiscene, jointMapping, animatedMesh);

			return animatedMesh;
		}
	}
}