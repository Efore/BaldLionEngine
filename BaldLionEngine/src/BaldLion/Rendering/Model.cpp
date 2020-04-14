#include "blpch.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace BaldLion
{
	namespace Rendering
	{
		Model::Model(const std::string& filePath) : m_modelPath(filePath)
		{
			BL_PROFILE_FUNCTION();
			SetUpModel(filePath);
		}

		Model::~Model()
		{

		}

		void Model::SetUpModel(const std::string& filePath)
		{
			Assimp::Importer import;
			const aiScene *scene = import.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
				return;
			}

			processNode(scene->mRootNode, scene);
		}

		void Model::Draw() const
		{
			BL_PROFILE_FUNCTION();
			for (const Mesh& m : m_subMeshes)
			{
				m.Draw();
			}
		}

		void Model::processNode(const aiNode *node, const aiScene *scene)
		{
			// process all the node's meshes (if any)
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
				m_subMeshes.emplace_back(processMesh(mesh, scene));
			}
			// then do the same for each of its children
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}
		}

		Mesh Model::processMesh(const aiMesh *aimesh, const aiScene *aiscene)
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
			std::vector<Texture> textures;

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

			for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
			{
				aiFace face = aimesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			
			const aiMaterial* aimaterial = aiscene->mMaterials[aimesh->mMaterialIndex];

			aiColor3D ambientColor;
			aimaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);

			aiColor3D diffuseColor;
			aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);

			aiColor3D specularColor;
			aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);

			aiColor3D emissiveColor;
			aimaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);		
			
			std::string ambientTexPath = "";
			if (aimaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
			{
				aiString ambientTex;
				aimaterial->GetTexture(aiTextureType_AMBIENT, 0, &ambientTex);
				ambientTexPath = m_modelPath;
				ambientTexPath.append(ambientTex.C_Str());
			}

			std::string diffuseTexPath = "";
			if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString diffuseTex;
				aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTex);
				diffuseTexPath = m_modelPath;
				diffuseTexPath.append(diffuseTex.C_Str());
			}

			std::string specularTexPath = "";
			if (aimaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
			{
				aiString specularTex;
				aimaterial->GetTexture(aiTextureType_SPECULAR, 0, &specularTex);
				specularTexPath = m_modelPath;
				specularTexPath.append(specularTex.C_Str());
			}

			std::string emissiveTexPath = "";
			if (aimaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
			{
				aiString emissiveTex;
				aimaterial->GetTexture(aiTextureType_EMISSIVE, 0, &emissiveTex);
				emissiveTexPath = m_modelPath;
				emissiveTexPath.append(emissiveTex.C_Str());
			}

			std::string normalTexPath = "";
			if (aimaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
			{
				aiString normalTex;
				aimaterial->GetTexture(aiTextureType_NORMALS, 0, &normalTex);
				normalTexPath = m_modelPath;
				normalTexPath.append(normalTex.C_Str());
			}


			return Mesh(vertices, indices,
				Material::Create("assets/shaders/BaseLit.glsl", 
					glm::vec3(ambientColor.r,ambientColor.g,ambientColor.b),
					glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b),
					glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b),
					glm::vec3(specularColor.r, specularColor.g, specularColor.b),
					32.0f,
					ambientTexPath,
					diffuseTexPath,
					specularTexPath,
					emissiveTexPath,
					normalTexPath));
		}
	}

}

