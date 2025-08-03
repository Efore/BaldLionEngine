#pragma once
#include "BaldLion/Core/Core.h"
#include "BaldLion/Core/StringId.h"
#include <glm/gtc/quaternion.hpp>

namespace BaldLion
{	
	enum class VariantType : ui16
	{
		Unknown,
		Bool,
		Int8,
		UInt8,
		Int16,
		UInt16,
		Int32,
		UInt32,
		Int64,
		UInt64,
		Float,
		Vector2,
		Vector3,
		Quaternion,
		StringID
	};
	
	enum class VariantComparisonType : ui8 {
		Lower = 0,
		LowerOrEqual = 1,
		Equal = 2,
		Unequal = 3,
		HigherOrEqual = 4,
		Higher = 5
	};
	

	#define VARIANT_SIZE sizeof(glm::quat)

	class Variant
	{
	public:

		const static Variant EmptyVariant;

		union VariantValue
		{
			bool Bool;
			i8 Int8;
			ui8 UInt8;
			i16 Int16;
			ui16 UInt16;
			i32 Int32;
			ui32 UInt32;
			i64 Int64;
			ui64 UInt64;
			float Float;
			glm::vec2 Vec2;
			glm::vec3 Vec3;
			glm::quat Quat;
			StringId String;

			VariantValue() { memset(this, 0, sizeof(*this)); }
		};

		Variant();
		Variant(bool val);
		Variant(ui8 val);
		Variant(i8 val);
		Variant(i16 val);
		Variant(ui16 val);
		Variant(i32 val);
		Variant(ui32 val);
		Variant(const i64& val);
		Variant(const ui64& val);
		Variant(float val);
		Variant(const glm::vec2& val);
		Variant(const glm::vec3& val);
		Variant(const glm::quat& val);

		Variant operator=(bool val);
		Variant operator=(i8 val);
		Variant operator=(ui8 val);
		Variant operator=(i16 val);
		Variant operator=(ui16 val);
		Variant operator=(i32 val);
		Variant operator=(ui32 val);
		Variant operator=(const i64& val);
		Variant operator=(const ui64& val);
		Variant operator=(float val);
		Variant operator=(const glm::vec2& val);
		Variant operator=(const glm::vec3& val);
		Variant operator=(const glm::quat& val);

		operator bool() const;
		operator i8() const;
		operator ui8() const;
		operator i16() const;
		operator ui16() const;
		operator i32() const;
		operator ui32() const;
		operator i64() const;
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
		
		const VariantValue& GetValue() const
		{
			return m_value;
		}

		VariantValue& GetValue()
		{
			return m_value;
		}

	private:	

		template <class T>
		const T& As() const
		{
			return *(T*)&m_value;
		}

		VariantValue m_value;		

	public:
		VariantType m_valueType;
	};
}