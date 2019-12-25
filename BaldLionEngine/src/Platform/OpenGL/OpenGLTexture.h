#pragma once
#include "BaldLion/Renderer/Texture.h"

namespace BaldLion
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_width; }
		virtual uint32_t GetHeight() const override { return m_height; }

		virtual void Bind(uint32_t slot = 0) const override;

		virtual const std::string& GetName() const override { return m_name; }

	private:
		std::string m_path;
		std::string m_name;

		uint32_t m_width, m_height;
		uint32_t m_rendererID;
	};
}