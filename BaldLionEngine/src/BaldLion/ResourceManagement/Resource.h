#pragma once

#include "BaldLion/Core/Core.h"
#include "BaldLion/Utils/StringUtils.h"

namespace BaldLion
{
	namespace ResourceManagement
	{
		enum class ResourceType {
			None = 0,
			Texture = 1,
			Model = 2,
			Mesh = 3,
			Skeleton = 4,
			Material = 5,
			Shader = 6,
			Animator = 7,
			Animation = 8,

			Meta
		};
		
		class Resource
		{
		public:
			Resource() = default;

			Resource(const Resource& other) = delete;
			Resource(Resource&& other) = delete;
			Resource & operator=(const Resource&) = delete;

			Resource(ui32 resourceID, const std::string& resourcePath, ResourceType resourceType) : 
				m_resourceID(resourceID), 
				m_resourcePath(BL_STRING_TO_STRINGID(resourcePath)), 
				m_resourceName(BL_STRING_TO_STRINGID(StringUtils::GetFileNameFromPath(resourcePath))),
				m_resourceType(resourceType)
			{
			}

			const ui32 GetResourceID() const { return m_resourceID; }
			const StringId GetResourcePath() const { return m_resourcePath; }
			const StringId GetResourceName() const { return m_resourceName; }
			const ResourceType GetResourceType() const { return m_resourceType; }

		protected:

			ui32 m_resourceID = 0;
			StringId m_resourcePath = 0;
			StringId m_resourceName = 0;
			ResourceType m_resourceType = ResourceType::None;
		};
	}
}