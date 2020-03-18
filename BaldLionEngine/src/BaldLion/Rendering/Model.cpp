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

		void Model::processNode(aiNode *node, const aiScene *scene)
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

		Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
			std::vector<Texture> textures;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				// process vertex positions, normals and texture coordinates
				glm::vec3 position = glm::vec3(0.0f);
				if (mesh->HasPositions())
				{
					position.x = mesh->mVertices[i].x;
					position.y = mesh->mVertices[i].y;
					position.z = mesh->mVertices[i].z;
				}

				glm::vec3 normal = glm::vec3(0.0f);
				if (mesh->HasNormals())
				{
					normal.x = mesh->mNormals[i].x;
					normal.y = mesh->mNormals[i].y;
					normal.z = mesh->mNormals[i].z;
				}

				glm::vec3 color = glm::vec3(0.0f);
				if (mesh->HasVertexColors(i))
				{
					color.x = mesh->mColors[i]->r;
					color.y = mesh->mColors[i]->g;
					color.z = mesh->mColors[i]->b;
				}

				glm::vec2 texCoord = glm::vec2(0.0f);
				if (mesh->HasTextureCoords(0)) // does the mesh contain texture coordinates?
				{
					texCoord.x = mesh->mTextureCoords[0][i].x;
					texCoord.y = mesh->mTextureCoords[0][i].y;
				}

				vertices.emplace_back(Vertex({ position, color, normal, texCoord }));
			}

			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			return Mesh(vertices, indices,
				Material::Create("assets/shaders/BaseLit.glsl", glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 32.0f,
					"assets/textures/Glock_17/albedo.png",
					"assets/textures/Glock_17/albedo.png",
					"assets/textures/Glock_17/metallic.png",
					"assets/textures/Glock_17/normal.png"));
		}
	}

}

