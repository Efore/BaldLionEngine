#pragma once
#include "BaldLion/Core/Core.h"

namespace BaldLion
{
	namespace Rendering
	{
		struct FramebufferSpecification
		{
			uint32_t Width, Height;
			uint32_t Samples = 1;
			bool SwapChainTarget = false;
		};

		class Framebuffer
		{
		public:
			static Ref<Framebuffer> Create(const FramebufferSpecification& specs);

			virtual void Bind() = 0;
			virtual void Unbind() = 0;

			virtual uint32_t GetColorAttachmentID() const = 0;
			virtual uint32_t GetDepthAttachmentID() const = 0;
			virtual const FramebufferSpecification& GetSpecification() const = 0;
		};
	}
}