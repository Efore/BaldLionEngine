#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "BaldLion/Core/Core.h"

namespace BaldLion {

	namespace Compression {

		struct QuantizedVector3 {
			ui16 x;
			ui16 y;
			ui16 z;
		};

		struct QuantizedQuaterion {
			ui16 x;
			ui16 y;
			ui16 z;
		};

		static ui32 CompressFloat01RL(float float01, ui32 nBits) 
		{
			//Determine the number of intervals based on the number of output bits we've been asked to produce
			ui32 nIntervals = 1u << nBits;

			//Scale the input value from the range [0, 1] into the range [0, nintervals - 1]. We substract one interval because we want the largest output value to fit into nBits bits
			float scaled = float01 * (float)(nIntervals - 1u);

			//Finally, round to the nearest interval center. We do this by adding 0.5f and then truncating to the next-lowest interval index (by casting to i32)
			ui32 rounded = (ui32)(scaled + 0.5f);

			//Guard against invalid input values
			if (rounded > (nIntervals - 1u))
				rounded = nIntervals - 1u;

			return rounded;
		}

		static ui32 CompressFloatRL(float value, float min, float max, ui32 nBits)
		{
			float unitFloat = (value - min) / (max - min);
			ui32 quantized = CompressFloat01RL(unitFloat, nBits);

			return quantized;
		}

		static float DecompressFloat01RL(ui32 quantized, ui32 nBits)
		{
			//Determine the number of intervals based on the numb er of bits we used when encoded the value
			ui32 nIntervals = 1u << nBits;

			//Decode by simply converting the i32 to a float, and scaling by the interval size
			float intervalSize = 1.0f / (float)(nIntervals - 1u);
			float approxFloat01 = (float)quantized * intervalSize;

			return approxFloat01;
		}

		static float DecompressFloatRL(ui32 quantized, float min, float max, ui32 nBits)
		{
			float float01 = DecompressFloat01RL(quantized, nBits);
			float value = min + (float01 * (max - min));

			return value;
		}

		static QuantizedQuaterion CompressQuaternion(const glm::quat& quaternion)
		{
			return QuantizedQuaterion
			{
				(ui16)CompressFloatRL(quaternion.x, -1.0f, 1.0f, 16u),
				(ui16)CompressFloatRL(quaternion.y, -1.0f, 1.0f, 16u),
				(ui16)CompressFloatRL(quaternion.z, -1.0f, 1.0f, 16u)
			};
		}

		static glm::quat DecompressCuaternion(const QuantizedQuaterion& quantizedQuaterion) {

			float x = DecompressFloatRL((ui32)quantizedQuaterion.x, -1.0f, 1.0f, 16u);
			float y = DecompressFloatRL((ui32)quantizedQuaterion.y, -1.0f, 1.0f, 16u);
			float z = DecompressFloatRL((ui32)quantizedQuaterion.z, -1.0f, 1.0f, 16u);

			float w = glm::sqrt(1.0f - (x*x + y*y + z*z));

			return glm::quat(w, x, y, z);
		}

		static QuantizedVector3 CompressVector3(const glm::vec3& vector3, float range)
		{
			float x = vector3.x > range ? range : vector3.x < -range ? -range : vector3.x;
			float y = vector3.y > range ? range : vector3.y < -range ? -range : vector3.y;
			float z = vector3.z > range ? range : vector3.z < -range ? -range : vector3.z;

			return QuantizedVector3
			{
				(ui16)CompressFloatRL(x, -range, range, 16u),
				(ui16)CompressFloatRL(y, -range, range, 16u),
				(ui16)CompressFloatRL(z, -range, range, 16u)
			};
		}

		static glm::vec3 DecompressVector3(const QuantizedVector3& vector3, float range)
		{
			return glm::vec3(
				DecompressFloatRL((ui32)vector3.x, -range, range, 16u),
				DecompressFloatRL((ui32)vector3.y, -range, range, 16u),
				DecompressFloatRL((ui32)vector3.z, -range, range, 16u)
			);
		}
	}
}


