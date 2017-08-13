#include "texture.hpp"

#include <cassert>
#include <cstring>
#include <stbi.h>
#include <glad/glad.h>
#include <cmath>

const unsigned PIXEL_FORMAT_SIZE[(int)PixelFormat::COUNT] =
{
	3,		// RGB8
	4,		// RGBA8
};

const GLuint TO_GL_WRAP_MODE[(int)TextureWrapMode::COUNT] =
{
	GL_REPEAT,
	GL_MIRRORED_REPEAT,
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_BORDER
};

// IMAGE
unsigned Image::getPerPixelSize()const
{
	assert(data != nullptr);

	return PIXEL_FORMAT_SIZE[(int)format];
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
}

Image Image::loadFromFile(const char* fileName)
{
	Image image;
	int channels;
	image.data = (fileName, &image.width, &image.height, &channels, 0);
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

}