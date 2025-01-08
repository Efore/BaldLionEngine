#include "blpch.h"
#include "Variant.h"
#include "BaldLion/Utils/MathUtils.h"

namespace BaldLion
{
	Variant Variant::EmptyVariant;

	Variant::Variant()
	{
		memset(&m_value, 0, sizeof(VariantValue));
		m_valueType = VariantType::Unknown;
	}

	Variant::Variant(bool val)
	{
		m_valueType = VariantType::Bool;
		memcpy(&m_value, &val, sizeof(bool));
	}

	Variant::Variant(i32 val)
	{
		m_valueType = VariantType::Int;
		memcpy(&m_value, &val, sizeof(i32));
	}

	Variant::Variant(ui8 val)
	{
		m_valueType = VariantType::UInt8;
		memcpy(&m_value, &val, sizeof(ui8));
	}

	Variant::Variant(ui16 val)
	{
		m_valueType = VariantType::UInt16;
		memcpy(&m_value, &val, sizeof(ui16));
	}

	Variant::Variant(ui32 val)
	{
		m_valueType = VariantType::UInt32;
		memcpy(&m_value, &val, sizeof(ui32));
	}

	Variant::Variant(const ui64& val)
	{
		m_valueType = VariantType::UInt64;
		memcpy(&m_value, &val, sizeof(ui64));
	}

	Variant::Variant(float val)
	{
		m_valueType = VariantType::Float;
		memcpy(&m_value, &val, sizeof(float));
	}

	Variant::Variant(const glm::vec2& val)
	{
		m_valueType = VariantType::Vector2;
		memcpy(&m_value, &val, sizeof(glm::vec2));
	}

	Variant::Variant(const glm::vec3& val)
	{
		m_valueType = VariantType::Vector3;
		memcpy(&m_value, &val, sizeof(glm::vec3));
	}

	Variant::Variant(const glm::quat& val)
	{
		m_valueType = VariantType::Quaternion;
		memcpy(&m_value, &val, sizeof(glm::quat));
	}

	Variant Variant::operator=(bool val)
	{
		m_valueType = VariantType::Bool;
		memcpy(&m_value, &val, sizeof(bool));
		return *this;
	}

	Variant Variant::operator=(i32 val)
	{
		m_valueType = VariantType::Int;
		memcpy(&m_value, &val, sizeof(i32));
		return *this;
	}

	Variant Variant::operator=(ui8 val)
	{
		m_valueType = VariantType::UInt8;
		memcpy(&m_value, &val, sizeof(ui8));
		return *this;
	}

	Variant Variant::operator=(ui16 val)
	{
		m_valueType = VariantType::UInt16;
		memcpy(&m_value, &val, sizeof(ui16));
		return *this;
	}

	Variant Variant::operator=(ui32 val)
	{
		m_valueType = VariantType::UInt32;
		memcpy(&m_value, &val, sizeof(ui32));
		return *this;
	}

	Variant Variant::operator=(const ui64& val)
	{
		m_valueType = VariantType::UInt64;
		memcpy(&m_value, &val, sizeof(ui64));
		return *this;
	}

	Variant Variant::operator=(float val)
	{
		m_valueType = VariantType::Float;
		memcpy(&m_value, &val, sizeof(float));
		return *this;
	}

	Variant Variant::operator=(const glm::vec2& val)
	{
		m_valueType = VariantType::Vector2;
		memcpy(&m_value, &val, sizeof(glm::vec2));
		return *this;
	}

	Variant Variant::operator=(const glm::vec3& val)
	{
		m_valueType = VariantType::Vector3;
		memcpy(&m_value, &val, sizeof(glm::vec3));
		return *this;
	}

	Variant Variant::operator=(const glm::quat& val)
	{
		m_valueType = VariantType::Quaternion;
		memcpy(&m_value, &val, sizeof(glm::quat));
		return *this;
	}

	Variant::operator bool() const
	{
		BL_ASSERT_LOG(m_valueType == VariantType::Bool, "Variant is not bool type");
		return As<bool>();
	}

	Variant::operator i32() const
	{
		switch (m_valueType)
		{
		case VariantType::Bool:		
			return As<bool>() ? 1 : 0;		
			break;
		case VariantType::Int:
			return As<i32>();
			break;
		case VariantType::UInt8:
			return (i32)As<ui8>();
			break;
		case VariantType::UInt16:
			return (i32)As<ui16>();
			break;
		case VariantType::Float:
			return (i32)As<float>();
			break;
		default:
			BL_ASSERT_LOG(false, "Incorrect Variant type");
			break;
		}

		return 0;
	}

	Variant::operator ui8() const
	{
		switch (m_valueType)
		{
		case VariantType::Bool:
			return As<bool>() ? 1 : 0;
			break;
		case VariantType::UInt8:
			return As<ui8>();
			break;
		default:
			BL_ASSERT_LOG(false, "Incorrect Variant type");
			break;
		}

		return 0;
	}

	Variant::operator ui16() const
	{
		switch (m_valueType)
		{
		case VariantType::Bool:
			return As<bool>() ? 1 : 0;
			break;
		case VariantType::UInt8:
			return (ui16)As<ui8>();
			break;
		case VariantType::UInt16:
			return As<ui16>();
			break;
		default:
			BL_ASSERT_LOG(false, "Incorrect Variant type");
			break;
		}

		return 0;
	}

	Variant::operator ui32() const
	{
		switch (m_valueType)
		{
		case VariantType::Bool:
			return As<bool>() ? 1 : 0;
			break;
		case VariantType::UInt8:
			return (ui32)As<ui8>();
			break;
		case VariantType::UInt16:
			return (ui32)As<ui16>();
			break;
		case VariantType::UInt32:
			return As<ui32>();
			break;
		case VariantType::StringID:
			return As<ui32>();
			break;
		default:
			BL_ASSERT_LOG(false, "Incorrect Variant type");
			break;
		}

		return 0;
	}

	Variant::operator ui64() const
	{
		switch (m_valueType)
		{
		case VariantType::Bool:
			return As<bool>() ? 1 : 0;
			break;
		case VariantType::Int:
			return (ui64)As<i32>() ? 1 : 0;
			break;
		case VariantType::UInt8:
			return (ui64)As<ui8>();
			break;
		case VariantType::UInt16:
			return (ui64)As<ui16>();
			break;
		case VariantType::UInt32:
			return (ui64)As<ui32>();
			break;
		case VariantType::UInt64:
			return As<ui64>();
			break;
		case VariantType::StringID:
			return (ui64)As<ui32>();
			break;
		default:
			BL_ASSERT_LOG(false, "Incorrect Variant type");
			break;
		}

		return 0;
	}

	Variant::operator float() const
	{
		BL_ASSERT_LOG(m_valueType == VariantType::Float, "Incorrect Variant type");		
		return As<float>();
	}

	Variant::operator glm::vec2() const
	{
		BL_ASSERT_LOG(m_valueType == VariantType::Vector2, "Incorrect Variant type");
		return As<glm::vec2>();
	}

	Variant::operator glm::vec3() const
	{
		BL_ASSERT_LOG(m_valueType == VariantType::Vector3, "Incorrect Variant type");
		return As<glm::vec3>();
	}

	Variant::operator glm::quat() const
	{
		BL_ASSERT_LOG(m_valueType == VariantType::Quaternion, "Incorrect Variant type");
		return As<glm::quat>();
	}

	bool Variant::operator<(const Variant& other) const
	{
		switch (m_valueType)
		{
		case VariantType::Unknown:
			return false;
			break;
		case VariantType::Bool:
			return false;
			break;
		case VariantType::Int:
			return As<i32>() < other.As<i32>();
			break;
		case VariantType::UInt8:
			return As<ui8>() < other.As<ui8>();
			break;
		case VariantType::UInt16:
			return As<ui16>() < other.As<ui16>();
			break;
		case VariantType::UInt32:
			return As<ui32>() < other.As<ui32>();
			break;
		case VariantType::UInt64:
			return As<ui64>() < other.As<ui64>();
			break;
		case VariantType::Float:
			return As<float>() < other.As<float>();
			break;
		case VariantType::Vector2:
			return false;
			break;
		case VariantType::Vector3:
			return false;
			break;
		case VariantType::Quaternion:
			return false;
			break;
		case VariantType::StringID:
			return false;
			break;
		}

		return false;
	}

	bool Variant::operator<=(const Variant& other) const
	{
		switch (m_valueType)
		{
		case VariantType::Unknown:
			return false;
			break;
		case VariantType::Bool:
			return false;
			break;
		case VariantType::Int:
			return As<i32>() <= other.As<i32>();
			break;
		case VariantType::UInt8:
			return As<ui8>() <= other.As<ui8>();
			break;
		case VariantType::UInt16:
			return As<ui16>() <= other.As<ui16>();
			break;
		case VariantType::UInt32:
			return As<ui32>() <= other.As<ui32>();
			break;
		case VariantType::UInt64:
			return As<ui64>() <= other.As<ui64>();
			break;
		case VariantType::Float:
			return As<float>() <= other.As<float>();
			break;
		case VariantType::Vector2:
			return false;
			break;
		case VariantType::Vector3:
			return false;
			break;
		case VariantType::Quaternion:
			return false;
			break;
		case VariantType::StringID:
			return false;
			break;
		}

		return false;
	}

	bool Variant::operator>=(const Variant& other) const
	{
		switch (m_valueType)
		{
		case VariantType::Unknown:
			return false;
			break;
		case VariantType::Bool:
			return false;
			break;
		case VariantType::Int:
			return As<i32>() >= other.As<i32>();
			break;
		case VariantType::UInt8:
			return As<ui8>() >= other.As<ui8>();
			break;
		case VariantType::UInt16:
			return As<ui16>() >= other.As<ui16>();
			break;
		case VariantType::UInt32:
			return As<ui32>() >= other.As<ui32>();
			break;
		case VariantType::UInt64:
			return As<ui64>() >= other.As<ui64>();
			break;
		case VariantType::Float:
			return As<float>() >= other.As<float>();
			break;
		case VariantType::Vector2:
			return false;
			break;
		case VariantType::Vector3:
			return false;
			break;
		case VariantType::Quaternion:
			return false;
			break;
		case VariantType::StringID:
			return false;
			break;
		}

		return false;
	}

	bool Variant::operator>(const Variant& other) const
	{
		switch (m_valueType)
		{
		case VariantType::Unknown:
			return false;
			break;
		case VariantType::Bool:
			return false;
			break;
		case VariantType::Int:
			return As<i32>() > other.As<i32>();
			break;
		case VariantType::UInt8:
			return As<ui8>() > other.As<ui8>();
			break;
		case VariantType::UInt16:
			return As<ui16>() > other.As<ui16>();
			break;
		case VariantType::UInt32:
			return As<ui32>() > other.As<ui32>();
			break;
		case VariantType::UInt64:
			return As<ui64>() > other.As<ui64>();
			break;
		case VariantType::Float:
			return As<float>() > other.As<float>();
			break;
		case VariantType::Vector2:
			return false;
			break;
		case VariantType::Vector3:
			return false;
			break;
		case VariantType::Quaternion:
			return false;
			break;
		case VariantType::StringID:
			return false;
			break;
		}

		return false;
	}

	bool Variant::operator!=(const Variant& other) const
	{
		switch (m_valueType)
		{
		case VariantType::Unknown:
			return false;
			break;
		case VariantType::Bool:
			return As<bool>() != other.As<bool>();
			break;
		case VariantType::Int:
			return As<i32>() != other.As<i32>();
			break;
		case VariantType::UInt8:
			return As<ui8>() != other.As<ui8>();
			break;
		case VariantType::UInt16:
			return As<ui16>() != other.As<ui16>();
			break;
		case VariantType::UInt32:
			return As<ui32>() != other.As<ui32>();
			break;
		case VariantType::UInt64:
			return As<ui64>() != other.As<ui64>();
			break;
		case VariantType::Float:
			return !glm::epsilonEqual(As<float>(), other.As<float>(), glm::epsilon<float>());
			break;
		case VariantType::Vector2:
			return !MathUtils::AlmostEqual(As<glm::vec2>(), other.As<glm::vec2>());
			break;
		case VariantType::Vector3:
			return !MathUtils::AlmostEqual(As<glm::vec2>(), other.As<glm::vec2>());
			break;
		case VariantType::Quaternion:
			return !MathUtils::AlmostEqual(As<glm::quat>(), other.As<glm::quat>());
			break;
		case VariantType::StringID:
			return As<ui32>() != other.As<ui32>();
			break;
		}

		return false;
	}

	bool Variant::operator==(const Variant& other) const
	{
		switch (m_valueType)
		{
		case VariantType::Unknown:
			return false;
			break;
		case VariantType::Bool:
			return As<bool>() == other.As<bool>();
			break;
		case VariantType::Int:
			return As<i32>() == other.As<i32>();
			break;
		case VariantType::UInt8:
			return As<ui8>() == other.As<ui8>();
			break;
		case VariantType::UInt16:
			return As<ui16>() == other.As<ui16>();
			break;
		case VariantType::UInt32:
			return As<ui32>() == other.As<ui32>();
			break;
		case VariantType::UInt64:
			return As<ui64>() == other.As<ui64>();
			break;
		case VariantType::Float:
			return glm::epsilonEqual(As<float>(), other.As<float>(), glm::epsilon<float>());
			break;
		case VariantType::Vector2:
			return MathUtils::AlmostEqual(As<glm::vec2>(), other.As<glm::vec2>());
			break;
		case VariantType::Vector3:
			return MathUtils::AlmostEqual(As<glm::vec2>(), other.As<glm::vec2>());
			break;
		case VariantType::Quaternion:
			return MathUtils::AlmostEqual(As<glm::quat>(), other.As<glm::quat>());
			break;
		case VariantType::StringID:
			return As<ui32>() == other.As<ui32>();
			break;
		}

		return false;
	}
}
