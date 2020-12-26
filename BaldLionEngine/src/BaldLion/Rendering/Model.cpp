#include "blpch.h"
#include "Model.h"
#include "Renderer.h"

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

			m_modelPath = STRING_TO_ID(filePath);
			auto lastSlash = filePath.find_last_of("/\\");		
			m_modelFolderPath = STRING_TO_ID(filePath.substr(0, lastSlash + 1));
			m_subMeshes = DynamicArray<Mesh*>(AllocationType::FreeList_Renderer, 5);
		}

		Model::~Model()
		{
			for (uint32_t i = 0; i < m_subMeshes.Size(); ++i)
			{
				MemoryManager::DeleteNoDestructor(m_subMeshes[i]);
			}
			m_subMeshes.Clear();
		}

		void Model::SetUpModel()
		{
			BL_PROFILE_FUNCTION();

			Assimp::Importer import;			

			const aiScene *scene = import.ReadFile(ID_TO_STRING(m_modelPath), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

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
			for (uint32_t i = 0; i < m_subMeshes.Size(); ++i)
			{
				m_subMeshes[i]->Draw();
			}
		}

		void Model::ProcessNode(const aiNode *node, const aiScene *scene)
		{
			// process all the node's meshes (if any)
			for (uint32_t i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
				m_subMeshes.PushBack(ProcessMesh(mesh, scene));
			}
			// then do the same for each of its children
			for (uint32_t i = 0; i < node->mNumChildren; i++)
			{
				ProcessNode(node->mChildren[i], scene);
			}
		}

		void Model::FillVertexArrayData(const aiMesh *aimesh, DynamicArray<Vertex>& vertices, DynamicArray<uint32_t>& indices)
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
				glm::vec3 bitangent = glm::vec3(0.0f);
				if (aimesh->HasTangentsAndBitangents())
				{
					tangent.x = aimesh->mTangents[i].x;
					tangent.y = aimesh->mTangents[i].y;

					bitangent.x = aimesh->mBitangents[i].x;
					bitangent.y = aimesh->mBitangents[i].y;
				}

				vertices.PushBack(Vertex{ position, color, normal, texCoord, tangent, bitangent });
			}

			for (uint32_t i = 0; i < aimesh->mNumFaces; i++)
			{
				aiFace face = aimesh->mFaces[i];
				for (size_t j = 0; j < face.mNumIndices; j++)
					indices.PushBack(face.mIndices[j]);
			}
		}

		void Model::FillTextureData(const aiMesh *aimesh, 
			const aiScene *aiscene, 
			aiColor3D& ambientColor, 
			aiColor3D& diffuseColor, 
			aiColor3D& specularColor, 
			aiColor3D& emissiveColor, 
			Texture*&  ambientTex,
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
				completeTexPath = ID_TO_STRING(m_modelFolderPath);
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
				completeTexPath = ID_TO_STRING(m_modelFolderPath);
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
				completeTexPath = ID_TO_STRING(m_modelFolderPath);
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
				completeTexPath = ID_TO_STRING(m_modelFolderPath);
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
				completeTexPath = ID_TO_STRING(m_modelFolderPath);
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

		Mesh* Model::ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene)
		{
			DynamicArray<Vertex> vertices(AllocationType::FreeList_Renderer, aimesh->mNumVertices);
			DynamicArray<uint32_t> indices(AllocationType::FreeList_Renderer, aimesh->mNumVertices * 3);

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

			FillTextureData(aimesh, aiscene, ambientColor, diffuseColor, specularColor, emissiveColor, ambientTex, diffuseTex, specularTex, emissiveTex, normalTex);

			return MemoryManager::New<Mesh>(STRING_TO_ID("Mesh"), AllocationType::FreeList_Renderer, vertices, indices,
				Material::Create("assets/shaders/nanosuit.glsl", 
					glm::vec3(ambientColor.r,ambientColor.g,ambientColor.b),
					glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b),
					glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b),
					glm::vec3(specularColor.r, specularColor.g, specularColor.b),
					32.0f,
					ambientTex,
					diffuseTex,
					specularTex,
					emissiveTex,
					normalTex));
		}


	}

}

