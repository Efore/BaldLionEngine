#pragma once

#include "BaldLion/Rendering/Framebuffer.h"

namespace BaldLion
{
	namespace Rendering
	{
		class OpenGLFramebuffer : public Framebuffer
		{
		public:
			OpenGLFramebuffer(const FramebufferSpecification& specification);
			virtual ~OpenGLFramebuffer();

			virtual void Bind() override;
			virtual void Unbind() override;

			void Invalidate();

			virtual uint32_t GetColorAttachmentID() const override { return m_colorAttachmentID; }
			virtual uint32_t GetDepthAttachmentID() const override { return m_depthAttachmentID; }
			virtual const FramebufferSpecification& GetSpecification() const override { return m_framebufferSpecification; }

		private:
			uint32_t m_rendererID;
			uint32_t m_colorAttachmentID, m_depthAttachmentID;
			FramebufferSpecification m_framebufferSpecification;
		};
	}
}