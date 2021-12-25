#pragma once

#include "BaldLion/SceneManagement/Scene.h"
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

namespace BaldLion 
{
	namespace SceneManagement 
	{
		class SceneSerializer 
		{

		public:
			static void SerializeScene(const Scene* scene, const char* filepath);
			static bool DeserializeScene(Scene* scene, const char* filepath);

		private:

			static void SerializeEntity(const Scene* scene, YAML::Emitter &out, const ECS::ECSEntity* entity);
			static void DeserializeEntity(const YAML::detail::iterator_value& yamlEntity);

			static void SerializeComponent(YAML::Emitter &out, const ECS::ECSComponent* component);
			static void DeserializeComponent(ECS::ECSEntityID entityID, const YAML::detail::iterator_value& yamlComponent);

			static void SerializeVec3(YAML::Emitter &out, const std::string& key, const glm::vec3& vec);
			static glm::vec3 DeserializeVec3(const YAML::Node& node, const std::string& key);
		};
	}
}
