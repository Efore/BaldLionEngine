#include "blpch.h"
#include "Model.h"
#include "Renderer.h"
#include "BaldLion/Animation/AnimationManager.h"
#include "BaldLion/Utils/GeometryUtils.h"
#include "BaldLion/Utils/MathUtils.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

using namespace BaldLion::ResourceManagement;

namespace BaldLion
{
	namespace Rendering
	{
		Model::Model(const std::string& modelPath) : 
			ResourceManagement::Resource(BL_STRING_TO_STRINGID(modelPath), modelPath, ResourceManagement::ResourceType::Model)
		{
			BL_PROFILE_FUNCTION(); 

			// Extracting folder path from filePath

			auto lastSlash = modelPath.find_last_of("/\\");		
			m_modelFolderPath = BL_STRING_TO_STRINGID(modelPath.substr(0, lastSlash + 1));
			m_subMeshes = DynamicArray<Mesh*>(AllocationType::FreeList_Renderer, 5);
			m_importFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes;
			
		}

		Model::~Model()
		{
			
		}

		void Model::SetUpModel()
		{
			BL_PROFILE_FUNCTION();

			Assimp::Importer import;			

			const aiScene *scene = import.ReadFile(BL_STRINGID_TO_STR_C(m_resourcePath), m_importFlags);

			if (!scene || !scene->mRootNode)
			{
				std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
				return;
			}

			ProcessNode(scene->mRootNode, scene);
		}

		void Model::Draw() const
		{
			BL_PROFILE_FUNCTION();
			BL_DYNAMICARRAY_FOR(i, m_subMeshes, 0)			
			{
				m_subMeshes[i]->Draw();
			}
		}

		void Model::ProcessNode(const aiNode *node, const aiScene *scene)
		{
			// process all the node's meshes (if any)
			for (ui32 i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
				
				m_subMeshes.PushBack(Model::ProcessMesh(mesh, scene, m_resourcePath));
			}
			// then do the same for each of its children
			for (ui32 i = 0; i < node->mNumChildren; i++)
			{
				ProcessNode(node->mChildren[i], scene);
			}
		}

		void Model::FillVertexArrayData(const aiMesh *aimesh, DynamicArray<Vertex>& vertices, DynamicArray<ui32>& indices)
		{
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

				glm::vec2 texCoord = glm::vec2(0.0f);
				if (aimesh->HasTextureCoords(0)) // does the mesh contain texture coordinates?
				{
					texCoord.x = aimesh->mTextureCoords[0][i].x;
					texCoord.y = aimesh->mTextureCoords[0][i].y;
				}

				glm::vec3 tangent = glm::vec3(0.0f);
				const glm::vec3 c1 = glm::cross(normal, glm::vec3(0.0, 0.0, 1.0));
				const glm::vec3 c2 = glm::cross(normal, glm::vec3(0.0, 1.0, 0.0));

				if (glm::length2(c1) > glm::length2(c2))
				{
					tangent = c1;
				}
				else
				{
					tangent = c2;
				}				

				vertices.EmplaceBack(position, normal, texCoord, tangent);
			}

			for (ui32 i = 0; i < aimesh->mNumFaces; i++)
			{
				aiFace face = aimesh->mFaces[i];
				for (size_t j = 0; j < face.mNumIndices; j++)
					indices.PushBack(face.mIndices[j]);
			}
		}

		void Model::FillTextureData(const aiMesh *aimesh, 
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
			Texture*& normalTex)
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
				completeTexPath = StringUtils::GetPathWithoutFile(BL_STRINGID_TO_STR_C(modelFolderPath));
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					ambientTex = ResourceManager::LoadResource<Texture>(completeTexPath);

					if (!ambientTex) 
					{
						ambientTex = Texture2D::Create(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size);
						ResourceManager::AddResource(ambientTex);
					}
				}
				else
				{
					ambientTex = ResourceManager::AddResource<Texture>(completeTexPath, ResourceType::Texture);
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &relativeTexPath);
				completeTexPath = StringUtils::GetPathWithoutFile(BL_STRINGID_TO_STR_C(modelFolderPath));
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					diffuseTex = ResourceManager::LoadResource<Texture>(completeTexPath);

					if (!diffuseTex)
					{
						diffuseTex = Texture2D::Create(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size);
						ResourceManager::AddResource(diffuseTex);
					}
				}
				else
				{
					diffuseTex = ResourceManager::AddResource<Texture>(completeTexPath, ResourceType::Texture);
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
			{
				aimaterial->GetTexture(aiTextureType_SPECULAR, 0, &relativeTexPath);
				completeTexPath = StringUtils::GetPathWithoutFile(BL_STRINGID_TO_STR_C(modelFolderPath));
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					specularTex = ResourceManager::LoadResource<Texture>(completeTexPath);

					if (!specularTex)
					{
						specularTex = Texture2D::Create(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size);
						ResourceManager::AddResource(specularTex);
					}
				}
				else
				{
					specularTex = ResourceManager::AddResource<Texture>(completeTexPath, ResourceType::Texture);
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
			{
				aimaterial->GetTexture(aiTextureType_EMISSIVE, 0, &relativeTexPath);
				
				completeTexPath = StringUtils::GetPathWithoutFile(BL_STRINGID_TO_STR_C(modelFolderPath));
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					emissiveTex = ResourceManager::LoadResource<Texture>(completeTexPath);

					if (!emissiveTex)
					{
						emissiveTex = Texture2D::Create(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size);
						ResourceManager::AddResource(emissiveTex);
					}
				}
				else
				{
					emissiveTex = ResourceManager::AddResource<Texture>(completeTexPath, ResourceType::Texture);
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
			{
				aimaterial->GetTexture(aiTextureType_NORMALS, 0, &relativeTexPath);
				completeTexPath = StringUtils::GetPathWithoutFile(BL_STRINGID_TO_STR_C(modelFolderPath));
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					normalTex = ResourceManager::LoadResource<Texture>(completeTexPath);

					if (!normalTex)
					{
						normalTex = Texture2D::Create(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size);
						ResourceManager::AddResource(normalTex);
					}
				}
				else
				{
					normalTex = ResourceManager::AddResource<Texture>(completeTexPath, ResourceType::Texture);
				}
			}
		}

		void Model::GenerateJointMapping(const aiMesh *aimesh, HashTable<StringId, JointType>& jointMapping, HashMap<StringId, glm::mat4>& jointOffsetMapping)
		{
			for (ui32 i = 0; i < aimesh->mNumBones; ++i)
			{
				JointType jointType = ParseNodeName(aimesh->mBones[i]->mName.data);

				if (jointType != JointType::Count)
				{					
					jointMapping.Emplace(BL_STRING_TO_STRINGID(aimesh->mBones[i]->mName.data), std::move(jointType));
					jointOffsetMapping.Emplace(BL_STRING_TO_STRINGID(aimesh->mBones[i]->mName.data), MathUtils::AiMat4ToGlmMat4(aimesh->mBones[i]->mOffsetMatrix));
				}
			}
		}

		void Model::FillJointData(HashTable<StringId, JointType>& jointMapping,
			Animation::AnimationJoint* jointsData,
			const HashMap<StringId, glm::mat4>& jointOffsetMapping,
			const JointType parentJointType,
			const aiNode* node)
		{
			const StringId jointName = BL_STRING_TO_STRINGID(node->mName.data);

			JointType jointType = parentJointType;

			if (jointMapping.TryGet(jointName, jointType))
			{					
				jointsData[(ui32)jointType].parentJointType = parentJointType;
				jointsData[(ui32)jointType].jointOffsetTransform = jointOffsetMapping.Get(jointName);
				jointsData[(ui32)jointType].jointModelSpaceTransform = jointsData[(ui32)jointType].jointLocalSpaceTransform = glm::mat4(1.0f);
			}

			for (ui32 i = 0; i < node->mNumChildren; ++i)
			{	
				FillJointData(jointMapping, jointsData, jointOffsetMapping, jointType, node->mChildren[i]);
			}
		}

		void Model::FillVertexWeightData(const aiMesh* aimesh,
			const HashTable<StringId, JointType>& jointMapping,
			DynamicArray<VertexBone>& vertices)
		{
			ui32* jointsAssigned = new ui32[aimesh->mNumVertices]{ 0 };

			//Fill jointIDs and weights
			for (ui32 i = 0; i < aimesh->mNumBones; ++i)
			{				
				for (ui32 j = 0; j < aimesh->mBones[i]->mNumWeights; ++j)
				{
					ui32 vertexID = aimesh->mBones[i]->mWeights[j].mVertexId;

					const StringId jointName = BL_STRING_TO_STRINGID(aimesh->mBones[i]->mName.data);
					
					JointType jointType = JointType::Count;

					if (jointMapping.TryGet(jointName, jointType))
					{
						switch (jointsAssigned[vertexID])
						{
						case 0:
							vertices[vertexID].jointIDs.x = (ui32)jointType;
							vertices[vertexID].weights.x = aimesh->mBones[i]->mWeights[j].mWeight;
							break;
						case 1:
							vertices[vertexID].jointIDs.y = (ui32)jointType;
							vertices[vertexID].weights.y = aimesh->mBones[i]->mWeights[j].mWeight;
							break;
						case 2:
							vertices[vertexID].jointIDs.z = (ui32)jointType;
							vertices[vertexID].weights.z = aimesh->mBones[i]->mWeights[j].mWeight;
							break;
						default:
							break;
						}

						jointsAssigned[vertexID]++;
					}
				}

			}

			delete[] jointsAssigned;
		}
	

		Mesh* Model::ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene, const StringId modelPath)
		{
			DynamicArray<Vertex> vertices(AllocationType::FreeList_Renderer, aimesh->mNumVertices);
			DynamicArray<ui32> indices(AllocationType::FreeList_Renderer, aimesh->mNumVertices * 3);

			FillVertexArrayData(aimesh, vertices, indices);

			aiColor3D ambientColor;
			aiColor3D diffuseColor;
			aiColor3D specularColor;
			aiColor3D emissiveColor;
			
			Texture* ambientTex = nullptr;
			Texture* diffuseTex = nullptr;
			Texture* specularTex = nullptr;
			Texture* emissiveTex = nullptr;
			Texture* normalTex = nullptr;

			FillTextureData(aimesh, aiscene, modelPath, ambientColor, diffuseColor, specularColor, emissiveColor, ambientTex, diffuseTex, specularTex, emissiveTex, normalTex);

			Material::MaterialProperties materialProperties
			{
				aimesh->HasBones() ? BL_STRING_TO_STRINGID("assets/editorAssets/shaders/SkinnedLit.glsl") : BL_STRING_TO_STRINGID("assets/editorAssets/shaders/BaseLit.glsl"),
				glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b),
				glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b),
				glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b),
				glm::vec3(specularColor.r, specularColor.g, specularColor.b),
				32.0f,
				ambientTex,
				diffuseTex,
				specularTex,
				emissiveTex,
				normalTex,
				Material::BlendMode::None,
				Material::DepthBufferMode::TestAndWrite,
				Material::CullingMode::Back,
				(ui8)Material::ShadowsSettingsBitMask::CastShadows
			};
						
			const std::string matPath = StringUtils::GetPathWithoutExtension(BL_STRINGID_TO_STRING(modelPath)) + (aiscene->mMaterials[aimesh->mMaterialIndex]->GetName().data) + ResourceManager::GetResourceSuffixFromType(ResourceType::Material);

			Material* meshMaterial = ResourceManagement::ResourceManager::LoadResource<Material>(matPath);

			if (!meshMaterial)
			{
				meshMaterial = Material::Create(matPath, materialProperties);
				ResourceManagement::ResourceManager::AddResource(meshMaterial);
			}

			/*Material* meshMaterial = Material::Create(matName, materialProperties);*/
			
			meshMaterial->AssignShader();

			const char* aiMeshName = strlen(aimesh->mName.C_Str()) == 0 ? "unnamedMesh" : aimesh->mName.C_Str();
			const std::string meshPath = StringUtils::GetPathWithoutExtension(BL_STRINGID_TO_STRING(modelPath)) + (aiMeshName) + ResourceManager::GetResourceSuffixFromType(ResourceType::Mesh);

			Mesh* mesh = ResourceManagement::ResourceManager::LoadResource<Mesh>(meshPath);
				
			if (!mesh) 
			{
				mesh = MemoryManager::New<Mesh>("Mesh", AllocationType::FreeList_Renderer, meshMaterial, BL_STRINGID_TO_STRING(modelPath), meshPath);
				ResourceManagement::ResourceManager::AddResource(mesh);
			}

			if (aimesh->HasBones())
			{
				DynamicArray<VertexBone> verticesBoneData(AllocationType::Linear_Frame, aimesh->mNumVertices);
				verticesBoneData.Populate();				

				Animation::AnimationJoint jointsData[(ui32)JointType::Count];
				for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
				{
					jointsData[i].jointModelSpaceTransform = jointsData[i].jointOffsetTransform = jointsData[i].jointLocalSpaceTransform = glm::mat4(1.0f);
					jointsData[i].parentJointType = JointType::Count;
				}

				HashTable<StringId, JointType> jointMapping(AllocationType::Linear_Frame, aimesh->mNumBones * 2);
				HashMap<StringId, glm::mat4> jointOffsetMapping(AllocationType::Linear_Frame, aimesh->mNumBones * 2);				

				GenerateJointMapping(aimesh, jointMapping, jointOffsetMapping);

				FillJointData(jointMapping, jointsData, jointOffsetMapping, JointType::Count, aiscene->mRootNode);

				FillVertexWeightData(aimesh, jointMapping, verticesBoneData);

				const std::string skeletonName = StringUtils::GetPathWithoutExtension(BL_STRINGID_TO_STRING(modelPath)) + (aimesh->mName.C_Str()) + ResourceManager::GetResourceSuffixFromType(ResourceType::Skeleton);

				Animation::Skeleton* skeleton = ResourceManagement::ResourceManager::LoadResource<Animation::Skeleton>(skeletonName);
					
				if (!skeleton) 
				{
					skeleton = MemoryManager::New<Animation::Skeleton>("Skeleton", AllocationType::FreeList_Renderer, jointsData, skeletonName);
					ResourceManagement::ResourceManager::AddResource(skeleton);
				}

				mesh->SetSkeleton(skeleton);
				mesh->SetVertexBones(verticesBoneData);

				std::string animatorPath = StringUtils::GetPathWithoutExtension(BL_STRINGID_TO_STRING(modelPath)) + aiMeshName;
				Animation::AnimationManager::GenerateAnimator(aiscene, animatorPath, jointMapping);
			}

			mesh->SetUpMesh(vertices, indices);

			return mesh;
		}

		JointType Model::ParseNodeName(const char *nodeName)
		{
			static const char* jointNames[] = {
			"Hips",
			"Spine",
			"Spine1",
			"Spine2",
			"Neck",
			"Head",
			"HeadTop_End",
			"LeftShoulder",
			"LeftArm",
			"LeftForearm",
			"LeftHand",
			"LeftHandThumb1",
			"LeftHandThumb2",
			"LeftHandThumb3",
			"LeftHandThumb4",
			"LeftHandIndex1",
			"LeftHandIndex2",
			"LeftHandIndex3",
			"LeftHandIndex4",
			"LeftHandMiddle1",
			"LeftHandMiddle2",
			"LeftHandMiddle3",
			"LeftHandMiddle4",
			"LeftHandRing1",
			"LeftHandRing2",
			"LeftHandRing3",
			"LeftHandRing4",
			"LeftHandPinky1",
			"LeftHandPinky2",
			"LeftHandPinky3",
			"LeftHandPinky4",
			"RightShoulder",
			"RightArm",
			"RightForearm",
			"RightHand",
			"RightHandThumb1",
			"RightHandThumb2",
			"RightHandThumb3",
			"RightHandThumb4",
			"RightHandIndex1",
			"RightHandIndex2",
			"RightHandIndex3",
			"RightHandIndex4",
			"RightHandMiddle1",
			"RightHandMiddle2",
			"RightHandMiddle3",
			"RightHandMiddle4",
			"RightHandRing1",
			"RightHandRing2",
			"RightHandRing3",
			"RightHandRing4",
			"RightHandPinky1",
			"RightHandPinky2",
			"RightHandPinky3",
			"RightHandPinky4",
			"LeftUpLeg",
			"LeftLeg",
			"LeftFoot",
			"LeftToeBase",
			"LeftToe_End",
			"RightUpLeg",
			"RightLeg",
			"RightFoot",
			"RightToeBase",
			"RightToe_End"
			};

			for (i32 i = ((i32)JointType::Count) - 1; i >= 0; --i)
			{
				std::string lowerNodeName = nodeName;				
				std::transform(lowerNodeName.begin(), lowerNodeName.end(), lowerNodeName.begin(), ::tolower);
				size_t pos = lowerNodeName.rfind(':');
				lowerNodeName = lowerNodeName.substr(pos + 1);

				std::string lowerJointName = jointNames[i]; 
				std::transform(lowerJointName.begin(), lowerJointName.end(), lowerJointName.begin(), ::tolower);

				if (lowerNodeName == lowerJointName)
				{
					return (JointType)i;
				}
			}

			return JointType::Count;
		}

		void Model::GenerateEntities(ECS::ECSManager* ecsManager, bool isStatic) const
		{
			ECS::ECSEntityID rootEntityID = ecsManager->AddEntity(BL_STRINGID_TO_STR_C(m_resourcePath));
			ECS::ECSEntity* entity = ecsManager->GetEntityMap().Get(rootEntityID);
			entity->SetIsStatic(isStatic);

			ECS::ECSTransformComponent* rootTransformComponent = ecsManager->CreateComponent<ECS::ECSTransformComponent>(
				ECS::ECSComponentType::Transform,
				glm::vec3(0.0f),
				glm::vec3(0.0f),
				glm::vec3(1.0f));

			ecsManager->AddComponentToEntity(rootEntityID, rootTransformComponent);

			ECS::ECSEntityID childEntityID = 0;
			BL_DYNAMICARRAY_FOR(i, m_subMeshes, 0)
			{
				const Mesh* subMesh = m_subMeshes[i];

				childEntityID = subMesh->GenerateEntity(ecsManager, isStatic);	
				ecsManager->SetEntityParent(childEntityID, rootEntityID);
			}

			if (childEntityID > 0)
			{
				ECS::ECSEntity* childEntity = ecsManager->GetEntityMap().Get(childEntityID);
				entity->SetIsStatic(childEntity->GetIsStatic());
			}
			
			ecsManager->GenerateCachedHierarchyRoot();
		}
	} 

}

