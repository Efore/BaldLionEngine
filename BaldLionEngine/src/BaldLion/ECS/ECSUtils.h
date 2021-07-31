#pragma once
#include <bitset>
#include <cstdarg>
#include "BaldLion/Core/Core.h"

namespace BaldLion
{
	namespace ECS {

		enum class ECSComponentID : ui32
		{
			None,
			Transform,
			ProjectionCamera,


			Count
		};

		//---------------------------------------------------------------------------------------------------

		using ECSEntityID = ui32;
		using ECSSignature = std::bitset<(ui32)ECSComponentID::Count>;

		//---------------------------------------------------------------------------------------------------

		static ECSSignature GenerateSignature(ui32 count, ...)
		{
			std::va_list argsComponents;
			va_start(argsComponents, count);

			ECSComponentID componentID = va_arg(argsComponents, ECSComponentID);

			ECSSignature result;
			result.set((ui32)componentID);

			for (ui32 i = 1; i < count; ++i)
			{
				componentID = va_arg(argsComponents, ECSComponentID);
				result.set((ui32)componentID);
			}

			return result;
		}

		static ECSSignature NullSignature;

		//---------------------------------------------------------------------------------------------------

	}
}
