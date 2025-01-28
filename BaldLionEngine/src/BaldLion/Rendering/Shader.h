#pragma once

#include <string>
#include "BaldLion/Core/Containers/HashTable.h"
#include "BaldLion/Core/Core.h"
#include "BaldLion/ResourceManagement/Resource.h"
#include "BaldLion/Utils/StringUtils.h"

#define UNIFORM_AMBIENT_COLOR			BL_STRING_TO_STRINGID("u_material.ambientColor")
#define UNIFORM_DIFFUSE_COLOR			BL_STRING_TO_STRINGID("u_material.diffuseColor" )
#define UNIFORM_EMISSIVE_COLOR			BL_STRING_TO_STRINGID("u_material.emissiveColor")
#define UNIFORM_SPECULAR_COLOR			BL_STRING_TO_STRINGID("u_material.specularColor")
#define UNIFORM_SHININESS				BL_STRING_TO_STRINGID("u_material.shininess")
										
#define UNIFORM_USE_AMBIENT_TEX			BL_STRING_TO_STRINGID("u_material.useAmbientTex")
#define UNIFORM_AMBIENT_TEX				BL_STRING_TO_STRINGID("u_material.ambientTex")
#define UNIFORM_USE_DIFFUSE_TEX			BL_STRING_TO_STRINGID("u_material.useDiffuseTex" )
#define UNIFORM_DIFFUSE_TEX				BL_STRING_TO_STRINGID("u_material.diffuseTex")
#define UNIFORM_USE_EMISSIVE_TEX		BL_STRING_TO_STRINGID("u_material.useEmissiveTex" )
#define UNIFORM_EMISSIVE_TEX			BL_STRING_TO_STRINGID("u_material.emissiveTex")
#define UNIFORM_USE_SPECULAR_TEX		BL_STRING_TO_STRINGID("u_material.useSpecularTex" )
#define UNIFORM_SPECULAR_TEX			BL_STRING_TO_STRINGID("u_material.specularTex")
#define UNIFORM_USE_NORMAL_TEX			BL_STRING_TO_STRINGID("u_material.useNormalTex")
#define UNIFORM_NORMAL_TEX				BL_STRING_TO_STRINGID("u_material.normalTex")
										
#define UNIFORM_USE_SHADOWMAP_TEX			BL_STRING_TO_STRINGID("u_useShadowMap")
#define UNIFORM_SHADOWMAP_TEX				BL_STRING_TO_STRINGID("u_shadowMapTex")
#define UNIFORM_SHADOW_DISTANCE				BL_STRING_TO_STRINGID("u_shadowDistance")
#define UNIFORM_SHADOW_TRANSITION_DISTANCE	BL_STRING_TO_STRINGID("u_shadowTransitionDistance")
										
#define UNIFORM_MODEL_SPACE_TRANSFORM	BL_STRING_TO_STRINGID("u_worldTransformMatrix")
#define UNIFORM_VIEW_PROJECTION			BL_STRING_TO_STRINGID("u_viewProjectionMatrix")
#define UNIFORM_CAMERA_POS				BL_STRING_TO_STRINGID("u_cameraPos")
										
#define UNIFORM_LIGHT_SPACE_TRANSFORM	BL_STRING_TO_STRINGID("u_lightViewProjectionMatrix")
										
#define UNIFORM_DIR_LIGHT_DIRECTION		BL_STRING_TO_STRINGID("u_directionalLight.direction")
#define UNIFORM_DIR_LIGHT_AMBIENT		BL_STRING_TO_STRINGID("u_directionalLight.ambientColor")
#define UNIFORM_DIR_LIGHT_DIFFUSE		BL_STRING_TO_STRINGID("u_directionalLight.diffuseColor")
#define UNIFORM_DIR_LIGHT_SPECULAR		BL_STRING_TO_STRINGID("u_directionalLight.specularColor")

namespace BaldLion 
{
	namespace Rendering
	{
		enum class ShaderDataType : ui16
		{
			None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
		};

		static ui32 ShaderDataTypeSize(const ShaderDataType& type)
		{
			switch (type)
			{
			case ShaderDataType::Float:					return 4;
			case ShaderDataType::Float2:				return 4 * 2;
			case ShaderDataType::Float3:				return 4 * 3;
			case ShaderDataType::Float4:				return 4 * 4;
			case ShaderDataType::Mat3:					return 4 * 3 * 3;
			case ShaderDataType::Mat4:					return 4 * 4 * 4;
			case ShaderDataType::Int:					return 4;
			case ShaderDataType::Int2:					return 4 * 2;
			case ShaderDataType::Int3:					return 4 * 3;
			case ShaderDataType::Int4:					return 4 * 4;
			case ShaderDataType::Bool:					return 1;
			}

			BL_CORE_ASSERT(false, "Unknown ShaderDataType");
			return 0;
		}

		class Shader : public ResourceManagement::Resource
		{
		public:
			virtual ~Shader() = default;

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;
			virtual void SetUniform(StringId uniformName, ShaderDataType dataType, const void* uniformIndex) = 0;

			static Shader* Create(const std::string& filepath);
		protected:

			Shader(const std::string& filepath) : ResourceManagement::Resource(BL_STRING_TO_STRINGID(filepath), filepath, ResourceManagement::ResourceType::Shader){}
		};
	}
}

