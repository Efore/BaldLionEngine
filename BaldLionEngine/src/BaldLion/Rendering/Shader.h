#pragma once

#include <string>

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
			case ShaderDataType::Int4:					return 4 * 3;
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
			virtual void SetUniform(const std::string& uniformName, ShaderDataType dataType, const void* uniformIndex) = 0;

			virtual const std::string& GetName() const = 0;

			static Ref<Shader> Create(const std::string& filepath);
			static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		
		};

		class ShaderLibrary
		{
		public:
			void Add(const Ref<Shader>& shader);
			void Add(const std::string& name, const Ref<Shader>& shader);

			Ref<Shader> Load(const std::string& filepath);
			Ref<Shader> Load(const std::string& name, const std::string& filepath);

			Ref<Shader> Get(const std::string& name);

			bool Exists(const std::string& name) const;

			static std::string GetNameFromPath(const std::string& path);

		private:
			std::unordered_map<std::string, Ref<Shader>> m_shaders;
		};
	}
}

