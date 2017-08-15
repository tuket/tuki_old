#include "texture.hpp"

#include <cassert>
#include <cstring>
#include <stbi.h>
#include <glad/glad.h>
#include <cmath>
#include <SDL.h>
#include <utility>
#include <stb_image_write.h>
#include <thread>

const unsigned PIXEL_FORMAT_SIZE[(int)PixelFormat::COUNT] =
{
	3,		// RGB8
	4,		// RGBA8
};

const unsigned PIXEL_NUM_CHANNELS[(int)PixelFormat::COUNT] =
{
	3,		// RGB8
	4,		// RGBA8
};

const unsigned TEXEL_NUM_CHANNELS[(int)TexelFormat::COUNT] =
{
	3,		// RGB8
	4,		// RGBA8
	1,		// DEPTH16
	1,		// DEPTH24
	1,		// DEPTH32
	1,		// DEPTH_AUTO
	2,		// DEPTH24_STENCIL8
};

const GLuint TO_GL_WRAP_MODE[(int)TextureWrapMode::COUNT] =
{
	GL_REPEAT,
	GL_MIRRORED_REPEAT,
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_BORDER
};

const GLuint TO_GL_TEXEL_FORMAT[(int)TexelFormat::COUNT] =
{
	GL_RGB8,
	GL_RGBA8,
	GL_DEPTH_COMPONENT16,
	GL_DEPTH_COMPONENT24,
	GL_DEPTH_COMPONENT32,
	GL_DEPTH_COMPONENT,
	GL_DEPTH24_STENCIL8
};

const TexelFormat TO_DEFAULT_TEXEL_FORMAT[(int)PixelFormat::COUNT] =
{
	TexelFormat::RGB8,
	TexelFormat::RGBA8
};

const GLuint TO_GL_PIXEL_FORMAT[(int)PixelFormat::COUNT] =
{
	GL_RGB,
	GL_RGBA
};

// IMAGE
unsigned Image::getPerPixelSize()const
{
	assert(data != nullptr);

	return PIXEL_FORMAT_SIZE[(int)format];
}

unsigned Image::getNumChannels()const
{
	return PIXEL_NUM_CHANNELS[(int)format];
}

void Image::flipY()
{
	assert(data != nullptr);

	unsigned h2 = height / 2;
	unsigned w = width;
	unsigned nc = getNumChannels();
	unsigned char* d = (unsigned char*)data;

	for (int y = 0; y < h2; y++)
	for (int x = 0; x < w; x++)
	for (int c = 0; c < nc; c++)
	{
		int y2 = height - y - 1;
		std::swap(
			d[(y  * w + x) * nc + c],
			d[(y2 * w + x) * nc + c]
		);
	}
}

void Image::free()
{
	assert(data != nullptr);

	delete[] data;
}

Image Image::createEmpty(unsigned width, unsigned height, PixelFormat format)
{
	Image image;
	image.format = format;
	unsigned pixSize = PIXEL_FORMAT_SIZE[(int)format];
	unsigned imageSize = pixSize * width * height;
	image.data = new char[imageSize];
	image.width = width;
	image.height = height;
	memset(image.data, 0, imageSize);
	return image;
}

Image Image::loadFromFile(const char* fileName)
{
	Image image;
	int channels;
	image.data = stbi_load(fileName, &image.width, &image.height, &channels, 0);

	assert(image.data != nullptr);

	if (channels == 3)
	{
		image.format = PixelFormat::RGB8;
	}
	else if (channels == 4)
	{
		image.format = PixelFormat::RGBA8;
	}
	else
	{
		assert("image format not supported");
	}

	image.flipY();
	return image;
}

// TEXTURE
void Texture::bindToUnit(unsigned unit)const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, id);
}
void Texture::bindToUnit(TextureUnit unit)const
{
	bindToUnit((unsigned)unit);
}

unsigned Texture::getNumChannels()const
{
	return TEXEL_NUM_CHANNELS[(int)texelFormat];
}

void Texture::setWrapModeUv(TextureWrapMode wrapModeU, TextureWrapMode wrapModeV)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TO_GL_WRAP_MODE[(int)wrapModeU]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TO_GL_WRAP_MODE[(int)wrapModeV]);
}

void Texture::setWrapModeU(TextureWrapMode wrapMode)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TO_GL_WRAP_MODE[(int)wrapMode]);
}

void Texture::setWRapModeV(TextureWrapMode wrapMode)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TO_GL_WRAP_MODE[(int)wrapMode]);
}

void Texture::setFilterMode(TextureFilterMode filterMode)
{
	this->filterMode = filterMode;
	resetFilterMode();
}

void Texture::resetFilterMode()
{
	// mipmapping enabled
	if(hasMipmaps())
	switch (filterMode)
	{
	case TextureFilterMode::NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case TextureFilterMode::BILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case TextureFilterMode::LINEAR_MIPMAP:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case TextureFilterMode::TRILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	}

	// mipmapping disabled
	else
	switch (filterMode)
	{
	case TextureFilterMode::NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

void Texture::generateMipmaps()
{
	mipmapLevels = 1 + floor(log2(max(width, height)));
	glBindTexture(GL_TEXTURE_2D, id);
	glGenerateMipmap(GL_TEXTURE_2D);
	resetFilterMode();
}

void Texture::save(const char* fileName, bool async, bool transparency)
{
	PixelFormat pixFormat = transparency ? PixelFormat::RGBA8 : PixelFormat::RGB8;
	Image image = Image::createEmpty(width, height, pixFormat);
	unsigned nc = PIXEL_NUM_CHANNELS[(int)pixFormat];

	glBindTexture(GL_TEXTURE_2D, id);
	glGetTexImage(GL_TEXTURE_2D, 0, TO_GL_PIXEL_FORMAT[(int)pixFormat], GL_UNSIGNED_BYTE, image.getData());

	image.flipY();

	auto saveToDisk =
		[&]()
		{
			stbi_write_png(fileName, width, height, nc, image.getData(), nc * width);
			image.free();
		};

	if (async)
	{
		std::thread(saveToDisk);
	}
	else
	{
		saveToDisk();
	}
}

void Texture::free()
{
	glDeleteTextures(1, (GLuint*)&id);
}

Texture Texture::createEmpty(unsigned width, unsigned height, TexelFormat texelFormat)
{
	Texture texture;
	glGenTextures(1, (GLuint*)&texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glTexImage2D(GL_TEXTURE_2D, 0, TO_GL_TEXEL_FORMAT[(int)texelFormat],
		width, height, 0,
		0, 0,
		(void*)0
	);
	texture.texelFormat = texelFormat;
	texture.width = width;
	texture.height = height;
	texture.setWrapMode(TextureWrapMode::REPEAT);
	texture.setFilterMode(TextureFilterMode::NEAREST);
	return texture;
}

Texture Texture::loadFromFile(const char* fileName, TexelFormat internalFormat)
{
	// load from file
	Image img = Image::loadFromFile(fileName);

	return createFromImage(img, internalFormat);
}

Texture Texture::createFromImage(const Image& img, TexelFormat internalFormat)
{
	Texture texture;
	glGenTextures(1, (GLuint*)&texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	if (internalFormat == TexelFormat::COUNT)
	{
		// choose automatically
		internalFormat = TO_DEFAULT_TEXEL_FORMAT[(int)img.getPixelFormat()];
	}
	GLuint glInternalFormat;
	glInternalFormat = TO_GL_TEXEL_FORMAT[(int)internalFormat];

	glTexImage2D(GL_TEXTURE_2D, 0,
		glInternalFormat,
		img.getWidth(), img.getHeight(), 0,
		TO_GL_PIXEL_FORMAT[(int)img.getPixelFormat()], GL_UNSIGNED_BYTE,
		img.getData());

	texture.texelFormat = internalFormat;
	texture.width = img.getWidth();
	texture.height = img.getHeight();
	texture.setWrapMode(TextureWrapMode::REPEAT);
	texture.setFilterMode(TextureFilterMode::NEAREST);
	return texture;
}