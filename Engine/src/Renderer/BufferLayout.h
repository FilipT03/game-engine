#pragma once

#include "Core/Core.h"
#include <initializer_list>

namespace ft {
	enum class LayoutElementType
	{
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool
	};

	static uint32_t LayoutElementTypeSize(LayoutElementType type)
	{
		switch (type)
		{
			case LayoutElementType::Float:   return 4;
			case LayoutElementType::Float2:  return 4 * 2;
			case LayoutElementType::Float3:  return 4 * 3;
			case LayoutElementType::Float4:  return 4 * 4;
			case LayoutElementType::Mat3:    return 4 * 3 * 3;
			case LayoutElementType::Mat4:    return 4 * 4 * 4;
			case LayoutElementType::Int:     return 4;
			case LayoutElementType::Int2:    return 4 * 2;
			case LayoutElementType::Int3:    return 4 * 3;
			case LayoutElementType::Int4:    return 4 * 4;
			case LayoutElementType::Bool:    return 1;
		}
		return 0;
	}

	static uint32_t LayoutElementTypeCount(LayoutElementType type)
	{
		switch (type)
		{
			case LayoutElementType::Float:   return 1;
			case LayoutElementType::Float2:  return 2;
			case LayoutElementType::Float3:  return 3;
			case LayoutElementType::Float4:  return 4;
			case LayoutElementType::Mat3:    return 3 * 3;
			case LayoutElementType::Mat4:    return 4 * 4;
			case LayoutElementType::Int:     return 1;
			case LayoutElementType::Int2:    return 2;
			case LayoutElementType::Int3:    return 3;
			case LayoutElementType::Int4:    return 4;
			case LayoutElementType::Bool:    return 1;
		}
		return 0;
	}

	struct LayoutElement
	{
		LayoutElementType type;
		std::string name;
		uint32_t size;
		uint32_t offset;
		bool normalized;

		LayoutElement(LayoutElementType type, std::string name, bool normalized = false)
			: type(type), name(name), size(LayoutElementTypeSize(type)), offset(0), normalized(normalized) {}

		uint32_t GetComponentCount() const { return LayoutElementTypeCount(type); }
	};

	class BufferLayout
	{
	public:
		BufferLayout(std::initializer_list<LayoutElement> data) : m_Elements(data), m_Stride(0), m_ComponentCount(0)
		{
			uint32_t offset = 0;
			for (auto& element : m_Elements)
			{
				element.offset = offset;
				offset += element.size;
				m_Stride += element.size;
				m_ComponentCount += element.GetComponentCount();
			}
		}
		BufferLayout() = default;
		~BufferLayout() = default;
		inline const std::vector<LayoutElement>& GetElements() const { return m_Elements; }
		inline uint32_t GetStride() const { return m_Stride; }
		inline uint32_t GetElementCount() const { return static_cast<uint32_t>(m_Elements.size()); };
		inline uint32_t GetComponentCount() const { return m_ComponentCount; };
	private:
		std::vector<LayoutElement> m_Elements;
		uint32_t m_Stride;
		uint32_t m_ComponentCount;
	};
}

