#pragma once
#include "BaldLion/Core/Core.h"
#include "BaldLion/Core/StringId.h"
#include <glm/gtc/quaternion.hpp>

namespace BaldLion
{	
	enum class VariantType
	{
		Unknown,
		Bool,
		Int,
		UInt8,
		UInt16,
		UInt32,
		UInt64,
		Float,
		Vector2,
		Vector3,
		Quaternion,
		StringID
	};
	
	enum class VariantComparisonType {
		Lower = 0,
		LowerOrEqual = 1,
		Equal = 2,
		Unequal = 3,
		HigherOrEqual = 4,
		Higher = 5
	};
	

	#define VARIANT_SIZE sizeof(glm::quat)

	class Variant;

	class Variant
	{
	public:

		static Variant EmptyVariant;

		Variant();
		Variant(bool val);
		Variant(i32 val);
		Variant(ui8 val);
		Variant(ui16 val);
		Variant(ui32 val);
		Variant(const ui64& val);
		Variant(float val);
		Variant(const glm::vec2& val);
		Variant(const glm::vec3& val);
		Variant(const glm::quat& val);

		Variant operator=(bool val);
		Variant operator=(i32 val);
		Variant operator=(ui8 val);
		Variant operator=(ui16 val);
		Variant operator=(ui32 val);
		Variant operator=(const ui64& val);
		Variant operator=(float val);
		Variant operator=(const glm::vec2& val);
		Variant operator=(const glm::vec3& val);
		Variant operator=(const glm::quat& val);

		operator bool() const;
		operator i32() const;
		operator ui8() const;
		operator ui16() const;
		operator ui32() const;
		operator ui64() const;
		operator float() const;
		operator glm::vec2() const;
		operator glm::vec3() const;
		operator glm::quat() const;

		bool operator<(const Variant& other) const;
		bool operator<=(const Variant& other) const;
		bool operator==(const Variant& other) const;
		bool operator!=(const Variant& other) const;
		bool operator>=(const Variant& other) const;
		bool operator>(const Variant& other) const;

	private:	

		template <class T>
		const T& As() const
		{
			return *(T*)&m_value;
		}

		union VariantValue
		{
			bool Bool;
			i32 Int;
			ui8 UInt8;
			ui16 UInt16;
			ui32 UInt32;
			ui64 UInt64;
			float Float;
			glm::vec2 Vec2;
			glm::vec3 Vec3;
			glm::quat Quat;
			StringId String;
		} m_value;		

		VariantType m_valueType;
	};
}