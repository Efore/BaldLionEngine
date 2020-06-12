#include "blpch.h"
#include "AnimationManager.h"

namespace BaldLion
{
	namespace Animation
	{
		AnimationManager* AnimationManager::s_instance;

		AnimationManager::AnimationManager()
		{
			m_registeredAnimators.reserve(10);
		}

		AnimationManager::~AnimationManager()
		{

		}

		void AnimationManager::OnUpdate(float timeStep)
		{
			for (Ref<Animator> animator : m_registeredAnimators)
			{
				animator->OnUpdate(timeStep);
			}
		}

		AnimationManager* AnimationManager::GetInstance()
		{
			if (s_instance == nullptr)
				s_instance = new AnimationManager();

			return s_instance;
		}

		void AnimationManager::GenerateAnimator(const aiScene *scene, const std::map<std::string, uint32_t>& jointMapping, const Ref<AnimatedMesh>& animatedMesh)
		{
			if (scene->HasAnimations())
			{
				std::vector<Ref<AnimationData>> animations(scene->mNumAnimations);
				for (size_t i = 0; i < scene->mNumAnimations; ++i)
				{
					Ref<AnimationData> animationData = CreateRef<AnimationData>();

					animationData->animationName = scene->mAnimations[i]->mName.data;
					animationData->animationLength = (float)(scene->mAnimations[i]->mDuration / scene->mAnimations[i]->mTicksPerSecond);
					animationData->frames = std::vector<KeyFrame>((int)scene->mAnimations[i]->mChannels[0]->mNumPositionKeys);

					float timeStamp = (float)(1.0f / scene->mAnimations[i]->mTicksPerSecond);

					for (size_t j = 0; j < animationData->frames.size(); ++j)
					{
						KeyFrame keyFrame;
						keyFrame.timeStamp = glm::min(timeStamp * j, animationData->animationLength);
						keyFrame.jointTranforms = std::vector<JointTransform>(glm::max((int)scene->mAnimations[i]->mNumChannels, (int)jointMapping.size()));

						for (size_t k = 0; k < scene->mAnimations[i]->mNumChannels; ++k)
						{
							keyFrame.jointTranforms[jointMapping.at(scene->mAnimations[i]->mChannels[k]->mNodeName.data)] = (JointTransform({
								glm::vec3(scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.x, scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.y, scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.z),
								glm::quat(scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.x, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.y, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.z, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.w),
								glm::vec3(scene->mAnimations[i]->mChannels[k]->mScalingKeys[j].mValue.x, scene->mAnimations[i]->mChannels[k]->mScalingKeys[j].mValue.y, scene->mAnimations[i]->mChannels[k]->mScalingKeys[j].mValue.z)
								}));
						}

						animationData->frames[j] = keyFrame;
					}

					animations[i] = animationData;
				}

				glm::mat4 rootTransform = AnimatedMesh::AiMat4ToGlmMat4(scene->mRootNode->mTransformation);

				Ref<Animator> animator = CreateRef<Animator>(animatedMesh, animations, rootTransform);
				RegisterAnimator(animator);
			}
		}

		void AnimationManager::RegisterAnimator(Ref<Animator> animator)
		{
			if (std::find(m_registeredAnimators.begin(), m_registeredAnimators.end(), animator) == m_registeredAnimators.end())
			{
				m_registeredAnimators.push_back(animator);
			}
		}

		void AnimationManager::UnregisterAnimator(Ref<Animator> animator)
		{
			auto it = std::find(m_registeredAnimators.begin(), m_registeredAnimators.end(), animator);				

			if (it != m_registeredAnimators.end())
			{
				std::swap(*it, m_registeredAnimators.back());				
				m_registeredAnimators.pop_back();
			}
		}
	}
}