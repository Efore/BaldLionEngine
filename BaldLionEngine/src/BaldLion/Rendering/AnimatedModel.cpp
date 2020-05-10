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