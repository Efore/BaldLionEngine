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
			virtual void Resize(ui32 width, ui32 height) override;

			void Invalidate();

			virtual ui32 GetColorAttachmentID() const override { return m_colorAttachmentID; }
			virtual ui32 GetDepthAttachmentID() const override { return m_depthAttachmentID; }
			virtual const FramebufferSpecification& GetSpecification() const override { return m_framebufferSpecification; }

		private:
			FramebufferSpecification m_framebufferSpecification;
			ui32 m_rendererID = 0;
			ui32 m_colorAttachmentID = 0, m_depthAttachmentID = 0;
		};
	}
}