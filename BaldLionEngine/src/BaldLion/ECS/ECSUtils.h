#pragma once
#include <bitset>
#include <cstdarg>
#include "BaldLion/Core/Core.h"

namespace BaldLion
{
	namespace ECS {

		enum class ECSComponentType : ui32
		{
			Transform,
			ProjectionCamera,
			Mesh,
			Skeleton,
			DirectionalLight,
			Animation,
			PhysicsBody,
			NavMeshAgent,
			Locomotion,

			Count
		};

		//---------------------------------------------------------------------------------------------------

		using ECSEntityID = ui32;
		using ECSSignature = std::bitset<(ui32)ECSComponentType::Count>;

		//---------------------------------------------------------------------------------------------------

		static ECSSignature GenerateSignature(ui32 count, ...)
		{
			std::va_list argsComponents;
			va_start(argsComponents, count);

			ECSComponentType componentType = ECSComponentType::Count;
			ECSSignature result;

			for (ui32 i = 0; i < count; ++i)
			{
				componentType = va_arg(argsComponents, ECSComponentType);
				result.set((ui32)componentType);
			}

			return result;
		}

		static ECSSignature NullSignature;

		//---------------------------------------------------------------------------------------------------

	}
}
