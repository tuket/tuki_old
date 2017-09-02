#pragma once

#include "texture.hpp"

#include <vector>

class RenderTarget
{
public:
	RenderTarget(
		unsigned numTextures = 0,
		unsigned width = 0, unsigned height = 0,
		TexelFormat texelFormat = TexelFormat::RGBA8,	// all textures have the same texel format
		bool depthTex = false
	);

	RenderTarget(
		unsigned numTextures,
		unsigned width, unsigned height,
		const TexelFormat* texelFormats,	// array of texel formats for each texture
		bool depthTex = false
	);

	Texture getTexture(unsigned slot)const;
	TextureId getTextureId(unsigned slot)const;
	TextureId getDepthTextureId()const;
	unsigned getNumTextures()const;

	void resize(unsigned width, unsigned height);

	void bind();

	unsigned getWidth()const { return width; }

	static const unsigned MAX_NUM_TEXTURES = 16;

private:

	std::vector<TexelFormat> texelFormats;
	std::vector<Texture> textures;
	Texture depthTexture;
	unsigned fbo;

	unsigned width, height;

public:
	static void bindDefault();
};