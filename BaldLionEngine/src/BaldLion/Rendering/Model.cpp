#include "blpch.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace BaldLion
{
	namespace Rendering
	{
		Model::Model(const std::string& filePath)
		{
			BL_PROFILE_FUNCTION();

			// Extracting folder path from filePath

			m_modelPath = filePath;
			auto lastSlash = filePath.find_last_of("/\\");		
			m_modelFolderPath = filePath.substr(0, lastSlash + 1);
		}

		Model::~Model()
		{

		}

		void Model::SetUpModel()
		{
			BL_PROFILE_FUNCTION();

			Assimp::Importer import;
			import.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, Rendering::NUM_WEIGHTS_PER_VEREX);

			const aiScene *scene = import.ReadFile(m_modelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights);

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
			for (const Mesh& m : m_subMeshes)
			{
				m.Draw();
			}
		}

		void Model::ProcessNode(const aiNode *node, const aiScene *scene)
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

		void Model::FillVertexArrayData(const aiMesh *aimesh, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
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

				vertices.emplace_back(Vertex({ position, color, normal, texCoord, tangent, bitangent }));
			}

			for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
			{
				aiFace face = aimesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
		}

		void Model::FillTextureData(const aiMesh *aimesh, const aiScene *aiscene, aiColor3D& ambientColor, aiColor3D& diffuseColor, aiColor3D& specularColor, aiColor3D& emissiveColor, std::string& ambientTexPath,
			std::string& diffuseTexPath, std::string& specularTexPath, std::string& emissiveTexPath, std::string& normalTexPath)
		{
			const aiMaterial* aimaterial = aiscene->mMaterials[aimesh->mMaterialIndex];

			aimaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
			aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
			aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
			aimaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);			

			if (aimaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
			{
				aiString ambientTex;
				aimaterial->GetTexture(aiTextureType_AMBIENT, 0, &ambientTex);								
				ambientTexPath = m_modelFolderPath;
				ambientTexPath.append(ambientTex.C_Str());
			}

			if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString diffuseTex;
				aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTex);		
				diffuseTexPath = m_modelFolderPath;
				diffuseTexPath.append(diffuseTex.C_Str());
			}

			if (aimaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
			{
				aiString specularTex;
				aimaterial->GetTexture(aiTextureType_SPECULAR, 0, &specularTex);
				specularTexPath = m_modelFolderPath;
				specularTexPath.append(specularTex.C_Str());
			}

			if (aimaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
			{
				aiString emissiveTex;
				aimaterial->GetTexture(aiTextureType_EMISSIVE, 0, &emissiveTex);
				emissiveTexPath = m_modelFolderPath;
				emissiveTexPath.append(emissiveTex.C_Str());
			}			
			
			if (aimaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
			{				 
				aiString normalTex;
				aimaterial->GetTexture(aiTextureType_NORMALS, 0, &normalTex);
				normalTexPath = m_modelFolderPath;
				normalTexPath.append(normalTex.C_Str());
			}
		}

		Mesh Model::ProcessMesh(const aiMesh *aimesh, const aiScene *aiscene)
		{
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

			return Mesh(vertices, indices,
				Material::Create("assets/shaders/nanosuit.glsl", 
					glm::vec3(ambientColor.r,ambientColor.g,ambientColor.b),
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

