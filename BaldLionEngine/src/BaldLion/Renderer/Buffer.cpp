#include "blpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace BaldLion
{
	//-------------------------------------------------
	//Vertex buffer
	//-------------------------------------------------
	VertexBuffer * VertexBuffer::Create(float * vertices, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererAPI::API::OpenGL:		return new OpenGLVertexBuffer(vertices, size);
		}

		BL_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	//-------------------------------------------------
	//Index buffer
	//-------------------------------------------------
	IndexBuffer * IndexBuffer::Create(uint32_t * indices, uint32_t count)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererAPI::API::OpenGL:		return new OpenGLIndexBuffer(indices, count);
		}

		BL_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}