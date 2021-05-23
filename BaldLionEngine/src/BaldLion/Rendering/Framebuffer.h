#pragma once
#include "BaldLion/Core/Core.h"
#include "BaldLion/Rendering/Texture.h"

namespace BaldLion
{
	namespace Rendering
	{
		enum class FramebufferUsage
		{
			ColorBuffer = 1 << 0,
			DepthBuffer = 1 << 1,
			StencilBuffer = 1 << 2
		};

		struct FramebufferSpecification
		{
			ui32 Width, Height;
			ui32 Samples = 1;

			ui8 ActiveBuffers = 0;

			bool SwapChainTarget = false;
			Texture* DepthMap = nullptr;
		};

		class Framebuffer
		{
		public:
			static Framebuffer* Create(const FramebufferSpecification& specs);
			static void Destroy(Framebuffer* element);

			virtual void Bind() = 0;
			virtual void Unbind() = 0;
			virtual void Resize(ui32 width, ui32 height) = 0;

			virtual ui32 GetColorAttachmentID() const = 0;
			virtual ui32 GetDepthAttachmentID() const = 0;			
			virtual ui32 GetStencilAttachmentID() const = 0;			

			virtual const FramebufferSpecification& GetSpecification() const = 0;
		};
	}
}