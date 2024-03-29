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

			virtual ui32 GetColorAttachmentID() const override { return m_colorBufferID; }
			virtual ui32 GetDepthAttachmentID() const override { return m_depthBufferID; }
			virtual ui32 GetStencilAttachmentID() const override { return m_stencilBufferID; }

			virtual const FramebufferSpecification& GetSpecification() const override { return m_framebufferSpecification; }

		private:

			ui32 m_framebufferID = 0;
			ui32 m_colorBufferID = 0;
			ui32 m_depthBufferID = 0;	
			ui32 m_stencilBufferID = 0;

			FramebufferSpecification m_framebufferSpecification;
			
		};
	}
}