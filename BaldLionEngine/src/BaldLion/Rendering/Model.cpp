#include "blpch.h"
#include "Model.h"
#include "Renderer.h"
#include "BaldLion/Animation/AnimationManager.h"
#include "BaldLion/Utils/GeometryUtils.h"
#include "BaldLion/Utils/MathUtils.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


namespace BaldLion
{
	namespace Rendering
	{
		Model::Model(const std::string& filePath)
		{
			BL_PROFILE_FUNCTION(); 

			// Extracting folder path from filePath

			m_modelPath = STRING_TO_STRINGID(filePath);
			auto lastSlash = filePath.find_last_of("/\\");		
			m_modelFolderPath = STRING_TO_STRINGID(filePath.substr(0, lastSlash + 1));
			m_subMeshes = DynamicArray<Mesh*>(AllocationType::FreeList_Renderer, 5);
			m_importFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes;
			
		}

		Model::~Model()
		{
			for (ui32 i = 0; i < m_subMeshes.Size(); ++i)
			{
				MemoryManager::DeleteNoDestructor(m_subMeshes[i]);
			}
			m_subMeshes.Delete();
		}

		void Model::SetUpModel()
		{
			BL_PROFILE_FUNCTION();

			Assimp::Importer import;			

			const aiScene *scene = import.ReadFile(STRINGID_TO_STRING(m_modelPath), m_importFlags);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
				return;
			}

			ProcessNode(scene->mRootNode, scene);
		}

		void Model::Draw() const
		{
			BL_PROFILE_FUNCTION();
			for (ui32 i = 0; i < m_subMeshes.Size(); ++i)
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
				
				m_subMeshes.PushBack(Model::ProcessMesh(mesh, scene, m_modelPath));
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

				glm::vec3 tangent = glm::vec3(0.0f);
				if (aimesh->HasTangentsAndBitangents())
				{
					tangent.x = aimesh->mTangents[i].x;
					tangent.y = aimesh->mTangents[i].y;
				}

				vertices.EmplaceBack(position, color, normal, texCoord, tangent);
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
				completeTexPath = STRINGID_TO_STRING(modelFolderPath);
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					ambientTex = Renderer::GetTextureLibrary().Load(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size, TextureType::Texture2d);
				}
				else
				{
					ambientTex = Renderer::GetTextureLibrary().Load(completeTexPath, TextureType::Texture2d);
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &relativeTexPath);
				completeTexPath = STRINGID_TO_STRING(modelFolderPath);
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					diffuseTex = Renderer::GetTextureLibrary().Load(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size, TextureType::Texture2d);
				}
				else
				{
					diffuseTex = Renderer::GetTextureLibrary().Load(completeTexPath, TextureType::Texture2d);
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
			{
				aimaterial->GetTexture(aiTextureType_SPECULAR, 0, &relativeTexPath);
				completeTexPath = STRINGID_TO_STRING(modelFolderPath);
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					specularTex = Renderer::GetTextureLibrary().Load(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size, TextureType::Texture2d);
				}
				else
				{
					specularTex = Renderer::GetTextureLibrary().Load(completeTexPath, TextureType::Texture2d);
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
			{
				aimaterial->GetTexture(aiTextureType_EMISSIVE, 0, &relativeTexPath);
				
				completeTexPath = STRINGID_TO_STRING(modelFolderPath);
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					emissiveTex = Renderer::GetTextureLibrary().Load(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size, TextureType::Texture2d);
				}
				else
				{
					emissiveTex = Renderer::GetTextureLibrary().Load(completeTexPath, TextureType::Texture2d);
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
			{
				aimaterial->GetTexture(aiTextureType_NORMALS, 0, &relativeTexPath);
				completeTexPath = STRINGID_TO_STRING(modelFolderPath);
				completeTexPath.append(relativeTexPath.C_Str());

				if (const aiTexture* embeddedTex = aiscene->GetEmbeddedTexture(relativeTexPath.C_Str()))
				{
					const int size = embeddedTex->mHeight == 0 ? embeddedTex->mWidth : embeddedTex->mWidth * embeddedTex->mHeight;
					normalTex = Renderer::GetTextureLibrary().Load(completeTexPath, reinterpret_cast<const unsigned char*>(embeddedTex->pcData), size, TextureType::Texture2d);
				}
				else
				{
					normalTex = Renderer::GetTextureLibrary().Load(completeTexPath, TextureType::Texture2d);
				}
			}
		}

		void Model::GenerateJointMapping(const aiMesh *aimesh, HashTable<StringId, ui32>& jointMapping, HashMap<StringId, glm::mat4>& jointOffsetMapping)
		{
			for (ui32 i = 0; i < aimesh->mNumBones; ++i)
			{
				jointMapping.Emplace(STRING_TO_STRINGID(aimesh->mBones[i]->mName.data), std::move(i));
				jointOffsetMapping.Emplace(STRING_TO_STRINGID(aimesh->mBones[i]->mName.data), MathUtils::AiMat4ToGlmMat4(aimesh->mBones[i]->mOffsetMatrix));
			}
		}

		void Model::FillJointData(HashTable<StringId, ui32>& jointMapping,
			DynamicArray<Animation::Joint>& jointsData,
			const HashMap<StringId, glm::mat4>& jointOffsetMapping,
			ui32& currentID,
			const int32_t parentID,
			const aiNode* node)
		{
			const StringId jointName = STRING_TO_STRINGID(node->mName.data);


			if (jointMapping.Contains(jointName))
			{
				jointMapping.Set(jointName,currentID);
				jointsData[currentID].jointID = currentID;
				jointsData[currentID].parentID = parentID;
				jointsData[currentID].jointBindTransform = jointOffsetMapping.Get(jointName);
				jointsData[currentID].jointAnimationTransform = jointsData[currentID].jointModelSpaceTransform = glm::mat4(1.0f);

				++currentID;
			}

			for (ui32 i = 0; i < node->mNumChildren; ++i)
			{
				ui32 jointId = -1;

				if (!jointMapping.TryGet(jointName, jointId))
				{
					jointId = parentID;
				}

				FillJointData(jointMapping, jointsData, jointOffsetMapping, currentID, jointId, node->mChildren[i]);
			}
		}

		void Model::FillVertexWeightData(const aiMesh* aimesh,
			const HashTable<StringId, ui32>& jointMapping,
			DynamicArray<VertexBone>& vertices)
		{
			ui32* jointsAssigned = new ui32[aimesh->mNumVertices]{ 0 };

			//Fill jointIDs and weights
			for (ui32 i = 0; i < aimesh->mNumBones; ++i)
			{
				for (ui32 j = 0; j < aimesh->mBones[i]->mNumWeights; ++j)
				{
					ui32 vertexID = aimesh->mBones[i]->mWeights[j].mVertexId;

					const StringId jointName = STRING_TO_STRINGID(aimesh->mBones[i]->mName.data);

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
	

		Mesh* Model::ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene, const StringId modelFolderPath)
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

			FillTextureData(aimesh, aiscene, modelFolderPath, ambientColor, diffuseColor, specularColor, emissiveColor, ambientTex, diffuseTex, specularTex, emissiveTex, normalTex);

			Material::MaterialProperties materialProperties
			{
				aimesh->HasBones() ? STRING_TO_STRINGID("assets/shaders/SkinnedLit.glsl") : STRING_TO_STRINGID("assets/shaders/BaseLit.glsl"),
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

			Material* meshMaterial = MaterialLibrary::Load(aiscene->mMaterials[aimesh->mMaterialIndex]->GetName().data, &materialProperties);

			meshMaterial->AssignShader();
			
			Mesh* mesh = MemoryManager::New<Mesh>("Mesh", AllocationType::FreeList_Renderer,  meshMaterial);

			if (aimesh->HasBones())
			{
				DynamicArray<VertexBone> verticesBoneData(AllocationType::Linear_Frame, aimesh->mNumVertices);
				DynamicArray<Animation::Joint> jointsData(AllocationType::FreeList_Renderer, aimesh->mNumBones);

				verticesBoneData.Populate();
				jointsData.Populate();

				HashTable<StringId, ui32> jointMapping(AllocationType::Linear_Frame, aimesh->mNumBones * 2);
				HashMap<StringId, glm::mat4> jointOffsetMapping(AllocationType::Linear_Frame, aimesh->mNumBones * 2);

				GenerateJointMapping(aimesh, jointMapping, jointOffsetMapping);

				ui32 firstID = 0;
				FillJointData(jointMapping, jointsData, jointOffsetMapping, firstID, -1, aiscene->mRootNode);

				FillVertexWeightData(aimesh, jointMapping, verticesBoneData);

				mesh->SetSkeleton(MemoryManager::New<Animation::Skeleton>("Skeleton", AllocationType::FreeList_Renderer, jointsData));		
				mesh->SetVertexBones(verticesBoneData);

				Animation::AnimationManager::GenerateAnimator(aiscene, jointMapping);
			}

			mesh->SetUpMesh(vertices, indices);

			return mesh;
		}

	} 

}

