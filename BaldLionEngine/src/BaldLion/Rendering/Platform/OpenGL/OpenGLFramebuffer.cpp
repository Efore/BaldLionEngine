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
			if (m_framebufferID)
				glDeleteFramebuffers(1, &m_framebufferID);

			if (m_colorBufferID)
				glDeleteTextures(1, &m_colorBufferID);

			if (m_depthBufferID)
				glDeleteRenderbuffers(1, &m_depthBufferID);

			if (m_stencilBufferID)
				glDeleteRenderbuffers(1, &m_stencilBufferID);
		}

		void OpenGLFramebuffer::Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);
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
			if (m_framebufferID)			
				glDeleteFramebuffers(1, &m_framebufferID);

			if(m_colorBufferID)
				glDeleteTextures(1, &m_colorBufferID);

			if(m_depthBufferID)
				glDeleteRenderbuffers(1, &m_depthBufferID);

			if(m_stencilBufferID)
				glDeleteRenderbuffers(1, &m_stencilBufferID);

			glCreateFramebuffers(1, &m_framebufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);

			//Color buffer
			if ((m_framebufferSpecification.ActiveBuffers & (ui8)FramebufferUsage::ColorBuffer) > 0)
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &m_colorBufferID);
				glBindTexture(GL_TEXTURE_2D, m_colorBufferID);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_framebufferSpecification.Width, m_framebufferSpecification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBufferID, 0);
			}
			else
			{
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
			}			

			//Depth & stencil render buffer
			int depthAndStencil = (ui8)FramebufferUsage::DepthBuffer & (ui8)FramebufferUsage::StencilBuffer;
			if ((m_framebufferSpecification.ActiveBuffers & depthAndStencil) > 0)
			{
				glCreateRenderbuffers(1, &m_depthBufferID);
				glBindRenderbuffer(GL_RENDERBUFFER, m_depthBufferID);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_framebufferSpecification.Width, m_framebufferSpecification.Height);
				glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthBufferID);				
			}
			else if((m_framebufferSpecification.ActiveBuffers & (ui8)FramebufferUsage::DepthBuffer) > 0)
			{
				if (m_framebufferSpecification.DepthMap != nullptr)
				{
					ui32 depthMapID;
					glGenTextures(1, &depthMapID);

					m_framebufferSpecification.DepthMap->SetRendererID(depthMapID);
					m_framebufferSpecification.DepthMap->SetWidth(m_framebufferSpecification.Width);
					m_framebufferSpecification.DepthMap->SetHeight(m_framebufferSpecification.Height);

					glBindTexture(GL_TEXTURE_2D, depthMapID);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_framebufferSpecification.Width, m_framebufferSpecification.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
					float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
					glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

					glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapID, 0);
				}
				else
				{
					glCreateRenderbuffers(1, &m_depthBufferID);
					glBindRenderbuffer(GL_RENDERBUFFER, m_depthBufferID);
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_framebufferSpecification.Width, m_framebufferSpecification.Height);
					glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBufferID);				
				}

			}
			else if ((m_framebufferSpecification.ActiveBuffers & (ui8)FramebufferUsage::StencilBuffer) > 0)
			{
				glCreateRenderbuffers(1, &m_stencilBufferID);
				glBindRenderbuffer(GL_RENDERBUFFER, m_stencilBufferID);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_framebufferSpecification.Width, m_framebufferSpecification.Height);
				glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_stencilBufferID);				
			}

			//BL_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}