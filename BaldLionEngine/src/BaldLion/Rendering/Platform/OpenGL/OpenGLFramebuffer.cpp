#include "blpch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace BaldLion
{
	namespace Rendering
	{
		OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& specification) :
			m_framebufferSpecification(specification)
		{
			Invalidate();
		}

		OpenGLFramebuffer::~OpenGLFramebuffer()
		{
			glDeleteFramebuffers(1, &m_rendererID);
			glDeleteTextures(1, &m_colorAttachmentID);
			glDeleteTextures(1, &m_depthAttachmentID);
		}

		void OpenGLFramebuffer::Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
			glViewport(0, 0, m_framebufferSpecification.Width, m_framebufferSpecification.Height);
		}

		void OpenGLFramebuffer::Unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void OpenGLFramebuffer::Resize(ui32 width, ui32 height)
		{
			m_framebufferSpecification.Width = width;
			m_framebufferSpecification.Height = height;
			Invalidate();
		}

		void OpenGLFramebuffer::Invalidate()
		{
			if (m_rendererID)
			{
				glDeleteFramebuffers(1, &m_rendererID);
				glDeleteTextures(1, &m_colorAttachmentID);
				glDeleteTextures(1, &m_depthAttachmentID);
			}

			glCreateFramebuffers(1, &m_rendererID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

			glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachmentID);
			glBindTexture(GL_TEXTURE_2D, m_colorAttachmentID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_framebufferSpecification.Width, m_framebufferSpecification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachmentID, 0);

			glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachmentID);
			glBindTexture(GL_TEXTURE_2D, m_depthAttachmentID);
			glTextureStorage2D(m_depthAttachmentID, 1, GL_DEPTH24_STENCIL8, m_framebufferSpecification.Width, m_framebufferSpecification.Height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachmentID, 0);

			BL_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}