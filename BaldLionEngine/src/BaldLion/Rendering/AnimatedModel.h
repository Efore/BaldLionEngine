#pragma once
#include "Model.h"
#include "BaldLion/Animation/Joint.h"
#include <assimp/scene.h>

namespace BaldLion
{
	namespace Rendering
	{
		class AnimatedModel : public Model {

		public:

			AnimatedModel(const std::string& filePath);
			~AnimatedModel();

		protected:
			virtual Mesh processMesh(const aiMesh *aimesh, const aiScene *aiscene) override;

		private:
			int GenerateJoints(std::vector<Animation::Joint>& joints, const int parentID, int& currentID, const aiNode* node);

		};
	}
}
