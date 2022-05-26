#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "BaldLion/Core/Core.h"
#include <math.h>

namespace BaldLion {

	namespace Compression {

		struct QuantizationRange {

			float minValue = 0;
			float maxValue = 0;
		};

		template<unsigned N>
		static ui16 CompressFloat01RL(float float01) 
		{
			static_assert(N > 0, "Invalid number of bits specified");
			static_assert(N <= 16, "Encoding into more than 16bits is not allowed");

			//Determine the number of intervals based on the number of output bits we've been asked to produce
			ui32 nIntervals = 1u << N;

			//Scale the input value from the range [0, 1] into the range [0, nintervals - 1]. We substract one interval because we want the largest output value to fit into nBits bits
			float scaled = float01 * (float)(nIntervals - 1u);

			//Finally, round to the nearest interval center. We do this by adding 0.5f and then truncating to the next-lowest interval index (by casting to i32)
			ui32 rounded = (ui32)(scaled + 0.5f);

			//Guard against invalid input values
			if (rounded > (nIntervals - 1u))
				rounded = nIntervals - 1u;

			return (ui16)rounded;
		}

		template<unsigned N>
		static ui16 CompressFloatRL(float value, float min, float max)
		{
			float unitFloat = (value - min) / (max - min);
			ui16 quantized = CompressFloat01RL<N>(unitFloat);

			return quantized;
		}

		template<unsigned N>
		static float DecompressFloat01RL(ui16 quantized)
		{
			static_assert(N > 0, "Invalid number of bits specified");
			static_assert(N <= 16, "Encoding into more than 16bits is not allowed");

			//Determine the number of intervals based on the numb er of bits we used when encoded the value
			ui32 nIntervals = 1u << N;

			//Decode by simply converting the i32 to a float, and scaling by the interval size
			float intervalSize = 1.0f / (float)(nIntervals - 1u);
			float approxFloat01 = (float)quantized * intervalSize;

			return approxFloat01;
		}

		template<unsigned N>
		static float DecompressFloatRL(ui16 quantized, float min, float max)
		{
			float float01 = DecompressFloat01RL<N>(quantized);
			float value = min + (float01 * (max - min));

			return value;
		}

		static void CompressSmallestThree(float inputW, float inputX, float inputY, float inputZ, ui16& outputA, ui16& outputB, ui16& outputC)
		{
			ui16 largestValue = 0;
			float maxAbsValue = glm::abs(inputW);

			float absValue = glm::abs(inputX);
			if (absValue > maxAbsValue)
			{
				largestValue = 1;
				maxAbsValue = absValue;
			}

			absValue = glm::abs(inputY);
			if (absValue > maxAbsValue)
			{
				largestValue = 2;
				maxAbsValue = absValue;
			}

			absValue = glm::abs(inputZ);
			if (absValue > maxAbsValue)
			{
				largestValue = 3;
				maxAbsValue = absValue;
			}

			static float const minValue = -(1.0f / 1.4142135623730950488016887242097f);
			static float const maxValue = (1.0f / 1.4142135623730950488016887242097f);

			switch (largestValue)
			{
			case 0:
				outputA = CompressFloatRL<15>(inputX, minValue, maxValue);
				outputB = CompressFloatRL<15>(inputY, minValue, maxValue);
				outputC = CompressFloatRL<15>(inputZ, minValue, maxValue);
				break;

			case 1:
				outputA = CompressFloatRL<15>(inputW, minValue, maxValue);
				outputB = CompressFloatRL<15>(inputY, minValue, maxValue);
				outputC = CompressFloatRL<15>(inputZ, minValue, maxValue);

				outputA |= 0x8000;
				break;

			case 2:
				outputA = CompressFloatRL<15>(inputW, minValue, maxValue);
				outputB = CompressFloatRL<15>(inputX, minValue, maxValue);
				outputC = CompressFloatRL<15>(inputZ, minValue, maxValue);

				outputB |= 0x8000;
				break;

			case 3:
				outputA = CompressFloatRL<15>(inputW, minValue, maxValue);
				outputB = CompressFloatRL<15>(inputX, minValue, maxValue);
				outputC = CompressFloatRL<15>(inputY, minValue, maxValue);

				outputA |= 0x8000;
				outputB |= 0x8000;
				break;
			}
		}

		static void DecompressSmallestThree( ui16 inputA, ui16 inputB, ui16 inputC, glm::quat& output)
		{
			const ui16 largestValue = (inputB >> 14 & 0x0002) | inputA >> 15;
			inputA = inputA & 0x7FFF;
			inputB = inputB & 0x7FFF;

			static float const minValue = -(1.0f / 1.4142135623730950488016887242097f);
			static float const maxValue = (1.0f / 1.4142135623730950488016887242097f);

			float valueA = DecompressFloatRL<15>(inputA, minValue, maxValue);
			float valueB = DecompressFloatRL<15>(inputB, minValue, maxValue);
			float valueC = DecompressFloatRL<15>(inputC, minValue, maxValue);
			float sum = (valueA * valueA) + (valueB * valueB) + (valueC * valueC);

			float valueD = sqrtf(1.0f - sum);

			switch (largestValue)
			{
			case 0:
				output.w = valueD;
				output.x = valueA;
				output.y = valueB;
				output.z = valueC;
				break;

			case 1:
				output.w = valueA;
				output.x = valueD;
				output.y = valueB;
				output.z = valueC;
				break;

			case 2:
				output.w = valueA;
				output.x = valueB;
				output.y = valueD;
				output.z = valueC;
				break;

			case 3:				
				output.w = valueA;
				output.x = valueB;
				output.y = valueC;
				output.z = valueD;
				break;
			}

		}
	}
}


