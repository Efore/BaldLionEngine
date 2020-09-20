#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace BaldLion {

	namespace Quantization {

		struct QuantizedVector3 {
			uint16_t x;
			uint16_t y;
			uint16_t z;
		};

		struct QuantizedQuaterion {
			uint16_t x;
			uint16_t y;
			uint16_t z;
		};

		static uint32_t CompressFloat01RL(float float01, uint32_t nBits) 
		{
			//Determine the number of intervals based on the number of output bits we've been asked to produce
			uint32_t nIntervals = 1u << nBits;

			//Scale the input value from the range [0, 1] into the range [0, nintervals - 1]. We substract one interval because we want the largest output value to fit into nBits bits
			float scaled = float01 * (float)(nIntervals - 1u);

			//Finally, round to the nearest interval center. We do this by adding 0.5f and then truncating to the next-lowest interval index (by casting to uint32_t)
			uint32_t rounded = (uint32_t)(scaled + 0.5f);

			//Guard against invalid input values
			if (rounded > nIntervals - 1u)
				rounded = nIntervals - 1u;

			return rounded;
		}

		static uint32_t CompressFloatRL(float value, float min, float max, uint32_t nBits)
		{
			float unitFloat = (value - min) / (max - min);
			uint32_t quantized = CompressFloat01RL(unitFloat, nBits);

			return quantized;
		}

		static float DecompressFloat01RL(uint32_t quantized, uint32_t nBits)
		{
			//Determine the number of intervals based on the numb er of bits we used when encoded the value
			uint32_t nIntervals = 1u << nBits;

			//Decode by simply converting the uint32_t to a float, and scaling by the interval size
			float intervalSize = 1.0f / (float)(nIntervals - 1u);
			float approxFloat01 = (float)quantized * intervalSize;

			return approxFloat01;
		}

		static float DecompressFloatRL(uint32_t quantized, float min, float max, uint32_t nBits)
		{
			float float01 = DecompressFloat01RL(quantized, nBits);
			float value = min + (float01 * (max - min));

			return value;
		}

		static QuantizedQuaterion CompressQuaternion(const glm::quat& quaternion)
		{
			return QuantizedQuaterion
			{
				(uint16_t)CompressFloatRL(quaternion.x, -1.0f, 1.0f, 16u),
				(uint16_t)CompressFloatRL(quaternion.y, -1.0f, 1.0f, 16u),
				(uint16_t)CompressFloatRL(quaternion.z, -1.0f, 1.0f, 16u)
			};
		}

		static glm::quat DecompressCuaternion(const QuantizedQuaterion& quantizedQuaterion) {

			float x = DecompressFloatRL((uint32_t)quantizedQuaterion.x, -1.0f, 1.0f, 16u);
			float y = DecompressFloatRL((uint32_t)quantizedQuaterion.y, -1.0f, 1.0f, 16u);
			float z = DecompressFloatRL((uint32_t)quantizedQuaterion.z, -1.0f, 1.0f, 16u);

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
				(uint16_t)CompressFloatRL(x, -range, range, 16u),
				(uint16_t)CompressFloatRL(y, -range, range, 16u),
				(uint16_t)CompressFloatRL(z, -range, range, 16u)
			};
		}

		static glm::vec3 DecompressVector3(const QuantizedVector3& vector3, float range)
		{
			return glm::vec3(
				DecompressFloatRL((uint32_t)vector3.x, -range, range, 16u),
				DecompressFloatRL((uint32_t)vector3.y, -range, range, 16u),
				DecompressFloatRL((uint32_t)vector3.z, -range, range, 16u)
			);
		}
	}
}


