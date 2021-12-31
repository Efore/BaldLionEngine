#pragma once

#include "BaldLion/Core/Core.h"

namespace BaldLion
{
	namespace ResourceManagement
	{
		enum class ResourceType {
			Texture,
			Model,
			Mesh,
			Skeleton,
			Material,
			Shader,

			Count
		};
		
		class Resource
		{
		public:
			Resource() = default;

			Resource(const Resource& other) = delete;
			Resource(Resource&& other) = delete;
			Resource & operator=(const Resource&) = delete;

			Resource(ui32 resourceID, const std::string& resourceName, ResourceType resourceType) : m_resourceID(resourceID), m_resourceName(STRING_TO_STRINGID(resourceName)), m_resourceType(resourceType)
			{
			}

			const ui32 GetResourceID() const { return m_resourceID; }
			const StringId GetResourceName() const { return m_resourceName; }

		protected:

			ui32 m_resourceID;
			StringId m_resourceName;
			ResourceType m_resourceType;
		};
	}
}