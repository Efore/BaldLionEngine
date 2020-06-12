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

		AnimatedModel::AnimatedModel(const std::string& filePath)
		{
			BL_PROFILE_FUNCTION();

			// Extracting folder path from filePath

			m_modelPath = filePath;
			auto lastSlash = filePath.find_last_of("/\\");
			m_modelFolderPath = filePath.substr(0, lastSlash + 1);
		}

		AnimatedModel::~AnimatedModel()
		{

		}

		void AnimatedModel::SetUpModel()
		{
			BL_PROFILE_FUNCTION();

			Assimp::Importer import;
			import.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, BL_JOINT_WEIGHTS_PER_VERTEX);

			const aiScene *scene = import.ReadFile(m_modelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
				return;
			}

			ProcessNode(scene->mRootNode, scene);
		}

		void AnimatedModel::Draw() const
		{
			BL_PROFILE_FUNCTION();
			for (auto const &m : m_subMeshes)
			{
				m->Draw();
			}
		}

		void AnimatedModel::ProcessNode(const aiNode *node, const aiScene *scene)
		{
			// process all the node's meshes (if any)
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
				m_subMeshes.emplace_back(ProcessMesh(mesh, scene));
			}
			// then do the same for each of its children
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				ProcessNode(node->mChildren[i], scene);
			}
		}

		
		void AnimatedModel::FillVertexArrayData(const aiMesh *aimesh, 
			std::vector<AnimatedVertex>& vertices, 
			std::vector<uint32_t>& indices, 
			std::map<std::string, uint32_t>& jointMapping, 
			std::map<std::string, glm::mat4>& jointOffsetMapping)
		{
			for (size_t i = 0; i < aimesh->mNumVertices; i++)
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
				if (aimesh->HasVertexColors((uint32_t)i))
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

				vertices[i] = AnimatedVertex({ position, color, normal, texCoord, tangent, bitangent, glm::ivec3(0), glm::vec3(0.0f) });
			}
			
			for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
			{
				aiFace face = aimesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}	

			for (uint32_t i = 0; i < aimesh->mNumBones; ++i)
			{				
				jointMapping.emplace(aimesh->mBones[i]->mName.data, i);
				jointOffsetMapping.emplace(aimesh->mBones[i]->mName.data, AnimatedMesh::AiMat4ToGlmMat4(aimesh->mBones[i]->mOffsetMatrix));					
			}
		}

		void AnimatedModel::FillTextureData(const aiMesh *aimesh,
			const aiScene *aiscene,
			aiColor3D& ambientColor,
			aiColor3D& diffuseColor,
			aiColor3D& specularColor,
			aiColor3D& emissiveColor,
			Ref<Texture>& ambientTex,
			Ref<Texture>& diffuseTex,
			Ref<Texture>& specularTex,
			Ref<Texture>& emissiveTex,
			Ref<Texture>& normalTex)
		{
			const aiMaterial* aimaterial = aiscene->mMaterials[aimesh->mMaterialIndex];

			aimaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
			aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
			aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
			aimaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);

			aiString relativeTexPath;
			std::string completeTexPath;

			if (aimaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
			{
				aimaterial->GetTexture(aiTextureType_AMBIENT, 0, &relativeTexPath);
				completeTexPath = m_modelFolderPath;
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					ambientTex = Renderer::GetTextureLibrary().Load(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size, BL_TEXTURE_TYPE_2D);
				}
				else
				{
					ambientTex = Renderer::GetTextureLibrary().Load(completeTexPath, BL_TEXTURE_TYPE_2D);
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &relativeTexPath);
				completeTexPath = m_modelFolderPath;
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					diffuseTex = Renderer::GetTextureLibrary().Load(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size, BL_TEXTURE_TYPE_2D);
				}
				else
				{
					diffuseTex = Renderer::GetTextureLibrary().Load(completeTexPath, BL_TEXTURE_TYPE_2D);
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
			{
				aimaterial->GetTexture(aiTextureType_SPECULAR, 0, &relativeTexPath);
				completeTexPath = m_modelFolderPath;
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					specularTex = Renderer::GetTextureLibrary().Load(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size, BL_TEXTURE_TYPE_2D);
				}
				else
				{
					specularTex = Renderer::GetTextureLibrary().Load(completeTexPath, BL_TEXTURE_TYPE_2D);
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
			{
				aimaterial->GetTexture(aiTextureType_EMISSIVE, 0, &relativeTexPath);
				completeTexPath = m_modelFolderPath;
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					emissiveTex = Renderer::GetTextureLibrary().Load(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size, BL_TEXTURE_TYPE_2D);
				}
				else
				{
					emissiveTex = Renderer::GetTextureLibrary().Load(completeTexPath, BL_TEXTURE_TYPE_2D);
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
			{
				aimaterial->GetTexture(aiTextureType_NORMALS, 0, &relativeTexPath);
				completeTexPath = m_modelFolderPath;
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					normalTex = Renderer::GetTextureLibrary().Load(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size, BL_TEXTURE_TYPE_2D);
				}
				else
				{
					normalTex = Renderer::GetTextureLibrary().Load(completeTexPath, BL_TEXTURE_TYPE_2D);
				}
			}
		}

		void AnimatedModel::FillJointData(std::map<std::string, uint32_t>& jointMapping,
			std::vector<Animation::Joint>& jointsData,
			const std::map<std::string, glm::mat4>& jointOffsetMapping,
			uint32_t& currentID,
			const int32_t parentID,
			const aiNode* node)
		{
			auto it = jointMapping.find(node->mName.data);

			if (it != jointMapping.end())
			{
				jointMapping[node->mName.data] = currentID;

				jointsData[currentID].jointID = currentID;
				jointsData[currentID].parentID = parentID;
				jointsData[currentID].jointOffsetMatrix = jointOffsetMapping.at(node->mName.data);
				jointsData[currentID].jointAnimationTransform = jointsData[currentID].jointGlobalTransform = glm::mat4(1.0f);

				++currentID;
			}
						
			for (size_t i = 0; i < node->mNumChildren; ++i)
			{
				FillJointData(jointMapping, jointsData, jointOffsetMapping, currentID, it != jointMapping.end() ? it->second : parentID, node->mChildren[i]);
			}			
		}

		void AnimatedModel::FillVertexWeightData(const aiMesh* aimesh,const std::map<std::string, uint32_t>& jointMapping,  std::vector<AnimatedVertex>& vertices)
		{
			uint32_t* jointsAssigned = new uint32_t[aimesh->mNumVertices]{ 0 };

			//Fill jointIDs and weights
			for (uint32_t i = 0; i < aimesh->mNumBones; ++i)
			{
				for (uint32_t j = 0; j < aimesh->mBones[i]->mNumWeights; ++j)
				{
					uint32_t vertexID = aimesh->mBones[i]->mWeights[j].mVertexId;

					switch (jointsAssigned[vertexID])
					{
					case 0:
						vertices[vertexID].jointIDs.x = jointMapping.at(aimesh->mBones[i]->mName.data);
						vertices[vertexID].weights.x = aimesh->mBones[i]->mWeights[j].mWeight;
						break;
					case 1:
						vertices[vertexID].jointIDs.y = jointMapping.at(aimesh->mBones[i]->mName.data);
						vertices[vertexID].weights.y = aimesh->mBones[i]->mWeights[j].mWeight;
						break;
					case 2:
						vertices[vertexID].jointIDs.z = jointMapping.at(aimesh->mBones[i]->mName.data);
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

		Ref<AnimatedMesh> AnimatedModel::ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene)
		{		
			std::vector<AnimatedVertex> vertices(aimesh->mNumVertices);
			std::vector<uint32_t> indices;
			std::map<std::string, uint32_t> jointMapping;
			std::map<std::string, glm::mat4> jointOffsetMapping;
			std::vector<Animation::Joint> jointsData(aimesh->mNumBones);

			aiColor3D ambientColor;
			aiColor3D diffuseColor;
			aiColor3D specularColor;
			aiColor3D emissiveColor;

			Ref<Texture> ambientTex = nullptr;
			Ref<Texture> diffuseTex = nullptr;
			Ref<Texture> specularTex = nullptr;
			Ref<Texture> emissiveTex = nullptr;
			Ref<Texture> normalTex = nullptr;

			FillVertexArrayData(aimesh, vertices, indices, jointMapping, jointOffsetMapping);

			FillTextureData(aimesh, aiscene, ambientColor, diffuseColor, specularColor, emissiveColor, ambientTex, diffuseTex, specularTex, emissiveTex, normalTex);			

			uint32_t firstID = 0;
			FillJointData(jointMapping, jointsData, jointOffsetMapping, firstID, -1, aiscene->mRootNode);

			FillVertexWeightData(aimesh, jointMapping, vertices);

			Ref<AnimatedMesh> animatedMesh = CreateRef<AnimatedMesh>(vertices, indices, jointsData,
				Material::Create("assets/shaders/monster.glsl",
					glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b),
					glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b),
					glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b),
					glm::vec3(specularColor.r, specularColor.g, specularColor.b),
					32.0f,
					ambientTex,
					diffuseTex,
					specularTex,
					emissiveTex,
					normalTex));

			AnimationManager::GetInstance()->GenerateAnimator(aiscene, jointMapping, animatedMesh);

			return animatedMesh;
		}
	}
}