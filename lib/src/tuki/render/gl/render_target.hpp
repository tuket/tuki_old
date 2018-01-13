#pragma once

class FrameBuffer;

// region in a frame buffer
struct RenderTarget
{
	RenderTarget(FrameBuffer* frameBuffer) : frameBuffer(frameBuffer) {}

	void bind();

	FrameBuffer* frameBuffer;

	bool percentDefined;	// if true, defines the region as percentage, else: as number of texels
	union {
		float percent;
		unsigned texels;
	} x;
	union {
		float percent;
		unsigned texels;
	} w;
	union {
		float percent;
		unsigned texels;
	} y;
	union {
		float percent;
		unsigned texels;
	} h;
};