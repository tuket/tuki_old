#include "render_target.hpp"

#include "frame_buffer.hpp"
#include <glad/glad.h>

void RenderTarget::bind()
{
	frameBuffer->bind();

	unsigned px, pw, py, ph;
	if (percentDefined)
	{
		px = frameBuffer->getWidth() * x.percent;
		pw = frameBuffer->getWidth() * w.percent;
		py = frameBuffer->getHeight() * y.percent;
		ph = frameBuffer->getHeight() * h.percent;
	}
	else
	{
		px = x.texels;
		pw = w.texels;
		py = y.texels;
		ph = h.texels;
	}

	glViewport(px, pw, py, ph);
	glScissor(px, pw, py, ph);
	
}