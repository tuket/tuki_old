#pragma once

typedef int TextureId;

// The internal GPU formal of the texels
enum class TexelFormat
{
	RGB8,
	RGBA8,
	DEPTH16,
	DEPTH24,
	DEPTH32,
	DEPTH24_STENCIL8
};

enum class TextureAddressMode
{
	WRAP,
	MIRROR,
	CLAMP
};

enum class TextureFilterMode
{
	NEAREST,
	BILINEAR,
	TRILINEAR
};

enum class TextureUnit
{
	COLOR = 0,
	NORMAL,
	SPECULAR,
	EMISSIVE,
};

// 2D texture
class Texture
{
public:

	TextureId getId()const { return id; }
	void bindToUnit()const;

	void free();

private:
	TextureId id;
	unsigned width, height;
};