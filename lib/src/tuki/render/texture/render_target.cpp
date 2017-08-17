#include "render_target.hpp"

#include <glad/glad.h>
#include <cassert>

using namespace std;

RenderTarget::RenderTarget(
	unsigned numTextures = 0,
	unsigned width = 0, unsigned height = 0,
	TexelFormat texelFormat = TexelFormat::RGBA8,
	bool depthTex = false
)
{
	if (
		(numTextures == 0 && !depthTex) ||
		width == 0 ||height == 0
	)return;

	assert(numTextures <= MAX_NUM_TEXTURES);

	texelFormats.resize(numTextures);
	fill(texelFormats.begin(), texelFormats.end(), texelFormat);

	textures.resize(numTextures);
	for (unsigned i = 0; i < numTextures; i++)
	{
		textures[i] = Texture::createEmpty(width, height, texelFormat);
	}

	if (depthTex)
	{
		depthTexture = Texture::createEmpty(width, height, TexelFormat::DEPTH_AUTO);
	}

	glGenFramebuffers(1, &fbo);

	resize(width, height);	
}

RenderTarget::RenderTarget(
	unsigned numTextures,
	unsigned width, unsigned height,
	const TexelFormat* texForms,	// array of texel formats for each texture
	bool depthTex
)
{
	if (
		(numTextures == 0 && !depthTex) ||
		width == 0 || height == 0 ||
		texForms == nullptr
	)return;

	assert(numTextures <= MAX_NUM_TEXTURES);

	texelFormats.resize(numTextures);
	for (int i = 0; i < numTextures; i++) texelFormats[i] = texForms[i];

	textures.resize(numTextures);
	for (unsigned i = 0; i < numTextures; i++)
	{
		textures[i] = Texture::createEmpty(width, height, texelFormats[i]);
	}

	if (depthTex)
	{
		depthTexture = Texture::createEmpty(width, height, TexelFormat::DEPTH_AUTO);
	}

	glGenFramebuffers(1, &fbo);

	resize(width, height);
}

TextureId RenderTarget::getTextureId(unsigned slot)const
{
	return textures[slot].getId();
}

TextureId RenderTarget::getDepthTextureId()const
{
	return depthTexture.getId();
}

unsigned RenderTarget::getNumTextures()const
{
	return textures.size();
}

void RenderTarget::resize(unsigned w, unsigned h)
{
	width = w;
	height = h;

	const unsigned nt = getNumTextures();

	// reserve space in textures
	for (unsigned i = 0; i < nt; i++)
	{
		textures[i].resize(); // CCPI
	}
}

void RenderTarget::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}