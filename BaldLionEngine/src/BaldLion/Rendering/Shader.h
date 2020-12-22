#pragma once

#include <string>
#include "BaldLion/Core/Containers/HashTable.h"

namespace BaldLion 
{
	namespace Rendering
	{
		enum class ShaderDataType
		{
			None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
		};

		static uint32_t ShaderDataTypeSize(const ShaderDataType& type)
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

		class Shader
		{
		public:
			virtual ~Shader() = default;

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;
			virtual void SetUniform(const char* uniformName, ShaderDataType dataType, const void* uniformIndex) = 0;

			virtual const char* GetName() const = 0;

			static Shader* Create(const std::string& filepath);
		};

		class ShaderLibrary
		{
		public:

			virtual ~ShaderLibrary();

			void Init();

			void Add(Shader* shader);
			void Add(const char*, Shader* shader);

			Shader* Load(const std::string& filepath);
			Shader* Load(const char* name, const std::string& filepath);
			
			void Clear();
			bool Exists(const char*) const;

			static void GetNameFromPath(const std::string& path, char *name);

		private:
			HashTable<const char*, Shader*> m_shaders;
		};
	}
}

