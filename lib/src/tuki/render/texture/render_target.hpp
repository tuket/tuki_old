#pragma once

#include "texture.hpp"

class RenderTarget
{
public:
	RenderTarget(
		unsigned numTextures = 0,
		unsigned width = 0, unsigned height = 0,
		TexelFormat texelFormat = TexelFormat::RGBA8,
		bool depthTex = false
	);

	RenderTarget(
		unsigned numTextures,
		unsigned width, unsigned height,
		TexelFormat* texelFormats,
		bool depthTex = false
	);

	TextureId getTexture(unsigned slot)const;
	TextureId getDepthTexture()const;
};