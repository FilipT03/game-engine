#include "pch.h"
#include "RendererCommon.h"

#include<gl/GL.h>

namespace ft {
	void RendererCommon::Clear()
	{
		#ifdef FT_OPENGL_RENDERER
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		#endif
	}

	void RendererCommon::SetClearColor(float r, float g, float b, float a)
	{
		#ifdef FT_OPENGL_RENDERER
		glClearColor(r, g, b, a);
		#endif
	}

	/// Sources should be in the same order as the layout.
	void RendererCommon::PackInterleaved(
		std::vector<uint8_t>& out,
		uint32_t vertexCount,
		const std::vector<const void*>& sources,
		const BufferLayout& layout)
	{
		const auto& elements = layout.GetElements();
		const uint32_t stride = layout.GetStride();

		out.resize(vertexCount * stride);

		uint8_t* destination = out.data();

		for (uint32_t v = 0; v < vertexCount; ++v)
		{
			for (size_t a = 0; a < elements.size(); ++a)
			{
				const auto& element = elements[a];
				const uint8_t* source = (const uint8_t*)sources[a];

				const uint8_t* sourcePtr = source + v * element.size;
				uint8_t* destinationPtr = destination + element.offset;

				std::memcpy(destinationPtr, sourcePtr, element.size);
			}

			destination += stride;
		}
	}

}